file(GLOB DRIVER_LIST *.cc)

foreach(FILE_PATH ${DRIVER_LIST})
  STRING(REGEX REPLACE ".+/(.+)\\..*" "\\1" FILE_NAME ${FILE_PATH})
  message(STATUS "summerdb driver files found: ${FILE_NAME}.cc")
  add_executable(summerdb_driver ${FILE_NAME}.cc)
  target_link_libraries(summerdb_driver summerdb)
endforeach()