#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "mio.hpp"

// These are basic sanity tests for the advice wrapper function
// introduced in mio 1.2.0.
TEST_CASE("advise returns invalid_argument when addr is nullptr") {
    std::error_code ec = mio::advise(nullptr, 1024, mio::advice::will_need);
    CHECK(ec == std::errc::invalid_argument);
}

TEST_CASE("advise returns success when len == 0") {
    int             dummy;
    std::error_code ec = mio::advise(&dummy, 0, mio::advice::will_need);
    CHECK_FALSE(ec);
}
TEST_CASE("advise with will_need on valid buffer") {
    std::vector<char> buf(4096);
    std::error_code   ec = mio::advise(buf.data(), buf.size(), mio::advice::will_need);
    CHECK((!ec || ec.category() == std::system_category() ||
           ec.category() == std::generic_category()));
}

TEST_CASE("advise with dont_need on valid buffer") {
    std::vector<char> buf(4096);
    std::error_code ec = mio::advise(buf.data(), buf.size(), mio::advice::dont_need);
    CHECK((!ec || ec == std::errc::invalid_argument));
}

TEST_CASE("advise with sequential on valid buffer") {
    std::vector<char> buf(4096);
    std::error_code ec = mio::advise(buf.data(), buf.size(), mio::advice::sequential);
    CHECK((!ec || ec == std::errc::invalid_argument));
}

TEST_CASE("advise with random on valid buffer") {
    std::vector<char> buf(4096);
    std::error_code ec = mio::advise(buf.data(), buf.size(), mio::advice::random);
    CHECK((!ec || ec == std::errc::invalid_argument));
}