#ifndef NETWORKING_H
#define NETWORKING_H
void sendSysInfoUDP(byte repeats);
void checkUDP();
void refreshNodeList();
void SendUDPCommand(byte destUnit, char* data, byte dataLength);
void SendUDPTaskData(byte destUnit, byte sourceTaskIndex, byte destTaskIndex);
#endif
