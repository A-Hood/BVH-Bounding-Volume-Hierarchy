#ifndef SAT_H
#define SAT_H

#include <valarray>

#include "Colliders/Collider.h"

/// <summary>
/// 
/// Contains logic for seperating-axis-theorem
///
/// </summary>

namespace Physics {
    namespace CollisionDetection {
        static float ProjectVertex(const sf::Vector2f& _axis, const sf::Vector2f& _vert) ;
        inline static float Length(const sf::Vector2f& _vec);
        inline static sf::Vector2f Normalise(const sf::Vector2f& _vec);

        static bool PolygonOnPolygonSATCollision(const PolygonCollider* _poly1, const PolygonCollider* _poly2, float& _returnDepth, sf::Vector2f& _returnNormal) {
            // Get edge along two vertices
            // Get normal, project vertices onto new axis
            // Get min and max of shape vertices in relation to axis
            // Check whether projected shape vertices lies within other projected shapes projected vertices
            // If any do not, exit algo as there is a seperating axis

            float depth = INFINITY;
            sf::Vector2f normal = { 0.0f, 0.0f };

            // Get vertices from both polys
            //auto vertArray1 = _poly1.GetVertices();
            auto vertArray1 = &_poly1->GetVertices();
            auto vertArray2 = &_poly2->GetVertices();

            // Perform checks against both shapes
            for (int shapeNum = 0; shapeNum < 2; shapeNum++) {
                // If already checked the first shape, check the other against the first
                if (shapeNum == 1) {
                    vertArray1 = &_poly2->GetVertices();
                    vertArray2 = &_poly1->GetVertices();
                }

                // Check shape against other
                for (int i = 0; i < static_cast<int>(vertArray1->size()); i++) {
                    int i2 = (i + 1) % static_cast<int>(vertArray1->size()); // Get next vert

                    // Get opposite of normal axis, gives us axis to project onto
                    sf::Vector2f projAxis = { -(vertArray1->at(i2).y - vertArray1->at(i).y), vertArray1->at(i2).x - vertArray1->at(i).x };
                    projAxis = Normalise(projAxis);
                    // Initialise MIN/MAX as infinity so we have a value that will always be below/above our new value
                    
                    // Project all vertices from FIRST poly onto axis
                    float minPoly1 = INFINITY;
                    float maxPoly1 = -INFINITY;
                    for (const auto& vert : *vertArray1) {
                        // Get min and max of poly 1
                        float proj = ProjectVertex(projAxis, vert);

                        minPoly1 = std::min(minPoly1, proj);
                        maxPoly1 = std::max(maxPoly1, proj);
                    }

                    // Project all vertices from SECOND poly onto axis
                    float minPoly2 = INFINITY;
                    float maxPoly2 = -INFINITY;
                    for (const auto& vert : *vertArray2) {
                        // Get min and max of poly 1
                        float proj = ProjectVertex(projAxis, vert);

                        minPoly2 = std::min(minPoly2, proj);
                        maxPoly2 = std::max(maxPoly2, proj);
                    }

                    // Perform the check using the min/max that was calculated for both shapes
                    if (!(maxPoly1 >= minPoly2 && maxPoly2 >= minPoly1)) {
                        return false;
                    }

                    float tempDepth = std::min(maxPoly2 - minPoly1, maxPoly1 - minPoly2);

                    // Check whether this axis' depth is small that current depth
                    if (depth > tempDepth) {
                        depth = tempDepth;
                        normal = projAxis;
                    }
                }
            } 

            // Return these values
            _returnDepth = depth / Length(normal);
            _returnNormal = normal;

            // Make sure that the normal is facing the correct way
            sf::Vector2f dir = _poly1->GetCentre() - _poly2->GetCentre();
            float dotDir = dir.x * normal.x + dir.y * normal.y;
            if (dotDir > 0.0f) {
                _returnNormal = -_returnNormal;
            }

            return true;
        }

        // Return projected vertex onto parameter axis
        inline static float ProjectVertex(const sf::Vector2f& _axis, const sf::Vector2f& _vert) {
            return (_vert.x * _axis.x) + (_vert.y * _axis.y);
        }

        inline static float Length(const sf::Vector2f& _vec) {
            return std::sqrt(_vec.x * _vec.x + _vec.y * _vec.y);
        }
        inline static sf::Vector2f Normalise(const sf::Vector2f& _vec) {
            float temp = 1.0f / Length(_vec);
            return { _vec * temp };
        }
        
    }
}

#endif