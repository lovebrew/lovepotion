#include "polyline/miterjoin.h"

using namespace love;

void MiterJoinPolyline::RenderEdge(std::vector<Vector2>& anchors, std::vector<Vector2>& normals,
                                   Vector2& s, float& len_s, Vector2& ns, const Vector2& q,
                                   const Vector2& r, float hw)
{
    Vector2 t   = (r - q);
    float len_t = t.getLength();
    Vector2 nt  = t.getNormal(hw / len_t);

    anchors.push_back(q);
    anchors.push_back(q);

    float det = Vector2::cross(s, t);
    if (fabs(det) / (len_s * len_t) < Polyline::LINES_PARALLEL_EPS && Vector2::dot(s, t) > 0)
    {
        // lines parallel, compute as u1 = q + ns * w/2, u2 = q - ns * w/2
        normals.push_back(ns);
        normals.push_back(-ns);
    }
    else
    {
        // cramers rule
        float lambda = Vector2::cross((nt - ns), t) / det;
        Vector2 d    = ns + s * lambda;
        normals.push_back(d);
        normals.push_back(-d);
    }

    s     = t;
    ns    = nt;
    len_s = len_t;
}
