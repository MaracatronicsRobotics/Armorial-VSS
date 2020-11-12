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
    Q_OBJECT
private:
    // Behaviours
    Behaviour_Goalkeeper *_bh_gk;
    Behaviour_DoNothing *_bh_dn;
    Behaviour_TakeFoul *_bh_tf;
    Behaviour_PushBall *_bh_pb;

    // Behaviours Enum
    enum{
        BHV_GK,
        BHV_DONOTHING,
        BHV_TAKEFOUL,
        BHV_PUSH
    };

    //variables and functions
    bool ourTeamShouldTake(VSSRef::Color teamColor);
    void penaltyKick(Position* pos, Angle* ang);
    void goalKick(Position* pos, Angle* ang);
    void gameOn();
    void freeBall(Position* pos, Angle* ang, VSSRef::Quadrant quadrant);
    void kickOff(Position* pos, Angle* ang);
    void isGKStucked();

    bool flagPenalti = 0;

    int lastFoul; //it can be FREE_KICK = 0, PENALTY_KICK = 1, GOAL_KICK = 2, FREE_BALL = 3, KICKOFF = 4
    bool isNormalGame, canGoBackToNormalGame;
    bool weTake;
    bool _backUp;
    Timer timer;
    Timer timerGetOut;
    float limitTimer;

    // Inherited functions
    void configure();
    void run();

public:
    Role_Goalkeeper();
    void initializeBehaviours();
    QString name();

signals:
    void getOut(quint8 playerId, quint8 GK_ID);

public slots:
    void backUp(quint8 playerId);
    void receiveFoul(VSSRef::Foul foul, VSSRef::Quadrant quadrant, VSSRef::Color teamColor);
};

#endif // ROLE_GOALKEEPER_H
