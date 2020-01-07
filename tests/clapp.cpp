#include <gmock/gmock.h>
#include <cassert>
#include <iterator>
#include <map>
#include <utility>

TEST(DummyTest, Desc) {
    ASSERT_THAT(true, testing::Eq(true));
}
