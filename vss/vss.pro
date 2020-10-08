TEMPLATE = app
DESTDIR  = ../bin
TARGET   = Maracatronics-VSS
VERSION  = 0.0.1

# Temporary dirs
OBJECTS_DIR = tmp/obj
MOC_DIR = tmp/moc
UI_DIR = tmp/moc
RCC_DIR = tmp/rc

CONFIG += c++14 console
CONFIG -= app_bundle
QT += core \
        gui \
        widgets \
        network \
        opengl

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
LIBS *= -lprotobuf -lGLU -pthread -lGEARSystem -lomniORB4 -lomnithread -lQt5Core -lpthread


# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        include/vssref_command.pb.cc \
        include/vssref_common.pb.cc \
        include/vssref_placement.pb.cc \
        main.cpp \
        src/const/constants.cpp \
        src/entity/controlmodule/coach/coach.cpp \
        src/entity/controlmodule/coach/coachutils.cpp \
        src/entity/controlmodule/coach/playersdistribution.cpp \
        src/entity/controlmodule/controlmodule.cpp \
        src/entity/controlmodule/playbook/basics/playbook_balance.cpp \
        src/entity/controlmodule/playbook/basics/playbook_defensive.cpp \
        src/entity/controlmodule/playbook/basics/playbook_halt.cpp \
        src/entity/controlmodule/playbook/basics/playbook_offensive.cpp \
        src/entity/controlmodule/playbook/playbook.cpp \
        src/entity/controlmodule/strategy/basics/strategy_halt.cpp \
        src/entity/controlmodule/strategy/essentials/mrcstrategy.cpp \
        src/entity/controlmodule/strategy/essentials/vssstrategy.cpp \
        src/entity/controlmodule/strategy/strategy.cpp \
        src/entity/controlmodule/strategy/strategystate.cpp \
        src/entity/controlmodule/vssteam.cpp \
        src/entity/entity.cpp \
        src/entity/locations.cpp \
        src/entity/player/behaviour/basics/behaviour_assistant.cpp \
        src/entity/player/behaviour/basics/behaviour_attacker.cpp \
        src/entity/player/behaviour/basics/behaviour_barrier.cpp \
        src/entity/player/behaviour/basics/behaviour_donothing.cpp \
        src/entity/player/behaviour/basics/behaviour_goalkeeper.cpp \
        src/entity/player/behaviour/basics/behaviour_goback.cpp \
        src/entity/player/behaviour/basics/behaviour_pushball.cpp \
        src/entity/player/behaviour/basics/behaviour_stayback.cpp \
        src/entity/player/behaviour/behaviour.cpp \
        src/entity/player/control/pid.cpp \
        src/entity/player/navigation/fana.cpp \
        src/entity/player/navigation/navalgorithm.cpp \
        src/entity/player/navigation/navigation.cpp \
        src/entity/player/navigation/rrt/rrt.cpp \
        src/entity/player/navigation/rrt/rrtnode.cpp \
        src/entity/player/navigation/rrt/rrtobstacle.cpp \
        src/entity/player/navigation/rrt/rrttree.cpp \
        src/entity/player/navigation/rrt/rrtvertex.cpp \
        src/entity/player/playeraccess.cpp \
        src/entity/player/playerbus.cpp \
        src/entity/player/role/basics/role_defender.cpp \
        src/entity/player/role/basics/role_supporter.cpp \
        src/entity/player/role/basics/role_goalkeeper.cpp \
        src/entity/player/role/basics/role_halt.cpp \
        src/entity/player/role/role.cpp \
        src/entity/player/skill/basics/skill_donothing.cpp \
        src/entity/player/skill/basics/skill_spin.cpp \
        src/entity/player/skill/basics/skill_goto.cpp \
        src/entity/player/skill/basics/skill_interceptball.cpp \
        src/entity/player/skill/basics/skill_pushball.cpp \
        src/entity/player/skill/basics/skill_rotateto.cpp \
        src/entity/player/skill/skill.cpp \
        src/entity/player/vssplayer.cpp \
        src/entity/referee/vssreferee.cpp \
        src/entity/world/world.cpp \
        src/entity/world/worldmapupdater.cpp \
        src/exithandler.cpp \
        src/gui/mainwindow.cpp \
        src/gui/soccerview/soccerview.cpp \
        src/gui/soccerview/util/glfield.cpp \
        src/gui/soccerview/util/gltext.cpp \
        src/gui/vssgui.cpp \
        src/instancechecker.cpp \
        src/utils/fields/field.cpp \
        src/utils/fields/field_vss.cpp \
        src/utils/fieldside/fieldside.cpp \
        src/utils/freeangles/freeangles.cpp \
        src/utils/freeangles/obstacle.cpp \
        src/utils/graph/edge.cc \
        src/utils/graph/graph.cc \
        src/utils/graph/vertex.cc \
        src/utils/knn/knn.cpp \
        src/utils/timer/timer.cpp \
        src/utils/utils.cpp \
        src/utils/vssclient/netraw.cpp \
        src/utils/vssclient/vssclient.cpp \
        src/vss.cpp \
    src/entity/player/role/basics/role_striker.cpp


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    include/vssref_command.pb.h \
    include/vssref_common.pb.h \
    include/vssref_placement.pb.h \
    src/const/constants.h \
    src/entity/controlmodule/coach/basecoach.h \
    src/entity/controlmodule/coach/coach.h \
    src/entity/controlmodule/coach/coachutils.h \
    src/entity/controlmodule/coach/playersdistribution.h \
    src/entity/controlmodule/controlmodule.h \
    src/entity/controlmodule/playbook/basics/playbook_balance.h \
    src/entity/controlmodule/playbook/basics/playbook_defensive.h \
    src/entity/controlmodule/playbook/basics/playbook_halt.h \
    src/entity/controlmodule/playbook/basics/playbook_offensive.h \
    src/entity/controlmodule/playbook/playbook.h \
    src/entity/controlmodule/playbook/vssplaybooks.h \
    src/entity/controlmodule/strategy/basics/strategy_halt.h \
    src/entity/controlmodule/strategy/essentials/mrcstrategy.h \
    src/entity/controlmodule/strategy/essentials/vssstrategy.h \
    src/entity/controlmodule/strategy/strategy.h \
    src/entity/controlmodule/strategy/strategystate.h \
    src/entity/controlmodule/strategy/vssstrategies.h \
    src/entity/controlmodule/vssteam.h \
    src/entity/entity.h \
    src/entity/locations.h \
    src/entity/player/baseplayer.h \
    src/entity/player/behaviour/basics/behaviour_assistant.h \
    src/entity/player/behaviour/basics/behaviour_attacker.h \
    src/entity/player/behaviour/basics/behaviour_barrier.h \
    src/entity/player/behaviour/basics/behaviour_donothing.h \
    src/entity/player/behaviour/basics/behaviour_goalkeeper.h \
    src/entity/player/behaviour/basics/behaviour_goback.h \
    src/entity/player/behaviour/basics/behaviour_pushball.h \
    src/entity/player/behaviour/basics/behaviour_stayback.h \
    src/entity/player/behaviour/behaviour.h \
    src/entity/player/behaviour/vssbehaviours.h \
    src/entity/player/control/pid.h \
    src/entity/player/navigation/fana.h \
    src/entity/player/navigation/navalgorithm.h \
    src/entity/player/navigation/navigation.h \
    src/entity/player/navigation/rrt/rrt.h \
    src/entity/player/navigation/rrt/rrtnode.h \
    src/entity/player/navigation/rrt/rrtobstacle.h \
    src/entity/player/navigation/rrt/rrttree.h \
    src/entity/player/navigation/rrt/rrtvertex.h \
    src/entity/player/playeraccess.h \
    src/entity/player/playerbus.h \
    src/entity/player/role/basics/role_defender.h \
    src/entity/player/role/basics/role_supporter.h \
    src/entity/player/role/basics/role_goalkeeper.h \
    src/entity/player/role/basics/role_halt.h \
    src/entity/player/role/role.h \
    src/entity/player/role/vssroles.h \
    src/entity/player/skill/basics/skill_donothing.h \
    src/entity/player/skill/basics/skill_spin.h \
    src/entity/player/skill/basics/skill_goto.h \
    src/entity/player/skill/basics/skill_interceptball.h \
    src/entity/player/skill/basics/skill_pushball.h \
    src/entity/player/skill/basics/skill_rotateto.h \
    src/entity/player/skill/skill.h \
    src/entity/player/skill/vssskills.h \
    src/entity/player/vssplayer.h \
    src/entity/referee/vssreferee.h \
    src/entity/world/world.h \
    src/entity/world/worldmapupdater.h \
    src/exithandler.h \
    src/gui/mainwindow.h \
    src/gui/soccerview/soccerview.h \
    src/gui/soccerview/util/field_default_constants.h \
    src/gui/soccerview/util/geometry.h \
    src/gui/soccerview/util/glfield.h \
    src/gui/soccerview/util/gltext.h \
    src/gui/soccerview/util/gvector.h \
    src/gui/soccerview/util/timer.h \
    src/gui/soccerview/util/util.h \
    src/gui/vssgui.h \
    src/instancechecker.h \
    src/utils/color/color.h \
    src/utils/fields/field.h \
    src/utils/fields/field_vss.h \
    src/utils/fields/fields.h \
    src/utils/fieldside/fieldside.h \
    src/utils/fieldside/side.h \
    src/utils/freeangles/freeangles.h \
    src/utils/freeangles/obstacle.h \
    src/utils/graph/basegraph.hh \
    src/utils/graph/edge.hh \
    src/utils/graph/graph.hh \
    src/utils/graph/vertex.hh \
    src/utils/knn/knn.h \
    src/utils/timer/timer.h \
    src/utils/utils.h \
    src/utils/vssclient/netraw.h \
    src/utils/vssclient/util.h \
    src/utils/vssclient/vssclient.h \
    src/vss.h \
    src/entity/player/role/basics/role_striker.h

FORMS += \
    src/gui/mainwindow.ui

RESOURCES += \
    resources/resources.qrc
