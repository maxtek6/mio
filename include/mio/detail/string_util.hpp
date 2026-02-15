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

#ifndef MIO_STRING_UTIL_HPP
#define MIO_STRING_UTIL_HPP

#include <type_traits>
#include <utility>

namespace mio { namespace detail {

#include <type_traits>
#include <utility>

    template <typename S, typename C = typename std::decay<S>::type,
              typename = decltype(std::declval<C>().data())>
    struct char_type_helper
    {
        using type = typename C::value_type;
    };

    template <typename T, typename Enable = void>
    struct char_type;

    template <typename T>
    struct char_type<T, typename std::void_t<typename char_type_helper<T>::type>>
    {
        using type = typename char_type_helper<T>::type;
    };

    template <typename T>
    struct char_type<
            T, typename std::enable_if<
                       std::is_same<typename std::remove_cv<typename std::remove_pointer<
                                            typename std::remove_extent<T>::type>::type>::type,
                                    char
#ifdef _WIN32
                                    >::value ||
                       std::is_same<typename std::remove_cv<typename std::remove_pointer<
                                            typename std::remove_extent<T>::type>::type>::type,
                                    wchar_t
#endif
                                    >::value>::type>
    {
        using type = typename std::remove_cv<
                typename std::remove_pointer<typename std::remove_extent<T>::type>::type>::type;
    };

    template <typename CharT, typename S>
    struct is_c_str_helper
    {
        using Decayed = typename std::decay<S>::type;
        using Base    = typename std::remove_cv<typename std::remove_pointer<Decayed>::type>::type;
        static constexpr bool value = std::is_same<Base, CharT>::value;
    };

    template <typename S>
    struct is_c_str
    {
        static constexpr bool value = is_c_str_helper<char, S>::value;
    };

    template <typename S>
    struct is_c_str_or_c_wstr
    {
        static constexpr bool value = is_c_str_helper<char, S>::value
#ifdef _WIN32
                                      || is_c_str_helper<wchar_t, S>::value
#endif
                ;
    };

    template <typename String, typename = decltype(std::declval<String>().data()),
              typename = typename std::enable_if<!is_c_str_or_c_wstr<String>::value>::type>
    const typename char_type<String>::type* c_str(const String& path) {
        return path.data();
    }

    template <typename String, typename = decltype(std::declval<String>().empty()),
              typename = typename std::enable_if<!is_c_str_or_c_wstr<String>::value>::type>
    bool empty(const String& path) {
        return path.empty();
    }

    template <typename String,
              typename = typename std::enable_if<is_c_str_or_c_wstr<String>::value>::type>
    const typename char_type<String>::type* c_str(String path) {
        return path;
    }

    template <typename String,
              typename = typename std::enable_if<is_c_str_or_c_wstr<String>::value>::type>
    bool empty(String path) {
        return !path || (*path == 0);
    }

}} // namespace mio::detail

#endif // MIO_STRING_UTIL_HPP
