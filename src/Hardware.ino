#include "Misc.h"

/********************************************************************************************\
* Initialize specific hardware setings (only global ones, others are set through devices)
\*********************************************************************************************/

void hardwareInit()
{

  // set GPIO pins state if not set to default
  for (byte x=0; x < 17; x++)
    if (Settings.PinBootStates[x] != 0)
      switch(Settings.PinBootStates[x])
      {
        case 1:
          pinMode(x,OUTPUT);
          digitalWrite(x,LOW);
          setPinState(1, x, PIN_MODE_OUTPUT, LOW);
          break;
        case 2:
          pinMode(x,OUTPUT);
          digitalWrite(x,HIGH);
          setPinState(1, x, PIN_MODE_OUTPUT, HIGH);
          break;
        case 3:
          pinMode(x,INPUT);
          setPinState(1, x, PIN_MODE_INPUT, 0);
          break;
        case 4:
          pinMode(x,INPUT_PULLDOWN);
          setPinState(1, x, PIN_MODE_INPUT, 0);
          break;
        case 5:
          pinMode(x,INPUT_PULLUP);
          setPinState(1, x, PIN_MODE_INPUT, 1);
          break;
      }

#if FEATURE_I2C
  String log = F("INIT : I2C");
  addLog(LOG_LEVEL_INFO, log);
  Wire.begin();
#endif  
}
