#include "constants.h"

int VSSConstants::_threadFrequency = 60; // frequency of threads
int VSSConstants::_qtPlayers       = 3;  // num of players

VSSConstants::VSSConstants()
{

}

int VSSConstants::threadFrequency(){
    return _threadFrequency;
}

int VSSConstants::qtPlayers(){
    return _qtPlayers;
}
