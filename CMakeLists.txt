cmake_minimum_required(VERSION 3.5)

project(uvm)

set(CMAKE_CXX_STANDARD 11)

file(GLOB HEADERS "include/uvm/*.h" "../jsondiff/jsondiff-cpp/include/jsondiff/*.h")

set(SOURCES src/uvm/lapi.cpp
    src/uvm/lauxlib.cpp
    src/uvm/lbaselib.cpp
    src/uvm/lbitlib.cpp
    src/uvm/lcode.cpp
    src/uvm/lcorolib.cpp
    src/uvm/lctype.cpp
    src/uvm/ldblib.cpp
    src/uvm/ldebug.cpp
    src/uvm/ldo.cpp
    src/uvm/ldump.cpp
    src/uvm/lfunc.cpp
    src/uvm/lgc.cpp
    src/uvm/lhashmap.cpp
    src/uvm/lhttplib.cpp
    src/uvm/linit.cpp
    src/uvm/liolib.cpp
    src/uvm/ljsonlib.cpp
    src/uvm/ljsonrpclib.cpp
    src/uvm/llex.cpp
    src/uvm/lmathlib.cpp
    src/uvm/lsafemathlib.cpp
    src/uvm/lmem.cpp
    src/uvm/lnetlib.cpp
    src/uvm/loadlib.cpp
    src/uvm/lobject.cpp
    src/uvm/lopcodes.cpp
    src/uvm/loslib.cpp
    src/uvm/lparser.cpp
    src/uvm/lprefix.cpp
    src/uvm/lstate.cpp
    src/uvm/lstring.cpp
    src/uvm/lstrlib.cpp
    src/uvm/ltable.cpp
    src/uvm/ltablib.cpp
    src/uvm/ltimelib.cpp
    src/uvm/ltm.cpp
    src/uvm/lundump.cpp
    src/uvm/lutf8lib.cpp
    src/uvm/lvm.cpp
    src/uvm/lzio.cpp
    src/uvm/uvm_api_types.cpp
    src/uvm/uvm_lib.cpp
    src/uvm/uvm_lutil.cpp
    src/uvm/uvm_state_scope.cpp
    src/uvm/uvm_storage.cpp
    src/uvm/uvm_tokenparser.cpp
)

find_package(OpenSSL REQUIRED)

add_library( uvm ${SOURCES} ${HEADERS} )

target_link_libraries( uvm 
  PUBLIC fc)
target_include_directories( uvm 
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/../jsondiff/jsondiff-cpp/include"
)

aux_source_directory(./simplechain/src simplechain_src1)
aux_source_directory(./simplechain/src/simplechain simplechain_src2)

include_directories(simplechain/include)

link_directories(../fc /usr/local/lib/boost/lib)
link_libraries(fc boost_system boost_system boost_chrono boost_date_time boost_coroutine boost_context boost_thread boost_filesystem pthread)

add_executable(simplechain_runner ${simplechain_src1} ${simplechain_src2})

target_include_directories( simplechain_runner 
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/simplechain/include" "${CMAKE_CURRENT_SOURCE_DIR}/../jsondiff/jsondiff-cpp/include"
)

target_link_libraries(simplechain_runner PUBLIC uvm fc OpenSSL::SSL ${CMAKE_SOURCE_DIR}/../jsondiff/libjsondiff_cpp.a)


if (USE_PCH)
  set_target_properties(uvm PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
  cotire(uvm)
endif(USE_PCH)

install( TARGETS
   uvm

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
install( FILES ${HEADERS} DESTINATION "include/uvm" )

