:: -------------------------------------------------------------
:: run-tests.bat
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
SETLOCAL enableDelayedExpansion

WHERE ctest.exe > NUL 2>&1
IF %ERRORLEVEL% NEQ 0 (
  ECHO ctest.exe not found.
  EXIT /B 1
)

ctest -C Debug --output-on-failure
