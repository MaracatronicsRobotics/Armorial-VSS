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

#include "skill_spin.h"

#define SPIN_SPEED 50.0f

QString Skill_Spin::name() {
    return "Skill_Spin";
}

Skill_Spin::Skill_Spin() {
    _isClockWise = true;
}

void Skill_Spin::run() {
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
    float velLinear = 0.0f;
    float velAngular;

    if (_isClockWise) velAngular = -SPIN_SPEED;
    else velAngular = SPIN_SPEED;

    float wr = ((2.0f * velLinear) + (L * velAngular)) / (2.0f * r);
    float wl = wr - ((L * velAngular) / r);

    player()->setSpeed(wl, wr);
}
