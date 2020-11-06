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

#include "skill_pushball.h"

QString Skill_PushBall::name(){
    return "Skill_PushBall";
}

Skill_PushBall::Skill_PushBall(){
}

void Skill_PushBall::run(){
    player()->setSpeed(_speed, _omega);
}

void Skill_PushBall::setSpeedAndOmega(float speed, float omega){
    // equações pra transformar em vr e vl
    /*
     * Onde:
     * L       = distancia entre as rodas
     * r       = raio da roda
     * VLinear = vx desejada
     * w       = velocidade angular desejada
     *
     * (2 * VLinear)/r = Wr + Wl
     * (w * L)/r       = Wr - Wl
     * Wr              = (2 * Vlin + w * L)/(2 * r)
     * Wl              = Wr - (w * L) / r
    */
    float L = 0.075f;
    float r = 0.0325f;

    float wr = ((2.0f * speed) + (L * omega)) / (2.0f * r);
    float wl = wr - ((L * omega) / r);

    _speed = wl;
    _omega = wr;
}
