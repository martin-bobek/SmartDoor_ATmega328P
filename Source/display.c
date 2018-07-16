#include "display.h"
#include "lcd.h"
#include "rtc.h"
#include "main_door.h"
#include "buttons.h"
#include "times.h"
#include "id_check.h"

static void InitMainMenu(void);
static void MainMenu(void);
static void InitTimeSetup(void);
static void TimeSetup(void);
static void InitUnlockSetup(void);
static void InitLockSetup(void);
static void ExitTimeSetup(void);
static void InitRfidSetup(void);
static void RfidSetup(void);
static void AddTag(void);

//static char HexToAscii(uint8_t hex);

void (*DisplayThread)(void) = InitMainMenu;

#define TIME			0
#define UNLOCK			1
#define LOCK			2
#define NOTFULL			0
#define FULL_DOOR 		1
#define FULL_PET		2

static uint8_t lcdSuccess = 0;
static uint8_t state = 0;
static char time[] = "  :  :  ";
static uint8_t setupMode;

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
		if (LcdWrite(LINE2_START, "ENTER  UP  SETUP"))
			state++;
		break;
	case 1:
		if (LcdStartEdit(4)) {
			state = 0;
			setupMode = TIME;
			lcdSuccess = 0;
			DisplayThread = TimeSetup;
			G_ButtonPressed &= ~(LEFT_BUTTON | MIDDLE_BUTTON | RIGHT_BUTTON);
		}
		break;
	}
}

static void InitUnlockSetup(void) {
	switch (state) {
	case 0:
		if (LcdFinishEdit())
			state++;
		break;
	case 1:
		time[0] = (G_UnlockHour >> 4) + '0';
		time[1] = (G_UnlockHour & 0xF) + '0';
		time[3] = (G_UnlockMinute >> 4) + '0';
		time[4] = (G_UnlockMinute & 0xF) + '0';
		time[5] = '\0';
		state++;
		break;
	case 2:
		if (LcdWrite(0, " UNLOCK   "))
			state++;
		break;
	case 3:
		if (LcdWrite(10, time))
			state++;
		break;
	case 4:
		if (LcdStartEdit(10)) {
			state = 0;
			setupMode = UNLOCK;
			DisplayThread = TimeSetup;
			G_ButtonPressed &= ~(LEFT_BUTTON | MIDDLE_BUTTON | RIGHT_BUTTON);
		}
		break;
	}
}

static void InitLockSetup(void) {
	switch (state) {
	case 0:
		if (LcdFinishEdit())
			state++;
		break;
	case 1:
		time[0] = (G_LockHour >> 4) + '0';
		time[1] = (G_LockHour & 0xF) + '0';
		time[3] = (G_LockMinute >> 4) + '0';
		time[4] = (G_LockMinute & 0xF) + '0';
		state++;
		break;
	case 2:
		if (LcdWrite(0, " LOCK   "))
			state++;
		break;
	case 3:
		if (LcdWrite(10, time))
			state++;
		break;
	case 4:
		if (LcdStartEdit(10)) {
			state = 0;
			setupMode = LOCK;
			DisplayThread = TimeSetup;
			G_ButtonPressed &= ~(LEFT_BUTTON | MIDDLE_BUTTON | RIGHT_BUTTON);
		}
	}
}

static void ExitTimeSetup(void) {
	if (LcdFinishEdit()) {
		DisplayThread = InitRfidSetup;
		time[5] = ':';
		lcdSuccess = 1;
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
		case 5:
			if (setupMode == UNLOCK) {
				max = '2';
				state = 0;
				G_UnlockHour = ((time[0] - '0') << 4) | (time[1] - '0');
				G_UnlockMinute = ((time[3] - '0') << 4) | (time[4] - '0');
				G_WriteTime |= WRITE_UNLOCK;
				DisplayThread = InitLockSetup;
				return;
			} else if (setupMode == LOCK) {
				max = '2';
				state = 0;
				G_LockHour = ((time[0] - '0') << 4) | (time[1] - '0');
				G_LockMinute = ((time[3] - '0') << 4) | (time[4] - '0');
				G_WriteTime |= WRITE_LOCK;
				DisplayThread = ExitTimeSetup;
				return;
			}
			// no break
		case 2:
			state++;
			max = '5';
			break;
		case 4:
		case 7:
			max = '9';
			break;
		case 8:
			G_Hours = ((time[0] - '0') << 4) | (time[1] - '0');
			G_Minutes = ((time[3] - '0') << 4) | (time[4] - '0');
			G_Seconds = ((time[6] - '0') << 4) | (time[7] - '0');
			G_RtcWrite = CHANGE_FLAG;
			max = '2';
			state = 0;
			DisplayThread = InitUnlockSetup;
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
		max = '2';
		state = 0;
		if (setupMode == TIME)
			DisplayThread = InitUnlockSetup;
		else if (setupMode == UNLOCK)
			DisplayThread = InitLockSetup;
		else
			DisplayThread = ExitTimeSetup;
		return;
	}

	if (lcdSuccess == 1)
		lcdSuccess = LcdEdit(time[state]) ? 0 : 1;
	else if (lcdSuccess == 2) {
		lcdSuccess = LcdStartEdit((setupMode == TIME) ? (state + 4) : (state + 10)) ? 0 : 2;
		if (!lcdSuccess && time[state] > max) {
			time[state] = max;
			lcdSuccess = 1;
		}
	}
}

static void InitRfidSetup(void) {
	switch (state) {
	case 0:
		if (LcdWrite(1, "  RFID SETUP  "))
			state++;
		break;
	case 1:
		if (LcdWrite(LINE2_START, "DOOR  PET   EXIT")) {
			state = 0;
			DisplayThread = RfidSetup;
			G_ButtonPressed &= ~(LEFT_BUTTON | MIDDLE_BUTTON | RIGHT_BUTTON);
		}
		break;
	}
}

static void RfidSetup(void) {
	switch (state) {
	case 1:
		if (LcdWrite(1, "   SCAN TAG  "))
			state = 3;
		return;
	case 2:
		if (LcdWrite(1, "   TAGS FULL "))
			state = 3;
		return;
	case 3:
		if (LcdWrite(LINE2_START, "DELETE    CANCEL")) {
			state = 0;
			DisplayThread = AddTag;
			G_ButtonPressed &= ~(LEFT_BUTTON | MIDDLE_BUTTON);
		}
		return;
	}

	if (G_ButtonPressed & LEFT_BUTTON) {
		if (G_RfidDetected & MAIN_DOOR_FULL) {
			state = 2;
			setupMode = FULL_DOOR;
		}
		else {
			state = 1;
			G_AddId = MAIN_DOOR_RFID;
			setupMode = NOTFULL;
		}
	}
	else if (G_ButtonPressed & MIDDLE_BUTTON) {
		if (G_RfidDetected & PET_DOOR_FULL) {
			state = 2;
			setupMode = FULL_PET;
		}
		else {
			state = 1;
			G_AddId = PET_DOOR_RFID;
			setupMode = NOTFULL;
		}
	}
	else if (G_ButtonPressed & RIGHT_BUTTON) {
		DisplayThread = InitMainMenu;
	}
}

static void AddTag(void) {
	switch (state) {
	case 1:
		if (LcdWrite(3, "RFID SETUP"))
			state = 4;
		return;
	case 2:
		if (LcdWrite(3, "RFID ADDED"))
			state = 4;
		return;
	case 3:
		if (LcdWrite(1, "RFIDs DELETED"))
			state = 4;
		return;
	case 4:
		if (LcdWrite(LINE2_START, "DOOR  PET   EXIT")) {
			state = 0;
			DisplayThread = RfidSetup;
			G_ButtonPressed &= ~(LEFT_BUTTON | MIDDLE_BUTTON | RIGHT_BUTTON);
		}
		return;
	}

	if (G_AddId == 0 && setupMode == NOTFULL)
		state = 2;
	else if (G_ButtonPressed & LEFT_BUTTON) {
		switch (setupMode) {
		case NOTFULL:
			G_AddId = G_AddId << 2;
			break;
		case FULL_PET:
			G_AddId = PET_DOOR_DELETE;
			break;
		case FULL_DOOR:
			G_AddId = MAIN_DOOR_DELETE;
			break;
		}
		state = 3;
	}
	else if (G_ButtonPressed & RIGHT_BUTTON) {
		G_AddId = 0;
		state = 1;
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
