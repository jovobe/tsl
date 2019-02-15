#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <gtest/gtest.h>

#include <tsl/attrmaps/stable_vector.hpp>
#include <tsl/util/base_handle.hpp>

using namespace tsl;

struct dummy {
    uint32_t val;
};

class test_handle : public base_handle<uint32_t> {
    using base_handle<uint32_t>::base_handle;
};

class StableVectorTest : public ::testing::Test {
protected:
    stable_vector<test_handle, dummy> vector;
};

TEST_F(StableVectorTest, PushCreatesCopy) {
    dummy temp = {42};
    auto handle = vector.push(temp);
    temp.val = 21;

    EXPECT_EQ(42, vector[handle].val);
    EXPECT_EQ(21, temp.val);
}

#pragma clang diagnostic pop
