#ifndef TSL_PICKING_BUFFER_HPP
#define TSL_PICKING_BUFFER_HPP

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
    bool operator<(const picking_element& r) const;
};

class picking_map {
private:
    uint32_t next;
    unordered_map<uint32_t, picking_element> map;
public:
    picking_map() : next(1) {}
    uint32_t add_object(object_type type, base_handle<index> handle);
    optional<picking_element> get_object(uint32_t idx) const;
    void clear();
};

}

#endif //TSL_PICKING_BUFFER_HPP
