#pragma once

void DTCTRL_Task(timeUs_t t);

bool DTCTRL_Timeout();
float*  DTCTRL_GetValues();

void DTCTRL_HandleNewDirectMatrixActuatorsValues(float* values);
void DTCTRL_HandlNewDirectActuatorsValues(float* values);

void DTCTRL_Enable();
void DTCTRL_Disable();