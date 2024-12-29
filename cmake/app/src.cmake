file(GLOB_RECURSE src CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/src/**/*.cc"
        "${PROJECT_SOURCE_DIR}/src/**/*.hh")

target_sources(app PUBLIC ${src})