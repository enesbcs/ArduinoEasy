#ifndef MISC_H
#define MISC_H
void addLog(byte loglevel, String& string);
void rulesProcessing(String& event);
unsigned long FreeMem(void);
void delayedReboot(int rebootDelay);
void rulesTimers();
void createRuleEvents(byte TaskIndex);
int Calculate(const char *input, float* result);
void emergencyReset();
boolean LoadSettings();
void ResetFactory(void);
void initTime();
void checkTime();
void LoadTaskSettings(byte TaskIndex);
unsigned long getNtpTime();
void SaveTaskSettings(byte TaskIndex);
#endif
