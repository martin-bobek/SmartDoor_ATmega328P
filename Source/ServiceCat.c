#include "ServiceCat.h"

typedef enum { CHECK, UNLOCK, UNLOCK, OUTHOLD, INHOLD, OUTEXIT, INEXIT } CatStates;

bool ControlCatDoor(bool inDoorLED, bool outDoorLED, bool inDoorCat, bool outDoorCat, bool catDoorStatus) {

  static CatStates state = CHECK;
  static CatStates nextState = CHECK;
  bool result = catDoorStatus;

  switch(CatStates) {
    case CHECK;
      if(outDoorCat == true || inDoorCat == true)
        nextState = UNLOCK;
      break;

    case UNLOCK;
      if(outDoorCat == true) {
        ServoPosition(unlock inner bolt);
        nextState = INHOLD;
        result = true;
      }
      else if(inDoorCat == true) {
        ServoPosition(unlock outer bolt);
        nextState = OUTHOLD;
        result = true;
      }
      else
        nextState = CHECK;
      break;

    case INHOLD;
      if(outDoorCat == false && outDoorLED == false) {
        ServoPosition(lock inner bolt);
        result = false;
        nextState = CHECK;
      }
      else if(outDoorLED == true || outDoorCat == true)
        nextState = INHOLD;
      else if(inDoorLED == true)
        nextState = INEXIT;
      break;

    case INEXIT;
      if(outDoorLED == false && inDoorLED == false && inDoorCat == false) {
        ServoPosition(lock inner bolt);
        result = false;
        nextState = CHECK;
      }
      else if(inDoorLED == false && outDoorLED == true)
        nextState = INHOLD;
      else if(inDoorLED == true || inDoorCat == true)
        nextState == INEXIT;
      break;


    case OUTHOLD;
      if(inDoorCat == false && inDoorLED == false) {
        ServoPosition(lock outer bolt);
        result = false;
        nextState = CHECK;
      }
      else if(inDoorLED == true || inDoorCat == true)
        nextState = OUTHOLD;
      else if(outDoorLED == true)
        nextState = OUTEXIT;
      break;

    case OUTEXIT;
      if(inDoorLED == false && u=outDoorLED == false && outDoorCat == false) {
        ServoPosition(lock outer bolt);
        result = false;
        nextState = CHECK;
      }
      else if(outDoorLED == false && inDoorLED == true)
        nextState = OUTHOLD;
      else if(outDoorLED == true || outDoorCat == true)
        nextState == OUTEXIT;
      break;
  }
  state = nextState;

  return result;
}
