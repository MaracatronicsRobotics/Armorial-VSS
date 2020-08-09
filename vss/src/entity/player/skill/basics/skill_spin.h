#ifndef SKILL_SPIN_H
#define SKILL_SPIN_H

#include <src/entity/player/skill/skill.h>


class Skill_spin : public Skill
{
private:
    void run();
    double speed_spin;

public:

    Skill_spin();
    QString name();


};

#endif // SKILL_SPIN_H
