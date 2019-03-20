#ifndef TSL_PICKING_BUFFER_HPP
#define TSL_PICKING_BUFFER_HPP

#include <unordered_map>
#include <optional>
#include <utility>

#include <tsl/util/base_handle.hpp>
#include <tsl/geometry/handles.hpp>

using std::unordered_map;
using std::pair;

namespace tsl {

enum class object_type {
    vertex,
    edge,
    face
};

struct picking_element {
    object_type type;
    base_handle<index> handle;

    picking_element(object_type type, base_handle<index> handle) : type(type), handle(handle) {}
};

class picking_map {
private:
    uint32_t next;
    unordered_map<uint32_t, picking_element> map;
public:
    picking_map() : next(0) {}
    uint32_t add_object(object_type type, base_handle<index> handle);
};

}

#endif //TSL_PICKING_BUFFER_HPP
