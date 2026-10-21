#include "Collision.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

namespace
{
    void ClosestPointsSegmentBox(
        const glm::vec3& start,
        const glm::vec3& end,
        const glm::vec3& halfExtents,
        glm::vec3& segmentPoint,
        glm::vec3& boxPoint)
    {
        glm::vec3 direction = end - start;

        std::vector<float> parameters = { 0.0f, 1.0f };
        parameters.reserve(8);

        for (int axis = 0; axis < 3; ++axis)
        {
            if (direction[axis] == 0.0f)
            {
                continue;
            }

            float first =
                (-halfExtents[axis] - start[axis]) /
                direction[axis];

            float second =
                (halfExtents[axis] - start[axis]) /
                direction[axis];

            if (first > 0.0f && first < 1.0f)
            {
                parameters.push_back(first);
            }

            if (second > 0.0f && second < 1.0f)
            {
                parameters.push_back(second);
            }
        }

        std::sort(parameters.begin(), parameters.end());

        float bestDistanceSquared =
            (std::numeric_limits<float>::max)();

        auto evaluate = [&](float t)
            {
                glm::vec3 pointOnSegment =
                    start + direction * t;

                glm::vec3 pointOnBox = glm::clamp(
                    pointOnSegment,
                    -halfExtents,
                    halfExtents
                );

                glm::vec3 difference =
                    pointOnSegment - pointOnBox;

                float distanceSquared =
                    glm::dot(difference, difference);

                if (distanceSquared < bestDistanceSquared)
                {
                    bestDistanceSquared = distanceSquared;

                    segmentPoint = pointOnSegment;
                    boxPoint = pointOnBox;
                }
            };

        for (float t : parameters)
        {
            evaluate(t);
        }

        for (std::size_t i = 0; i + 1 < parameters.size(); ++i)
        {
            float left = parameters[i];
            float right = parameters[i + 1];

            if (right <= left)
            {
                continue;
            }

            float middle = (left + right) * 0.5f;

            float coefficientA = 0.0f;
            float coefficientB = 0.0f;

            for (int axis = 0; axis < 3; ++axis)
            {
                float coordinate =
                    start[axis] + direction[axis] * middle;

                float boundary = 0.0f;

                if (coordinate < -halfExtents[axis])
                {
                    boundary = -halfExtents[axis];
                }
                else if (coordinate > halfExtents[axis])
                {
                    boundary = halfExtents[axis];
                }
                else
                {
                    continue;
                }

                float offset = start[axis] - boundary;

                coefficientA +=
                    direction[axis] * direction[axis];

                coefficientB +=
                    direction[axis] * offset;
            }

            if (coefficientA > 0.0f)
            {
                float t = -coefficientB / coefficientA;

                evaluate(std::clamp(t, left, right));
            }
        }
    }

    Contact GetInsideContact(
        const glm::vec3& start,
        const glm::vec3& end,
        const glm::vec3& halfExtents,
        float radius)
    {
        glm::vec3 segmentMin = glm::min(start, end);
        glm::vec3 segmentMax = glm::max(start, end);

        Contact candidates[] = {
            {
                glm::vec3(-1.0f, 0.0f, 0.0f),
                segmentMax.x + radius + halfExtents.x
            },
            {
                glm::vec3(1.0f, 0.0f, 0.0f),
                halfExtents.x - segmentMin.x + radius
            },
            {
                glm::vec3(0.0f, -1.0f, 0.0f),
                segmentMax.y + radius + halfExtents.y
            },
            {
                glm::vec3(0.0f, 1.0f, 0.0f),
                halfExtents.y - segmentMin.y + radius
            },
            {
                glm::vec3(0.0f, 0.0f, -1.0f),
                segmentMax.z + radius + halfExtents.z
            },
            {
                glm::vec3(0.0f, 0.0f, 1.0f),
                halfExtents.z - segmentMin.z + radius
            }
        };

        Contact result = candidates[0];

        for (const Contact& candidate : candidates)
        {
            if (candidate.penetration < result.penetration)
            {
                result = candidate;
            }
        }

        return result;
    }
}

bool IntersectCapsuleOBB(
    const CapsuleCollider& capsule,
    const OBB& box,
    Contact& contact)
{
    float halfSegment =
        capsule.height * 0.5f - capsule.radius;

    glm::vec3 worldStart =
        capsule.center -
        glm::vec3(0.0f, halfSegment, 0.0f);

    glm::vec3 worldEnd =
        capsule.center +
        glm::vec3(0.0f, halfSegment, 0.0f);

    // Для ортонормированной матрицы обратная = транспонированная.
    glm::mat3 inverseOrientation =
        glm::transpose(box.orientation);

    glm::vec3 localStart =
        inverseOrientation * (worldStart - box.center);

    glm::vec3 localEnd =
        inverseOrientation * (worldEnd - box.center);

    glm::vec3 segmentPoint;
    glm::vec3 boxPoint;

    ClosestPointsSegmentBox(
        localStart,
        localEnd,
        box.halfExtents,
        segmentPoint,
        boxPoint
    );

    glm::vec3 difference = segmentPoint - boxPoint;

    float distanceSquared =
        glm::dot(difference, difference);

    float radiusSquared =
        capsule.radius * capsule.radius;

    if (distanceSquared >= radiusSquared)
    {
        return false;
    }

    Contact localContact;

    if (distanceSquared > 0.000000000001f)
    {
        float distance = std::sqrt(distanceSquared);

        localContact.normal = difference / distance;

        localContact.penetration =
            capsule.radius - distance;
    }
    else
    {
        localContact = GetInsideContact(
            localStart,
            localEnd,
            box.halfExtents,
            capsule.radius
        );
    }

    contact.normal = glm::normalize(
        box.orientation * localContact.normal
    );

    contact.penetration = localContact.penetration;

    return true;
}