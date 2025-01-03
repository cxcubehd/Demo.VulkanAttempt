# Check for vcpkg
include(check_vcpkg.cmake)

# Include CPM
include(cpm/CPM.cmake)

# App
add_subdirectory(app)