#include <glm/glm.hpp>
#include <math.h>
#include "material.h"
#include <glm/gtx/normal.hpp>
#include "../shapes/shape2D.h"
#include "../light/ray.h"
#include "../light/lightsource.h"
using glm::vec3;
using glm::vec4;
using glm::mix;
using glm::clamp;

#ifndef SMOKE_H
#define SMOKE_H

class Smoke : public Material{
public:
    const vec3 base_color;

    Smoke(vec3 base_color): base_color(base_color), Material("Smoke") 
    {
    };


    virtual glm::vec3 material_color(   Intersection& intersection, 
                                        const Ray& primary_ray, 
                                        const std::vector<Shape2D*>& shapes, 
                                        LightSource* lightSource,
                                        vec3 directLight,
                                        vec3 indirectLight)  override 
    {

        Ray outgoing = Ray(intersection.position + vec4(intersection.shape2D->getnormal(intersection) * -0.001f, 1), intersection.direction, primary_ray.bounces + 1);
        Intersection behindObjectIntersection;
        bool isObjectBehind = outgoing.ClosestIntersection(shapes, behindObjectIntersection, "Smoke" );
        
        vec3 behindObjectColor = vec3(0,0,0);
        if(isObjectBehind){
            behindObjectColor = behindObjectIntersection.shape2D->getcolor(behindObjectIntersection, outgoing, shapes, lightSource);
        }
         
        Intersection smokeExitIntersection;
        bool smokeExit = outgoing.ClosestIntersection(shapes, smokeExitIntersection);
        float smoke_dist = glm::length(intersection.position - smokeExitIntersection.position);
        float t = clamp(pow(smoke_dist, 3) * 5, 0.0, 1.0);

        vec3 col = glm::mix(behindObjectColor, base_color, t);

        vec3 light = lightSource->lightAtPosition(intersection, shapes);
        float t_obj = clamp(pow(smoke_dist, 3) * 5, 0.0, 1.0);

        vec3 m = vec3(1,1,1 ) - (3.0f * (t_obj * light));

        return mix(vec3(0.8, .8, .8), col, m);
    } 
    

    virtual float transparency() const {
        return 0.8f;
    }
};

#endif
