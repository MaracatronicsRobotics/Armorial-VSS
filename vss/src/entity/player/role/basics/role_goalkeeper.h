/***
 * Maracatronics Robotics
 * Federal University of Pernambuco (UFPE) at Recife
 * http://www.maracatronics.com/
 *
 * This file is part of Armorial project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***/

#ifndef ROLE_GOALKEEPER_H
#define ROLE_GOALKEEPER_H

#include <src/entity/player/behaviour/vssbehaviours.h>
#include <src/entity/player/role/role.h>

class Role_Goalkeeper : public Role {
private:
    // Behaviours
    Behaviour_Goalkeeper *_bh_gk;
    Behaviour_DoNothing *_bh_dn;
    Behaviour_TakeFoul *_bh_tf;

    // Behaviours Enum
    enum{
        BHV_GK,
        BHV_DONOTHING,
        BHV_TAKEFOUL
    };

    //variables and functions
    bool ourTeamShouldTake(VSSRef::Color teamColor);
    void penaltyKick(Position* pos, Angle* ang);
    void goalKick(Position* pos, Angle* ang);
    void gameOn();
    void freeBall(Position* pos, Angle* ang, VSSRef::Quadrant quadrant);
    void kickOff(Position* pos, Angle* ang);

    int lastFoul; //it can be FREE_KICK = 0, PENALTY_KICK = 1, GOAL_KICK = 2, FREE_BALL = 3, KICKOFF = 4
    bool isNormalGame, canGoBackToNormalGame;
    bool weTake;
    long int counter;

    // Inherited functions
    void configure();
    void run();

public:
    Role_Goalkeeper();
    void initializeBehaviours();
    QString name();

public slots:
    void receiveFoul(VSSRef::Foul foul, VSSRef::Quadrant quadrant, VSSRef::Color teamColor);
};

#endif // ROLE_GOALKEEPER_H
