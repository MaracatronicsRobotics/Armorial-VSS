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

#ifndef VSSPLAYER_H
#define VSSPLAYER_H

#include <src/entity/entity.h>
#include <src/utils/utils.h>
#include <src/entity/player/baseplayer.h>
#include <src/entity/player/control/pid.h>

class VSSPlayer : public Entity
{
public:  
    VSSPlayer(quint8 playerId, VSSTeam *playerTeam, Controller *ctr, Role *defaultRole, PID *vwPID, NavigationAlgorithm *navAlg);
    ~VSSPlayer();

    // Getters
    QString name();
    Position position() const;
    Velocity velocity() const;
    Angle angle() const;
    AngularSpeed angularSpeed() const;
    quint8 playerId() const;
    quint8 teamId() const;
    quint8 opTeamId() const;
    PlayerAccess* access() const;

    // Booleans (checks)
    bool isLookingTo(const Position &pos) const;
    bool isLookingTo(const Position &pos, float error) const;
    bool isAtPosition(const Position &pos) const;
    bool isNearbyPosition(const Position &pos, float error) const;
    bool hasBallPossession() const;

    // Distances
    float distanceTo(const Position &pos) const;
    float distBall() const;
    float distOurGoal() const;
    float distTheirGoal() const;

    // Angles
    Angle angleTo(const Position &pos) const;

    // Team
    VSSTeam* playerTeam();

    // Role
    void setRole(Role *b);
    QString getRoleName();

    // pp
    QLinkedList<Position> getPath() const;
    void setGoal(Position pos) const;

    // Control functions
    void idle();
    void setSpeed(float vx, float omega);
    float getRotateAngle(Position targetPosition);
    float getRotateSpeed(float angleRobotToTarget);
    float getVxToTarget(Position targetPosition);
    void rotateTo(Position targetPosition, bool rot, float angle);
    void goTo(Position targetPosition, float velocityFactor = 1.0f, float minVel = 0.0, bool avoidTeammates = false, bool avoidOpponents = false, bool avoidBall = false, bool avoidOurGoalArea = false, bool avoidTheirGoalArea = false, bool pathActivated = false);
    Angle orientation() const;
    std::pair<Angle,float> getNavDirectionDistance(const Position &destination, const Angle &positionToLook, bool avoidTeammates, bool avoidOpponents, bool avoidBall, bool avoidOurGoalArea, bool avoidTheirGoalArea);

private:
    // Inherited methods from Entity
    void initialization();
    void loop();
    void finalization();

    // Player info
    quint8 _playerId;
    VSSTeam *_playerTeam;
    float _lError;
    float _aError;
    int _idleCount;

    // PID Controller
    PID *_vxPID;
    PID *_vwPID;

    // Player data
    Position _playerPosition;
    Velocity _playerVelocity;
    Angle _playerAngle;
    AngularSpeed _playerAngularSpeed;
    PlayerAccess *_playerAccessSelf;
    PlayerAccess *_playerAccessBus;

    // Navigation
    Navigation *_nav;

    // Controller acess
    Controller *_ctr;

    // Role
    Role *_role;
    Role *_defaultRole;
    QMutex _mutexRole;
};

#endif // VSSPLAYER_H
