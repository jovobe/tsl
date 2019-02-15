#ifndef TSL_HANDLES_HPP
#define TSL_HANDLES_HPP

namespace tsl {

/**
 * @brief Datatype used as index for each vertex, face and edge.
 *
 * This index is used within {edge, face, vertex}_handles. Since those
 * handles are also used within each {edge, face, vertex} reducing the
 * size of this type can greatly decrease memory usage, which in
 * turn might increase performance due to cache locality.
 *
 * When we assume a basic half-edge structure, we have to deal with the
 * following struct sizes:
 * - edge: 4 handles
 * - face: 1 handle + 1 vector
 * - vertex: 1 handle + 1 vector
 *
 * Assuming the most common case of `float` vectors, this results in the
 * following sizes (in bytes):
 * - 16 bit handles: edge (8), face (14), vertex (14)
 * - 32 bit handles: edge (16), face (16), vertex (16)
 * - 64 bit handles: edge (32), face (20), vertex (20)
 *
 * Using another approximation of the number of faces, edges and vertices
 * in a triangle-mesh described at [1], we can calculate how much RAM we
 * would need in order to run out of handles. The approximation: for each
 * vertex, we have three edges and two faces. The de-facto cost per vertex
 * can be calculated from that resulting in
 *
 * - 16 bit handles: 14 + 2*14 + 3*8 = 66 bytes/vertex = 22 bytes/edge
 *   ==> 22 * 2^16 = 1.4 MiB RAM necessary to exhaust handle space
 *
 * - 32 bit handles: 16 + 2*16 + 3*16 = 96 bytes/vertex = 32 bytes/edge
 *   ==> 32 * 2^32 = 137 GiB RAM necessary to exhaust handle space
 *
 * - 64 bit handles: 20 + 2*20 + 3*32 = 156 bytes/vertex = 52 bytes/edge
 *   ==> 52 * 2^64 = 1.1 ZiB RAM necessary to exhaust handle space
 *       (it's called zetta or zebi and is ≈ 1 million tera bytes)
 *   ==> Note: funnily enough, the estimated disk (not RAM!) capacity of
 *       the whole  world (around 2015) comes very close to this number.
 *
 *
 * Also note that this accounts for the mesh only and ignores all other
 * data that might need to be stored in RAM. So you will need even more
 * RAM.
 *
 * From this, I think, we can safely conclude: 16 bit handles are way too
 * small; 32 bit handles are probably fine for the next few years, even
 * when working on a medium-sized cluster and 64 bit handles will be fine
 * until after the singularity. And by then, I probably don't care anymore.
 *
 * [1]: https://math.stackexchange.com/q/425968/340615
 */
using index = uint32_t;

}

#endif //TSL_HANDLES_HPP
