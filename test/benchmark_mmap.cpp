
// File sizes: 1MB, 5MB, 10MB, 100MB, 200MB
// Benchmark: sequential read using memory mapping
// Benchmark: sequential write using memory mapping
// benkacl between chuned write of 64 KB

#include "mio/mmap.hpp"
#include "fixture/fixture.hpp"
#include <chrono>
#include <fstream>
#include <string>
#include <vector>

void benchmark_mmap_read(const std::string& path, size_t size) {
    mio::mmap_source mmap(path);

    auto          start = std::chrono::high_resolution_clock::now();
    volatile char c;
    for(size_t i = 0; i < mmap.size(); ++i) {
        c = mmap[i];
    }
    auto end = std::chrono::high_resolution_clock::now();

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    print_benchmark_result("mio mmap read", ms, size);
}

void benchmark_mmap_write(const std::string& path, size_t size) {
    mio::mmap_sink  mmap(path);
    std::error_code ec;

    const size_t chunk_size = 64 * 1024;

    auto start = std::chrono::high_resolution_clock::now();
    for(size_t i = 0; i < mmap.size(); i += chunk_size) {
        size_t end = std::min(i + chunk_size, mmap.size());
        for(size_t j = i; j < end; ++j) {
            mmap[j] = static_cast<char>((mmap[j] + 1) % 256);
        }
    }
    mmap.sync(ec);
    auto end = std::chrono::high_resolution_clock::now();

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    print_benchmark_result("mio mmap write", ms, size);
}

void benchmark_stream_read(const std::string& path, size_t size) {
    std::ifstream ifs(path, std::ios::binary);
    auto          start = std::chrono::high_resolution_clock::now();
    volatile char c;
    char          buffer;
    while(ifs.read(&buffer, 1)) {
        c = buffer;
    }
    auto end = std::chrono::high_resolution_clock::now();

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    print_benchmark_result("c++ iostream read", ms, size);
}

void benchmark_stream_write(const std::string& path, size_t size) {
    std::fstream fs(path, std::ios::in | std::ios::out | std::ios::binary);

    const size_t      chunk_size = 64 * 1024;
    std::vector<char> buffer(chunk_size);

    auto start = std::chrono::high_resolution_clock::now();

    while(fs.read(buffer.data(), buffer.size()) || fs.gcount() > 0) {
        size_t count = fs.gcount();
        for(size_t i = 0; i < count; ++i) {
            buffer[i] = static_cast<char>((buffer[i] + 1) % 256);
        }
        fs.seekp(-static_cast<std::streamoff>(count), std::ios::cur);
        fs.write(buffer.data(), count);
    }

    fs.flush();

    auto end = std::chrono::high_resolution_clock::now();
    auto ms  = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    print_benchmark_result("c++ iostream write", ms, size);
}

int main() {
    std::vector<size_t> file_sizes = {1 << 20,   5 << 20,   10 << 20, 100 << 20,
                                      200 << 20, 300 << 20, 350 << 20};

    for(auto size : file_sizes) {
        std::string path = "test_" + std::to_string(size) + ".dat";
        create_test_file(path, size);

        std::cout << "\nBenchmarking file size: " << size / (1 << 20) << " MB\n";
        benchmark_mmap_read(path, size);
        benchmark_mmap_write(path, size);
        benchmark_stream_read(path, size);
        benchmark_stream_write(path, size);
    }

    return 0;
}
