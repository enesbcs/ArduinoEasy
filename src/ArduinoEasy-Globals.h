#ifndef ARDUINOEASY_GLOBALS_H_
#define ARDUINOEASY_GLOBALS_H_

// Set default configuration settings if you want (not mandatory)
// You can always change these during runtime and save to eeprom
// After loading firmware, issue a 'reset' command to load the defaults.

#define DEFAULT_NAME        "newdevice"         // Enter your device friendly name
#define DEFAULT_SERVER      "192.168.0.8"       // Enter your Domoticz Server IP address
#define DEFAULT_PORT        8080                // Enter your Domoticz Server port value
#define DEFAULT_DELAY       60                  // Enter your Send delay in seconds

#define DEFAULT_USE_STATIC_IP   true            // true or false enabled or disabled set static IP
#define DEFAULT_IP          "192.168.0.50"      // Enter your IP address
#define DEFAULT_DNS         "192.168.0.1"       // Enter your DNS
#define DEFAULT_GW          "192.168.0.1"       // Enter your gateway
#define DEFAULT_SUBNET      "255.255.255.0"     // Enter your subnet

#define DEFAULT_PROTOCOL    0                   // Protocol used for controller communications
#define UNIT                0

//#define EthernetShield              // uncomment if using a complete shield with W5100 !! - IF commented than single W5500 is the default on SPI1
#define NETSPD_FUNC                   // comment this line if you do not want to set W5500 LAN speed 10/100 manually
#define FEATURE_SD             false // enable if no STM32 flash storage available ~10-17kB
#define FEATURE_UDP            true // needed for DHCP,NTP,P2P ~5kB
#define FEATURE_BASE64         true // needed for C001 password encoding -640bytes
#define FEATURE_I2C            true // ~1.8kB needed for I2C plugins, STM32 SDA=PB7,SCL=PB6

#define FEATURE_MQTT                    true // NEEDED for MQTT protocols ~3kB
#define FEATURE_NODELIST_NAMES          true // 80bytes
#define FEATURE_NODELIST_NAMESSIZE      10
#define FEATURE_NOISE                   true // add tone and rtttl commands to p001 ~2kB
#define FEATURE_SERIAL                  true // provide serial data to plugins - 256bytes
#define FEATURE_WEBLOG                  true // 184bytes
#define FEATURE_DASHBOARD               true

#define USES_C001   // Domoticz HTTP ~2kB
#define USES_C002   // Domoticz MQTT ~48kB
#define USES_C005   // OpenHAB MQTT ~4kB

#define USES_P001   // Switch ~4-6kB
#define USES_P002   // ADC    344bytes
#define USES_P003   // Pulse  ~1.8kB
#define USES_P004   // Dallas ~2kB
#define USES_P005   // DHT    ~1.3kB
#define USES_P010   // BH1750 848bytes
#define USES_P014   // SI7021 ~1.4kB
#define USES_P023   // OLED (experimental, font size halved) ~3.8kB
#define USES_P026   // SysInfo  640bytes
#define USES_P028   // BMx280  ~9kB
#define USES_P029   // Output  184bytes
#define USES_P033   // Dummy   928bytes
#define USES_P034   // DHT12   528bytes
#define USES_P038   // Neopixel/WS2812 ~1k
#define USES_P051   // AM2320  720bytes

// Challenges on Arduino/W5100 ethernet platform:
// Only 4 ethernet sockets:
//  1: UPD traffic server/send
//  2: Webserver
//  3: MQTT client
//  4: Webclient, active when webserver serves an incoming client or outgoing webclient calls.

#define socketdebug                     false

// DO NOT MODIFY BELOW THIS LINE UNLESS YOU REALLY UNDERSTAND WHAT YOU DO!
#define EMPTY_IP          "0.0.0.0"      // Leave it as zero

#if (defined(USES_P034) || defined(USES_P010) || defined(USES_P014) || defined(USES_P023)|| defined(USES_P028)|| defined(USES_P051))
 #define FEATURE_I2C            true
#else
 #define FEATURE_I2C            false
#endif
#if (defined(USES_C002) || defined(USES_C005))
 #define FEATURE_MQTT           true
#else
 #define FEATURE_MQTT           false
#endif

#define ARDUINO_PROJECT_PID       2016110201L
#define VERSION                             2
#define BUILD                             156
#define BUILD_NOTES                        "STM32fix"

#define NODE_TYPE_ID_ESP_EASY_STD           1
#define NODE_TYPE_ID_RPI_EASY_STD           5
#define NODE_TYPE_ID_ESP_EASYM_STD         17
#define NODE_TYPE_ID_ESP_EASY32_STD        33
#define NODE_TYPE_ID_ARDUINO_EASY_STD      65
#define NODE_TYPE_ID_NANO_EASY_STD         81
#define NODE_TYPE_ID                        NODE_TYPE_ID_ARDUINO_EASY_STD

#define CPLUGIN_PROTOCOL_ADD                1
#define CPLUGIN_PROTOCOL_TEMPLATE           2
#define CPLUGIN_PROTOCOL_SEND               3
#define CPLUGIN_PROTOCOL_RECV               4
#define CPLUGIN_GET_DEVICENAME              5
#define CPLUGIN_WEBFORM_SAVE                6
#define CPLUGIN_WEBFORM_LOAD                7

#define LOG_LEVEL_ERROR                     1
#define LOG_LEVEL_INFO                      2
#define LOG_LEVEL_DEBUG                     3
#define LOG_LEVEL_DEBUG_MORE                4

#define CMD_REBOOT                         89

#ifdef __AVR_ATmega2560__      // Arduino Mega
 #define DEVICES_MAX                        8 // ESP Easy 64
 #ifndef TASKS_MAX
  #define TASKS_MAX                          8 // ESP Easy 12, Arduino maybe 4/8
 #endif
#else
 #define DEVICES_MAX                        16 // ESP Easy 64
 #ifndef TASKS_MAX
  #define TASKS_MAX                          16 // ESP Easy 12, STM32 with 128k flash=4/8, if more can be 8/16!
 #endif
#endif 

#define VARS_PER_TASK                       4
#define PLUGIN_MAX                DEVICES_MAX // ESP Easy 64
#define PLUGIN_CONFIGVAR_MAX                8
#define PLUGIN_CONFIGFLOATVAR_MAX           4
#define PLUGIN_CONFIGLONGVAR_MAX            4
#define PLUGIN_EXTRACONFIGVAR_MAX           4
#define CPLUGIN_MAX                         4 // ESP Easy 16
#define UNIT_MAX                           32 // Only relevant for UDP unicast message 'sweeps' and the nodelist.
#define RULES_TIMER_MAX                     8
#define SYSTEM_TIMER_MAX                    4 // ESP Easy  8
#define SYSTEM_CMD_TIMER_MAX                1 // ESP Easy  2
#define PINSTATE_TABLE_MAX                 16 // ESP Easy 32
#define RULES_MAX_SIZE                   1024 // ESP Easy 2048
#define RULES_MAX_NESTING_LEVEL             3

#define PIN_MODE_UNDEFINED                  0
#define PIN_MODE_INPUT                      1
#define PIN_MODE_OUTPUT                     2
#define PIN_MODE_PWM                        3
#define PIN_MODE_SERVO                      4

#define SEARCH_PIN_STATE                 true
#define NO_SEARCH_PIN_STATE             false

#define DEVICE_TYPE_SINGLE                  1  // connected through 1 datapin
#define DEVICE_TYPE_DUAL                    2  // connected through 2 datapins
#define DEVICE_TYPE_TRIPLE                  3  // connected through 3 datapins
#define DEVICE_TYPE_ANALOG                 10  // AIN/tout pin
#define DEVICE_TYPE_I2C                    20  // connected through I2C
#define DEVICE_TYPE_DUMMY                  99 // Dummy device, has no physical connection

#define SENSOR_TYPE_NONE                    0
#define SENSOR_TYPE_SINGLE                  1
#define SENSOR_TYPE_TEMP_HUM                2
#define SENSOR_TYPE_TEMP_BARO               3
#define SENSOR_TYPE_TEMP_HUM_BARO           4
#define SENSOR_TYPE_DUAL                    5
#define SENSOR_TYPE_TRIPLE                  6
#define SENSOR_TYPE_QUAD                    7
#define SENSOR_TYPE_TEMP_EMPTY_BARO         8
#define SENSOR_TYPE_SWITCH                 10
#define SENSOR_TYPE_DIMMER                 11
#define SENSOR_TYPE_LONG                   20
#define SENSOR_TYPE_WIND                   21

#define PLUGIN_INIT_ALL                     1
#define PLUGIN_INIT                         2
#define PLUGIN_READ                         3
#define PLUGIN_ONCE_A_SECOND                4
#define PLUGIN_TEN_PER_SECOND               5
#define PLUGIN_DEVICE_ADD                   6
#define PLUGIN_EVENTLIST_ADD                7
#define PLUGIN_WEBFORM_SAVE                 8
#define PLUGIN_WEBFORM_LOAD                 9
#define PLUGIN_WEBFORM_SHOW_VALUES         10
#define PLUGIN_GET_DEVICENAME              11
#define PLUGIN_GET_DEVICEVALUENAMES        12
#define PLUGIN_WRITE                       13
#define PLUGIN_EVENT_OUT                   14
#define PLUGIN_WEBFORM_SHOW_CONFIG         15
#define PLUGIN_SERIAL_IN                   16
#define PLUGIN_UDP_IN                      17
#define PLUGIN_CLOCK_IN                    18
#define PLUGIN_TIMER_IN                    19

#define VALUE_SOURCE_SYSTEM                 1
#define VALUE_SOURCE_SERIAL                 2
#define VALUE_SOURCE_HTTP                   3
#define VALUE_SOURCE_MQTT                   4
#define VALUE_SOURCE_UDP                    5

#define INT_MIN -32767
#define INT_MAX 32767

#if FEATURE_BASE64
 #include <base64.h>
#endif

#if FEATURE_MQTT
 #include <PubSubClient.h>
 #include <ArduinoJson.h>
#endif

#ifndef UID_BASE
// https://github.com/rogerclarkmelbourne/Arduino_STM32 //
 #define UID_BASE 0x1FFFF7E8U /*!< Unique device ID register base address - F1 series only */
 #if defined(MCU_STM32F103TB) || defined(MCU_STM32F103CB) || defined(MCU_STM32F103RB) || defined(MCU_STM32F103VB)
  #define STM32_F1
  #define FLASH_SIZE       131072
  #define FLASH_PAGE_SIZE  0x400
 #elif defined(MCU_STM32F103RC) || defined(MCU_STM32F103VC) || defined(MCU_STM32F103ZC)
  #define STM32_F1
  #define FLASH_SIZE       262144
  #define FLASH_PAGE_SIZE  0x800 
 #elif defined(MCU_STM32F103RD) || defined(MCU_STM32F103VD) || defined(MCU_STM32F103ZD)
  #define STM32_F1
  #define FLASH_SIZE       393216
  #define FLASH_PAGE_SIZE  0x800  
 #elif defined(MCU_STM32F103RE) || defined(MCU_STM32F103VE) || defined(MCU_STM32F103ZE)
  #define STM32_F1
  #define FLASH_SIZE       524288
  #define FLASH_PAGE_SIZE  0x800  
 #else
  #define FLASH_SIZE 0
  #undef UID_BASE
 #endif
#else // Official core
 #pragma message "This Core is totally unsupported!"  
 #if defined(STM32F103xB)
  #define STM32_OFFICIAL
  #define STM32_F1
  #define FLASH_SIZE       131072
  #ifndef FLASH_PAGE_SIZE
   #define FLASH_PAGE_SIZE  0x400
  #endif
 #elif defined(STM32F103xE)
  #define STM32_OFFICIAL
  #define STM32_F1
  #define FLASH_SIZE       524288
  #ifndef FLASH_PAGE_SIZE
   #define FLASH_PAGE_SIZE  0x800  
  #endif
 #else
  #define FLASH_SIZE 0
  #error "Unknown board!"  
 #endif

#endif

#include <SPI.h>
#if (!defined(UID_BASE) && defined(EthernetShield)) || defined(STM32_OFFICIAL)
 #include <Ethernet.h>
#else
 #include <Ethernet_STM.h>
#endif

#if FEATURE_UDP
  #include <EthernetUdp.h>
  #include <Dns.h>
#else
  #define DEFAULT_USE_STATIC_IP   true // DHCP needs UDP!
#endif

#if FEATURE_I2C
  #include <Wire.h> // STM32 SDA=PB7,SCL=PB6
#endif

#if FEATURE_SD
 #ifdef EthernetShield
  #define EthernetShield_CS_W5100     10
  #define EthernetShield_CS_SDCard     4
 #else
  #define EthernetShield_CS_SDCard    10
  #define EthernetShield_CS_W5100     BOARD_SPI1_NSS_PIN  // CONTROL_CS 4, SPI_SCK 5,  SPI_MISO 6, SPI_MOSI 7
 #endif
 #include <SD.h>
#elif defined(STM32_F1)
 #ifndef STM32_OFFICIAL
  #include "flash_stm32.h" // currently only STM32 internal Flash is supported as Data storage
 #endif
 #if FLASH_SIZE>=120000
  #if defined(FLASH_PAGE_SIZE)
   #define RULES_MAX_SIZE FLASH_PAGE_SIZE
   #if (FLASH_PAGE_SIZE==0x400)
     #if TASKS_MAX<=4
      #define DATA_PARTITION_SIZE 4 // 4kB data partition
      #define TASKS_MAX           4 // overwrite existing value
     #else
      #define DATA_PARTITION_SIZE 6 // 6kB data partition
      #define TASKS_MAX           8 // overwrite existing value
     #endif
     #define GS_START   (0x8000000 + FLASH_SIZE - (DATA_PARTITION_SIZE * FLASH_PAGE_SIZE))
     #define ETS_START  (0x8000000 + FLASH_SIZE - ((DATA_PARTITION_SIZE-1) * FLASH_PAGE_SIZE))
     #define RUL_START  (0x8000000 + FLASH_SIZE - (1 * FLASH_PAGE_SIZE))   
   #elif (FLASH_PAGE_SIZE==0x800)
     #if TASKS_MAX<=4
      #define DATA_PARTITION_SIZE 3 // 6kB data partition
      #define TASKS_MAX           4
     #elif TASKS_MAX<=8
      #define DATA_PARTITION_SIZE 4 // 8kB data partition
      #define TASKS_MAX           8
     #elif TASKS_MAX>8
      #define DATA_PARTITION_SIZE 6 // 12kB data partition
      #define TASKS_MAX          16
     #endif
     #define GS_START   (0x8000000 + FLASH_SIZE - (DATA_PARTITION_SIZE * FLASH_PAGE_SIZE))
     #define ETS_START  (0x8000000 + FLASH_SIZE - ((DATA_PARTITION_SIZE-1) * FLASH_PAGE_SIZE))
     #define RUL_START  (0x8000000 + FLASH_SIZE - (1 * FLASH_PAGE_SIZE))
   #else
     #define DATA_PARTITION_SIZE 0   //  not supported scenario
     #define GS_START   0
     #undef ETS_START
     #define RUL_START  0
     #error "Neither SD storage, nor STM32F1 flash storage is available!"
   #endif
  #endif
 #endif
#endif

#if defined(UID_BASE)
 byte *id1 =  (byte *) (UID_BASE + 0x02);
 byte *id2 =  (byte *) (UID_BASE + 0x05);
 byte *id3 =  (byte *) (UID_BASE + 0x09);
 byte mac[] = { 0x10, 0x00, 0x00, *id1, *id2, *id3 };
#elif defined(__AVR_ATmega2560__)      // Arduino Mega
 byte mac[] = { 0x10, 0x00, 0x00, 0, 0, 0 }; // MAC address will be filled in the main loop, not here because "statement-expressions are not allowed outside functions nor in template-argument lists"
#else
 byte mac[] = { 0x10, 0x00, 0x00, 0xEF, 0xFE, 0 }; // specify an own MAC addres if nothing works
#endif

// MEMORY: sector 0=GS+SS=1024 bytes, sector 1=8*512=4096bytes ETS, sector 2=Rules 1024bytes
struct SecurityStruct // 87 bytes 1x at sector SS
{
  char          ControllerUser[26];
  char          ControllerPassword[35];
  char          Password[26];
} SecuritySettings;

struct SettingsStruct // ~936bytes 1x at sector GS
{
  unsigned long PID;
  int           Version;
  byte          Unit;
  int16_t       Build;
  byte          IP[4];
  byte          Gateway[4];
  byte          Subnet[4];
  byte          DNS[4];
  byte          Controller_IP[4];
  unsigned int  ControllerPort;
  byte          IP_Octet;
  char          NTPHost[64];
  unsigned long Delay;
  byte          Syslog_IP[4];
  unsigned int  UDPPort;
  byte          Protocol;
  char          Name[26];
  byte          SyslogLevel;
  byte          SerialLogLevel;
  unsigned long BaudRate;
  unsigned long MessageDelay;
  boolean       CustomCSS;
  char          ControllerHostName[64];
  boolean       UseNTP;
  boolean       DST;
  byte          WebLogLevel;
  int8_t        PinBootStates[17];
  byte          UseDNS;
  boolean       UseRules;
  int8_t        Pin_status_led;
  boolean       UseSerial;
  boolean       GlobalSync;
  unsigned long ConnectionFailuresThreshold;
  int16_t       TimeZone;
  byte          SDLogLevel;
  byte          TaskDeviceNumber[TASKS_MAX];
  unsigned int  TaskDeviceID[TASKS_MAX];
  int8_t        TaskDevicePin1[TASKS_MAX];
  int8_t        TaskDevicePin2[TASKS_MAX];
  byte          TaskDevicePort[TASKS_MAX];
  boolean       TaskDevicePin1PullUp[TASKS_MAX];
  int16_t       TaskDevicePluginConfig[TASKS_MAX][PLUGIN_CONFIGVAR_MAX];
  boolean       TaskDevicePin1Inversed[TASKS_MAX];
  float         TaskDevicePluginConfigFloat[TASKS_MAX][PLUGIN_CONFIGFLOATVAR_MAX];
  long          TaskDevicePluginConfigLong[TASKS_MAX][PLUGIN_CONFIGLONGVAR_MAX];
  boolean       TaskDeviceSendData[TASKS_MAX];
  boolean       TaskDeviceGlobalSync[TASKS_MAX];
  int8_t        TaskDevicePin3[TASKS_MAX];
  byte          TaskDeviceDataFeed[TASKS_MAX];
  unsigned long TaskDeviceTimer[TASKS_MAX];
  boolean       MQTTRetainFlag;
  char          MQTTpublish[75];
  char          MQTTsubscribe[75];
} Settings;

struct ExtraTaskSettingsStruct // ~390bytes/TASK  -- (512 * tasknum) bytes after global conf
{
  byte    TaskIndex;
  char    TaskDeviceName[41];
  char    TaskDeviceFormula[VARS_PER_TASK][41];
  char    TaskDeviceValueNames[VARS_PER_TASK][41];
  long    TaskDevicePluginConfigLong[PLUGIN_EXTRACONFIGVAR_MAX];
  byte    TaskDeviceValueDecimals[VARS_PER_TASK];
} ExtraTaskSettings;

struct EventStruct
{
  byte Source;
  byte TaskIndex;
  byte BaseVarIndex;
  int idx;
  byte sensorType;
  int Par1;
  int Par2;
  int Par3;
  byte OriginTaskIndex;
  String String1;
  String String2;
  byte *Data;
};

struct LogStruct
{
  unsigned long timeStamp;
  char* Message;
} Logging[10];
int logcount = -1;

struct DeviceStruct
{
  byte Number;
  byte Type;
  byte VType;
  byte Ports;
  boolean PullUpOption;
  boolean InverseLogicOption;
  boolean FormulaOption;
  byte ValueCount;
  boolean Custom;
  boolean SendDataOption;
  boolean GlobalSyncOption;
  boolean TimerOption;
  boolean TimerOptional;
  boolean DecimalsOnly;
} Device[DEVICES_MAX + 1]; // 1 more because first device is empty device

struct ProtocolStruct
{
  byte Number;
  boolean usesMQTT;
  boolean usesAccount;
  boolean usesPassword;
  int defaultPort;
  boolean usesTemplate;
} Protocol[CPLUGIN_MAX];

#if FEATURE_UDP
struct NodeStruct
{
  byte ip[4];
  byte age;
  uint16_t build;
#if FEATURE_NODELIST_NAMES
  char nodeName[FEATURE_NODELIST_NAMESSIZE + 1];
#endif
  byte nodeType;
} Nodes[UNIT_MAX];
#endif

struct systemTimerStruct
{
  unsigned long timer;
  byte plugin;
  byte Par1;
  byte Par2;
  byte Par3;
} systemTimers[SYSTEM_TIMER_MAX];

struct systemCMDTimerStruct
{
  unsigned long timer;
  String action;
} systemCMDTimers[SYSTEM_CMD_TIMER_MAX];

struct pinStatesStruct
{
  byte plugin;
  byte index;
  byte mode;
  uint16_t value;
} pinStates[PINSTATE_TABLE_MAX];

byte lastlanstate = -1;

#endif
