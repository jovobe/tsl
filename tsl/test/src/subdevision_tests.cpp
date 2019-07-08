#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-goto"
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <gtest/gtest.h>

#include <tsl/evaluation/subdevision.hpp>

using namespace tsl;

namespace tsl_tests {

class EigenCacheTest : public ::testing::Test {
protected:
    EigenCacheTest() : cache() {}
    eigen_cache cache;
};

TEST_F(EigenCacheTest, Get) {
    // Check values for valence 3.
    uint32_t valence3 = 3;
    eigen_handle handle3(valence3);
    auto& eigen3 = cache.get(handle3);
    EXPECT_EQ(valence3, eigen3.N_);
    EXPECT_EQ(2 * valence3, eigen3.twoN_);
    EXPECT_EQ(2 * valence3 + 8, eigen3.K_);
    EXPECT_EQ(2 * valence3 + 8 + 9, eigen3.M_);
    EXPECT_TRUE(eigen3.loaded_);
}

TEST_F(EigenCacheTest, Cached) {
    // TODO: Test if the eigen_cache really caches the elements.
}

#ifndef NDEBUG
TEST_F(EigenCacheTest, InvalidValence) {
    EXPECT_THROW(cache.get(eigen_handle(0)), panic_exception);
    EXPECT_THROW(cache.get(eigen_handle(1)), panic_exception);
    EXPECT_THROW(cache.get(eigen_handle(2)), panic_exception);
    EXPECT_THROW(cache.get(eigen_handle(MAX_VALENCE + 1)), panic_exception);
}
#endif

}
