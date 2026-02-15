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

#ifndef MIO_TEST_FIXTURE_PRINT_BENCHMARK_RESULT
#define MIO_TEST_FIXTURE_PRINT_BENCHMARK_RESULT

#include <iomanip>
#include <iostream>
#include <string>

inline void print_benchmark_result(const std::string& label, long ms, size_t size) {
    double throughput = (size / (1024.0 * 1024.0)) / (ms / 1000.0);

    std::cout << std::left << std::setw(25) << label << ": " << std::right << std::setw(8) << ms
              << " ms"
              << "   (" << std::fixed << std::setprecision(1) << throughput << " MB/s)" << '\n';
}

#endif // MIO_TEST_FIXTURE_PRINT_BENCHMARK_RESULT