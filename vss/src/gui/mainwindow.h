#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <src/entity/controlmodule/vssteam.h>
#include <QProgressBar>
#include <QLabel>
#include <QTreeWidgetItem>
#include <QGroupBox>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Update detection
    void updateDetection(VSSTeam *ourTeam, VSSTeam *theirTeam);

    // Robot Status
    void resetRobots();

    // Setters
    void setPlayerRole(quint8 id, QString role);
    void setPlayerBattery(quint8 id, int qt);
    void setPlayerStatus(int id, bool status);
    void setGameStage(QString stage);
    void setRefereeCommand(QString command);
    void setTimeLeft(QString timeLeft);

    // tree
    void addRoot();
    void resetTree(QString strat, QList<QString> playbookList, QMap<QString, QList<QString>> rolesList,
                       QMap<std::pair<QString, QString>, QList<std::pair<QString, quint8>>> playersList,
                       QMap<QString, QString> behavioursList);
    bool isContained(QTreeWidgetItem *parent, QString text);
    void removeOld(QTreeWidgetItem *parent, QList<QString> stringList);
    QTreeWidgetItem* addChild(QTreeWidgetItem* parent, QString text);

private:
    Ui::MainWindow *ui;

    // Widgets
    std::vector<QProgressBar*> playerBatteries;
    std::vector<std::pair<QLabel*, QLabel*>> playerRoles;
    std::vector<QPixmap> playerPixmaps;
    std::vector<QGroupBox*> playerBoxes;

    // Utils
    QPixmap getRolePixmap(QString role);
    QPixmap getPlaybookPixmap(QString playbook);

    // Role Tree
    QTreeWidget *treeWidget;
    QTreeWidgetItem* root;
    QMap<QTreeWidgetItem*, QList<QTreeWidgetItem*>> roles;
    QList<QTreeWidgetItem*> robots;
};

#endif // MAINWINDOW_H
