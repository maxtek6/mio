#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "mio.hpp"

TEST_CASE("page_size returns consistent values") {
    size_t ps1 = mio::page_size();
    size_t ps2 = mio::page_size();
    CHECK(ps1 == ps2);
}

TEST_CASE("page_size returns greater than zero") {
    size_t ps = mio::page_size();
    CHECK(ps > 0);
}
