#include "deko3d/vertex.h"


// int vertex::GetIndexCount(DkPrimitive mode, int vertexCount)
// {
//     switch (mode)
//     {
//         case DkPrimitive_TriangleStrip:
//         case DkPrimitive_TriangleFan:
//             return 3 * (vertexCount - 2);
//         case DkPrimitive_Quads:
//             return vertexCount * 6 / 4;
//         default:
//             break;
//     }

//     return 0;
// }