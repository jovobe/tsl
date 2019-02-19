#ifndef TSL_TESTS_MOCKS_HPP
#define TSL_TESTS_MOCKS_HPP

#include <tsl/util/base_handle.hpp>
#include <tsl/geometry/handles.hpp>

using namespace tsl;

namespace tsl_tests {

class test_handle : public base_handle<uint32_t> {
    using base_handle<uint32_t>::base_handle;
};

struct dummy {
    uint32_t val = 0;

    bool operator==(const dummy& other) const {
        return val == other.val;
    }

    bool operator!=(const dummy& other) const {
        return val != other.val;
    }
};

}

namespace std {

template<>
struct hash<tsl_tests::test_handle> {
    size_t operator()(const tsl_tests::test_handle& h) const {
        return hash<tsl::index>()(h.get_idx());
    }
};

}

#endif //TSL_TESTS_MOCKS_HPP
