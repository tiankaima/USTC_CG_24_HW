#include "path.h"

#include <random>

#include "surfaceInteraction.h"
#include "utils/sampling.hpp"
USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;

VtValue PathIntegrator::Li(const GfRay& ray, std::default_random_engine& random)
{
    std::uniform_real_distribution<float> uniform_dist(
        0.0f, 1.0f - std::numeric_limits<float>::epsilon());
    std::function<float()> uniform_float = std::bind(uniform_dist, random);

    auto color = EstimateOutGoingRadiance(ray, uniform_float, 0);

    return VtValue(GfVec3f(color[0], color[1], color[2]));
}

GfVec3f PathIntegrator::EstimateOutGoingRadiance(
    const GfRay& ray,
    const std::function<float()>& uniform_float,
    int recursion_depth)
{
    if (recursion_depth >= 50) {
        return {};
    }

    SurfaceInteraction si;
    if (!Intersect(ray, si)) {
        if (recursion_depth == 0) {
            return IntersectDomeLight(ray);
        }

        return GfVec3f{ 0, 0, 0 };
    }

    // This can be customized : Do we want to see the lights? (Other than dome lights?)
    if (recursion_depth == 0) {
    }

    // Flip the normal if opposite
    if (GfDot(si.shadingNormal, ray.GetDirection()) > 0) {
        si.flipNormal();
        si.PrepareTransforms();
    }

    GfVec3f color{ 0 };
    GfVec3f directLight = EstimateDirectLight(si, uniform_float);

    // Russian roulette：
    float p = 0.8f;
    float q = 1 - p;
    float r = uniform_float();
    if (r < p) {
        return directLight / p;
    }
    else {
        const unsigned spp = (2 << 9);

        for (int i = 0; i < spp; i++) {
            float pdf;
            GfVec3f shadowDir = si.TangentToWorld(
                CosineWeightedDirection(GfVec2f{ uniform_float(), uniform_float() }, pdf));
            GfRay shadow_ray;
            shadow_ray.SetPointAndDirection(si.position + 0.00001f * si.geometricNormal, shadowDir);

            if (VisibilityTest(shadow_ray)) {
                GfVec3f newColor =
                    EstimateOutGoingRadiance(shadow_ray, uniform_float, recursion_depth + 1);
                color += GfDot(shadowDir, si.shadingNormal) / pdf * newColor;
            }
        }

        color /= spp;
        return color / q + directLight;
    }
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
