#include "ServiceDoor.h"

typedef enum { CHECK, DETECT, RESOLVE = 101 } DoorStates;


bool ControlDoor(bool RFIDstatus, bool BUTTONstatus, bool DOORstatus) {
  static DoorStates state = CHECK;
  bool result = DOORstatus;
  switch(state) {
    case CHECK;
      if(RFIDstatus == true || BUTTONstatus)
        state++;
      else
        break;
      break;
    case DETECT;
      if(DOORstatus = true) {
        ServoPosition(closed);
        result = false;
      }
      else {
        ServoPosition(open);
        result = true;
      }
      state++;
      break;
  }
  if(state > 1 && state <101)
    state++;
  return result;
}
