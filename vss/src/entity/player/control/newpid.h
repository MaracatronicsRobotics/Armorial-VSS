#ifndef NEWPID_H
#define NEWPID_H

#include <src/utils/timer/timer.h>
#include <iostream>
#include <QList>

class NewPID
{
public:
    NewPID();
    NewPID(double kpTheta, double kdTheta, double kpVel, double kdVel, double max, double min);
    ~NewPID();
    void setPIDParameters(double kpTheta, double kdTheta, double kpVel, double kdVel, double max, double min);
    QList<double> calculate(double vel, double orientation, double disiredOrientation, double disiredVel);
private:
    //Timer *_timer; // timer to get _dt
    double _kpTheta, _kiTheta, _kdTheta, _kpVel, _kiVel, _kdVel; // pid terms
    double _max, _min; // max and min values
    double _integral, _pre_error; // save last error and integral sum

    double _lastErrorTheta, _errorTheta, _lastErrorVel, _errorVel;
    double _thetaRef, _velRef, _theta, _vel, _rotateSpeed, _linearSpeed;

    QList<double> _newSpeedLinearRotate;
};

#endif // NEWPID_H
