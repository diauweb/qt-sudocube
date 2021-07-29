#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include "gamelogic.h"
#include "cubewidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class GameWindow; }
QT_END_NAMESPACE

class GameWindow : public QMainWindow
{
    Q_OBJECT

public:
    GameWindow(QWidget *parent = nullptr);
    ~GameWindow();

private slots:
    void on_actionStart_Game_triggered();
    void on_actionExit_triggered();
    void boardUpdate();
    void candidateChanged();
    void blockTriggered(int x, int y, int src);
    void blockHovered(int x, int y, bool hover);

    void on_actionsetblock_triggered();

    void on_actionsetcandidates_triggered();

private:
    Ui::GameWindow *ui;
    GameLogic *logic;

    void addCubes();
    QList<CubeWidget*> displayBlockList;
    QList<CubeWidget*> candidateList[3];
};
#endif // GAMEWINDOW_H
