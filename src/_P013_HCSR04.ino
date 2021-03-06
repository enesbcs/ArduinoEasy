//#######################################################################################################
//#################################### Plugin 013: HC-SR04 ##############################################
//#######################################################################################################
#ifdef USES_P013
//MyMessage *msgDist013;

#define PLUGIN_013
#define PLUGIN_ID_013        13
#define PLUGIN_NAME_013       "Ultrasonic Sensor - HC-SR04"
#define PLUGIN_VALUENAME1_013 "Distance"

boolean Plugin_013_init = false;
volatile unsigned long Plugin_013_timer = 0;
volatile unsigned long Plugin_013_state = 0;
byte Plugin_013_TRIG_Pin = 0;
byte Plugin_013_IRQ_Pin = 0;

boolean Plugin_013(byte function, struct EventStruct *event, String& string)
{
  static byte switchstate[TASKS_MAX];
  boolean success = false;

  switch (function)
  {
    case PLUGIN_DEVICE_ADD:
      {
        Device[++deviceCount].Number = PLUGIN_ID_013;
        Device[deviceCount].Type = DEVICE_TYPE_DUAL;
        Device[deviceCount].VType = SENSOR_TYPE_SINGLE;
        Device[deviceCount].Ports = 0;
        Device[deviceCount].PullUpOption = false;
        Device[deviceCount].InverseLogicOption = false;
        Device[deviceCount].FormulaOption = false;
        Device[deviceCount].ValueCount = 1;
        break;
      }

    case PLUGIN_GET_DEVICENAME:
      {
        string = F(PLUGIN_NAME_013);
        break;
      }

    case PLUGIN_GET_DEVICEVALUENAMES:
      {
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_013));
        break;
      }
    

    case PLUGIN_WEBFORM_LOAD:
      {
        byte choice = Settings.TaskDevicePluginConfig[event->TaskIndex][0];
        String options[2];
        options[0] = F("Value");
        options[1] = F("State");
        int optionValues[2];
        optionValues[0] = 1;
        optionValues[1] = 2;
        string += F("<TR><TD>Mode:<TD><select name='plugin_013_mode'>");
        for (byte x = 0; x < 2; x++)
        {
          string += F("<option value='");
          string += optionValues[x];
          string += "'";
          if (choice == optionValues[x])
            string += F(" selected");
          string += ">";
          string += options[x];
          string += F("</option>");
        }
        string += F("</select>");

        if (Settings.TaskDevicePluginConfig[event->TaskIndex][0] == 2)
        {
          char tmpString[80];
          sprintf_P(tmpString, PSTR("<TR><TD>Threshold:<TD><input type='text' name='plugin_013_threshold' value='%u'>"), Settings.TaskDevicePluginConfig[event->TaskIndex][1]);
          string += tmpString;
        }
        success = true;
        break;
      }

    case PLUGIN_WEBFORM_SAVE:
      {
        String plugin1 = WebServer.arg("plugin_013_mode");
        Settings.TaskDevicePluginConfig[event->TaskIndex][0] = plugin1.toInt();
        if (Settings.TaskDevicePluginConfig[event->TaskIndex][0] == 2)
        {
          String plugin2 = WebServer.arg("plugin_013_threshold");
          Settings.TaskDevicePluginConfig[event->TaskIndex][1] = plugin2.toInt();
        }
        success = true;
        break;
      }

    case PLUGIN_INIT:
      {
/*        if (!msgDist013)
          msgDist013 = new MyMessage(event->BaseVarIndex, V_DISTANCE);
        present(event->BaseVarIndex, S_DISTANCE);
        Serial.print("Present Dist: ");
        Serial.println(event->BaseVarIndex);*/

        Plugin_013_init = true;
        pinMode(Settings.TaskDevicePin1[event->TaskIndex], OUTPUT);
        pinMode(Settings.TaskDevicePin2[event->TaskIndex], INPUT_PULLUP);
        Plugin_013_IRQ_Pin = Settings.TaskDevicePin2[event->TaskIndex];
        attachInterrupt(Settings.TaskDevicePin2[event->TaskIndex], Plugin_013_interrupt, CHANGE);
        success = true;
        break;
      }

    case PLUGIN_READ: // If we select value mode, read and send the value based on global timer
      {
        if (Settings.TaskDevicePluginConfig[event->TaskIndex][0] == 1)
        {
          Plugin_013_TRIG_Pin = Settings.TaskDevicePin1[event->TaskIndex];
          float value = Plugin_013_read();
          String log = F("SR04 : Distance: ");
          if (value != -1)
          {
            UserVar[event->BaseVarIndex] = (float)Plugin_013_timer / 58;
            log += UserVar[event->BaseVarIndex];

//            send(msgDist013->set((uint16_t)UserVar[event->BaseVarIndex]));
                  
            success = true;
          }
          else
            log += F("SR04 : Distance: No reading!");

        addLog(LOG_LEVEL_INFO,log);
        }
        break;
      }

    case PLUGIN_TEN_PER_SECOND: // If we select state mode, do more frequent checks and send only state changes
      {
        if (Settings.TaskDevicePluginConfig[event->TaskIndex][0] == 2)
        {
          Plugin_013_TRIG_Pin = Settings.TaskDevicePin1[event->TaskIndex];
          byte state = 0;
          float value = Plugin_013_read();
          if (value != -1)
          {
            if (value < Settings.TaskDevicePluginConfig[event->TaskIndex][1])
              state = 1;
            if (state != switchstate[event->TaskIndex])
            {
              String log = F("SR04 : State ");
              log += state;
              addLog(LOG_LEVEL_INFO,log);
              switchstate[event->TaskIndex] = state;
              UserVar[event->BaseVarIndex] = state;
              event->sensorType = SENSOR_TYPE_SWITCH;
              
//              send(msgDist013->set((uint16_t)UserVar[event->BaseVarIndex]));

            }
          }
        }
        success = true;
        break;
      }
  }
  return success;
}

/*********************************************************************/
float Plugin_013_read()
/*********************************************************************/
{
  float value = -1;
  Plugin_013_timer = 0;
  Plugin_013_state = 0;
  noInterrupts();
  digitalWrite(Plugin_013_TRIG_Pin, LOW);
  delayMicroseconds(2);
  digitalWrite(Plugin_013_TRIG_Pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(Plugin_013_TRIG_Pin, LOW);
  interrupts();

  delay(25);  // wait for measurement to finish (max 400 cm * 58 uSec = 23uSec)
  if (Plugin_013_state == 2)
  {
    value = (float)Plugin_013_timer / 58;
  }
  return value;
}

/*********************************************************************/
void Plugin_013_interrupt()
/*********************************************************************/
{
  byte pinState = digitalRead(Plugin_013_IRQ_Pin);
  if (pinState == 1) // Start of pulse
  {
    Plugin_013_state = 1;
    Plugin_013_timer = micros();
  }
  else // End of pulse, calculate timelapse between start & end
  {
    Plugin_013_state = 2;
    Plugin_013_timer = micros() - Plugin_013_timer;
  }
}

#endif