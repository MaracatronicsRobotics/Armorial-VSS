#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

class VSSConstants
{
public:
    VSSConstants();
    static int threadFrequency();
    static int qtPlayers();
    static QString refereeAddress();
    static int refereePort();
    static int replacerPort();
private:
    static int _threadFrequency;
    static int _qtPlayers;
    static QString _refereeAddress;
    static int _refereePort;
    static int _replacerPort;
};

#endif // CONSTANTS_H
