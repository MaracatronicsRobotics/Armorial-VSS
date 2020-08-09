#ifndef SKILL_PUSHBALL_H
#define SKILL_PUSHBALL_H

#include <src/entity/player/skill/skill.h>
#include <src/entity/player/skill/basics/skill_goto.h>

class Skill_PushBall : public Skill{
private:
    void run();

    float _speed;
    float _omega;
public:
    Skill_PushBall();
    QString name();

    void setSpeedAndOmega(float speed, float omega);
};

#endif // SKILL_PUSHBALL_H
