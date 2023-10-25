#include <stdio.h>
#include <dlfcn.h>
#include "plugin/plugin_interface.h"

int main() {
    void* plugin_handle = dlopen("./libplugin_example.so", RTLD_LAZY);
    if (!plugin_handle) {
        printf("Error loading plugin: %s\n", dlerror());
        return 1;
    }

    PluginInterface* plugin_interface = dlsym(plugin_handle, "plugin_interface");
    if (!plugin_interface) {
        printf("Error loading plugin interface: %s\n", dlerror());
        return 1;
    }

    printf("Plugin name: %s\n", plugin_interface->get_name());
    printf("Plugin version: %d\n", plugin_interface->get_version());

    dlclose(plugin_handle);
    return 0;
}
