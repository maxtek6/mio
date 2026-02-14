:: -------------------------------------------------------------
:: run-cppcheck.bat
::
:: This file is part of the mio library.
::
:: Copyright 2026 Maxtek Consulting
:: Copyright 2017 mandreyel (https://github.com/mandreyel/mio)
:: Copyright 2026 Wissem Chiha
::
:: Licensed under the MIT License. See the LICENSE file.
:: -------------------------------------------------------------
@echo off
SETLOCAL enableDelayedExpansion

WHERE cppcheck.exe > NUL 2>&1
IF %ERRORLEVEL% NEQ 0 (
  ECHO cppcheck.exe not found.
  EXIT /B 1
)

cppcheck ^
    --enable=all ^
    --inconclusive ^
    --std=c++11 ^
    --suppress=missingIncludeSystem ^
    --suppress=missingInclude ^
    --enable=style ^
    --enable=performance ^
    --suppress=missingSystemInclude ^
    --suppress=unmatchedSuppression ^
    --error-exitcode=1 ^
    --inline-suppr ^
    ./include/** ^
    ./example/*.cpp
