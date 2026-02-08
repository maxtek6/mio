#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "mio.hpp"

TEST_CASE("page_size stablilty cross calls") {
    size_t ps1 = mio::page_size();
    size_t ps2 = mio::page_size();

    CHECK(ps1 == ps2);
}

TEST_CASE("page_size returns positive non-zero value") {
    size_t ps = mio::page_size();
    CHECK(ps > 0);
}

TEST_CASE("make_offset_page_aligned aligns correctly") {

    size_t ps = mio::page_size();
    CHECK(mio::make_offset_page_aligned(ps) == ps);
    CHECK(mio::make_offset_page_aligned(ps + 123) == ps);
    CHECK(mio::make_offset_page_aligned(2 * ps + 999) == 2 * ps);
}
