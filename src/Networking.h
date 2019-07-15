#ifndef NETWORKING_H
#define NETWORKING_H
 #ifndef EthernetShield
 enum {
        PHYCFGR_RST = ~(1<<7),  //< For PHY reset, must operate AND mask.
        PHYCFGR_OPMD = (1<<6),   // Configre PHY with OPMDC value
        PHYCFGR_OPMDC_ALLA = (7<<3),
        PHYCFGR_OPMDC_PDOWN = (6<<3),
        PHYCFGR_OPMDC_NA = (5<<3),
        PHYCFGR_OPMDC_100FA = (4<<3),
        PHYCFGR_OPMDC_100F = (3<<3),
        PHYCFGR_OPMDC_100H = (2<<3),
        PHYCFGR_OPMDC_10F = (1<<3),
        PHYCFGR_OPMDC_10H = (0<<3),
        PHYCFGR_DPX_FULL = (1<<2),
        PHYCFGR_DPX_HALF = (0<<2),
        PHYCFGR_SPD_100 = (1<<1),
        PHYCFGR_SPD_10 = (0<<1),
        PHYCFGR_LNK_ON = (1<<0),
        PHYCFGR_LNK_OFF = (0<<0),
 };
 #endif
void sendSysInfoUDP(byte repeats);
void checkUDP();
void refreshNodeList();
void SendUDPCommand(byte destUnit, char* data, byte dataLength);
void SendUDPTaskData(byte destUnit, byte sourceTaskIndex, byte destTaskIndex);
#endif
