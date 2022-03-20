#pragma once

void DTCTRL_Task(timeUs_t t);

bool DTCTRL_Timeout();
float*  DTCTRL_GetValues();

void DTCTRL_HandleNewDirectMatrixThrustersValues(float* values);
void DTCTRL_HandlNewDirectThrustersValues(float* values);
