cmake_minimum_required(VERSION 3.17)
project(FluxionCore C)

set(CMAKE_C_STANDARD 99)
add_library(FluxionCore SHARED fluxion_core.h fluxion_core.c internals/fluxion_parser.c internals/fluxion_parser.h internals/fluxion_token.c internals/fluxion_token.h internals/commons.c internals/commons.h)
add_executable(FluxionRunner main.c)
target_link_libraries(FluxionRunner FluxionCore)