#include <optional>

#include <tsl/rendering/picking_map.hpp>

using std::nullopt;

namespace tsl {

uint32_t tsl::picking_map::add_object(object_type type, base_handle<index> handle) {
    map.insert({next, picking_element(type, handle)});
    return next++;
}

optional<reference_wrapper<const picking_element>> picking_map::get_object(uint32_t idx) const {
    if (map.count(idx) != 0) {
        return map.at(idx);
    } else {
        return nullopt;
    }
}

}
