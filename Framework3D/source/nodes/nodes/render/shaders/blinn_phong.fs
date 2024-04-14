#version 430 core

// Define a uniform struct for lights
struct Light {
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
uniform sampler2D normalMapSampler;
uniform sampler2D tangentMapSampler;
uniform sampler2D bitangentMapSampler;
uniform sampler2D metallicRoughnessSampler;
uniform sampler2DArray shadow_maps;
uniform sampler2D position;

// uniform float alpha;
uniform vec3 camPos;

uniform int light_count;

layout(location = 0) out vec4 Color;

void main() {
    vec2 uv = gl_FragCoord.xy / iResolution;

    vec3 pos = texture2D(position,uv).xyz;
    vec3 normal = texture2D(normalMapSampler,uv).xyz;

    vec3 T = texture2D(tangentMapSampler,uv).xyz;
    vec3 B = texture2D(bitangentMapSampler,uv).xyz;
    vec3 N = normal;
    mat3 TBN = mat3(T, B, N);

    Color = vec4(B , 1.0);
    return;

//     normal = normalize(TBN * normal);

    vec4 metallicRoughness = texture2D(metallicRoughnessSampler,uv);
    float metal = metallicRoughness.x;
    float roughness = metallicRoughness.y;
    float shininess = (1.0 - roughness) * 100;

    for(int i = 0; i < light_count; i ++) {
        vec3 lightDir = normalize(lights[i].position - pos);
        vec3 viewDir = normalize(camPos - pos);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        vec3 light_color = lights[i].color;
        vec3 ambient_color = 0 * light_color;
        vec3 diffuse_color = texture2D(diffuseColorSampler, uv).xyz;

        float diffuse = abs(dot(normal, lightDir));
        float specular = abs(dot(normal, halfwayDir));

        if (diffuse == 0.0) {
            specular = 0.0;
        } else {
            specular = pow(specular, shininess);
        }

        Color = vec4(ambient_color + diffuse * light_color * diffuse_color + specular * light_color, 1.0);

        int shadow_map_id = lights[i].shadow_map_id;
        float shadow_map_value = texture(shadow_maps, vec3(uv, shadow_map_id)).x;
    }
}