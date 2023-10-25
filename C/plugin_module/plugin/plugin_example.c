#include "plugin_interface.h"

const char* get_name() {
    return "Example Plugin";
}

int get_version() {
    return 1;
}

__attribute__((visibility("default")))
PluginInterface plugin_interface = {
    .get_name = get_name,
    .get_version = get_version,
};