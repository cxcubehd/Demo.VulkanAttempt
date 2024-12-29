add_executable(app)

set_target_properties(app PROPERTIES OUTPUT_NAME lxe)

# Source Files
include(src.cmake)

# Include Directories
include(inc.cmake)

# Dependencies
include(libs.cmake)