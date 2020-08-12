#ifndef SKILL_SPIN_H
#define SKILL_SPIN_H

#include <src/entity/player/skill/skill.h>


class Skill_spin : public Skill {
private:
    void run();

    // Parameters
    bool _isClockWise;

public:

    Skill_spin();
    QString name();

    void setClockWise (bool isClockWise) { _isClockWise = isClockWise; }
};

#endif // SKILL_SPIN_H
