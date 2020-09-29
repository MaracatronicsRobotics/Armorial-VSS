#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <GEARSystem/gearsystem.hh>

class Obstacle{
private:
    Position _position;
    quint8 _team;
    quint8 _id;
    float _initialAngle;
    float _finalAngle;
    float _radius;

public:
    Obstacle();

    Position& position() { return _position; }
    quint8& team() { return _team; }
    quint8& id() { return _id; }
    float& initialAngle() { return _initialAngle; }
    float& finalAngle() { return _finalAngle; }
    float& radius() { return _radius; }

    void calcAnglesFrom(const Position &watcher, float factor = 1.0f);

    Obstacle &operator=(const Obstacle &obstacle) {
        _position = obstacle._position;
        _team = obstacle._team;
        _id = obstacle._id;
        _initialAngle = obstacle._initialAngle;
        _finalAngle = obstacle._finalAngle;
        _radius = obstacle._radius;
        return *this;
    }
};

#endif // OBSTACLE_H
