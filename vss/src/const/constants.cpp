#include "constants.h"

int VSSConstants::_threadFrequency = 60; // frequency of threads
int VSSConstants::_qtPlayers       = 3;  // num of players

// Referee
int VSSConstants::_refereePort = 10003;
QString VSSConstants::_refereeAddress = "224.5.23.2";

// Replacer (Referee)
int VSSConstants::_replacerPort = 10004;

VSSConstants::VSSConstants()
{

}

int VSSConstants::threadFrequency(){
    return _threadFrequency;
}

int VSSConstants::qtPlayers(){
    return _qtPlayers;
}

int VSSConstants::refereePort(){
    return _refereePort;
}

int VSSConstants::replacerPort(){
    return _replacerPort;
}

QString VSSConstants::refereeAddress(){
    return _refereeAddress;
}
