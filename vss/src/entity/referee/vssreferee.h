#ifndef VSSREFEREE_H
#define VSSREFEREE_H

#include <src/utils/vssclient/vssclient.h>
#include <src/entity/entity.h>
#include <src/const/constants.h>
#include <QUdpSocket>
#include <src/entity/controlmodule/vssteam.h>

class VSSReferee : public Entity
{
public:
    VSSReferee(VSSTeam *ourTeam);

    // Entity inherit
    QString name();

    // Checks
    bool isGameOn();
    bool isStop();
private:
    // Entity inherit
    void initialization();
    void loop();
    void finalization();

    // Referee command to str
    QString getFoulNameById(VSSRef::Foul foul);
    QString getTeamColorNameById(VSSRef::Color color);
    QString getQuadrantNameById(VSSRef::Quadrant quadrant);
    QString getHalfNameById(VSSRef::Half half);

    // Processing placement
    void placeByCommand(VSSRef::Foul foul, VSSRef::Quadrant quadrant, VSSRef::Color team);

    // Connect
    int connect();

    // Referee last command
    QMutex _commandMutex;
    VSSRef::Foul _lastCommand;

    // Team
    VSSTeam *_ourTeam;

    // Socket
    QUdpSocket *_replacerSocket;
    VSSClient *_client;
};

#endif // VSSREFEREE_H