/****************************************************************************************************************************\
   Arduino project "Arduino Easy" © Copyright www.letscontrolit.com

   This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABIlLITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
   You received a copy of the GNU General Public License along with this program in file 'License.txt'.

   IDE download    : https://www.arduino.cc/en/Main/Software

   Source Code     : https://github.com/ESP8266nu/ESPEasy
   Support         : http://www.letscontrolit.com
   Discussion      : http://www.letscontrolit.com/forum/

   Additional information about licensing can be found at : http://www.gnu.org/licenses
  \*************************************************************************************************************************/

// This file incorporates work covered by the following copyright and permission notice:

/****************************************************************************************************************************\
  Arduino project "Nodo" © Copyright 2010..2015 Paul Tonkes

  This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
  You received a copy of the GNU General Public License along with this program in file 'License.txt'.

  Voor toelichting op de licentievoorwaarden zie    : http://www.gnu.org/licenses
  Uitgebreide documentatie is te vinden op          : http://www.nodo-domotica.nl
  Compiler voor deze programmacode te downloaden op : http://arduino.cc
  \*************************************************************************************************************************/

// ********************************************************************************
//   User specific configuration
// ********************************************************************************

#include "ArduinoEasy-Globals.h"
#include "_Plugin.h"
#include "_CPlugin.h"
#include "Misc.h"
#include "Networking.h"
#include "Serial.h"
#include "WebServer.h"

#ifdef STM32_F1
 #ifdef STM32_OFFICIAL
  void Reboot(void) {
   HAL_NVIC_SystemReset();
  }
 #else
  #include <libmaple/nvic.h> 
  void Reboot(void) {
   nvic_sys_reset();
  }
 #endif
#else
 void(*Reboot)(void) = 0;
#endif

EthernetServer MyWebServer(80); // WebServer

#if FEATURE_MQTT
 EthernetClient mqtt;
 PubSubClient MQTTclient(mqtt);
 long lastMQTTReconnectAttempt = 0;
#endif

#if FEATURE_UDP
 EthernetUDP portUDP; // syslog stuff
#endif

int deviceCount = -1;
int protocolCount = -1;

boolean printToWeb = false;
String printWebString = "";
boolean printToWebJSON = false;

float UserVar[VARS_PER_TASK * TASKS_MAX];
unsigned long RulesTimer[RULES_TIMER_MAX];

unsigned long timerSensor[TASKS_MAX];
unsigned long timer;
unsigned long timer100ms;
unsigned long timer1s;
unsigned long timerwd;
unsigned long lastSend;
byte cmd_within_mainloop = 0;
unsigned long connectionFailures;
unsigned long wdcounter = 0;

boolean WebLoggedIn = false;
int WebLoggedInTimer = 300;

boolean (*Plugin_ptr[PLUGIN_MAX])(byte, struct EventStruct*, String&);
byte Plugin_id[PLUGIN_MAX];

boolean (*CPlugin_ptr[CPLUGIN_MAX])(byte, struct EventStruct*, String&);
byte CPlugin_id[CPLUGIN_MAX];

String dummyString = "";

boolean systemOK = false;

unsigned long start = 0;
unsigned long elapsed = 0;
unsigned long loopCounter = 0;
unsigned long loopCounterLast = 0;
unsigned long loopCounterMax = 1;

unsigned long flashWrites = 0;

String eventBuffer = "";

int freeMem;

/*********************************************************************************************\
   SETUP
  \*********************************************************************************************/
void setup()
{
  Serial.begin(115200);
#if FEATURE_SD
  fileSystemCheck();
#endif
  emergencyReset();
  LoadSettings();
  
//if (Settings.UseRules){ResetFactory(); }// DEBUG ONLY!!

  ExtraTaskSettings.TaskIndex = 255; // make sure this is an unused nr to prevent cache load on boot

  // if different version, eeprom settings structure has changed. Full Reset needed
  // on a fresh ESP module eeprom values are set to 255. Version results into -1 (signed int)
  if (Settings.Version == VERSION && Settings.PID == ARDUINO_PROJECT_PID)
  {
    systemOK = true;
  }
  else
  {
    // Direct Serial is allowed here, since this is only an emergency task.
    Serial.print(F("\nPID:"));
    Serial.println(Settings.PID);
    Serial.print(F("Version:"));
    Serial.println(Settings.Version);
    Serial.println(F("INIT : Incorrect PID or version!"));
    delay(1000);
    ResetFactory(); // DEBUG !!
  }

  if (systemOK)
  {
    if (Settings.UseSerial)
      Serial.begin(Settings.BaudRate);

    if (Settings.Build != BUILD)
      BuildFixes();

#if defined(__AVR_ATmega2560__)      // Arduino Mega
#include <avr/boot.h>
byte id1 = boot_signature_byte_get(0x01);
byte id2 = boot_signature_byte_get(0x02);
byte mac[] = { 0x10, 0x00, 0x00, id1, id2, 0 };
#endif

    if (mac[5] == 0)
     mac[5] = Settings.Unit; // make sure every unit has a unique mac address    
    if (Settings.IP[0] == 0)
      Ethernet.begin(mac);
    else
      Ethernet.begin(mac, Settings.IP, Settings.DNS, Settings.Gateway, Settings.Subnet);
#if FEATURE_UDP
    // setup UDP
    if (Settings.UDPPort != 0)
      portUDP.begin(Settings.UDPPort);
    else
      portUDP.begin(123); // setup for NTP and other stuff if no user port is selected
#endif
    hardwareInit();
    PluginInit();
    CPluginInit();

    MyWebServer.begin();

#if FEATURE_MQTT
    // Setup MQTT Client
    byte ProtocolIndex = getProtocolIndex(Settings.Protocol);
    if (Protocol[ProtocolIndex].usesMQTT)
      MQTTConnect();
#endif

    String log = F("\nINIT : Booting Build nr:");
    log += BUILD;
    addLog(LOG_LEVEL_INFO, log);

#if FEATURE_UDP
    sendSysInfoUDP(3);
#endif
    // Setup timers
    byte bootMode = 0;
    if (bootMode == 0)
    {
      for (byte x = 0; x < TASKS_MAX; x++)
        if (Settings.TaskDeviceTimer[x] != 0)
          timerSensor[x] = millis() + 30000 + (x * Settings.MessageDelay);

      timer = millis() + 30000; // startup delay 30 sec
    }
    else
    {
      for (byte x = 0; x < TASKS_MAX; x++)
        timerSensor[x] = millis() + 0;
      timer = millis() + 0; // no startup from deepsleep wake up
    }

    timer100ms = millis() + 100; // timer for periodic actions 10 x per/sec
    timer1s = millis() + 1000; // timer for periodic actions once per/sec
    timerwd = millis() + 30000; // timer for watchdog once per 30 sec

#if FEATURE_UDP
    if (Settings.UseNTP)
      initTime();
#endif
    if (Settings.UseRules)
    {
      String event = F("System#Boot");
      rulesProcessing(event);
    }

    log = F("INIT : Boot OK");
    addLog(LOG_LEVEL_INFO, log);

  }
  else
  {
    Serial.println(F("Entered Rescue mode!"));
  }
}


/*********************************************************************************************\
   MAIN LOOP
  \*********************************************************************************************/
void loop()
{
  loopCounter++;

#if FEATURE_SERIAL
  if (Settings.UseSerial)
    if (Serial.available())
      if (!PluginCall(PLUGIN_SERIAL_IN, 0, dummyString))
        serial();
#endif
  if (systemOK)
  {
    if (millis() > timer100ms)
      run10TimesPerSecond();

    if (millis() > timer1s)
      runOncePerSecond();

    if (millis() > timerwd)
      runEach30Seconds();

    backgroundtasks();

  }
  else
    delay(1);
}


/*********************************************************************************************\
   Tasks that run 10 times per second
  \*********************************************************************************************/
void run10TimesPerSecond()
{
  start = micros();
  timer100ms = millis() + 100;
  PluginCall(PLUGIN_TEN_PER_SECOND, 0, dummyString);
  if (Settings.UseRules && eventBuffer.length() > 0)
  {
    rulesProcessing(eventBuffer); 
    eventBuffer = "";
  }
  elapsed = micros() - start;
}


/*********************************************************************************************\
   Tasks each second
  \*********************************************************************************************/
void runOncePerSecond()
{
  freeMem = FreeMem();

  timer1s = millis() + 1000;

  checkSensors();

  if (Settings.ConnectionFailuresThreshold)
    if (connectionFailures > Settings.ConnectionFailuresThreshold)
      delayedReboot(60);

  if (cmd_within_mainloop != 0)
  {
    switch (cmd_within_mainloop)
    {
      case CMD_REBOOT:
        {
          Reboot();
          break;
        }
    }
    cmd_within_mainloop = 0;
  }
#if FEATURE_UDP 
  if (Settings.UseNTP) // clock events
    checkTime();
#endif

  unsigned long timer = micros();
  PluginCall(PLUGIN_ONCE_A_SECOND, 0, dummyString);

  checkSystemTimers();

  if (Settings.UseRules) {
    rulesTimers();
  }
  
  timer = micros() - timer;

  if (SecuritySettings.Password[0] != 0)
  {
    if (WebLoggedIn)
      WebLoggedInTimer++;
    if (WebLoggedInTimer > 300)
      WebLoggedIn = false;
  }

  if (Settings.SerialLogLevel == 5)
  {
    Serial.print(F("10 ps:"));
    Serial.print(elapsed);
    Serial.print(F(" uS  1 ps:"));
    Serial.println(timer);
  }
  byte lanstate = LinkState();
  if (lastlanstate != lanstate) {
   String event = "";
   if (lanstate==1) {
      event = F("LAN#Connected");
   } else {
      event = F("LAN#Disconnected");
   }
   rulesProcessing(event);    
//   Serial.print(event);
   lastlanstate = lanstate;
  }
}

/*********************************************************************************************\
   Tasks each 30 seconds
  \*********************************************************************************************/
void runEach30Seconds()
{
  wdcounter++;
  timerwd = millis() + 30000;
  String log = F("WD   : Uptime ");
  log += wdcounter / 2;
  log += F(" ConnectFailures ");
  log += connectionFailures;
  log += F(" Freemem ");
  log += FreeMem();
  addLog(LOG_LEVEL_INFO, log);
#if FEATURE_UDP
  sendSysInfoUDP(1);
  refreshNodeList();
  Ethernet.maintain();
#endif    
  loopCounterLast = loopCounter;
  loopCounter = 0;
  if (loopCounterLast > loopCounterMax)
    loopCounterMax = loopCounterLast;
}


/*********************************************************************************************\
   Check sensor timers
  \*********************************************************************************************/
void checkSensors()
{
  for (byte x = 0; x < TASKS_MAX; x++)
  {
    if ((Settings.TaskDeviceTimer[x] != 0) && (millis() > timerSensor[x]))
    {
      timerSensor[x] = millis() + Settings.TaskDeviceTimer[x] * 1000;
      if (timerSensor[x] == 0) // small fix if result is 0, else timer will be stopped...
        timerSensor[x] = 1;
      SensorSendTask(x);
    }
  }
}


/*********************************************************************************************\
   send all sensordata
  \*********************************************************************************************/
void SensorSend()
{
  for (byte x = 0; x < TASKS_MAX; x++)
  {
    SensorSendTask(x);
  }
}


/*********************************************************************************************\
   send specific sensor task data
  \*********************************************************************************************/
void SensorSendTask(byte TaskIndex)
{
  if (Settings.TaskDeviceID[TaskIndex] != 0)
  {
    byte varIndex = TaskIndex * VARS_PER_TASK;

    boolean success = false;
    byte DeviceIndex = getDeviceIndex(Settings.TaskDeviceNumber[TaskIndex]);
    LoadTaskSettings(TaskIndex);

    struct EventStruct TempEvent;
    TempEvent.TaskIndex = TaskIndex;
    TempEvent.BaseVarIndex = varIndex;
    TempEvent.idx = Settings.TaskDeviceID[TaskIndex];
    TempEvent.sensorType = Device[DeviceIndex].VType;

    float preValue[VARS_PER_TASK]; // store values before change, in case we need it in the formula
    for (byte varNr = 0; varNr < VARS_PER_TASK; varNr++)
      preValue[varNr] = UserVar[varIndex + varNr];

    if (Settings.TaskDeviceDataFeed[TaskIndex] == 0) // only read local connected sensorsfeeds
      success = PluginCall(PLUGIN_READ, &TempEvent, dummyString);
    else
      success = true;

    if (success)
    {
      for (byte varNr = 0; varNr < VARS_PER_TASK; varNr++)
      {
        if (ExtraTaskSettings.TaskDeviceFormula[varNr][0] != 0)
        {
          String spreValue = String(preValue[varNr]);
          String formula = ExtraTaskSettings.TaskDeviceFormula[varNr];
          float value = UserVar[varIndex + varNr];
          float result = 0;
          String svalue = String(value);
          formula.replace(F("%pvalue%"), spreValue);
          formula.replace(F("%value%"), svalue);
          byte error = Calculate(formula.c_str(), &result);
          if (error == 0)
            UserVar[varIndex + varNr] = result;
        }
      }
      sendData(&TempEvent);
    }
  }
}


/*********************************************************************************************\
   set global system timer
  \*********************************************************************************************/
boolean setSystemTimer(unsigned long timer, byte plugin, byte Par1, byte Par2, byte Par3)
{
  // plugin number and par1 form a unique key that can be used to restart a timer
  // first check if a timer is not already running for this request
  boolean reUse = false;
  for (byte x = 0; x < SYSTEM_TIMER_MAX; x++)
    if (systemTimers[x].timer != 0)
    {
      if ((systemTimers[x].plugin == plugin) && (systemTimers[x].Par1 == Par1))
      {
        systemTimers[x].timer = millis() + timer;
        reUse = true;
        break;
      }
    }

  if (!reUse)
  {
    // find a new free timer slot...
    for (byte x = 0; x < SYSTEM_TIMER_MAX; x++)
      if (systemTimers[x].timer == 0)
      {
        systemTimers[x].timer = millis() + timer;
        systemTimers[x].plugin = plugin;
        systemTimers[x].Par1 = Par1;
        systemTimers[x].Par2 = Par2;
        systemTimers[x].Par3 = Par3;
        break;
      }
  }
}


/*********************************************************************************************\
   set global system command timer
  \*********************************************************************************************/
boolean setSystemCMDTimer(unsigned long timer, String& action)
{
  for (byte x = 0; x < SYSTEM_CMD_TIMER_MAX; x++)
    if (systemCMDTimers[x].timer == 0)
    {
      systemCMDTimers[x].timer = millis() + timer;
      systemCMDTimers[x].action = action;
      break;
    }
}


/*********************************************************************************************\
   check global system timers
  \*********************************************************************************************/
boolean checkSystemTimers()
{
  for (byte x = 0; x < SYSTEM_TIMER_MAX; x++)
    if (systemTimers[x].timer != 0)
    {
      if (timeOut(systemTimers[x].timer))
      {
        struct EventStruct TempEvent;
        TempEvent.Par1 = systemTimers[x].Par1;
        TempEvent.Par2 = systemTimers[x].Par2;
        TempEvent.Par3 = systemTimers[x].Par3;
        for (byte y = 0; y < PLUGIN_MAX; y++)
          if (Plugin_id[y] == systemTimers[x].plugin)
            Plugin_ptr[y](PLUGIN_TIMER_IN, &TempEvent, dummyString);
        systemTimers[x].timer = 0;
      }
    }

  for (byte x = 0; x < SYSTEM_CMD_TIMER_MAX; x++)
    if (systemCMDTimers[x].timer != 0)
      if (timeOut(systemCMDTimers[x].timer))
      {
        struct EventStruct TempEvent;
        parseCommandString(&TempEvent, systemCMDTimers[x].action);
        if (!PluginCall(PLUGIN_WRITE, &TempEvent, systemCMDTimers[x].action))
          ExecuteCommand(VALUE_SOURCE_SYSTEM, systemCMDTimers[x].action.c_str());
        systemCMDTimers[x].timer = 0;
        systemCMDTimers[x].action = "";
      }
}


/*********************************************************************************************\
   run background tasks
  \*********************************************************************************************/
void backgroundtasks()
{
  WebServerHandleClient();
#if FEATURE_MQTT
  if (!MQTTclient.loop()) {
    MQTTCheck();   // MQTT client is no longer connected. Attempt to reconnect
  }
#endif
  statusLED(false);
#if FEATURE_UDP
  checkUDP();
#endif
}
