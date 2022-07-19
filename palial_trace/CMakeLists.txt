cmake_minimum_required(VERSION 3.2)
project(palial_trace)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_VERBOSE_MAKEFILE ON)

# set up path
set(OMP_INSTALL /home/cc/llvm-project/build-openmp)
set(OMP_LIB_PATH ${OMP_INSTALL}/lib)
set(OMP_INCLUDE ${OMP_INSTALL}/include)

set(FEATURES "")
set(FEATURES "${FEATURES} -DOMPT_MEASUREMENT_SUPPORT=1")

# For tracing and output
set(FEATURES "${FEATURES} -DOMPT_TRACING_SUPPORT=1")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FEATURES} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FEATURES} -g")

set(SOURCE_FILES
    palial_trace.h
    palial_trace.c
    ompt_callbacks.h
)

include_directories(${OMP_INCLUDE})
add_library(palial_trace SHARED ${SOURCE_FILES})
target_link_libraries(palial_trace -lzmq)
install(TARGETS palial_trace DESTINATION lib)
