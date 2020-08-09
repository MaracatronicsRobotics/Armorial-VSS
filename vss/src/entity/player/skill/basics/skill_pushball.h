#ifndef SKILL_PUSHBALL_H
#define SKILL_PUSHBALL_H

#include <src/entity/player/skill/skill.h>
#include <src/entity/player/skill/basics/skill_goto.h>

class Skill_PushBall : public Skill{
private:
    void run();

    //state machine
    enum{
        STATE_MOVETOPOS,
        STATE_ROTATE,
        STATE_PUSH
    };
    int _state;
public:
    Skill_PushBall();
    QString name();

};

#endif // SKILL_PUSHBALL_H
