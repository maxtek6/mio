set(mio_example_sources
  example.cpp
)

foreach(src IN LISTS mio_example_sources)
  get_filename_component(exe ${src} NAME_WE)
  add_executable(${exe} ${src})
  target_include_directories(${exe} PRIVATE ${prefix})
  target_link_libraries(${exe} PRIVATE mio::mio)
endforeach()