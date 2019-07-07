#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-goto"
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <memory>
#include <vector>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "tsl/attrmaps/attribute_map.hpp"
#include "tsl/attrmaps/vector_map.hpp"
#include "tsl/attrmaps/hash_map.hpp"

#include "tsl_tests/mocks.hpp"

using std::make_unique;
using std::unique_ptr;
using std::vector;

using namespace tsl;

namespace tsl_tests {

template<class T>
unique_ptr<attribute_map<test_handle, dummy>> CreateMap(bool with_default);

template<>
unique_ptr<attribute_map<test_handle, dummy>> CreateMap<vector_map<test_handle, dummy>>(bool with_default) {
    if (with_default) {
        return make_unique<vector_map<test_handle, dummy>>(dummy());
    } else {
        return make_unique<vector_map<test_handle, dummy>>();
    }
}

template<>
unique_ptr<attribute_map<test_handle, dummy>> CreateMap<hash_map<test_handle, dummy>>(bool with_default) {
    if (with_default) {
        return make_unique<hash_map<test_handle, dummy>>(dummy());
    } else {
        return make_unique<hash_map<test_handle, dummy>>();
    }
}

template<class T>
unique_ptr<attribute_map<test_handle, vector<dummy>>> CreateMapVector();

template<>
unique_ptr<attribute_map<test_handle, vector<dummy>>> CreateMapVector<vector_map<test_handle, vector<dummy>>>() {
    return make_unique<vector_map<test_handle, vector<dummy>>>(vector<dummy>());
}

template<>
unique_ptr<attribute_map<test_handle, vector<dummy>>> CreateMapVector<hash_map<test_handle, vector<dummy>>>() {
    return make_unique<hash_map<test_handle, vector<dummy>>>(vector<dummy>());
}

using Implementations = ::testing::Types<vector_map<test_handle, dummy>, hash_map<test_handle, dummy>>;
using ImplementationsVector = ::testing::Types<vector_map<test_handle, vector<dummy>>, hash_map<test_handle, vector<dummy>>>;

template<class T>
class AttributeMapTest : public ::testing::Test {
protected:
    AttributeMapTest() : map(CreateMap<T>(false)) {}
    const unique_ptr<attribute_map<test_handle, dummy>> map;
};

template<class T>
class AttributeMapWithDefaultTest : public ::testing::Test {
protected:
    AttributeMapWithDefaultTest() : map(CreateMap<T>(true)) {}
    const unique_ptr<attribute_map<test_handle, dummy>> map;
};

template<class T>
class AttributeMapWithDefaultAsVectorTest : public ::testing::Test {
protected:
    AttributeMapWithDefaultAsVectorTest() : map(CreateMapVector<T>()) {}
    const unique_ptr<attribute_map<test_handle, vector<dummy>>> map;
};

TYPED_TEST_CASE(AttributeMapTest, Implementations);

TYPED_TEST(AttributeMapTest, WholeInterface) {
    dummy temp = {42};
    test_handle handle(0);
    attribute_map<test_handle, dummy>& map = *(this->map);

    // Test empty map
    EXPECT_FALSE(map.contains_key(handle));
    EXPECT_EQ(0, map.num_values());
    EXPECT_THROW(map[handle], panic_exception);
    EXPECT_EQ(nullopt, map.get(handle));

    // Test insert
    EXPECT_EQ(nullopt, map.insert(handle, temp));
    EXPECT_EQ(temp, (*map.insert(handle, temp)));
    EXPECT_EQ(1, map.num_values());
    EXPECT_TRUE(map.contains_key(handle));

    // Test ref
    EXPECT_EQ(temp, map[handle]);
    map[handle].val = 21;
    EXPECT_EQ(21, map[handle].val);

    // Test const ref
    {
        const auto& const_map = map;
        EXPECT_EQ(21, const_map[handle].val);
    }

    // Test multiple elements
    test_handle handle2(1);
    EXPECT_EQ(nullopt, map.insert(handle2, temp));
    EXPECT_EQ(2, map.num_values());
    EXPECT_TRUE(map.contains_key(handle2));

    // Test remove
    EXPECT_EQ(temp, (*map.erase(handle2)));
    EXPECT_EQ(1, map.num_values());
    EXPECT_FALSE(map.contains_key(handle2));

    // Test clear
    map.clear();
    EXPECT_FALSE(map.contains_key(handle));
    EXPECT_EQ(0, map.num_values());
}

TYPED_TEST(AttributeMapTest, Iterator) {
    // Prepare test data
    vector<dummy> dummies;
    dummies.insert(dummies.begin(), {dummy{42}, dummy{21}, dummy{7}});
    vector<test_handle> handles;
    handles.insert(handles.begin(), {test_handle(0), test_handle(1), test_handle(2)});
    attribute_map<test_handle, dummy>& map = *(this->map);

    // Insert data into map
    map.insert(handles[0], dummies[0]);
    map.insert(handles[1], dummies[1]);
    map.insert(handles[2], dummies[2]);

    // Extract data from map
    vector<test_handle> expected_handles;
    vector<dummy> expected_dummies;
    for (const auto& h: map) {
        expected_handles.push_back(h);
        expected_dummies.push_back(map[h]);
    }

    ASSERT_THAT(handles, ::testing::UnorderedElementsAreArray(expected_handles));
    ASSERT_THAT(dummies, ::testing::UnorderedElementsAreArray(expected_dummies));
}

TYPED_TEST_CASE(AttributeMapWithDefaultTest, Implementations);

TYPED_TEST(AttributeMapWithDefaultTest, WholeInterface) {
    dummy default_val = {0};
    dummy temp = {42};
    test_handle handle(0);
    attribute_map<test_handle, dummy>& map = *(this->map);

    // Test empty map
    EXPECT_FALSE(map.contains_key(handle));
    EXPECT_EQ(0, map.num_values());

    {
        // Immutable get called -> default value should NOT be inserted
        const auto& const_map = map;
        EXPECT_EQ(default_val, const_map[handle]);

        EXPECT_FALSE(map.contains_key(handle));
        EXPECT_EQ(0, map.num_values());
    }

    // Mutable get called -> default value should be inserted now
    EXPECT_EQ(default_val, map[handle]);
    EXPECT_TRUE(map.contains_key(handle));
    EXPECT_EQ(1, map.num_values());

    map[handle].val = 21;
    EXPECT_EQ(21, map[handle].val);

    test_handle handle2(1);
    EXPECT_EQ(nullopt, map.insert(handle2, temp));
    EXPECT_EQ(2, map.num_values());
    EXPECT_TRUE(map.contains_key(handle2));
    EXPECT_EQ(temp, map[handle2]);

    EXPECT_EQ(temp, (*map.erase(handle2)));
    EXPECT_EQ(1, map.num_values());
    EXPECT_FALSE(map.contains_key(handle2));

    map.clear();
    EXPECT_FALSE(map.contains_key(handle));
    EXPECT_EQ(0, map.num_values());
}

TYPED_TEST(AttributeMapWithDefaultTest, Iterator) {
    // Prepare test data
    vector<dummy> dummies;
    dummies.insert(dummies.begin(), {dummy{42}, dummy{21}, dummy{7}});
    vector<test_handle> handles;
    handles.insert(handles.begin(), {test_handle(0), test_handle(1), test_handle(2)});
    attribute_map<test_handle, dummy>& map = *(this->map);

    // Insert data into map
    map.insert(handles[0], dummies[0]);
    map.insert(handles[1], dummies[1]);
    map.insert(handles[2], dummies[2]);

    // Extract data from map
    vector<test_handle> expected_handles;
    vector<dummy> expected_dummies;
    for (const auto& h: map) {
        expected_handles.push_back(h);
        expected_dummies.push_back(map[h]);
    }

    ASSERT_THAT(handles, ::testing::UnorderedElementsAreArray(expected_handles));
    ASSERT_THAT(dummies, ::testing::UnorderedElementsAreArray(expected_dummies));
}

TYPED_TEST_CASE(AttributeMapWithDefaultAsVectorTest, ImplementationsVector);

TYPED_TEST(AttributeMapWithDefaultAsVectorTest, Default) {
    auto& map = *(this->map);
    test_handle h0(0);
    test_handle h1(1);
    ASSERT_EQ(0, map[h0].size());
    map[h0].emplace_back();
    ASSERT_EQ(1, map[h0].size());
    ASSERT_EQ(0, map[h1].size());
}

}

#pragma clang diagnostic pop
