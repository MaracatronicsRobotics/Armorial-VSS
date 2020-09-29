/***
 * Warthog Robotics
 * University of Sao Paulo (USP) at Sao Carlos
 * http://www.warthog.sc.usp.br/
 *
 * This file is part of WRCoach project.
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

#ifndef FANA_H
#define FANA_H

#define OBSTACLE_RADIUS 0.25f
#define HYSTERESIS_FACTOR 0.15f

#include <src/entity/player/navigation/navalgorithm.h>
#include <src/utils/freeangles/freeangles.h>

class FANA : public NavigationAlgorithm {
public:
    FANA();
    virtual ~FANA();

    // Reset
    void reset();

    // Add obstacles
    void addBall(const Position &pos, const Velocity &vel);
    void addGoalArea(const Position &pos);
    void addOwnRobot(const Position &pos, const Velocity &vel);
    void addEnemyRobot(const Position &pos, const Velocity &vel);

    static void setObstacleRadiusFactor(float factor) { _obstacleRadiusFactor = (factor > 0.0f) ? factor : 0.0f; }

    // Execute
    void run();

    // Results
    Angle getDirection() const;

private:
    // Inheritance implementation
    FANA* clone() const;

    static float _obstacleRadiusFactor;

    // Internal
    void addObstacle(const Position &pos, const double &radius = OBSTACLE_RADIUS);

    // Resultant angle
    Angle _resultantAngle;
    bool _hysteresisHigh;

    // Obstacles list
    QList<Obstacle> _obstacles;
};

#endif // FANA_H
