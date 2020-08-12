#include "skill_spin.h"

#define SPIN_SPEED 50.0f

QString Skill_spin::name() {
    return "Skill_Spin";
}

Skill_spin::Skill_spin() {
    _isClockWise = true;
}

void Skill_spin::run() {
    if (_isClockWise) {
        player()->setSpeed(0.0 , -SPIN_SPEED);
    } else {
        player()->setSpeed(0.0 , SPIN_SPEED);
    }
}
