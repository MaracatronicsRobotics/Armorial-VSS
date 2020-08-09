#ifndef SKILL_GOTO_H
#define SKILL_GOTO_H

#include <src/entity/player/skill/skill.h>

class Skill_GoTo : public Skill {
private:
    void run();
public:
    Position _goToPos;

    QString name();
    Skill_GoTo();

    void setGoToPos(Position pos) { _goToPos = pos; }
};

#endif // SKILL_GOTO_H
