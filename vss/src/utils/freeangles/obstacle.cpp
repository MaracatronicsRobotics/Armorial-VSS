#include "obstacle.h"
#include <src/utils/utils.h>
#include <src/const/constants.h>
#include <iostream>

Obstacle::Obstacle(){
    _position.setUnknown();
    _initialAngle = _finalAngle = 0;
    _radius = 0;
}

void Obstacle::calcAnglesFrom(const Position &watcher, float radiusFactor){
    if(_radius == 0){
        std::cout << "Raio = 0";
        return;
    }

    const float R = radiusFactor * _radius;

    // angulação/distancia do obtaculo
    const float dist = WR::Utils::distance(watcher, _position);
    const float fix = atan2(R, dist);

    // calcular o angulo
    float angle = WR::Utils::getAngle(watcher, _position);
    _initialAngle = angle - fix;
    _finalAngle = angle + fix;

    // ter certeza que os angulos calculados estao entre 0-360 graus
    WR::Utils::angleLimitZeroTwoPi(&_initialAngle);
    WR::Utils::angleLimitZeroTwoPi(&_finalAngle);
}
