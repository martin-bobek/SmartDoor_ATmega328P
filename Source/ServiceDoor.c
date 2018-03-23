#include "ServiceDoor.h"

typedef enum { CHECK, OPEN, CLOSE } DoorStates;

#if 0

bool ControlDoor(bool RFIDstatus, bool BUTTONstatus, bool DOORstatus, bool HFStatus) {
  static DoorStates state = CHECK;
  static DoorSstates nextState = CHECK;
  bool result = DOORstatus;
  switch(state) {
  case CHECK:
      if(RFIDstatus == true || BUTTONstatus == true)
        nextState = OPEN;
      if(HFstatus == true)
        nextState = CLOSE;
      break;
  case OPEN:
      //ServoPosition(open);
      result = true;
      nextState = CHECK;
      break;
  case CLOSE:
      //ServoPosition(close);
      result = false;
      nexeState = CHECK;
      break;
  }
  state = nextState;
  return result;
}

#endif
