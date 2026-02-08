set(mio_test_sources
  test-mio.cpp
  test-mmap.cpp
  test-page.cpp
)

enable_testing()

configure_file(
  "${PROJECT_SOURCE_DIR}/cmake/CTestCustom.cmake"
  "${PROJECT_BINARY_DIR}/CTestCustom.cmake"
  COPYONLY
)

foreach(src IN LISTS mio_test_sources)
  get_filename_component(exe ${src} NAME_WE)
  add_executable(${exe} ${src})
  target_include_directories(${exe} PRIVATE ${prefix})
  target_include_directories(${exe} PRIVATE ${CMAKE_SOURCE_DIR}/third_party)
  target_link_libraries(${exe} PRIVATE mio::mio)
  add_test(NAME ${exe} COMMAND ${exe})
endforeach()

if(WIN32)
  set(MIO_WIN_TEST_SRC test-mio.cpp)

  add_executable(mio.unicode.test ${MIO_WIN_TEST_SRC})
  target_link_libraries(mio.unicode.test PRIVATE mio::mio)
  target_compile_definitions(mio.unicode.test PRIVATE UNICODE)
  add_test(NAME mio.unicode.test COMMAND mio.unicode.test)

  add_executable(mio.fullwinapi.test ${MIO_WIN_TEST_SRC})
  target_link_libraries(mio.fullwinapi.test PRIVATE mio::mio_full_winapi)
  add_test(NAME mio.fullwinapi.test COMMAND mio.fullwinapi.test)

  add_executable(mio.minwinapi.test ${MIO_WIN_TEST_SRC})
  target_link_libraries(mio.minwinapi.test PRIVATE mio::mio_min_winapi)
  add_test(NAME mio.minwinapi.test COMMAND mio.minwinapi.test)
endif()
