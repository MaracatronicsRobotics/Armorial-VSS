#include "behaviour_barrier.h"


//raio da bola eh 0.021
//raio do gol eh 0.2

//constantes a mudar ainda para as dimensoes do vsss
#define INTERCEPT_MINBALLVELOCITY 0.1f//hyperparameter
#define INTERCEPT_MINBALLDIST 0.2f //hyperparameter
#define ERROR_GOAL_OFFSET 0.05f//hyperparameter

#define BALLPREVISION_MINVELOCITY 0.02f
#define BALLPREVISION_VELOCITY_FACTOR 3.0f
#define BALLPREVISION_FACTOR_LIMIT 0.15f

QString Behaviour_Barrier::name() {
    return "Behaviour_Barrier";
}

Behaviour_Barrier::Behaviour_Barrier()
{
    setD(0.05f);
    setRadius(0.29f);//hyperparameter para distancia do centro do gol

    _sk_goto = nullptr;
    _sk_intercept = nullptr;
    _sk_spin = nullptr;

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
    Position goalProjection;

    //considering ball velocity to define player position
    if(loc()->ballVelocity().abs() > BALLPREVISION_MINVELOCITY){
        Position projectedBall;
        //calc unitary vector of velocity
        const Position velUni(true, loc()->ballVelocity().x()/loc()->ballVelocity().abs(), loc()->ballVelocity().y()/loc()->ballVelocity().abs(), 0.0);

        //calc velocity factor
        float factor = BALLPREVISION_VELOCITY_FACTOR*loc()->ballVelocity().abs();
        WR::Utils::limitValue(&factor, 0.0f, BALLPREVISION_FACTOR_LIMIT);

        //calc projected position
        const Position delta(true, factor*velUni.x(), factor*velUni.y(), 0.0);
        Position projectedPos(true, loc()->ball().x()+delta.x(), loc()->ball().y()+delta.y(), 0.0);
        projectedBall = projectedPos;

        goalProjection = WR::Utils::hasInterceptionSegments(loc()->ball(), projectedBall, loc()->ourGoalRightMidPost(), loc()->ourGoalLeftMidPost());
    }
    //if interception isn't inside our defense area: consider only ball position
    if(!(abs(goalProjection.y()) < loc()->fieldDefenseLength()/2.0f) || !goalProjection.isValid()){
        goalProjection = WR::Utils::projectPointAtSegment(loc()->ourGoalRightMidPost(), loc()->ourGoalLeftMidPost(), loc()->ball());
    }

    //Pos barrier
    Position desiredPosition = WR::Utils::threePoints(goalProjection, loc()->ball(), _radius, 0.0f);

    // Position to look
    //Position aimPosition = WR::Utils::threePoints(loc()->ourGoal(), loc()->ball(), 1000.0f, 0.0); // high distance (always will look)

    // Error goal (desiredPosition sometimes goes off the field)
    if(loc()->ourSide().isRight() && desiredPosition.x() > (loc()->ourGoal().x()-ERROR_GOAL_OFFSET)){
        desiredPosition.setPosition(loc()->ourGoal().x()-ERROR_GOAL_OFFSET, desiredPosition.y(), 0.0);
    }else if(loc()->ourSide().isLeft() && desiredPosition.x() < (loc()->ourGoal().x()+ERROR_GOAL_OFFSET)){
        desiredPosition.setPosition(loc()->ourGoal().x()+ERROR_GOAL_OFFSET, desiredPosition.y(), 0.0);
    }

    //if ball is inside our defense area (function isInsideOurArea doesn't consider the area inside the goal)
    if(fabs(loc()->ball().x()) > (loc()->fieldMaxX() - loc()->fieldDefenseWidth()) && fabs(loc()->ball().y()) < loc()->fieldDefenseLength()/2.0f && loc()->isInsideOurField(loc()->ball())){
        desiredPosition = projectPosOutsideGoalArea(loc()->ball(), true, false);
    }

    //setting goto
    _sk_goto->setGoToPos(desiredPosition);
    //adjusting velocity factor according to: ball distance to our goal and ball velocity
    float velFacDist = 1 - (loc()->distBallOurGoal()/WR::Utils::distance(loc()->fieldLeftTopCorner(), Position(true, loc()->fieldMaxX(), 0.0f, 0.0f)));
    float velFacVel = (loc()->ballVelocity().abs())/15.0f;
    float velFac = (velFacDist + velFacVel) * 16.0f;
    if(velFac < 4.0f) velFac = 4.0f;
    _sk_goto->setGoToVelocityFactor(velFac);

    //setting intercept
    float multFactor = 1.0;
    if(loc()->ourSide().isLeft()){
        multFactor = -1.0;
    }
    Position interceptPoinLeft(true, multFactor * (loc()->fieldMaxX() - loc()->fieldDefenseWidth()), multFactor * loc()->fieldDefenseLength()/2.0f, 0.0f);
    Position interceptPointRight(true, multFactor * (loc()->fieldMaxX() - loc()->fieldDefenseWidth()), -1.0f * multFactor * loc()->fieldDefenseLength()/2.0f, 0.0f);
    _sk_intercept->setInterceptSegment(interceptPoinLeft, interceptPointRight);
    _sk_intercept->setDesiredVelocity(4.0f);

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
    /*
    //Transitions with intercept skill
    if(player()->distBall() > INTERCEPT_MINBALLDIST && isBallComingToGoal(INTERCEPT_MINBALLVELOCITY)){
        enableTransition(STATE_INTERCEPT);
    } else {
        if(player()->distBall() <=0.075f){ //hyperparameter
            enableTransition(STATE_SPIN);
        } else {
            enableTransition(STATE_GOTO);
        }
    }
    */
    //Transitions without intercept skill
    if(player()->distBall() <=0.075f){ //hyperparameter
        //std::cout<<"SPIN\n";
        enableTransition(STATE_SPIN);
    } else {
        //std::cout<<"GO TO\n";
        enableTransition(STATE_GOTO);
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

Position Behaviour_Barrier::projectPosOutsideGoalArea(Position pos, bool avoidOurArea, bool avoidTheirArea){
    Position L1, L2, R1, R2, goal;
    bool shouldProjectPos = false, isOurArea = false;
    float smallMargin = 0.05f;

    if(fabs(pos.x()) > (loc()->fieldMaxX() - loc()->fieldDefenseWidth()) && fabs(pos.y()) < loc()->fieldDefenseLength()/2){
        shouldProjectPos = true;
        //check if desiredPosition is inside our defense area and if we should avoid it
        if(loc()->isInsideOurField(pos) && avoidOurArea){
            shouldProjectPos = true;
            isOurArea = true;
            goal = loc()->ourGoal();
        }
        //check if desiredPosition is inside their defense area and if we should avoid it
        else if(loc()->isInsideTheirField(pos) && avoidTheirArea){
            shouldProjectPos = true;
            isOurArea = false;
            goal = loc()->theirGoal();
        }else{
            shouldProjectPos = false;
        }
    }
    //if should project position outside a defense area
    if(shouldProjectPos){
        //multiplying factor changes if area's team is the left team or the right team
        float mult = -1.0;
        if(isOurArea){
            if(loc()->ourSide().isLeft()) mult = 1.0;
        }else{
            if(loc()->theirSide().isLeft()) mult = 1.0;
        }
        //getting segments

        //left segment points (defense area)
        L1 = Position(true, -mult*loc()->fieldMaxX(), loc()->fieldDefenseLength()/2 + smallMargin, 0.0f);
        L2 = Position(true, L1.x() + mult * (loc()->fieldDefenseWidth() + smallMargin), L1.y(), 0.0f);
        //right segment points (defense area)
        R1 = Position(true, -mult*loc()->fieldMaxX(), -(loc()->fieldDefenseLength()/2 + smallMargin), 0.0f);
        R2 = Position(true, R1.x() + mult * (loc()->fieldDefenseWidth() + smallMargin), R1.y(), 0.0f);
        //front segment is composed by L2 and R2 (defense area)

        //projecting position on segments L1->L2, R1->R2, L2->R2
        Position pointProjLeft = WR::Utils::projectPointAtSegment(L1, L2, pos);
        Position pointProjRight = WR::Utils::projectPointAtSegment(R1, R2, pos);
        Position pointProjFront = WR::Utils::projectPointAtSegment(L2, R2, pos);

        //interception points between the segment playerPos->pos and defense area segments (L1->L2, R1->R2, L2->R2)
        Position left = WR::Utils::hasInterceptionSegments(L1, L2, goal, pos);
        Position right = WR::Utils::hasInterceptionSegments(R1, R2, goal, pos);
        Position front = WR::Utils::hasInterceptionSegments(L2, R2, goal, pos);

        //if there is an interception between playerPos->pos and L1->L2 on L1->L2
        if(left.isValid() && fabs(left.x()) >= (loc()->fieldMaxX() - (loc()->fieldDefenseWidth()+smallMargin)) && fabs(left.x()) <= loc()->fieldMaxX()){
            return pointProjLeft;
        }
        //if there is an interception between playerPos->pos and R1->R2 on R1->R2
        else if(right.isValid() && fabs(right.x()) >= (loc()->fieldMaxX() - (loc()->fieldDefenseWidth()+smallMargin)) && fabs(right.x()) <= loc()->fieldMaxX()){
            return pointProjRight;
        }
        //if there is an interception between playerPos->pos and L2->R2 on L2->R2
        else if(front.isValid() && fabs(front.x()) >= (loc()->fieldMaxX() - (loc()->fieldDefenseWidth()+smallMargin)) && fabs(front.x()) <= loc()->fieldMaxX()){
            return pointProjFront;
        }else{
            return pos;
        }
    }else{
        return pos;
    }
}
