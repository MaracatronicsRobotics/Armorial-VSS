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

#ifndef ROLE_H
#define ROLE_H

#include <src/entity/player/baseplayer.h>
//#include <src/entity/player/playeraccess.h>
//#include <src/entity/player/playerbus.h>
//#include <src/entity/controlmodule/vssteam.h>
//#include <src/entity/controlmodule/coach/playersdistribution.h>
#include <src/entity/player/behaviour/vssbehaviours.h>
#include <QObject>

#define STRIKER_INTERCEPT_MINVEL 0.5f

class Role : public QObject {
        Q_OBJECT
public:
    Role();
    virtual ~Role();

    // Initialization
    bool isInitialized() { return _initialized; }
    void initialize(VSSTeam *ourTeam, VSSTeam *theirTeam, Locations *loc);
    void setPlayer(VSSPlayer *player, PlayerAccess *playerAccess);

    // Called in Playbook loop
    void runRole();

    // Initialize
    virtual void initializeBehaviours() = 0;

    // Public functions
    void setBehaviour(int behaviour_id);
    QHash<int, Behaviour*> getBehaviours();
    virtual QString name() = 0;
    int getActualBehaviour();

    // PlayerAcess
    PlayerAccess* player();

    // Global behaviours
    Behaviour_GoBack *_bh_gb;
    bool _retreated;

protected:
    // Behaviour list functions
    void usesBehaviour(int id, Behaviour *behaviour);

    // canKickBall (for parameters)
    bool canKickBall() const;

    // Utils, loc and player access
    Locations* loc();

private:
    // Implemented by role children
    virtual void run() = 0;
    virtual void configure() = 0;
    bool _initialized;
    bool _configureEnabled;

    // Player access
    VSSPlayer *_player;
    PlayerAccess *_playerAccess;

    // Game Info
    Locations *_loc;
    VSSTeam *_ourTeam;
    VSSTeam *_theirTeam;
    CoachUtils *_utils;

    // Behaviours list
    QHash<int, Behaviour*> _behaviourList;
    Behaviour *_behaviour;
    int _actualBehaviour;

    //Check Collision
    bool canMove();
    Timer _timer;
    bool _retBefore;
    bool _wall;
};

#endif // ROLE_H
