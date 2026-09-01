/* Copyright 2017 https://github.com/mandreyel
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

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

#ifndef MIO_PAGE_HPP
#define MIO_PAGE_HPP

#include <errno.h>
#include <stddef.h>
#include <system_error> // std::error_code
#ifdef _WIN32
// !! NOTE DO NOT Reverse the include order of these two windows api files
// build errors could trigger !!
#include <windows.h>
#include <memoryapi.h> // PrefetchVirtualMemory
#else
#include <unistd.h>
#include <sys/mman.h> // madvise
#endif

namespace mio {

/**
 * This is used by `basic_mmap` to determine whether to create a read-only or
 * a read-write memory mapping.
 */
enum class access_mode
{
    read,
    write
};

/**
 * This is used by madvice to determine Memory access pattern hints
 */
enum class advice
{
    will_need,
    dont_need,
    sequential,
    random
};

/**
 * This is a cross platform Wrapper around linux madvice function mainly for 
 * windows users, which under the hood uses PrefetchVirtualMemory() from WinAPI
 * @note Windows does not have a direct equivalent to all madvise() behaviors
 * only advice::will_need is implemented, for other arguments we will
 * simply throw an invalid argument error code, fo unix platforms (Linux, MacOS)
 * all option are mapped nativlly to unix madvice internal flags
 * given this limitation, the performance will be heavily impacted by Windows memory 
 * manager 
 */
std::error_code advise(void* addr, std::size_t len, advice hint);

/**
 * Determines the operating system's page allocation granularity.
 * On the first call to this function, it invokes the operating system specific syscall
 * to determine the page size, caches the value, and returns it. Any subsequent call to
 * this function serves the cached value, so no further syscalls are made.
 */
inline size_t page_size();

/**
 * Alligns `offset` to the operating's system page size such that it subtracts the
 * difference until the nearest page boundary before `offset`, or does nothing if
 * `offset` is already page aligned.
 */
inline size_t make_offset_page_aligned(size_t offset) noexcept;

// --------------------------------------------------------------------------

std::error_code advise(void* addr, std::size_t len, advice hint) {
    if(addr == nullptr) {
        return std::make_error_code(std::errc::invalid_argument);
    }

    if(len == 0) {
        return {};
    }

#ifdef _WIN32

    switch(hint) {
        case advice::will_need: {
            WIN32_MEMORY_RANGE_ENTRY range{};
            range.VirtualAddress = addr;
            range.NumberOfBytes  = len;

            if(!PrefetchVirtualMemory(GetCurrentProcess(), 1, &range, 0)) {
                return std::error_code(static_cast<int>(GetLastError()), std::system_category());
            }

            return {};
        }
        case advice::dont_need:
        case advice::sequential:
        case advice::random: return {};
    }
    return std::make_error_code(std::errc::invalid_argument);

#else  // UNIX

    int madvise_flag;

    switch(hint) {
        case advice::will_need: madvise_flag = MADV_WILLNEED; break;
        case advice::dont_need: madvise_flag = MADV_DONTNEED; break;
        case advice::sequential: madvise_flag = MADV_SEQUENTIAL; break;
        case advice::random: madvise_flag = MADV_RANDOM; break;
        default: return std::make_error_code(std::errc::invalid_argument);
    }

    if(madvise(addr, len, madvise_flag) != 0) {
        return std::error_code(errno, std::generic_category());
    }

    return {};
#endif // _WIN32
}

// --------------------------------------------------------------------------
inline size_t page_size() {
    static const size_t page_size = [] {
#ifdef _WIN32
        SYSTEM_INFO SystemInfo;
        GetSystemInfo(&SystemInfo);
        return SystemInfo.dwAllocationGranularity;
#else
        return sysconf(_SC_PAGE_SIZE);
#endif
    }();
    return page_size;
}

// --------------------------------------------------------------------------
inline size_t make_offset_page_aligned(size_t offset) noexcept {
    const size_t page_size_ = page_size();
    // Use integer division to round down to the nearest page alignment.
    return offset / page_size_ * page_size_;
}

}; // namespace mio

#endif // MIO_PAGE_HPP
