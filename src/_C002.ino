//#######################################################################################################
//########################### Controller Plugin 002: Domoticz MQTT ######################################
//#######################################################################################################

#if FEATURE_MQTT && defined(USES_C002)
#define strcasecmp_P(s1, s2) strcasecmp((s1), (s2))
#include "Misc.h"

#define CPLUGIN_002
#define CPLUGIN_ID_002         2
#define CPLUGIN_NAME_002       "Domoticz MQTT"

boolean CPlugin_002(byte function, struct EventStruct *event, String& string)
{
  boolean success = false;

  switch (function)
  {
    case CPLUGIN_PROTOCOL_ADD:
      {
        Protocol[++protocolCount].Number = CPLUGIN_ID_002;
        Protocol[protocolCount].usesMQTT = true;
        Protocol[protocolCount].usesTemplate = true;
        Protocol[protocolCount].usesAccount = true;
        Protocol[protocolCount].usesPassword = true;
        Protocol[protocolCount].defaultPort = 1883;
        break;
      }

    case CPLUGIN_GET_DEVICENAME:
      {
        string = F(CPLUGIN_NAME_002);
        break;
      }
      
    case CPLUGIN_PROTOCOL_TEMPLATE:
      {
        strcpy_P(Settings.MQTTsubscribe, PSTR("domoticz/out"));
        strcpy_P(Settings.MQTTpublish, PSTR("domoticz/in"));
        break;
      }

    case CPLUGIN_PROTOCOL_RECV:
      {
        char json[512];
        json[0] = 0;
        event->String2.toCharArray(json, 512);

        StaticJsonBuffer<512> jsonBuffer;
        JsonObject& root = jsonBuffer.parseObject(json);

        if (root.success())
        {
          long idx = root[F("idx")];
          float nvalue = root[F("nvalue")];
          long nvaluealt = root[F("nvalue")];
          //const char* name = root["name"]; // Not used
          //const char* svalue = root["svalue"]; // Not used
          const char* svalue1 = root[F("svalue1")];
          //const char* svalue2 = root["svalue2"]; // Not used
          //const char* svalue3 = root["svalue3"]; // Not used
          const char* switchtype = root[F("switchType")]; // Expect "On/Off" or "dimmer"
          if (nvalue == 0)
            nvalue = nvaluealt;
          if ((int)switchtype == 0)
            switchtype = "?";
  
          for (byte x = 0; x < TASKS_MAX; x++)
          {
            if (Settings.TaskDeviceID[x] == idx)
            {
              if (Settings.TaskDeviceNumber[x] == 1) // temp solution, if input switch, update state
              {
                String action = F("inputSwitchState,");
                action += x;
                action += F(",");
                action += nvalue;
                struct EventStruct TempEvent;
                parseCommandString(&TempEvent, action);
                PluginCall(PLUGIN_WRITE, &TempEvent, action);
              }
              if (Settings.TaskDeviceNumber[x] == 29) // temp solution, if plugin 029, set gpio
              {
                String action = "";
                int baseVar = x * VARS_PER_TASK;
                struct EventStruct TempEvent;
                if (strcasecmp_P(switchtype, PSTR("dimmer")) == 0)
                {
                  int pwmValue = UserVar[baseVar];
                  action = F("pwm,");
                  action += Settings.TaskDevicePin1[x];
                  action += F(",");
                  switch ((int)nvalue)
                  {
                    case 0:
                      pwmValue = 0;
                      break;
                    case 1:
                      pwmValue = UserVar[baseVar];
                      break;
                    case 2:
                      pwmValue = 10 * atol(svalue1);
                      UserVar[baseVar] = pwmValue;
                      break;
                  }
                  action += pwmValue;
                }
                else
                {
                  UserVar[baseVar] = nvalue;
                  action = F("gpio,");
                  action += Settings.TaskDevicePin1[x];
                  action += F(",");
                  action += nvalue;
                }
                parseCommandString(&TempEvent, action);
                PluginCall(PLUGIN_WRITE, &TempEvent, action);
              }
            }
          }
        }
        break;
      }

    case CPLUGIN_PROTOCOL_SEND:
      {
        StaticJsonBuffer<200> jsonBuffer;

        JsonObject& root = jsonBuffer.createObject();

        root["idx"] = event->idx;

        String values;
        char str[80];

        switch (event->sensorType)
        {
          case SENSOR_TYPE_SINGLE:                      // single value sensor, used for Dallas, BH1750, etc
            root[F("nvalue")] = 0;
            values = toString(UserVar[event->BaseVarIndex],ExtraTaskSettings.TaskDeviceValueDecimals[0]);
            values.toCharArray(str, 80);
            root[F("svalue")] =  str;
            break;
          case SENSOR_TYPE_LONG:                      // single LONG value, stored in two floats (rfid tags)
            root[F("nvalue")] = 0;
            values = (unsigned long)UserVar[event->BaseVarIndex] + ((unsigned long)UserVar[event->BaseVarIndex + 1] << 16);
            values.toCharArray(str, 80);
            root[F("svalue")] =  str;
            break;
          case SENSOR_TYPE_DUAL:                       // any sensor that uses two simple values
            root[F("nvalue")] = 0;
            values  = toString(UserVar[event->BaseVarIndex ],ExtraTaskSettings.TaskDeviceValueDecimals[0]);
            values += ";";
            values += toString(UserVar[event->BaseVarIndex + 1],ExtraTaskSettings.TaskDeviceValueDecimals[1]);
            values.toCharArray(str, 80);
            root[F("svalue")] =  str;
            break;            
          case SENSOR_TYPE_TEMP_HUM:                      // temp + hum + hum_stat, used for DHT11
            root[F("nvalue")] = 0;
            values  = toString(UserVar[event->BaseVarIndex],ExtraTaskSettings.TaskDeviceValueDecimals[0]);
            values += F(";");
            values += toString(UserVar[event->BaseVarIndex + 1],ExtraTaskSettings.TaskDeviceValueDecimals[1]);
            values += F(";0");
            values.toCharArray(str, 80);
            root[F("svalue")] =  str;
            break;
          case SENSOR_TYPE_TEMP_BARO:                      // temp + hum + hum_stat + bar + bar_fore, used for BMP085
            root[F("nvalue")] = 0;
            values  = toString(UserVar[event->BaseVarIndex],ExtraTaskSettings.TaskDeviceValueDecimals[0]);
            values += F(";0;0;");
            values += toString(UserVar[event->BaseVarIndex + 1],ExtraTaskSettings.TaskDeviceValueDecimals[1]);
            values += F(";0");
            values.toCharArray(str, 80);
            root[F("svalue")] =  str;
            break;
          case SENSOR_TYPE_TEMP_HUM_BARO:                      // temp + hum + hum_stat + bar + bar_fore, used for BME280
            root[F("nvalue")] = 0;
            values  = toString(UserVar[event->BaseVarIndex],ExtraTaskSettings.TaskDeviceValueDecimals[0]);
            values += F(";");
            values += toString(UserVar[event->BaseVarIndex + 1],ExtraTaskSettings.TaskDeviceValueDecimals[1]);
            values += F(";0;");
            values += toString(UserVar[event->BaseVarIndex + 2],ExtraTaskSettings.TaskDeviceValueDecimals[2]);
            values += ";0";
            values.toCharArray(str, 80);
            root[F("svalue")] =  str;
            break;
          case SENSOR_TYPE_SWITCH:
            root[F("command")] = String(F("switchlight"));
            if (UserVar[event->BaseVarIndex] == 0)
              root[F("switchcmd")] = F("Off");
            else
              root[F("switchcmd")] = F("On");
            break;
          case SENSOR_TYPE_DIMMER:
            root[F("command")] =  String(F("switchlight"));
            if (UserVar[event->BaseVarIndex] == 0)
              root[F("switchcmd")] = String(F("Off"));
            else
              root[F("Set%20Level")] = UserVar[event->BaseVarIndex];
            break;
        }

        char json[256];
        root.printTo(json, sizeof(json));
        String log = F("MQTT : ");
        log += json;
        addLog(LOG_LEVEL_DEBUG, json);

        String pubname = Settings.MQTTpublish;
        pubname.replace(F("%sysname%"), Settings.Name);
        pubname.replace(F("%tskname%"), ExtraTaskSettings.TaskDeviceName);
        pubname.replace(F("%id%"), String(event->idx));

        if (!MQTTclient.publish(pubname.c_str(), json, Settings.MQTTRetainFlag))
        {
          log = F("MQTT publish failed");
          addLog(LOG_LEVEL_DEBUG, json);
          MQTTConnect();
          connectionFailures++;
        }
        else if (connectionFailures)
          connectionFailures--;
        break;
      }

  }
  return success;
}
#endif
