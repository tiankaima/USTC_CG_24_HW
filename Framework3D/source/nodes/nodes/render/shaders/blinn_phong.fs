#version 430 core

// Define a uniform struct for lights
struct Light {
    // The matrices are used for shadow mapping. You need to fill it according to how we are filling it when building the normal maps (node_render_shadow_mapping.cpp). 
    // Now, they are filled with identity matrix. You need to modify C++ code innode_render_deferred_lighting.cpp.
    // Position and color are filled.
    mat4 light_projection;
    mat4 light_view;
    vec3 position;
    float radius;
    vec3 color; // Just use the same diffuse and specular color.
    int shadow_map_id;
};

layout(binding = 0) buffer lightsBuffer {
Light lights[4];
};

uniform vec2 iResolution;

uniform sampler2D diffuseColorSampler;
uniform sampler2D normalMapSampler; // You should apply normal mapping in rasterize_impl.fs
uniform sampler2D metallicRoughnessSampler;
uniform sampler2DArray shadow_maps;
uniform sampler2D position;

// uniform float alpha;
uniform vec3 camPos;

uniform int light_count;

layout(location = 0) out vec4 Color;

void main() 
{
    vec2 uv = gl_FragCoord.xy / iResolution;

    vec3 pos = texture2D(position, uv).xyz;
    vec3 normal = texture2D(normalMapSampler,uv).xyz;

    vec4 metalnessRoughness = texture2D(metallicRoughnessSampler,uv);
    float metal = metalnessRoughness.x;
    float roughness = metalnessRoughness.y;

    float ka = .4;
    float ks = metal;
    float kd = 1 - ks;
    Color = vec4(0, 0, 0, 1);
    for(int i = 0; i < light_count; i++)
    {
        vec4 fragPosLightSpace = lights[i].light_projection * lights[i].light_view * vec4(pos, 1.0);
        vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
        float currentDepth = projCoords.z;
        float closetDepth = texture(shadow_maps, vec3(projCoords.xy * 0.5 + 0.5, lights[i].shadow_map_id)).x;

        vec3 ambient = lights[i].color * ka * texture2D(diffuseColorSampler,uv).xyz;
        vec3 diffuse = vec3(0, 0, 0);
        vec3 specular = vec3(0, 0, 0);

        float bias = 0.002;
        if (currentDepth - bias <= closetDepth)
        {
            normal = normalize(normal);
            vec3 LightDirection = normalize(lights[i].position - pos);
        
            float diff = max(0.0, dot(normal, LightDirection));
            diffuse = kd * lights[i].color * (diff * texture2D(diffuseColorSampler,uv).xyz);

            if (diff != 0)
            {
                vec3 HalfVector = normalize(camPos - pos) + LightDirection;
                HalfVector = normalize(HalfVector);
                float spec = pow(dot(normal, HalfVector), (1 - roughness) * 50);
                specular = ks * spec * lights[i].color;
            }
        }
        Color += vec4(min(ambient + diffuse + specular, vec3(1.0)), 0);
        // Visualization of shadow map

        // HW6_TODO: first comment the line above ("Color +=..."). That's for quick Visualization.
        // You should first do the Blinn Phong shading here. You can use roughness to modify alpha. Or you can pass in an alpha value through the uniform above.

        // After finishing Blinn Phong shading, you can do shadow mapping with the help of the provided shadow_map_value. You will need to refer to the node, node_render_shadow_mapping.cpp, for the light matrices definition. Then you need to fill the mat4 light_projection; mat4 light_view; with similar approach that we fill position and color.
        // For shadow mapping, as is discussed in the course, you should compare the value "position depth from the light's view" against the "blocking object's depth.", then you can decide whether it's shadowed.

        // PCSS is also applied here.
    }
}