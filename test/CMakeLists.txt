set(mio_test_sources
  test_mmap.cpp
  test_page_size.cpp
  test_shared_mmap.cpp
)

set(mio_benchmark_sources 
  benchmark_mmap.cpp
)

enable_testing()

configure_file(
  "${PROJECT_SOURCE_DIR}/cmake/CTestCustom.cmake"
  "${PROJECT_BINARY_DIR}/CTestCustom.cmake"
  COPYONLY
)

set(COVER_TARGETS)

foreach(src IN LISTS mio_test_sources)
  get_filename_component(exe ${src} NAME_WE)
  add_executable(${exe} ${src})
  target_include_directories(${exe} PRIVATE ${prefix})
  target_include_directories(${exe} PRIVATE ${CMAKE_SOURCE_DIR}/third_party)
  target_link_libraries(${exe} PRIVATE mio::mio)
  if(mio.cover AND NOT(WIN32))
    target_compile_options(${exe} PRIVATE -fprofile-arcs -ftest-coverage -g -O0)
    target_link_libraries(${exe} PRIVATE gcov "--coverage")
    list(APPEND COVER_TARGETS ${exe})
  endif()
  add_test(NAME ${exe} COMMAND ${exe})
endforeach()

if(WIN32)
  set(MIO_WIN_TEST_SRC test_mmap.cpp)

  add_executable(mio.unicode.test ${MIO_WIN_TEST_SRC})
  target_include_directories(mio.unicode.test PRIVATE ${CMAKE_SOURCE_DIR}/third_party)
  target_link_libraries(mio.unicode.test PRIVATE mio::mio)
  target_compile_definitions(mio.unicode.test PRIVATE UNICODE)
  add_test(NAME mio.unicode.test COMMAND mio.unicode.test)

  add_executable(mio.fullwinapi.test ${MIO_WIN_TEST_SRC})
  target_include_directories(mio.fullwinapi.test PRIVATE ${CMAKE_SOURCE_DIR}/third_party)
  target_link_libraries(mio.fullwinapi.test PRIVATE mio::mio_full_winapi)
  add_test(NAME mio.fullwinapi.test COMMAND mio.fullwinapi.test)

  add_executable(mio.minwinapi.test ${MIO_WIN_TEST_SRC})
  target_include_directories(mio.minwinapi.test PRIVATE ${CMAKE_SOURCE_DIR}/third_party)
  target_link_libraries(mio.minwinapi.test PRIVATE mio::mio_min_winapi)
  add_test(NAME mio.minwinapi.test COMMAND mio.minwinapi.test)
endif()

if(mio.benchmark)
  foreach(src IN LISTS mio_benchmark_sources)
    get_filename_component(exe ${src} NAME_WE)
    add_executable(${exe} ${src})
    target_include_directories(${exe} PRIVATE ${prefix})
    target_link_libraries(${exe} PRIVATE mio::mio)
  endforeach()
endif()

if(mio.cover)
  if(WIN32)
    message(STATUS "Skipping coverage for Windows")
  else()
    add_custom_target(
      cover
      DEPENDS ${COVER_TARGETS})
    add_custom_command(
      TARGET cover
      COMMAND gcovr -r ${CMAKE_CURRENT_SOURCE_DIR}/.. -e ${CMAKE_CURRENT_SOURCE_DIR} -e ${CMAKE_CURRENT_SOURCE_DIR}/../third_party)
  endif()
endif()