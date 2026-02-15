#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "mio.hpp"
#include "fixture/fixture.hpp"

TEST_CASE("shared mmap maps entire file") {
    std::error_code ec;
    auto            page_size = mio::page_size();
    auto            buffer    = create_test_file("shared-test-file", 4 * page_size - 250);

    mio::shared_mmap_source shared_mmap("shared-test-file", 0, mio::map_entire_file);
    shared_mmap.map("shared-test-file", 0, mio::map_entire_file, ec);

    REQUIRE_FALSE(ec);
    CHECK(shared_mmap.is_open());
    CHECK(shared_mmap.size() == buffer.size());

    for(size_t i = 0; i < shared_mmap.size(); ++i) {
        CHECK(shared_mmap[i] == buffer[i]);
    }
}