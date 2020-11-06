#ifndef VSSREFEREE_H
#define VSSREFEREE_H

#include <src/utils/vssclient/vssclient.h>
#include <src/entity/entity.h>
#include <src/const/constants.h>
#include <QUdpSocket>
#include <src/entity/controlmodule/vssteam.h>

class VSSReferee : public Entity
{
    Q_OBJECT
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

    // Connect
    bool connect();

    // Referee last command
    QMutex _commandMutex;
    VSSRef::Foul _lastCommand;

    // Team
    VSSTeam *_ourTeam;

    // Processing placement
    void placeReceivedPackets();

    std::pair<Position, Angle> _desiredPlacement[5];
    Timer _timer;
    bool _enableTimer;
    bool _desiredMark[5];
    bool _receivedAtLeastOne;

    // Socket
    QUdpSocket *_replacerSocket;
    QUdpSocket *_refereeSocket;

signals:
    void emitFoul(VSSRef::Foul foul, VSSRef::Quadrant quadrant, VSSRef::Color team);
public slots:
    void receivePosition(quint8 playerId, Position desiredPosition, Angle desiredOrientation);
};

#endif // VSSREFEREE_H
