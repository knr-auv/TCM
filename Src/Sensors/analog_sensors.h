#include <stdbool.h>

void ANALOG_Enable();
bool ANALOG_IsEnabled();
void ANALOG_Disable();

void ANALOG_beginConversion(void);

float ANALOG_onboardTemp(void);
float ANALOG_CPUTemp(void);

float ANALOG_onboardHumidity(void);