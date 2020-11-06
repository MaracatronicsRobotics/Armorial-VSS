#include "newpid.h"

NewPID::NewPID()
{
    _kpTheta = 0.0;
    _kdTheta = 0.0;

    _kpVel = 0.0;
    _kdVel = 0.0;

    _max = 0.0;
    _min = 0.0;

    _lastErrorVel = 0.0;
    _lastErrorTheta = 0.0;

}

NewPID::NewPID(double kpTheta, double kdTheta, double kpVel, double kdVel, double max, double min){

    _kpTheta = kpTheta;
    _kdTheta = kdTheta;

    _kpVel = kpVel;
    _kdVel = kdVel;

    _max = max;
    _min = min;

    _lastErrorVel = 0.0;
    _lastErrorTheta = 0.0;
}

NewPID::~NewPID(){
    //delete _timer;
}

void NewPID::setPIDParameters(double kpTheta, double kdTheta, double kpVel, double kdVel, double max, double min){
    _kpTheta = kpTheta;
    _kdTheta = kdTheta;

    _kpVel = kpVel;
    _kdVel = kdVel;

    _max = max;
    _min = min;

    _pre_error = 0.0;
    _integral = 0.0;
}

QList<double> NewPID::calculate(double vel, double orientation, double disiredOrientation, double disiredVel){

    //Definindo parametros
    _newSpeedLinearRotate.clear();
    _theta = orientation;
    _vel = vel;
    _thetaRef = disiredOrientation;
    _velRef = disiredVel;

    //Calculo da velocidade de rotação corrigida
    _errorTheta = _theta - _thetaRef;
    _rotateSpeed = - _kpTheta * _errorTheta - _kdTheta * (_errorTheta - _lastErrorTheta);

    //Calculo da velocidade linear corrigida
    _errorVel = _vel - _velRef;
    _linearSpeed = - _kpVel * _errorVel - _kdVel * (_errorVel - _lastErrorVel);


    _newSpeedLinearRotate.append(_linearSpeed);
    _newSpeedLinearRotate.append(_rotateSpeed);

    return _newSpeedLinearRotate;
}
