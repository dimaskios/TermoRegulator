#ifndef RELAY_H
#define RELAY_H

#include "defines.h"
#include <avr/interrupt.h>
#include "settings.h"

#define N_PARTS 50

#define RELAY_ON(N) Off(RELAY_PORT, N) 
#define RELAY_OFF(N) On(RELAY_PORT, N)

typedef enum {STATE0, STATE1, STATE2, STATE3, STATE4, STATE5, STATE6, STATE7, STATE8} RelayState;

typedef struct
{
	uint8_t N;
	uint8_t isRelayOn[3];
}RelayNormal; 

typedef struct
{
	uint8_t N;
	AllSettings * allSettings;
	uint8_t activeSettings;
	
	uint8_t isRelayOn[3];
	RelayState state;
	
	uint8_t duty;
	uint16_t counter_cycle;
	uint16_t counter_cycle2;
	uint16_t counter_minute;
}RelayPWM; 

void Relay_Normal_Init(RelayNormal * relay, uint8_t N);
void Relay_Normal_Process(RelayNormal * relay, AllSettings * allSettings, float * temp);

void Relay_PWM_Init(RelayPWM * relay, AllSettings * allSettings, uint8_t N);
void Relay_PWM_Process(RelayPWM * relay, float * temp, uint8_t PWMSignal);
void Relay_PWM_Interrupt(RelayPWM * relay);
void Relay_PWM_Reset_Counter_Cycle(RelayPWM * relay);
void Relay_PWM_Reset_Counter_M(RelayPWM * relay);
PWMSettings * Relay_PWM_GetCurrentPWMSettings(RelayPWM * relay);
void Relay_PWM_Find_Active_Settings(RelayPWM * relay);
void Relay_PWM_Set_Duty_Cicle(RelayPWM * relay, uint8_t duty);


//void Relay_Init();
/*void HandleRelay(SettingsType all_settings, float * temp, uint8_t relay);
void HandleRelayNormal(SettingsType all_settings, float * temp, uint8_t relay);

void HandleRelayPWM(SettingsType all_settings, float * temp, uint8_t relay);
void resetPWMParts(SettingsType all_settings);*/

uint8_t readPWMSignal();


//UP - heat
//Down - cold

#endif

