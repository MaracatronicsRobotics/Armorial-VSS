#include "behaviour_barrier.h"


//raio da bola eh 0.021
//raio do gol eh 0.2

//constantes a mudar ainda para as dimensoes do vsss
#define INTERCEPT_MINBALLVELOCITY 0.1f//hyperparameter
#define INTERCEPT_MINBALLDIST 0.2f //hyperparameter
#define ERROR_GOAL_OFFSET 0.05f//hyperparameter

QString Behaviour_Barrier::name() {
    return "Behaviour_Barrier";
}

Behaviour_Barrier::Behaviour_Barrier()
{
    setD(0.05);
    setRadius(0.21);//hyperparameter para distancia do centro do gol

    _sk_goto = NULL;
    _sk_intercept = NULL;
    _sk_spin = NULL;

}

void Behaviour_Barrier::configure(){
    usesSkill(_sk_goto = new Skill_GoTo());
    usesSkill(_sk_intercept = new Skill_InterceptBall());
    usesSkill(_sk_spin = new Skill_Spin());

    //Setting initial skill
    setInitialSkill(_sk_goto);

    //transitions
    addTransition(STATE_GOTO , _sk_spin , _sk_goto);
    addTransition(STATE_GOTO , _sk_intercept , _sk_goto);

    addTransition(STATE_INTERCEPT , _sk_spin , _sk_intercept);
    addTransition(STATE_INTERCEPT , _sk_goto , _sk_intercept);

    addTransition(STATE_SPIN , _sk_goto , _sk_spin);
    addTransition(STATE_SPIN , _sk_intercept , _sk_spin);

}

void Behaviour_Barrier::run(){
    //Pos barrier
    Position goalProjection = WR::Utils::projectPointAtSegment(loc()->ourGoalRightMidPost(), loc()->ourGoalLeftMidPost(), loc()->ball());
    Position desiredPosition = WR::Utils::threePoints(goalProjection, loc()->ball(), _radius, 0.0f);

    // Position to look
    //Position aimPosition = WR::Utils::threePoints(loc()->ourGoal(), loc()->ball(), 1000.0f, 0.0); // high distance (always will look)

    // Error goal (desiredPosition sometimes goes off the field)
        if(loc()->ourSide().isRight() && desiredPosition.x() > (loc()->ourGoal().x()-ERROR_GOAL_OFFSET)){
            desiredPosition.setPosition(loc()->ourGoal().x()-ERROR_GOAL_OFFSET, desiredPosition.y(), 0.0);
        }else if(loc()->ourSide().isLeft() && desiredPosition.x() < (loc()->ourGoal().x()+ERROR_GOAL_OFFSET)){
            desiredPosition.setPosition(loc()->ourGoal().x()+ERROR_GOAL_OFFSET, desiredPosition.y(), 0.0);
        }
    //setting goto
    _sk_goto->setGoToPos(desiredPosition);
    //setting intercept dont need

    //setting spin
    if(loc()->ourSide().isLeft()){//se for lado esquerdo
        if(player()->position().y() >=0){
            _sk_spin->setClockWise(true);
        }
        else{
            _sk_spin->setClockWise(false);
        }

    }
    else{//lado direito tem que inverter o giro
        if(player()->position().y() >=0){
            _sk_spin->setClockWise(false);
        }
        else{
            _sk_spin->setClockWise(true);
        }
    }

    //Transitions
    if(player()->distBall() > INTERCEPT_MINBALLDIST && isBallComingToGoal(INTERCEPT_MINBALLVELOCITY)){
        enableTransition(STATE_INTERCEPT);
    } else {
        if(player()->distBall() <=0.075){ //hyperparameter
            enableTransition(STATE_SPIN);
        } else {
            enableTransition(STATE_GOTO);
        }
    }

}

bool Behaviour_Barrier::isBallComingToGoal(float minSpeed , float postsFactor){
    // postFactor é caso queiramos alongar mais a posição y da barra (margem de erro)
    const Position posBall = loc()->ball();
    const Position posRightPost(true, loc()->ourGoalRightPost().x(), loc()->ourGoalRightPost().y()*postsFactor, 0.0);
    const Position posLeftPost(true, loc()->ourGoalLeftPost().x(), loc()->ourGoalLeftPost().y()*postsFactor, 0.0);

    // Check ball velocity
        if(loc()->ballVelocity().abs()<minSpeed)
             return false;
    // Angle ball velocity
    float angVel = loc()->ballVelocity().arg().value();
    float angRightPost = WR::Utils::getAngle(posBall, posRightPost);
    float angLeftPost = WR::Utils::getAngle(posBall, posLeftPost);
    float angDiffPosts = fabs(WR::Utils::angleDiff(angRightPost, angLeftPost));

    // Check angle difference with posts
    float angDiffRight = fabs(WR::Utils::angleDiff(angVel, angRightPost));
    float angDiffLeft = fabs(WR::Utils::angleDiff(angVel, angLeftPost));

    return (angDiffRight<angDiffPosts && angDiffLeft<angDiffPosts);
}
