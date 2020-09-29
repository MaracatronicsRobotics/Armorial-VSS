#ifndef BEHAVIOUR_GOBACK_H
#define BEHAVIOUR_GOBACK_H

#include <src/entity/player/behaviour/behaviour.h>
#include <src/entity/player/skill/vssskills.h>

class Behaviour_GoBack: public Behaviour
{
private:
    void configure();
    void run();

    Skill_GoTo *_sk_goTo;
    Skill_RotateTo *_sk_rotateTo;
    Skill_DoNothing *_sk_doNothing;

    float _angleTarget, _angleBefore;
    Position _newPos, _playerPos;
    bool _retreated;
    bool _start;

    //Machine State
    int _state;
    enum{
        STATE_GOBACK,
        STATE_LOOKAT,
        STATE_WAIT
    };

    //States
    enum{
        STATE_GOTO,
        STATE_ROTATE
    };

    Position newPosBack(Position playerPos);
public:
    Behaviour_GoBack();
    bool getDone(){return _retreated;}
    void setStart(bool start){_start = start;}
    QString name();
};

#endif // BEHAVIOUR_GOBACK_H
