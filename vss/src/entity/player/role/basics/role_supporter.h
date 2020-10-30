#ifndef ROLE_SUPPORTER_H
#define ROLE_SUPPORTER_H

#include <src/entity/player/behaviour/vssbehaviours.h>
#include <src/entity/player/role/role.h>

class Role_Supporter : public Role
{
    Q_OBJECT
private:
    // Behaviours

    Behaviour_Assistant *_bh_as;
    Behaviour_Barrier *_bh_br;
    Behaviour_StayBack *_bh_sb;

    // Behaviours ids!
    enum{
        BHV_STAYBACK,
        BHV_ASSISTANT,
        BHV_BARRIER
    };

    // positioning
    enum{
        BARRIER_PREDOMINANT,
        ASSIST_PREDOMINANT,
        FREE
    };

    // Inherited functions
    void configure();
    void run();

    //Coordenadas da quina de Cima
    float TopCorner_X ;
    float TopCorner_Y ;
    //Coordenadas da quina de Baixo
    float BottomCorner_X;
    float BottomCorner_Y;
    //Coordenadas auxiliares da Trave esquerda
    float Post_X ;
    float Post_Y ;
    //distancia constante da quina pra trave

    float dist_Corner_LeftPost ;

public:
    Role_Supporter();
    void initializeBehaviours();
    QString name();
    bool EnemyInOurField();
    int _positioning;
    int _bhv;
    bool BySideOfGoal();

signals:
    void sendSignal();

public slots:
    void receiveFoul(VSSRef::Foul foul, VSSRef::Quadrant quadrant, VSSRef::Color teamColor);
};

#endif // ROLE_SUPPORTER_H
