
SET(PLUGIN_FILES 
    plugin_example.c
    )

add_library(plugin_example SHARED ${PLUGIN_FILES})
install(TARGETS plugin_example LIBRARY DESTINATION ${ROOT_PATH}/target)