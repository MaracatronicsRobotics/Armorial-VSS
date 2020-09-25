#ifndef VSSGUI_H
#define VSSGUI_H

#include <src/entity/entity.h>
#include <src/gui/mainwindow.h>
#include <src/const/constants.h>

#include <src/entity/controlmodule/strategy/strategystate.h>
#include <src/entity/controlmodule/playbook/playbook.h>

class VSSGui : public Entity
{
public:
    QString name();
    VSSGui(VSSTeam *ourTeam, VSSTeam *theirTeam);

    static void updateTree(StrategyState *strat);
    static bool _isEnabled;

private:
    void initialization();
    void loop();
    void finalization();

    static MainWindow *_mainWindow;
    Timer _timer;
    double timeToUpdate;

    VSSTeam *_ourTeam;
    VSSTeam *_theirTeam;
};

#endif // VSSGUI_H
