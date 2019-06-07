#ifndef PLUGIN_H
#define PLUGIN_H
void PluginInit(void);
byte PluginCall(byte Function, struct EventStruct *event, String& str);
#endif