#include "ServiceCat.h"

typedef enum { CHECK, UNLOCK, HOLD, OUTEXIT, INEXIT } CatStates;

bool ControlCatDoor(bool proxIn, bool proxOut, bool RF, bool inTime, int catDoorStatus) {

  static CatStates state = CHECK;
  static CatStates nextState = CHECK;
  int result = catDoorStatus;

  switch(state) {
  case CHECK:
        if((proxOut == true && RF == true) {
          result = OUTEROPEN;
          nextState = UNLOCK;
        }
        else if(proxIn == true && inTime == true) {
          result = INNEROPEN;
          nextState = UNLOCK;
        }
      break;

  case UNLOCK:
      if(result == INNEROPEN)
        //ServoPosition(unlock inner bolt);
      else if(result == OUTEROPEN)
        //ServoPosition(unlock outer bolt);
      nextState = HOLD;
      break;

  case HOLD:
      if(rfin == true && proxIn == true )
        nextState = HOLD;
      else if(inDoorLED == true)
        nextState = INEXIT;
      break;

  case INEXIT:
      if(outDoorLED == false && inDoorLED == false && inDoorCat == false) {
        //ServoPosition(lock inner bolt);
        result = false;
        nextState = CHECK;
      }
      else if(inDoorLED == false && outDoorLED == true)
        nextState = INHOLD;
      else if(inDoorLED == true || inDoorCat == true)
        nextState = INEXIT;
      break;


  case OUTHOLD:
      if(inDoorCat == false && inDoorLED == false) {
        //ServoPosition(lock outer bolt);
        result = false;
        nextState = CHECK;
      }
      else if(inDoorLED == true || inDoorCat == true)
        nextState = OUTHOLD;
      else if(outDoorLED == true)
        nextState = OUTEXIT;
      break;

  case OUTEXIT:
      if(inDoorLED == false && outDoorLED == false && outDoorCat == false) {
        //ServoPosition();
        result = false;
        nextState = CHECK;
      }
      else if(outDoorLED == false && inDoorLED == true)
        nextState = OUTHOLD;
      else if(outDoorLED == true || outDoorCat == true)
        nextState = OUTEXIT;
      break;
  }
  state = nextState;

  return result;
}
