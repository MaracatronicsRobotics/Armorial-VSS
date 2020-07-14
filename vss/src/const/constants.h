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
private:
    static int _threadFrequency;
    static int _qtPlayers;
};

#endif // CONSTANTS_H
