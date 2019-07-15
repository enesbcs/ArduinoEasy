//#######################################################################################################
//#################################### Plugin 001: Input Switch #########################################
//#######################################################################################################
#ifdef USES_P001
// Adapted from ESP Easy, changes:
//   WebServer.arg() -> WebServer.arg()
//   Changed pin limit from 0-16 to 2-13
#define PLUGIN_001
#define PLUGIN_ID_001         1
#define PLUGIN_NAME_001       "Switch input"
#define PLUGIN_VALUENAME1_001 "Switch"

boolean Plugin_001(byte function, struct EventStruct *event, String& string)
{
  boolean success = false;
  static byte switchstate[TASKS_MAX];
  static byte outputstate[TASKS_MAX];

  switch (function)
  {
    case PLUGIN_DEVICE_ADD:
      {
        Device[++deviceCount].Number = PLUGIN_ID_001;
        Device[deviceCount].Type = DEVICE_TYPE_SINGLE;
        Device[deviceCount].VType = SENSOR_TYPE_SWITCH;
        Device[deviceCount].Ports = 0;
        Device[deviceCount].PullUpOption = true;
        Device[deviceCount].InverseLogicOption = true;
        Device[deviceCount].FormulaOption = false;
        Device[deviceCount].ValueCount = 1;
        Device[deviceCount].SendDataOption = true;
        Device[deviceCount].TimerOption = true;
        Device[deviceCount].TimerOptional = true;
        Device[deviceCount].GlobalSyncOption = true;
        break;
      }

    case PLUGIN_GET_DEVICENAME:
      {
        string = F(PLUGIN_NAME_001);
        break;
      }

    case PLUGIN_GET_DEVICEVALUENAMES:
      {
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_001));
        break;
      }

    case PLUGIN_WEBFORM_LOAD:
      {
        byte choice = Settings.TaskDevicePluginConfig[event->TaskIndex][0];
        String options[2];
        options[0] = F("Switch");
        options[1] = F("Dimmer");
        int optionValues[2];
        optionValues[0] = 1;
        optionValues[1] = 2;
        string += F("<TR><TD>Switch Type:<TD><select name='plugin_001_type'>");
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
          char tmpString[128];
          sprintf_P(tmpString, PSTR("<TR><TD>Dim value:<TD><input type='text' name='plugin_001_dimvalue' value='%u'>"), Settings.TaskDevicePluginConfig[event->TaskIndex][1]);
          string += tmpString;
        }

        choice = Settings.TaskDevicePluginConfig[event->TaskIndex][2];
        String buttonOptions[3];
        buttonOptions[0] = F("Normal Switch");
        buttonOptions[1] = F("Push Button Active Low");
        buttonOptions[2] = F("Push Button Active High");
        int buttonOptionValues[3];
        buttonOptionValues[0] = 0;
        buttonOptionValues[1] = 1;
        buttonOptionValues[2] = 2;
        string += F("<TR><TD>Switch Button Type:<TD><select name='plugin_001_button'>");
        for (byte x = 0; x < 3; x++)
        {
          string += F("<option value='");
          string += buttonOptionValues[x];
          string += "'";
          if (choice == buttonOptionValues[x])
            string += F(" selected");
          string += ">";
          string += buttonOptions[x];
          string += F("</option>");
        }
        string += F("</select>");

        string += F("<TR><TD>Send Boot state:<TD>");
        if (Settings.TaskDevicePluginConfig[event->TaskIndex][3])
          string += F("<input type=checkbox name=plugin_001_boot checked>");
        else
          string += F("<input type=checkbox name=plugin_001_boot>");

        success = true;
        break;
      }

    case PLUGIN_WEBFORM_SAVE:
      {
        String plugin1 = WebServer.arg(F("plugin_001_type"));
        Settings.TaskDevicePluginConfig[event->TaskIndex][0] = plugin1.toInt();
        if (Settings.TaskDevicePluginConfig[event->TaskIndex][0] == 2)
        {
          String plugin2 = WebServer.arg(F("plugin_001_dimvalue"));
          Settings.TaskDevicePluginConfig[event->TaskIndex][1] = plugin2.toInt();
        }
        String plugin3 = WebServer.arg(F("plugin_001_button"));
        Settings.TaskDevicePluginConfig[event->TaskIndex][2] = plugin3.toInt();

        String plugin4 = WebServer.arg(F("plugin_001_boot"));
        Settings.TaskDevicePluginConfig[event->TaskIndex][3] = (plugin4 == "on");

        success = true;
        break;
      }

    case PLUGIN_INIT:
      {
        if (Settings.TaskDevicePin1PullUp[event->TaskIndex])
          pinMode(Settings.TaskDevicePin1[event->TaskIndex], INPUT_PULLUP);
        else
          pinMode(Settings.TaskDevicePin1[event->TaskIndex], INPUT);

        setPinState(PLUGIN_ID_001, Settings.TaskDevicePin1[event->TaskIndex], PIN_MODE_INPUT, 0);

        switchstate[event->TaskIndex] = digitalRead(Settings.TaskDevicePin1[event->TaskIndex]);
        outputstate[event->TaskIndex] = switchstate[event->TaskIndex];

        // if boot state must be send, inverse default state
        if (Settings.TaskDevicePluginConfig[event->TaskIndex][3])
        {
          switchstate[event->TaskIndex] = !switchstate[event->TaskIndex];
          outputstate[event->TaskIndex] = !outputstate[event->TaskIndex];
        }
        success = true;
        break;
      }

    case PLUGIN_TEN_PER_SECOND:
      {
        byte state = digitalRead(Settings.TaskDevicePin1[event->TaskIndex]);
        if (state != switchstate[event->TaskIndex])
        {
          switchstate[event->TaskIndex] = state;
          byte currentOutputState = outputstate[event->TaskIndex];

          if (Settings.TaskDevicePluginConfig[event->TaskIndex][2] == 0) //normal switch
            outputstate[event->TaskIndex] = state;
          else
          {
            if (Settings.TaskDevicePluginConfig[event->TaskIndex][2] == 1)  // active low push button
            {
              if (state == 0)
                outputstate[event->TaskIndex] = !outputstate[event->TaskIndex];
            }
            else  // active high push button
            {
              if (state == 1)
                outputstate[event->TaskIndex] = !outputstate[event->TaskIndex];
            }
          }

          // send if output needs to be changed
          if (currentOutputState != outputstate[event->TaskIndex])
          {
            byte sendState = outputstate[event->TaskIndex];
            if (Settings.TaskDevicePin1Inversed[event->TaskIndex])
              sendState = !outputstate[event->TaskIndex];
            UserVar[event->BaseVarIndex] = sendState;
            event->sensorType = SENSOR_TYPE_SWITCH;
            if ((sendState == 1) && (Settings.TaskDevicePluginConfig[event->TaskIndex][0] == 2))
            {
              event->sensorType = SENSOR_TYPE_DIMMER;
              UserVar[event->BaseVarIndex] = Settings.TaskDevicePluginConfig[event->TaskIndex][1];
            }
            String log = F("SW   : State ");
            log += sendState;
            addLog(LOG_LEVEL_INFO, log);
            sendData(event);
          }
        }
        success = true;
        break;
      }

    case PLUGIN_READ:
      {
        // We do not actually read the pin state as this is already done 10x/second
        // Instead we just send the last known state stored in Uservar
        String log = F("SW   : State ");
        log += UserVar[event->BaseVarIndex];
        addLog(LOG_LEVEL_INFO, log);
        success = true;
        break;
      }

    case PLUGIN_WRITE:
      {
        String log = "";
        String command = parseString(string, 1);

        if (command == F("gpio"))
        {
          success = true;
          if (Plugin_001_updatable_pin(event->Par1))
          {
            pinMode(event->Par1, OUTPUT);
            digitalWrite(event->Par1, event->Par2);
            setPinState(PLUGIN_ID_001, event->Par1, PIN_MODE_OUTPUT, event->Par2);
            log = String(F("SW   : GPIO ")) + String(event->Par1) + String(F(" Set to ")) + String(event->Par2);
            addLog(LOG_LEVEL_INFO, log);
            SendStatus(event->Source, getPinStateJSON(SEARCH_PIN_STATE, PLUGIN_ID_001, event->Par1, log, 0));
          }
        }

        if (command == F("pwm"))
        {
          success = true;
          if (Plugin_001_updatable_pin(event->Par1))
          {
            pinMode(event->Par1, OUTPUT);

            if(event->Par3 != 0)
            {
              byte prev_mode;
              uint16_t prev_value;
              getPinState(PLUGIN_ID_001, event->Par1, &prev_mode, &prev_value);
              if(prev_mode != PIN_MODE_PWM)
                prev_value = 0;

              int32_t step_value = ((event->Par2 - prev_value) << 12) / event->Par3;
              int32_t curr_value = prev_value << 12;
              int16_t new_value;
              int i = event->Par3;
              while(i--){
                curr_value += step_value;
                new_value = (uint16_t)(curr_value >> 12);
                analogWrite(event->Par1, new_value);
                delay(1);
              }
            }

            analogWrite(event->Par1, event->Par2);
            setPinState(PLUGIN_ID_001, event->Par1, PIN_MODE_PWM, event->Par2);
            log = String(F("SW   : GPIO ")) + String(event->Par1) + String(F(" Set PWM to ")) + String(event->Par2);
            addLog(LOG_LEVEL_INFO, log);
            SendStatus(event->Source, getPinStateJSON(SEARCH_PIN_STATE, PLUGIN_ID_001, event->Par1, log, 0));
          }
        }

        if (command == F("pulse"))
        {
          success = true;
          if (Plugin_001_updatable_pin(event->Par1))
          {
            pinMode(event->Par1, OUTPUT);
            digitalWrite(event->Par1, event->Par2);
            delay(event->Par3);
            digitalWrite(event->Par1, !event->Par2);
            setPinState(PLUGIN_ID_001, event->Par1, PIN_MODE_OUTPUT, event->Par2);
            log = String(F("SW   : GPIO ")) + String(event->Par1) + String(F(" Pulsed for ")) + String(event->Par3) + String(F(" mS"));
            addLog(LOG_LEVEL_INFO, log);
            SendStatus(event->Source, getPinStateJSON(SEARCH_PIN_STATE, PLUGIN_ID_001, event->Par1, log, 0));
          }
        }

        if (command == F("longpulse"))
        {
          success = true;
          if (Plugin_001_updatable_pin(event->Par1))
          {
            pinMode(event->Par1, OUTPUT);
            digitalWrite(event->Par1, event->Par2);
            setPinState(PLUGIN_ID_001, event->Par1, PIN_MODE_OUTPUT, event->Par2);
            setSystemTimer(event->Par3 * 1000, PLUGIN_ID_001, event->Par1, !event->Par2, 0);
            log = String(F("SW   : GPIO ")) + String(event->Par1) + String(F(" Pulse set for ")) + String(event->Par3) + String(F(" S"));
            addLog(LOG_LEVEL_INFO, log);
            SendStatus(event->Source, getPinStateJSON(SEARCH_PIN_STATE, PLUGIN_ID_001, event->Par1, log, 0));
          }
        }

        if (command == F("servo"))
        {
          success = true;
          if (event->Par1 >= 0 && event->Par1 <= 2)
            switch (event->Par1)
            {
              case 1:
                // todo myservo1.attach(event->Par2);
                // todo myservo1.write(event->Par3);
                break;
              case 2:
                // todo myservo2.attach(event->Par2);
                // todo myservo2.write(event->Par3);
                break;
            }
          setPinState(PLUGIN_ID_001, event->Par2, PIN_MODE_SERVO, event->Par3);
          log = String(F("SW   : GPIO ")) + String(event->Par2) + String(F(" Servo set to ")) + String(event->Par3);
          addLog(LOG_LEVEL_INFO, log);
          SendStatus(event->Source, getPinStateJSON(SEARCH_PIN_STATE, PLUGIN_ID_001, event->Par2, log, 0));
        }

        if (command == F("status"))
        {
          if (parseString(string, 2) == F("gpio"))
          {
            success = true;
            SendStatus(event->Source, getPinStateJSON(SEARCH_PIN_STATE, PLUGIN_ID_001, event->Par2, dummyString, 0));
          }
        }

        if (command == F("inputswitchstate"))
        {
          success = true;
          UserVar[event->Par1 * VARS_PER_TASK] = event->Par2;
          outputstate[event->Par1] = event->Par2;
        }
#if FEATURE_NOISE        
        if (command == F("rtttl"))
        {
          success = true;
          if (Plugin_001_updatable_pin(event->Par1))
          {
            pinMode(event->Par1, OUTPUT);
            // char sng[1024] ="";
            String tmpString=string;
            tmpString.replace("-","#");
            // tmpString.toCharArray(sng, 1024);
            play_rtttl(event->Par1, tmpString.c_str());
            setPinState(PLUGIN_ID_001, event->Par1, PIN_MODE_OUTPUT, event->Par2);
            log = String(F("SW   : ")) + string;
            addLog(LOG_LEVEL_INFO, log);
            SendStatus(event->Source, getPinStateJSON(SEARCH_PIN_STATE, PLUGIN_ID_001, event->Par1, log, 0));
          }
        }

        //play a tone on pin par1, with frequency par2 and duration par3.
        if (command == F("tone"))
        {
          success = true;
          if (Plugin_001_updatable_pin(event->Par1))          
          {
            pinMode(event->Par1, OUTPUT);
            tone(event->Par1, event->Par2, event->Par3);
            setPinState(PLUGIN_ID_001, event->Par1, PIN_MODE_OUTPUT, event->Par2);
            log = String(F("SW   : ")) + string;
            addLog(LOG_LEVEL_INFO, log);
            SendStatus(event->Source, getPinStateJSON(SEARCH_PIN_STATE, PLUGIN_ID_001, event->Par1, log, 0));
          }
         }
#endif
        break;
      }

    case PLUGIN_TIMER_IN:
      {
        digitalWrite(event->Par1, event->Par2);
        setPinState(PLUGIN_ID_001, event->Par1, PIN_MODE_OUTPUT, event->Par2);
        break;
      }
  }
  return success;
}

boolean Plugin_001_updatable_pin(int pin) {
#ifdef STM32_F1 // STM32 F1 detected  
 #ifdef STM32_OFFICIAL
  #if defined(STM32F103xB)
   return (pin>=0 || pin<= 34);
  #endif
 #else
  #if defined(MCU_STM32F103TB) || defined(MCU_STM32F103CB) || defined(MCU_STM32F103RB) || defined(MCU_STM32F103VB)
   return (pin>=PA0 || pin<= PC15);
  #else
   return (pin>=PA0 || pin<= PD2);
  #endif
 #endif
#else // fallback
  return pin == 3 || ( pin >= 5 && pin <= 9) || ( pin >= 14 && pin <= 49) || ( pin >= 56 && pin <= 69);
#endif
 return false;
}

#if FEATURE_NOISE

void play_rtttl(uint8_t _pin, const char *p )
{
  #define OCTAVE_OFFSET 0
  // Absolutely no error checking in here

  int notes[] = { 0,
    262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494,
    523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988,
    1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976,
    2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951
  };

  byte default_dur = 4;
  byte default_oct = 6;
  int bpm = 63;
  int num;
  long wholenote;
  long duration;
  byte note;
  byte scale;

  // format: d=N,o=N,b=NNN:
  // find the start (skip name, etc)

  while(*p != ':') p++;    // ignore name
  p++;                     // skip ':'

  // get default duration
  if(*p == 'd')
  {
    p++; p++;              // skip "d="
    num = 0;
    while(isdigit(*p))
    {
      num = (num * 10) + (*p++ - '0');
    }
    if(num > 0) default_dur = num;
    p++;                   // skip comma
  }

  // get default octave
  if(*p == 'o')
  {
    p++; p++;              // skip "o="
    num = *p++ - '0';
    if(num >= 3 && num <=7) default_oct = num;
    p++;                   // skip comma
  }

  // get BPM
  if(*p == 'b')
  {
    p++; p++;              // skip "b="
    num = 0;
    while(isdigit(*p))
    {
      num = (num * 10) + (*p++ - '0');
    }
    bpm = num;
    p++;                   // skip colon
  }

  // BPM usually expresses the number of quarter notes per minute
  wholenote = (60 * 1000L / bpm) * 4;  // this is the time for whole note (in milliseconds)

  // now begin note loop
  while(*p)
  {
    // first, get note duration, if available
    num = 0;
    while(isdigit(*p))
    {
      num = (num * 10) + (*p++ - '0');
    }

    if (num) duration = wholenote / num;
    else duration = wholenote / default_dur;  // we will need to check if we are a dotted note after

    // now get the note
    note = 0;

    switch(*p)
    {
      case 'c':
        note = 1;
        break;
      case 'd':
        note = 3;
        break;
      case 'e':
        note = 5;
        break;
      case 'f':
        note = 6;
        break;
      case 'g':
        note = 8;
        break;
      case 'a':
        note = 10;
        break;
      case 'b':
        note = 12;
        break;
      case 'p':
      default:
        note = 0;
    }
    p++;

    // now, get optional '#' sharp
    if(*p == '#')
    {
      note++;
      p++;
    }

    // now, get optional '.' dotted note
    if(*p == '.')
    {
      duration += duration/2;
      p++;
    }

    // now, get scale
    if(isdigit(*p))
    {
      scale = *p - '0';
      p++;
    }
    else
    {
      scale = default_oct;
    }

    scale += OCTAVE_OFFSET;

    if(*p == ',')
      p++;       // skip comma for next note (or we may be at the end)

    // now play the note
    if(note)
    {
      tone(_pin, notes[(scale - 4) * 12 + note], duration);
#ifdef STM32_F1
      delay(duration); // FOR some reason STM32 tone needs a delay after tone! not sure about other platforms..
#endif      
    }
    else
    {
      delay(duration/10);
    }
  }
}
#endif
#endif
