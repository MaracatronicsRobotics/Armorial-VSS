#ifndef FREEANGLES_H
#define FREEANGLES_H

#include <GEARSystem/gearsystem.hh>
#include <src/entity/controlmodule/coach/basecoach.h>
#include "obstacle.h"

class FreeAngles{
public:
    class Interval{
    private:
        float _angInitial;
        float _angFinal;
        bool _obstructed;

    public:
        Interval(){
            _angInitial = _angFinal = 0.0;
            _obstructed = false;
        }
        Interval(float angInitial, float angFinal, bool obstructed){
            _angInitial = angInitial;
            _angFinal = angFinal;
            _obstructed = obstructed;
        }

        float angInitial() const { return _angInitial; }
        float angFinal() const { return _angFinal; }
        bool obstructed() const { return _obstructed; }
    };

private:
    static VSSTeam *_ourTeam;
    static VSSTeam *_opTeam;

    static QList<Interval> _getFreeAngles(const Position &watcher, float initialPosAngle, float finalPosAngle, const QList<Obstacle> &obstacles, bool returnObstructed = false);

    static QList<Obstacle> calcObstaclesObstruction(const Position &watcher, const QList<Obstacle> &obstacles);

public:
    static void initialize(VSSTeam *ourTeam, VSSTeam *opTeam) { FreeAngles::_ourTeam = ourTeam; FreeAngles::_opTeam = opTeam; }

    static QList<Obstacle> getObstacles();
    static QList<Obstacle> getObstacles(const Position &watcher);
    static QList<Obstacle> getObstacles(const Position &watcher, float distanceRadius);

    static QList<Interval> getFreeAngles(const Position &watcher, const Position &initialPos, const Position &finalPos, bool returnObstructed = false);
    static QList<Interval> getFreeAngles(const Position &watcher, const Position &initialPos, const Position &finalPos, const QList<Obstacle> &obstacles, bool returnObstructed = false);

    static QList<Interval> getFreeAngles(const Position &watcher, float initialPosAngle, float finalPosAngle, bool returnObstructed = false);
    static QList<Interval> getFreeAngles(const Position &watcher, float initialPosAngle, float finalPosAngle, const QList<Obstacle> &obstacles, bool returnObstructed = false);
};
#endif // FREEANGLES_H
