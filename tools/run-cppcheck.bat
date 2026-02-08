:: -------------------------------------------------------------
:: run-cppcheck.bat
::
:: This file is part of the mio library.
::
:: Copyright 2025 Maxtek Consulting
:: Copyright 2017 mandreyel (https://github.com/mandreyel/mio)
:: Copyright 2026 Wissem Chiha
::
:: Licensed under the MIT License. See the LICENSE file.
:: -------------------------------------------------------------
@echo off
cppcheck --enable=all ^
    --inconclusive ^
    --std=c++11 ^
    --suppress=missingIncludeSystem ^
    --suppress=missingInclude ^
  ./include/mio.hpp ^
  ./include/mio/mmap.hpp ^
  ./include/mio/page.hpp ^
  ./include/mio/shared_mmap.hpp ^
  ./include/mio/detail/mmap.ipp ^
  ./include/mio/detail/string_util.hpp


