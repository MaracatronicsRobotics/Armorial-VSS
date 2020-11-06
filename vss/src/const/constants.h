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
    static QString replacerAddress();
    static int refereePort();
    static int replacerPort();

    static void setRefereeInfo(QString *refAddress, int *refPort);
    static void setReplacerInfo(QString *repAddress, int *repPort);
private:
    static int _threadFrequency;
    static int _qtPlayers;
    static QString *_refereeAddress;
    static QString *_replacerAddress;
    static int *_refereePort;
    static int *_replacerPort;
};

#endif // CONSTANTS_H
