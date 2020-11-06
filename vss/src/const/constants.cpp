#include "constants.h"

int VSSConstants::_threadFrequency = 60; // frequency of threads
int VSSConstants::_qtPlayers       = 3;  // num of players

// Referee
int* VSSConstants::_refereePort = nullptr;
QString* VSSConstants::_refereeAddress = nullptr;

// Replacer (Referee)
int* VSSConstants::_replacerPort = nullptr;
QString* VSSConstants::_replacerAddress = nullptr;

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
    return *_refereePort;
}

int VSSConstants::replacerPort(){
    return *_replacerPort;
}

QString VSSConstants::refereeAddress(){
    return *_refereeAddress;
}

QString VSSConstants::replacerAddress(){
    return *_replacerAddress;
}

void VSSConstants::setRefereeInfo(QString *refAddress, int *refPort){
    _refereeAddress = refAddress;
    _refereePort = refPort;
}

void VSSConstants::setReplacerInfo(QString *repAddress, int *repPort){
    _replacerAddress = repAddress;
    _replacerPort = repPort;
}
