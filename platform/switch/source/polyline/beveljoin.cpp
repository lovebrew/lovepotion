#include "polyline/beveljoin.h"

using namespace love;

void BevelJoinPolyline::RenderEdge(std::vector<Vector2>& anchors, std::vector<Vector2>& normals,
                                   Vector2& s, float& len_s, Vector2& ns, const Vector2& q,
                                   const Vector2& r, float hw)
{
    Vector2 t   = (r - q);
    float len_t = t.getLength();

    float det = Vector2::cross(s, t);
    if (fabs(det) / (len_s * len_t) < Polyline::LINES_PARALLEL_EPS && Vector2::dot(s, t) > 0)
    {
        // lines parallel, compute as u1 = q + ns * w/2, u2 = q - ns * w/2
        Vector2 n = t.getNormal(hw / len_t);
        anchors.push_back(q);
        anchors.push_back(q);
        normals.push_back(n);
        normals.push_back(-n);
        s     = t;
        len_s = len_t;
        return; // early out
    }

    // cramers rule
    Vector2 nt   = t.getNormal(hw / len_t);
    float lambda = Vector2::cross((nt - ns), t) / det;
    Vector2 d    = ns + s * lambda;

    anchors.push_back(q);
    anchors.push_back(q);
    anchors.push_back(q);
    anchors.push_back(q);
    if (det > 0) // 'left' turn -> intersection on the top
    {
        normals.push_back(d);
        normals.push_back(-ns);
        normals.push_back(d);
        normals.push_back(-nt);
    }
    else
    {
        normals.push_back(ns);
        normals.push_back(-d);
        normals.push_back(nt);
        normals.push_back(-d);
    }
    s     = t;
    len_s = len_t;
    ns    = nt;
}
