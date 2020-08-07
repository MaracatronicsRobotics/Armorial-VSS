#include "skill_goto.h"
#include <src/entity/player/skill/vssskills.h>

Skill_GoTo::Skill_GoTo(){
    _goToPos = Position(true, 0.0, 0.0, 0.0);
}

QString Skill_GoTo::name(){
    return "Skill_GoTo";
}

void Skill_GoTo::run(){
    setGoToPos(loc()->ball());
    player()->goTo(_goToPos);
}
