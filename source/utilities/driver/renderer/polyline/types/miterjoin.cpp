#include <utilities/driver/renderer/polyline/types/miterjoin.hpp>

using namespace love;

/** Calculate line boundary points.
 *
 * Sketch:
 *
 *              u1
 * -------------+---...___
 *              |         ```'''--  ---
 * p- - - - - - q- - . _ _           | w/2
 *              |          ` ' ' r   +
 * -------------+---...___           | w/2
 *              u2         ```'''-- ---
 *
 * u1 and u2 depend on four things:
 *   - the half line width w/2
 *   - the previous line vertex p
 *   - the current line vertex q
 *   - the next line vertex r
 *
 * u1/u2 are the intersection points of the parallel lines to p-q and q-r,
 * i.e. the point where
 *
 *    (q + w/2 * ns) + lambda * (q - p) = (q + w/2 * nt) + mu * (r - q)   (u1)
 *    (q - w/2 * ns) + lambda * (q - p) = (q - w/2 * nt) + mu * (r - q)   (u2)
 *
 * with ns,nt being the normals on the segments s = p-q and t = q-r,
 *
 *    ns = perp(s) / |s|
 *    nt = perp(t) / |t|.
 *
 * Using the linear equation system (similar for u2)
 *
 *         q + w/2 * ns + lambda * s - (q + w/2 * nt + mu * t) = 0                 (u1)
 *    <=>  q-q + lambda * s - mu * t                          = (nt - ns) * w/2
 *    <=>  lambda * s   - mu * t                              = (nt - ns) * w/2
 *
 * the intersection points can be efficiently calculated using Cramer's rule.
 */
void MiterJoinPolyline::renderEdge(std::vector<Vector2>& anchors, std::vector<Vector2>& normals,
                                   Vector2& segment, float& segmentLength, Vector2& segmentNormal,
                                   const Vector2& pointA, const Vector2& pointB, float halfwidth)
{
    Vector2 newSegment     = (pointB - pointA);
    float newSegmentLength = newSegment.getLength();
    if (newSegmentLength == 0.0f)
    {
        // degenerate segment, skip it
        return;
    }

    Vector2 newSegmentNormal = newSegment.getNormal(halfwidth / newSegmentLength);

    anchors.push_back(pointA);
    anchors.push_back(pointA);

    float det = Vector2::cross(segment, newSegment);
    if (fabs(det) / (segmentLength * newSegmentLength) < LINES_PARALLEL_EPS)
    {
        // lines parallel, compute as u1 = q + ns * w/2, u2 = q - ns * w/2
        normals.push_back(segmentNormal);
        normals.push_back(-segmentNormal);

        if (Vector2::dot(segment, newSegment) < 0)
        {
            // line reverses direction; because the normal flips, the
            // triangle strip would twist here, so insert a zero-size
            // quad to contain the twist
            //  ____.___.____
            // |    |\ /|    |
            // p    q X q    r
            // |____|/ \|____|
            anchors.push_back(pointA);
            anchors.push_back(pointA);
            normals.push_back(-segmentNormal);
            normals.push_back(segmentNormal);
        }
    }
    else
    {
        // cramers rule
        float lambda = Vector2::cross((newSegmentNormal - segmentNormal), newSegment) / det;
        Vector2 d    = segmentNormal + segment * lambda;
        normals.push_back(d);
        normals.push_back(-d);
    }

    segment       = newSegment;
    segmentNormal = newSegmentNormal;
    segmentLength = newSegmentLength;
}
