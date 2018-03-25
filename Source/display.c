#include "display.h"
#include "lcd.h"
#include "rtc.h"
#include "main_door.h"
#include "buttons.h"

static void InitMainMenu(void);
static void MainMenu(void);
static void InitTimeSetup(void);
static void TimeSetup(void);
static void ExitTimeSetup(void);
//static char HexToAscii(uint8_t hex);

void (*DisplayThread)(void) = InitMainMenu;

static uint8_t lcdSuccess = 0;
static uint8_t state = 0;
static char time[] = "  :  :  ";

static void InitMainMenu(void) {
	switch (state) {
	case 0:
		if (LcdWrite(0, "      :  :      "))
			state++;
		break;
	case 1:
		if (LcdWrite(LINE2_START, "UNLOCK     SETUP")) {
			state = 0;
			DisplayThread = MainMenu;
			G_ButtonPressed &= ~(LEFT_BUTTON | MIDDLE_BUTTON | RIGHT_BUTTON);
		}
		break;
	}
}

static void MainMenu(void) {
	static uint8_t prevSec = 0xFF;

	if (prevSec != G_Seconds) {
	    prevSec = G_Seconds;
	    time[0] = (G_Hours >> 4) + '0';
	    time[1] = (G_Hours & 0xF) + '0';
	    time[3] = (G_Minutes >> 4) + '0';
	    time[4] = (G_Minutes & 0xF) + '0';
	    time[6] = (G_Seconds >> 4) + '0';
	    time[7] = (G_Seconds & 0xF) + '0';
	    lcdSuccess = 0;
	}
	if (!lcdSuccess)
		lcdSuccess = LcdWrite(4, time);

	if (lcdSuccess && (G_ButtonPressed & RIGHT_BUTTON)) {
		prevSec = 0xFF;
		DisplayThread = InitTimeSetup;
	}

	if (G_ButtonPressed & LEFT_BUTTON) {
		G_ButtonPressed &= ~LEFT_BUTTON;
		G_UnlockRequest = 1;
	}
}

static void InitTimeSetup(void) {
	switch (state) {
	case 0:
		if (LcdWrite(LINE2_START, "NEXT   UP   EXIT"))
			state++;
		break;
	case 1:
		if (LcdStartEdit(4)) {
			state = 0;
			lcdSuccess = 0;
			DisplayThread = TimeSetup;
			G_ButtonPressed &= ~(LEFT_BUTTON | MIDDLE_BUTTON | RIGHT_BUTTON);
		}
		break;
	}
}

static void TimeSetup(void) {
	static char max = '2'; // note lcdSuccess has been flipped in polarity

	if (!lcdSuccess && (G_ButtonPressed & LEFT_BUTTON)) {
		G_ButtonPressed &= ~LEFT_BUTTON;
		state++;
		switch (state) {
		case 1:
			if (time[0] == '2')
				max = '3';
			else
				max = '9';
			break;
		case 2:
		case 5:
			state++;
			max = '5';
			break;
		case 4:
		case 7:
			max = '9';
			break;
		case 8:
			max = '2';
			DisplayThread = ExitTimeSetup;
			return;
		}
		lcdSuccess = 2;
	}
	else if (!lcdSuccess && (G_ButtonPressed & MIDDLE_BUTTON)) {
		G_ButtonPressed &= ~MIDDLE_BUTTON;
		time[state]++;
		if (time[state] > max)
			time[state] = '0';
		lcdSuccess = 1;
	}
	else if (!lcdSuccess && (G_ButtonPressed & RIGHT_BUTTON)) {
		DisplayThread = ExitTimeSetup;
		max = '2';
		return;
	}

	if (lcdSuccess == 1)
		lcdSuccess = LcdEdit(time[state]) ? 0 : 1;
	else if (lcdSuccess == 2) {
		lcdSuccess = LcdStartEdit(state + 4) ? 0 : 2;
		if (!lcdSuccess && time[state] > max) {
			time[state] = max;
			lcdSuccess = 1;
		}
	}
}

static void ExitTimeSetup(void) {
	if (state == 8) {
		G_Hours = ((time[0] - '0') << 4) | (time[1] - '0');
		G_Minutes = ((time[3] - '0') << 4) | (time[4] - '0');
		G_Seconds = ((time[6] - '0') << 4) | (time[7] - '0');
	    G_TimeWrite = CHANGE_FLAG;
	    state = 0;
	}

	if (LcdFinishEdit()) {
		lcdSuccess = 1;
		state = 0;
		DisplayThread = InitMainMenu;
	}
}

/*
static char HexToAscii(uint8_t hex) {
  hex &= 0xF;
  if (hex < 10)
    return hex + '0';
  return hex - 10 + 'A';
}
*/
/*
static void TimeThread(void) {
  static char time[] = "  :  :  ";
  static uint8_t prevSec = 0xFF;
  static uint8_t lcdSuccess = 1;
  static uint16_t ticksToChange = 0;

  if (ticksToChange <= 5000)
    ticksToChange++;
  if (ticksToChange == 5000) {
    G_Seconds = (3 << 4) | 1;
    G_Minutes = (5 << 4) | 7;
    G_Hours = (1 << 4) | 2;
    G_TimeWrite = CHANGE_FLAG;
  }

  lcdSuccess = 0;
  }
  if (!lcdSuccess) {
    lcdSuccess = LcdWrite(0, time);
  }
}
*/
