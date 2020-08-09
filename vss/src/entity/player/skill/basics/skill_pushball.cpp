#include "skill_pushball.h"

Skill_PushBall::Skill_PushBall(){}

QString Skill_PushBall::name(){
    return "Skill_PushBall";
}

void Skill_PushBall::run(){
    player()->setSpeed(_speed, _omega);
}

void Skill_PushBall::setSpeedAndOmega(float speed, float omega){
    _speed = speed;
    _omega = omega;
}
