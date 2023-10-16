#include <utilities/driver/renderer/polyline/types/beveljoin.hpp>

using namespace love;

/** Calculate line boundary points.
 *
 * Sketch:
 *
 *     uh1___uh2
 *      .'   '.
 *    .'   q   '.
 *  .'   '   '   '.
 *.'   '  .'.  '   '.
 *   '  .' ul'.  '
 * p  .'       '.  r
 *
 *
 * ul can be found as above, uh1 and uh2 are much simpler:
 *
 * uh1 = q + ns * w/2, uh2 = q + nt * w/2
 */
void BevelJoinPolyline::renderEdge(std::vector<Vector2>& anchors, std::vector<Vector2>& normals,
                                   Vector2& segment, float& segmentLength, Vector2& segmentNormal,
                                   const Vector2& pointA, const Vector2& pointB, float halfWidth)
{
    Vector2 newSegment     = (pointB - pointA);
    float newSegmentLength = newSegment.getLength();

    float det = Vector2::cross(segment, newSegment);
    if (fabs(det) / (segmentLength * newSegmentLength) < LINES_PARALLEL_EPS)
    {
        // lines parallel, compute as u1 = q + ns * w/2, u2 = q - ns * w/2
        Vector2 newSegmentNormal = newSegment.getNormal(halfWidth / newSegmentLength);
        anchors.push_back(pointA);
        anchors.push_back(pointA);
        normals.push_back(segmentNormal);
        normals.push_back(-segmentNormal);

        if (Vector2::dot(segment, newSegment) < 0)
        {
            // line reverses direction; same as for miter
            anchors.push_back(pointA);
            anchors.push_back(pointA);
            normals.push_back(-segmentNormal);
            normals.push_back(segmentNormal);
        }

        segment       = newSegment;
        segmentLength = newSegmentLength;
        segmentNormal = newSegmentNormal;
        return; // early out
    }

    // cramers rule
    Vector2 newSegmentNormal = newSegment.getNormal(halfWidth / newSegmentLength);
    float lambda             = Vector2::cross((newSegmentNormal - segmentNormal), newSegment) / det;
    Vector2 d                = segmentNormal + segment * lambda;

    anchors.push_back(pointA);
    anchors.push_back(pointA);
    anchors.push_back(pointA);
    anchors.push_back(pointA);
    if (det > 0) // 'left' turn -> intersection on the top
    {
        normals.push_back(d);
        normals.push_back(-segmentNormal);
        normals.push_back(d);
        normals.push_back(-newSegmentNormal);
    }
    else
    {
        normals.push_back(segmentNormal);
        normals.push_back(-d);
        normals.push_back(newSegmentNormal);
        normals.push_back(-d);
    }
    segment       = newSegment;
    segmentLength = newSegmentLength;
    segmentNormal = newSegmentNormal;
}
