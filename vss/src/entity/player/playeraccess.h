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

#ifndef PLAYERACCESS_H
#define PLAYERACCESS_H

#include <src/entity/player/baseplayer.h>

class PlayerAccess {
private:
    bool _self;
    VSSPlayer *_player;
    Locations *_loc;
public:
    PlayerAccess(bool self, VSSPlayer *player, Locations *loc);
    virtual ~PlayerAccess();
    // General player info
    quint8 playerId() const;
    quint8 teamId() const;
    quint8 opTeamId() const;
    VSSTeam* team() const;
    // Robot auxiliary methods
    // Position
    Position position() const;
    // Orientation
    virtual Angle orientation() const;
    // Velocity
    Velocity velocity() const;
    // Booleans
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
    // Role
    QString roleName() const;
};


#endif // PLAYERACCESS_H
