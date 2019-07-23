#ifndef TSL_EDGE_DIRECTION_HPP
#define TSL_EDGE_DIRECTION_HPP

namespace tsl {

/**
 * @brief The direction of an edge with respect to a vertex.
 */
enum class edge_direction {
    /// The edge is pointing to the vertex.
    ingoing,

    /// The edge is poiting away from the vertex.
    outgoing
};

}

#endif //TSL_EDGE_DIRECTION_HPP
