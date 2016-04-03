
#include "defines.h"
#include "lcd.h"
#include "ds18b20.h"
#include "settings.h"
#include "relay.h"

#define BTNDDR DDRC
#define BTNPORT PORTC
#define BTNPIN PINC

#define BTN1 0
#define BTN2 2
#define BTN3 1
#define IS_BTN_PRESSED(BTN)  ((BTNPIN & (1<<BTN))==0)

#define BTN_long_time 1000

#define LED_OFF_TIME 10000



#define STATUS_DEFAULT 0
#define STATUS_TERM 10
#define STATUS_SETTINGS 20
#define STATUS_RELAY 30
#define STATUS_RESET 40

//#define NUMBER_OF_SETTINGS(RELAY) (RELAY==PWN_RELAY) ? 6 : 4
//#define NUMBER_OF_SETTINGS(RELAY) 4
#define CONTROL_LIMITS(VALUE, LOW, HIGH) if (VALUE<LOW) VALUE=HIGH; if (VALUE>HIGH) VALUE=LOW;

SettingsType all_settings;


int32_t BTN1_status;
int32_t BTN2_status;
int32_t BTN3_status;
uint32_t Led_On_time;

void BTN_Init();
void BTN_Process();
void BTN1_Pressed();
void BTN2_Pressed();
void BTN3_Pressed();
void BTN3_Long_Pressed();


void DoWork();
void ShowTerm();
void ShowSettings();
void ShowRelayMenu();
void ShowReset();

uint8_t current_status = 0;
uint8_t sub_current_status = 0;
uint8_t current_relay = 0;
uint8_t current_term = 0;

uint8_t settings_item1;
uint8_t settings_item2;

uint32_t iterations = 0;

uint8_t blinker = 0;

Settings * settings;

float temp[3];

uint8_t NUMBER_OF_SETTINGS(uint8_t RELAY)
{
	if (RELAY==PWN_RELAY)
		return 6;
	return 4;		
}

int main()
{
	
	int turn = 0;
	LCD_Init();
	BTN_Init();
	Settings_Init(all_settings);
	Relay_Init();

	ledON;
	LCD_Clear_Display();
	
	while (1)
	{
		BTN_Process();
		_delay_ms(1);
		//LCD_Clear_Display();
		switch(current_status)
		{
			case STATUS_DEFAULT:
				DoWork();			
			break;
			case STATUS_TERM:
				ShowTerm();
			break; 
			case STATUS_SETTINGS:
				ShowSettings();
			break; 
			case STATUS_RELAY:
				ShowRelayMenu();
			break; 
			case STATUS_RESET:
				ShowReset();
			break;
		}
		

		


	}
	return 0;
}

void switchToDefault()
{
	LCD_Clear_Display();
	current_status=STATUS_DEFAULT;
	sub_current_status = 0;
	iterations = 0;
	sei();
}	

void switchToRelay(uint8_t r)
{
	cli();
	current_status=STATUS_RELAY;
	sub_current_status = r;
	iterations = 0;
}	

void switchToTerm(uint8_t r, uint8_t t)
{
	current_status = STATUS_TERM;
	current_relay = r;
	sub_current_status = t;
	iterations = 0;
}	

void switchToSettings(uint8_t r, uint8_t t)
{
	current_status = STATUS_SETTINGS;
	sub_current_status = 0;
	current_relay = r;
	current_term = t;
	iterations = 0;
	settings_item1 = 0;
	settings_item2 = 0;
}

void switchToReset()
{
	current_status = STATUS_RESET;
	sub_current_status = 0;
	iterations = 0;
	settings_item1 = 0;
	settings_item2 = 0;
}

void DoWork()
{

	//LCD_Clear_Display();
	if (iterations%500!=0)
		{
			iterations++;
			//_delay_ms(1);
			return;
		}
	//LCD_Clear_Display();

	//LCD_printStr("Waiting...");
	//return ;
	//DS_Init(DSBIT1);
	DS_Init(DSBIT1);
	DS_Init(DSBIT2);
	DS_Init(DSBIT3);

		temp[0] = DS_getFloatTemperature(DSBIT1);


		//DS_Init(DSBIT2);
		temp[1] = DS_getFloatTemperature(DSBIT2);


		//DS_Init(DSBIT3);
		temp[2] = DS_getFloatTemperature(DSBIT3);

		
		HandleRelay(all_settings, temp, RELAY1);
		HandleRelay(all_settings, temp, RELAY2);
		HandleRelay(all_settings, temp, RELAY3);
		HandleRelay(all_settings, temp, RELAY4);
		

		LCD_2buffer_begin();
		//LCD_Clear_Display();
		LCD_2buffer_Move_Cursor(0);
		LCD_2buffer_Show_FloatTemperature1(temp[0]);

		LCD_2buffer_Move_Cursor(8);
		LCD_2buffer_Show_FloatTemperature1(temp[1]);

		LCD_2buffer_Move_Cursor(16);
		LCD_2buffer_Show_FloatTemperature1(temp[2]);
		
		LCD_2buffer_Move_Cursor(24);
		uint8_t i;
		for (i=0; i<3; i++)
		{
			if (RELAY_PIN & (1<<i))
				LCD_2buffer_printStr("1");
			else
				LCD_2buffer_printStr("0");			
		}
		
		LCD_2buffer_end();
		iterations = 1;
		_delay_us(100);
}

void ShowReset()
{
	if (iterations%12000!=0)
		{
			iterations++;
			//_delay_ms(1);
			return;
		}
	LCD_2buffer_begin();
	LCD_2buffer_Move_Cursor(1);
	LCD_2buffer_printStr("RESET?????");

	LCD_2buffer_Move_Cursor(17);
	LCD_2buffer_printStr("No");
	
	LCD_2buffer_Move_Cursor(25);
	LCD_2buffer_printStr("Yes");

	switch(	sub_current_status)
	{
		case 0:
			LCD_2buffer_Move_Cursor(16);
		break;

		case 1:
			LCD_2buffer_Move_Cursor(24);
		break;

	}
	
	LCD_2buffer_printStr(">");

	LCD_2buffer_end();
	//LCD_Clear_Display();
	//LCD_printStr("HELLO");
	//_delay_ms(1);
	iterations = 1;
}

void ShowTerm()
{
	if (iterations%12000!=0)
		{
			iterations++;
			//_delay_ms(1);
			return;
		}
	LCD_2buffer_begin();
	LCD_2buffer_Move_Cursor(1);
	LCD_2buffer_printStr("Temp1");

	LCD_2buffer_Move_Cursor(9);
	LCD_2buffer_printStr("Temp2");

	LCD_2buffer_Move_Cursor(17);
	LCD_2buffer_printStr("Temp3");

	switch(	sub_current_status)
	{
		case 0:
			LCD_2buffer_Move_Cursor(0);
		break;

		case 1:
			LCD_2buffer_Move_Cursor(8);
		break;

		case 2:
			LCD_2buffer_Move_Cursor(16);
		break;
	}
	LCD_2buffer_printStr(">");
	
	if (all_settings[current_relay][0].on)
	{
		LCD_2buffer_Move_Cursor(6);		
		LCD_2buffer_printStr("*");
	}
	if (all_settings[current_relay][1].on)
	{
		LCD_2buffer_Move_Cursor(14);		
		LCD_2buffer_printStr("*");
	}
	if (all_settings[current_relay][2].on)
	{
		LCD_2buffer_Move_Cursor(22);		
		LCD_2buffer_printStr("*");
	}

	LCD_2buffer_end();
	//LCD_Clear_Display();
	//LCD_printStr("HELLO");
	//_delay_ms(1);
	iterations = 1;
}

void ShowSettings()
{
	if (iterations%500!=0)
		{
			iterations++;
			//_delay_ms(1);
			return;
		}


	settings = &all_settings[current_relay][current_term];	
	/*switch(sub_current_status)
	{
		case 0: settings = &setting1; break;	
		case 1: settings = &setting2; break;
		case 2: settings = &setting3; break;	
	}*/

	blinker = !blinker;

	LCD_2buffer_begin();
	if (settings_item1<=3)
	{
		
		if ((blinker)||(settings_item2!=1))
		{
			LCD_2buffer_Move_Cursor(1);
			LCD_2buffer_Show_FloatTemperature1(settings->temp);
		}
	
		if ((blinker)||(settings_item2!=2))
		{
			LCD_2buffer_Move_Cursor(9);
			LCD_2buffer_Show_FloatTemperature1(settings->d);
		}
		if ((blinker)||(settings_item2!=3))
		{
			LCD_2buffer_Move_Cursor(17);
			if(settings->direction)
			{
				LCD_2buffer_printStr("UP");
			}else
			{
				LCD_2buffer_printStr("DOWN");
			}
		}
		if ((blinker)||(settings_item2!=4))
		{
			LCD_2buffer_Move_Cursor(25);
			if(settings->on)
			{
				LCD_2buffer_printStr("On");
			}else
			{
				LCD_2buffer_printStr("Off");
			}
		}
	}else
	{
		if ((blinker)||(settings_item2!=5))
		{
			LCD_2buffer_Move_Cursor(1);
			LCD_2buffer_Print_Number(settings->period);
		}
	
		if ((blinker)||(settings_item2!=6))
		{
			LCD_2buffer_Move_Cursor(9);
			LCD_2buffer_Print_Number(settings->part);
			LCD_2buffer_printStr("/");
			LCD_2buffer_Print_Number(N_PARTS);
		}	
	}	
	switch(	settings_item1)
	{
		case 0:
			LCD_2buffer_Move_Cursor(0);
		break;

		case 1:
			LCD_2buffer_Move_Cursor(8);
		break;

		case 2:
			LCD_2buffer_Move_Cursor(16);
		break;
		
		case 3:
			LCD_2buffer_Move_Cursor(24);
		break;
		
		case 4:
			LCD_2buffer_Move_Cursor(0);
		break;
		
		case 5:
			LCD_2buffer_Move_Cursor(8);
		break;
		}
		LCD_2buffer_printStr(">");	
	LCD_2buffer_end();
	iterations = 1;
}

void ShowRelayMenu()
{
	if (iterations%12000!=0)
		{
			iterations++;
			//_delay_ms(1);
			return;
		}
	LCD_2buffer_begin();
	LCD_2buffer_Move_Cursor(1);
	LCD_2buffer_printStr("Relay1");

	LCD_2buffer_Move_Cursor(9);
	LCD_2buffer_printStr("Relay2");

	LCD_2buffer_Move_Cursor(17);
	LCD_2buffer_printStr("Relay3");
	
	LCD_2buffer_Move_Cursor(25);
	LCD_2buffer_printStr("Relay4");

	switch(	sub_current_status)
	{
		case 0:
			LCD_2buffer_Move_Cursor(0);
		break;

		case 1:
			LCD_2buffer_Move_Cursor(8);
		break;

		case 2:
			LCD_2buffer_Move_Cursor(16);
		break;
		
		case 3:
			LCD_2buffer_Move_Cursor(24);
		break;
	}
	
	LCD_2buffer_printStr(">");

	LCD_2buffer_end();
	//LCD_Clear_Display();
	//LCD_printStr("HELLO");
	//_delay_ms(1);
	iterations = 1;
}

void BTN1_Pressed()
{
	switch(current_status)
	{
		case STATUS_RELAY:
			if(sub_current_status==0)
				sub_current_status = 4;
			sub_current_status--;
		break;
		case STATUS_RESET:
			if(sub_current_status==0)
				sub_current_status = 2;
			sub_current_status--;
		break;
		case STATUS_TERM:
			if(sub_current_status==0)
				sub_current_status = 3;
			sub_current_status--;
		break;
		case STATUS_SETTINGS:
			if (settings_item2==0)
			{
				if(settings_item1==0)
					settings_item1 = NUMBER_OF_SETTINGS(current_relay);
				settings_item1--;
			}
			if (settings_item2==1)
			{
				settings->temp-=0.1;
			}
			if (settings_item2==2)
			{
				settings->d-=0.1;
			}
			if (settings_item2==3)
			{
				settings->direction = !settings->direction;
			}
			if (settings_item2==4)
			{
				settings->on = !settings->on;
			}
			if (settings_item2==5)
			{
				settings->period --;
				CONTROL_LIMITS(settings->period, 1, 30)
			}
			if (settings_item2==6)
			{
				settings->part --;
				CONTROL_LIMITS(settings->part, 1, N_PARTS)
			}
		break;
	}
	iterations = 0;
}
void BTN2_Pressed()
{
	switch(current_status)
	{
		case STATUS_RELAY:
			sub_current_status++;
			if(sub_current_status==4)
				sub_current_status = 0;
		break;
		case STATUS_TERM:
			sub_current_status++;
			if(sub_current_status==3)
				sub_current_status = 0;
		break;
		case STATUS_SETTINGS:
			if (settings_item2==0)
			{	
				settings_item1++;
				if(settings_item1==NUMBER_OF_SETTINGS(current_relay))
					settings_item1 = 0;
			}
			if (settings_item2==1)
			{
				settings->temp += 0.1;
			}
			if (settings_item2==2)
			{
				settings->d+= 0.1;
			}
			if (settings_item2==3)
			{
				settings->direction = !settings->direction;
			}
			if (settings_item2==4)
			{
				settings->on = !settings->on;
			}
			if (settings_item2==5)
			{
				settings->period ++;
				CONTROL_LIMITS(settings->period, 1, 30)
			}
			if (settings_item2==6)
			{
				settings->part ++;
				CONTROL_LIMITS(settings->part, 1, N_PARTS)
			}
		break;
	}
	iterations = 0;
}
void BTN3_Pressed()
{
	switch(current_status)
	{
		case STATUS_TERM:
			switchToSettings(current_relay, sub_current_status);
		break;
		
		case STATUS_RESET:
			Settings_Reset(all_settings);
			switchToDefault();
		break;
		
		case STATUS_RELAY:
			switchToTerm(sub_current_status, 0);
		break;

		case STATUS_SETTINGS:
			if (settings_item2==0)
			{
				settings_item2 = settings_item1+1;
			}else
			{	
				settings_item2 = 0;
				Settings_Write(all_settings, current_relay, current_term);
			}
		break;
	}
}
void BTN1_Long_Pressed()
{
	switch(current_status)
	{
		case STATUS_TERM:
			switchToRelay(current_relay);
		break;
		case STATUS_SETTINGS:
			if (settings_item2==0)
			{
				switchToTerm(current_relay, current_term);
			}else
			{
				if (settings_item2==1)
				{
					settings->temp -= 5;
				}
				if (settings_item2==2)
				{
					settings->d -= 5;
				}
			}			
		break;
	}
	//Off(RELAY_PORT, RELAY4);
}

void BTN2_Long_Pressed()
{
	switch(current_status)
	{
		case STATUS_DEFAULT:
			switchToReset();
		break;
		case STATUS_SETTINGS:
			if (settings_item2==1)
			{
				settings->temp += 5;
			}
			if (settings_item2==2)
			{
				settings->d += 5;
			}

		break;
	}
}

void BTN3_Long_Pressed()
{
	//LCD_printStr("HELLO");
	if (current_status==STATUS_DEFAULT)
	{
		switchToRelay(0);
		
	}else
	{
		switchToDefault();
		
	}
	iterations=0;
}

void BTN_Init()
{
	BTN1_status = 0;
	BTN2_status = 0;
	BTN3_status = 0;

	Off(BTNDDR, BTN1); 
	Off(BTNDDR, BTN2); 
	Off(BTNDDR, BTN3); 

	On(BTNPORT, BTN1); 
	On(BTNPORT, BTN2); 
	On(BTNPORT, BTN3); 

	//Led_On_time = 0;
}

void BTN_Process()
{
	//if ((BTNPIN & (1<<BTN1))==0)
	if (IS_BTN_PRESSED(BTN1))
	{
		if (BTN1_status>=0)
			BTN1_status ++;
		//Led_On_time = 0;
	}

	if (IS_BTN_PRESSED(BTN2))
	{
		if (BTN2_status>=0)
			BTN2_status ++;
		//Led_On_time = 0;
	}

	if (IS_BTN_PRESSED(BTN3))
	{
		if (BTN3_status>=0)
			BTN3_status ++;
		//Led_On_time = 0;
	}

	if (BTN1_status>BTN_long_time)
	{
		if (BTN1_status>=0)
			BTN1_Long_Pressed();
		BTN1_status = -1;
	}
	
	if (BTN2_status>BTN_long_time)
	{
		if (BTN2_status>=0)
			BTN2_Long_Pressed();
		BTN2_status = -1;
	}

	if (BTN3_status>BTN_long_time)
	{
		if (BTN3_status>=0)
			BTN3_Long_Pressed();
		BTN3_status = -1;
	}

	if (!IS_BTN_PRESSED(BTN1))
	{
		if ((BTN1_status>0)&&(BTN1_status<BTN_long_time))
		{
			BTN1_Pressed();
		}
		BTN1_status = 0;
	}

	if (!IS_BTN_PRESSED(BTN2))
	{
		if ((BTN2_status>0)&&(BTN2_status<BTN_long_time))
		{
			BTN2_Pressed();
		}
		BTN2_status = 0;
	}

	if (!IS_BTN_PRESSED(BTN3))
	{
		if ((BTN3_status>0)&&(BTN3_status<BTN_long_time))
		{
			BTN3_Pressed();
		}
		BTN3_status = 0;
	}
	
	/*Led_On_time++;
	if (Led_On_time>LED_OFF_TIME)
	{
		ledOFF;
	}else
	{
		ledON;
	}*/
}