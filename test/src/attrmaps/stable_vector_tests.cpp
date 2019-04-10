#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <gtest/gtest.h>

#include <tsl/attrmaps/stable_vector.hpp>
#include <tsl/util/base_handle.hpp>

#include <tsl_tests/mocks.hpp>

using namespace tsl;

namespace tsl_tests {

class StableVectorTest : public ::testing::Test {
protected:
    stable_vector<test_handle, dummy> vector;
};

TEST_F(StableVectorTest, PushCreatesCopy) {
    dummy temp = {42};
    auto handle = vector.push(temp);
    temp.val = 21;

    // Test copy
    EXPECT_EQ(42, vector[handle].val);
    EXPECT_EQ(21, temp.val);
}

TEST(IndexOperatorTest, AssignCreatesCopy) {
    dummy temp = {42};
    dummy temp2 = {21};
    stable_vector<test_handle, dummy> vector(10, temp);

    // Test if the default value is changable.
    test_handle handle(0);
    auto& val = (*vector.get(handle)).get();
    EXPECT_EQ(42, val.val);
    val.val = 41;
    EXPECT_EQ(41, vector[handle].val);

    // Test if the index operator in combination with value assignment creates a copy of the value.
    test_handle handle1(1);
    EXPECT_EQ(42, vector[handle1].val);
    auto& new_val = (vector[handle1] = temp2);
    EXPECT_EQ(21, vector[handle1].val);
    temp2.val = 20;
    EXPECT_EQ(20, temp2.val);
    EXPECT_EQ(21, vector[handle1].val);
    EXPECT_EQ(21, new_val.val);

    new_val.val = 19;
    EXPECT_EQ(19, vector[handle1].val);
}

}

#pragma clang diagnostic pop
