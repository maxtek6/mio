/*
 * Copyright 2026 Maxtek Consulting
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef MIO_TEST_FIXTURE_CREATE_TEST_FILE_HPP
#define MIO_TEST_FIXTURE_CREATE_TEST_FILE_HPP

#include <string>
#include <fstream>

static std::string create_test_file(const std::string& path, size_t size) {
    std::string buffer(size, 0);
    char        v = 33;
    for(auto& b : buffer) {
        b = v;
        ++v;
        v %= 126;
        if(v == 0)
            v = 33;
    }
    std::ofstream file(path);
    file << buffer;
    return buffer;
}

#endif // MIO_TEST_FIXTURE_CREATE_TEST_FILE_HPP