#include "skill_spin.h"
#include <src/entity/player/skill/vssskills.h>
#include <src/entity/player/vssplayer.h>



QString Skill_spin::name() {
    return "Skill_Spin";
}
Skill_spin::Skill_spin()
{
    speed_spin = 2.0;
}

void Skill_spin::run() {
     player()->setSpeed(0.0 , speed_spin);
}
