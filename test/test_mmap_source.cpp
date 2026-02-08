#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "mio.hpp"
#include "fixture/create_test_file.hpp"

TEST_CASE("mmap maps entire file") {
    std::error_code ec;
    auto            page_size = mio::page_size();
    auto            buffer    = create_test_file("test-file", 4 * page_size - 250);

    auto m = mio::make_mmap_source("test-file", 0, mio::map_entire_file, ec);
    REQUIRE_FALSE(ec);
    CHECK(m.is_open());
    CHECK(m.size() == buffer.size());

    for(size_t i = 0; i < m.size(); ++i) {
        CHECK(m[i] == buffer[i]);
    }
}

TEST_CASE("mmap handles offsets") {
    std::error_code ec;
    auto            page_size = mio::page_size();
    auto            buffer    = create_test_file("test-file", 4 * page_size - 250);

    SUBCASE("offset below page size") {
        auto m = mio::make_mmap_source("test-file", page_size - 3, mio::map_entire_file, ec);
        REQUIRE_FALSE(ec);
        CHECK(m.size() == buffer.size() - (page_size - 3));
    }

    SUBCASE("offset above page size") {
        auto m = mio::make_mmap_source("test-file", page_size + 3, mio::map_entire_file, ec);
        REQUIRE_FALSE(ec);
        CHECK(m.size() == buffer.size() - (page_size + 3));
    }
}

TEST_CASE("invalid mmap error") {
    std::error_code  ec;
    mio::mmap_source m;

    m = mio::make_mmap_source("nonexistent-file", 0, 0, ec);
    CHECK(ec);
    CHECK(m.empty());
    CHECK_FALSE(m.is_open());

    m = mio::make_mmap_source(static_cast<const char*>(nullptr), 0, 0, ec);
    CHECK(ec);
    CHECK(m.empty());
    CHECK_FALSE(m.is_open());
}
