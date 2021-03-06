//#######################################################################################################
//#################################### Plugin 038: NeoPixel Basic #######################################
//#######################################################################################################

// Command: NeoPixel <led nr>,<red>,<green>,<blue>
#ifdef USES_P038
#include "Adafruit_NeoPixel-ANDnXOR.h"
// Download library from: https://github.com/ANDnXOR/Adafruit_NeoPixel-ANDnXOR.git
Adafruit_NeoPixel *Plugin_038_pixels;

int MaxPixels = 0;

#define PLUGIN_038
#define PLUGIN_ID_038         38
#define PLUGIN_NAME_038       "NeoPixel - Basic"
#define PLUGIN_VALUENAME1_038 ""

boolean Plugin_038(byte function, struct EventStruct *event, String& string)
{
  boolean success = false;

  switch (function)
  {

    case PLUGIN_DEVICE_ADD:
      {
        Device[++deviceCount].Number = PLUGIN_ID_038;
        Device[deviceCount].Type = DEVICE_TYPE_SINGLE;
        Device[deviceCount].Custom = true;
        Device[deviceCount].TimerOption = false;
        break;
      }

    case PLUGIN_GET_DEVICENAME:
      {
        string = F(PLUGIN_NAME_038);
        break;
      }

    case PLUGIN_GET_DEVICEVALUENAMES:
      {
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_038));
        break;
      }

    case PLUGIN_WEBFORM_LOAD:
      {
        char tmpString[128];
        sprintf_P(tmpString, PSTR("<TR><TD>Led Count:<TD><input type='text' name='plugin_038_leds' size='3' value='%u'>"), Settings.TaskDevicePluginConfig[event->TaskIndex][0]);
        string += tmpString;

        string += F("<TR><TD>GPIO:<TD>");
        addPinSelect(false, string, "taskdevicepin1", Settings.TaskDevicePin1[event->TaskIndex]);

        success = true;
        break;
      }

    case PLUGIN_WEBFORM_SAVE:
      {
        String plugin1 = WebServer.arg(F("plugin_038_leds"));
        Settings.TaskDevicePluginConfig[event->TaskIndex][0] = plugin1.toInt();
        MaxPixels = Settings.TaskDevicePluginConfig[event->TaskIndex][0];
        success = true;
        break;
      }

    case PLUGIN_INIT:
      {
        if (!Plugin_038_pixels)
        {
          MaxPixels = Settings.TaskDevicePluginConfig[event->TaskIndex][0];
          Plugin_038_pixels = new Adafruit_NeoPixel(MaxPixels, Settings.TaskDevicePin1[event->TaskIndex], NEO_GRB + NEO_KHZ800);
          Plugin_038_pixels->begin(); // This initializes the NeoPixel library.
        }
        success = true;
        break;
      }

    case PLUGIN_WRITE:
      {
        if (Plugin_038_pixels)
        {
          String tmpString  = string;
          int argIndex = tmpString.indexOf(',');
          if (argIndex)
            tmpString = tmpString.substring(0, argIndex);

          if (tmpString.equalsIgnoreCase(F("NeoPixel")))
          {
            char Line[80];
            char TmpStr1[80];
            TmpStr1[0] = 0;
            string.toCharArray(Line, 80);
            int Par4 = 0;
            if (GetArgv(Line, TmpStr1, 5)) Par4 = str2int(TmpStr1);
            Plugin_038_pixels->setPixelColor(event->Par1 - 1, Plugin_038_pixels->Color(event->Par2, event->Par3, Par4));
            Plugin_038_pixels->show(); // This sends the updated pixel color to the hardware.
            success = true;
          }
          if (tmpString.equalsIgnoreCase(F("NeoPixelAll")))
          {
            for (int i = 0; i < MaxPixels; i++)
            {
                Plugin_038_pixels->setPixelColor(i, Plugin_038_pixels->Color(event->Par1, event->Par2, event->Par3));
            }
            Plugin_038_pixels->show();
            success = true;
          }
          if (tmpString.equalsIgnoreCase(F("NeoPixelLine")))
          {
            char Line[80];
            char TmpStr1[80];
            TmpStr1[0] = 0;
            string.toCharArray(Line, 80);
            int Par4 = 0;
            int Par5 = 0;
            if (GetArgv(Line, TmpStr1, 5)) Par4 = str2int(TmpStr1);
            if (GetArgv(Line, TmpStr1, 6)) Par5 = str2int(TmpStr1);
            for (int i = event->Par1 - 1; i < event->Par2; i++)
            {
              Plugin_038_pixels->setPixelColor(i, Plugin_038_pixels->Color(event->Par3, Par4, Par5));
            }
            Plugin_038_pixels->show();
            success = true;
           }          
          
        }
        break;
      }

  }
  return success;
}
#endif
