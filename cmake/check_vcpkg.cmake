# Require vcpkg
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE OR NOT CMAKE_TOOLCHAIN_FILE MATCHES "vcpkg")
    message(FATAL_ERROR "Vcpkg is required but is not provided.")
endif ()