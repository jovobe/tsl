#include <tsl/rendering/picking_map.hpp>

namespace tsl {

uint32_t tsl::picking_map::add_object(object_type type, base_handle<index> handle) {
    map.insert({next, picking_element(type, handle)});
    return next++;
}

}
