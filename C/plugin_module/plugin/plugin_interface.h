#ifndef PLUGIN_INTERFACE_H
#define PLUGIN_INTERFACE_H

typedef struct {
    const char* (*get_name)(void);
    int (*get_version)(void);
} PluginInterface;

#endif