:: -------------------------------------------------------------
:: run-clang-format.bat
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

where clang-format.exe > NUL 2>&1
IF %ERRORLEVEL% NEQ 0 (
  ECHO clang-format.exe not found.
  EXIT /B 1
)

for /f %%f in ('dir /s/b ..\..\*.hpp ..\..\*.cpp') do clang-format -i -style=file %%f