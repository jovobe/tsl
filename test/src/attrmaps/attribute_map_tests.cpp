#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-goto"
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <memory>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tsl/attrmaps/attribute_map.hpp>
#include <tsl/attrmaps/vector_map.hpp>
#include <tsl/attrmaps/hash_map.hpp>
#include <tsl/util/base_handle.hpp>

using std::make_unique;
using std::unique_ptr;

using namespace tsl;

class test_handle : public base_handle<uint32_t> {
    using base_handle<uint32_t>::base_handle;
};

namespace std {

template<>
struct hash<test_handle> {
    size_t operator()(const test_handle& h) const {
        return hash<tsl::index>()(h.get_idx());
    }
};

}

struct dummy {
    uint32_t val = 0;

    bool operator==(const dummy& other) const {
        return val == other.val;
    }

    bool operator!=(const dummy& other) const {
        return val != other.val;
    }
};

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

using Implementations = ::testing::Types<vector_map<test_handle, dummy>, hash_map<test_handle, dummy>>;

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

TYPED_TEST_CASE(AttributeMapTest, Implementations);

TYPED_TEST(AttributeMapTest, WholeInterface) {
    dummy temp = {42};
    test_handle handle(0);
    attribute_map<test_handle, dummy>& map = *(this->map);

    EXPECT_FALSE(map.contains_key(handle));
    EXPECT_EQ(0, map.num_values());
    EXPECT_THROW(map[handle], panic_exception);
    EXPECT_EQ(nullopt, map.get(handle));

    EXPECT_EQ(nullopt, map.insert(handle, temp));
    EXPECT_EQ(42, (*map.insert(handle, temp)).val);

    EXPECT_EQ(1, map.num_values());
    EXPECT_TRUE(map.contains_key(handle));

    EXPECT_EQ(temp, map[handle]);
    map[handle].val = 21;
    EXPECT_EQ(21, map[handle].val);

    {
        const auto& const_map = map;
        EXPECT_EQ(21, const_map[handle].val);
    }

    test_handle handle2(1);
    EXPECT_EQ(nullopt, map.insert(handle2, temp));
    EXPECT_EQ(2, map.num_values());
    EXPECT_TRUE(map.contains_key(handle2));

    EXPECT_EQ(42, (*map.erase(handle2)).val);
    EXPECT_EQ(1, map.num_values());
    EXPECT_FALSE(map.contains_key(handle2));

    map.clear();
    EXPECT_FALSE(map.contains_key(handle));
    EXPECT_EQ(0, map.num_values());
}

TYPED_TEST(AttributeMapTest, Iterator) {
    vector<dummy> dummies;
    dummies.insert(dummies.begin(), {dummy{42}, dummy{21}, dummy{7}});
    vector<test_handle> handles;
    handles.insert(handles.begin(), {test_handle(0), test_handle(1), test_handle(2)});
    attribute_map<test_handle, dummy>& map = *(this->map);

    map.insert(handles[0], dummies[0]);
    map.insert(handles[1], dummies[1]);
    map.insert(handles[2], dummies[2]);

    vector<test_handle> expected_handles;
    vector<dummy> expected_dummies;
    for (auto&& h: map) {
        expected_handles.push_back(h);
        expected_dummies.push_back(map[h]);
    }

    ASSERT_THAT(handles, ::testing::UnorderedElementsAreArray(expected_handles));
    ASSERT_THAT(dummies, ::testing::UnorderedElementsAreArray(expected_dummies));
}

TYPED_TEST_CASE(AttributeMapWithDefaultTest, Implementations);

TYPED_TEST(AttributeMapWithDefaultTest, WholeInterface) {
    dummy temp = {42};
    test_handle handle(0);
    attribute_map<test_handle, dummy>& map = *(this->map);

    EXPECT_FALSE(map.contains_key(handle));
    EXPECT_EQ(0, map.num_values());

    {
        // Immutable get called -> default value should NOT be inserted
        const auto& const_map = map;
        EXPECT_EQ(0, const_map[handle].val);

        EXPECT_FALSE(map.contains_key(handle));
        EXPECT_EQ(0, map.num_values());
    }

    // Mutable get called -> default value should be inserted now
    EXPECT_EQ(0, map[handle].val);
    EXPECT_TRUE(map.contains_key(handle));
    EXPECT_EQ(1, map.num_values());

    map[handle].val = 21;
    EXPECT_EQ(21, map[handle].val);

    test_handle handle2(1);
    EXPECT_EQ(nullopt, map.insert(handle2, temp));
    EXPECT_EQ(2, map.num_values());
    EXPECT_TRUE(map.contains_key(handle2));
    EXPECT_EQ(42, map[handle2].val);

    EXPECT_EQ(42, (*map.erase(handle2)).val);
    EXPECT_EQ(1, map.num_values());
    EXPECT_FALSE(map.contains_key(handle2));

    map.clear();
    EXPECT_FALSE(map.contains_key(handle));
    EXPECT_EQ(0, map.num_values());
}

TYPED_TEST(AttributeMapWithDefaultTest, Iterator) {
    vector<dummy> dummies;
    dummies.insert(dummies.begin(), {dummy{42}, dummy{21}, dummy{7}});
    vector<test_handle> handles;
    handles.insert(handles.begin(), {test_handle(0), test_handle(1), test_handle(2)});
    attribute_map<test_handle, dummy>& map = *(this->map);

    map.insert(handles[0], dummies[0]);
    map.insert(handles[1], dummies[1]);
    map.insert(handles[2], dummies[2]);

    vector<test_handle> expected_handles;
    vector<dummy> expected_dummies;
    for (auto&& h: map) {
        expected_handles.push_back(h);
        expected_dummies.push_back(map[h]);
    }

    ASSERT_THAT(handles, ::testing::UnorderedElementsAreArray(expected_handles));
    ASSERT_THAT(dummies, ::testing::UnorderedElementsAreArray(expected_dummies));
}

#pragma clang diagnostic pop
