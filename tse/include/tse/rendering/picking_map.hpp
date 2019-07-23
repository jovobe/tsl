#ifndef TSE_PICKING_BUFFER_HPP
#define TSE_PICKING_BUFFER_HPP

#include <unordered_map>
#include <optional>
#include <utility>
#include <functional>

#include "tsl/util/base_handle.hpp"
#include "tsl/geometry/tmesh/handles.hpp"

using std::unordered_map;
using std::pair;
using std::reference_wrapper;
using std::optional;

using tsl::base_handle;

namespace tse {

/**
 * @brief Represents the type of a picked object.
 */
enum class object_type {
    vertex,
    edge,
    face
};

/**
 * @brief POD type to represent an element which was picked inside the editor.
 */
struct picking_element {
    /// Type of the picked element.
    object_type type;
    /// Handle of the picked element.
    base_handle<tsl::index> handle;

    picking_element(object_type type, base_handle<tsl::index> handle) : type(type), handle(handle) {}
    bool operator<(const picking_element& r) const;
};

/**
 * @brief A map, which maps a unique id to a picked element. All elements in a scene will be given a unique id, to be
 * identified later by this id.
 */
class picking_map {
private:
    /// Id of the next element which will be added.
    uint32_t next;
    /// Map to store relation between id and elements.
    unordered_map<uint32_t, picking_element> map;
public:
    picking_map() : next(1) {}

    /**
     * @brief Adds an object to the picking map with the given type and the given handle and returns the id of the
     * added element.
     */
    uint32_t add_object(object_type type, base_handle<tsl::index> handle);

    /**
     * @brief Returns a picking element from the map, identified by the given id.
     */
    optional<picking_element> get_object(uint32_t idx) const;

    /**
     * @brief Clears the map and removes all entries.
     */
    void clear();
};

}

#endif //TSE_PICKING_BUFFER_HPP
