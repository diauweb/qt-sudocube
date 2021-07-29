#include "gamewindow.h"
#include "./ui_gamewindow.h"
#include <QMessageBox>
#include <QFile>
#include <QStyle>
#include <QMouseEvent>
#include <QDebug>
#include <cubewidget.h>
#include <QInputDialog>

GameWindow::GameWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GameWindow)
    , logic(new GameLogic)
{
    ui->setupUi(this);

    addCubes();
    logic->newGame();

    connect(logic, &GameLogic::boardChanged, this, &GameWindow::boardUpdate);
    connect(logic, &GameLogic::blockCandidateChanged, this, &GameWindow::candidateChanged);

    connect(ui->actionboardChanged, &QAction::triggered, this, &GameWindow::boardUpdate);
    connect(ui->actioncandidateChanged, &QAction::triggered, this, &GameWindow::candidateChanged);
}

GameWindow::~GameWindow()
{
    delete ui;
    delete logic;
}

void GameWindow::addCubes() {
    QFile file(":/style/board.qss");
    QString stylesheet;
    if (file.open(QFile::ReadOnly)) {
        stylesheet = file.readAll();
    }

    for(int y = 0; y < 9; y++){
        for(int x = 0; x < 9; x++) {
            CubeWidget *widget = new CubeWidget(x, y, 0);
            widget->setFixedSize(50, 50);
            widget->setStyleSheet(stylesheet);
            widget->usePattern(true);

            connect(widget, &CubeWidget::pressed, this, &GameWindow::blockTriggered);
            connect(widget, &CubeWidget::hovered, this, &GameWindow::blockHovered);
            ui->gridLayout->addWidget(widget, y, x);
            this->displayBlockList.append(widget);
        }
    }

    // candidates block
    QGridLayout *layout[] = { ui->gridLayoutCandidate0, ui->gridLayoutCandidate1, ui->gridLayoutCandidate2 };
    for(int k = 0; k < 3; k++) {
        for(int y = 0; y < 3; y++) {
            for(int x = 0; x < 3; x++) {
                CubeWidget *widget = new CubeWidget(x, y, k + 1);

                widget->setFixedSize(46, 46);
                widget->setStyleSheet(stylesheet);

                connect(widget, &CubeWidget::pressed, this, &GameWindow::blockTriggered);
                layout[k]->addWidget(widget, y, x);
                this->candidateList[k].append(widget);
            }
        }
    }
}

void GameWindow::on_actionStart_Game_triggered()
{
    logic->newGame();
}


void GameWindow::on_actionExit_triggered()
{
    QApplication::exit();
}

void GameWindow::boardUpdate() {
    foreach (auto&& w, this->displayBlockList) {
        int x = w->boardX();
        int y = w->boardY();

        w->setState(logic->getBlock(x, y));

        w->style()->unpolish(w);
        w->style()->polish(w);
    }
}

void GameWindow::candidateChanged() {
    for(int i = 0; i < 3; i++) {
        int* myPat = logic->getPattern(logic->getCandidateId(i));

        bool sel = logic->selectedSlot() == i;
        foreach (auto&& w, this->candidateList[i]) {
            w->setProperty("preview", false);

            int state = myPat[w->boardY() * 3 + w->boardX()];
            w->setState(state);
            if (state) {
                w->setProperty("preview", sel);
            }

            w->style()->unpolish(w);
            w->style()->polish(w);
        }
    }
}

void GameWindow::on_actionsetblock_triggered()
{
    int x = QInputDialog::getInt(this, nullptr, "X");
    int y = QInputDialog::getInt(this, nullptr, "Y");
    int t = QInputDialog::getInt(this, nullptr, "Type");
    logic->setBlock(x, y, t);
    emit logic->boardChanged();
}

void GameWindow::on_actionsetcandidates_triggered()
{
    int a0 = QInputDialog::getInt(this, nullptr, "Candidate #1");
    int a1 = QInputDialog::getInt(this, nullptr, "Candidate #2");
    int a2 = QInputDialog::getInt(this, nullptr, "Candidate #3");
    logic->setCandidateId(0, a0);
    logic->setCandidateId(1, a1);
    logic->setCandidateId(2, a2);
    emit logic->blockCandidateChanged();
}

void GameWindow::blockTriggered(int x, int y, int src) {
    if(src == 0) {
        int patId = logic->getSelectedPatternId();
        if (patId == -1) return;

        if (logic->canPlace(patId, x, y)) {
            logic->place(patId, x, y);
            logic->findClear();
            logic->setCandidateId(logic->selectedSlot(), 0);
            logic->setSelectedSlot(-1);

            for (int i = 0; i < 3; i++) {
                if (logic->getCandidateId(i)) goto fall;
            }
            logic->spawnCandidates();
        }
    } else {
        logic->setSelectedSlot(src - 1);
    }

    fall:
    emit logic->blockCandidateChanged();
}

void GameWindow::blockHovered(int x, int y, bool enter) {
    if (enter) {
        int patId = logic->getSelectedPatternId();
        if (patId == -1) return;
        if (!logic->canPlace(patId, x, y)) return;
        int* pat = logic->getPattern(patId);

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                int mapX = x + j - 1;
                int mapY = y + i - 1;
                if (logic->getBlock(mapX, mapY) || !pat[i*3 + j]) continue;
                CubeWidget* w = this->displayBlockList.at(mapY * logic->BOARD_SIZE + mapX);
                w->setProperty("preview", true);
                w->style()->unpolish(w);
                w->style()->polish(w);
            }
        }
    } else {
        foreach (auto&& w, this->displayBlockList) {
            if(w->property("preview").toBool()) {
                w->setProperty("preview", false);
                w->style()->unpolish(w);
                w->style()->polish(w);
            }
        }
    }
}
