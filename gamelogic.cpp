#include "gamelogic.h"
#include<QMessageBox>
#include<QFile>
#include<QTextStream>
#include<QDebug>
#include<QRandomGenerator>

GameLogic::GameLogic(QObject *parent) : QObject(parent), iSelectedSlot(-1)
{
    this->flatSize = this->BOARD_SIZE * this->BOARD_SIZE;
    this->board = new int[flatSize]();
    this->candidateIds = new int[3]();

    /* index 0 is empty pattern */
    this->patterns.append(new int[3*3]());
    parsePatterns();
}

GameLogic::~GameLogic() {
    delete[] board;
    delete[] candidateIds;
}

void GameLogic::parsePatterns() {
    QFile file(":/pattern");
    if (file.open(QFile::ReadOnly)) {
        QTextStream qs(&file);
        while (!qs.atEnd()) {
            int *pat = new int[3*3]();
            for(int j = 0; j < 3; j++) {
                QString patStr = qs.readLine();
                for(int k = 0; k < 3; k++) {
                    pat[j * 3 + k] = patStr.at(k) == '#';
                }
            }
            this->patterns.append(pat);
            qs.readLine();
        }
    }
}

int GameLogic::getBlock(int x, int y) {
    if (x < 0 || x >= this->BOARD_SIZE || y < 0 || y >= this->BOARD_SIZE) return 3;
    return this->board[y*this->BOARD_SIZE + x];
}

int GameLogic::setBlock(int x, int y, int v) {
    this->board[y*this->BOARD_SIZE + x] = v;
    return v;
}


void GameLogic::newGame() {
    memset(this->board, 0x00, flatSize * sizeof(int));
    spawnCandidates();

    emit boardChanged();
    emit blockCandidateChanged();
}

bool GameLogic::canPlace(int patId, int x, int y) {
    int* pat = this->getPattern(patId);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int mapX = x + j - 1;
            int mapY = y + i - 1;
            if (pat[i * 3 + j] && this->getBlock(mapX, mapY)) {
                return false;
            }
        }
    }
    return true;
}

// notice: this function assume input is in-bound and legal.
void GameLogic::place(int patId, int x, int y) {
    int* pat = this->getPattern(patId);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int mapX = x + j - 1;
            int mapY = y + i - 1;
            if (pat[i * 3 + j]) {
                this->setBlock(mapX, mapY, pat[i * 3 + j]);
            }
        }
    }
    emit boardChanged();
}

int* GameLogic::getPattern(int patId) {
    return this->patterns.at(patId);
}

int GameLogic::getCandidateId(int p) {
    return this->candidateIds[p];
}

void GameLogic::setCandidateId(int p, int v) {
    if(v == 0 && this->selectedSlot() == p) {
        this->setSelectedSlot(-1);
    }
    this->candidateIds[p] = v;
}

int GameLogic::selectedSlot() {
    return this->iSelectedSlot;
}

void GameLogic::setSelectedSlot(int x) {
    if (this->getCandidateId(x) == 0 && x != -1) return;
    this->iSelectedSlot = x;
}

int GameLogic::getSelectedPatternId() {
    if (this->selectedSlot() == -1) return -1;
    return this->getCandidateId(this->selectedSlot());
}

void GameLogic::spawnCandidates() {
    for (int i = 0; i < 3; i++) {
        int k = QRandomGenerator::global()->bounded(1, patterns.length());
        this->setCandidateId(i, k);
    }
    this->setSelectedSlot(-1);
    emit blockCandidateChanged();
}

void GameLogic::findClear() {
    bool dirty = false;
    // x
    QList<int> deleteX;
    for (int i = 0; i < BOARD_SIZE; i++) {
        bool ok = true;
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (!getBlock(j, i)) goto xNext;
        }

        deleteX.append(i);
        dirty = true;
        xNext:;
    }

    // y
    QList<int> deleteY;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (!getBlock(i, j)) goto yNext;
        }

        deleteY.append(i);
        dirty = true;
        yNext:;
    }


    // square
    for (int i = 0; i < BOARD_SIZE / 3; i++) {
        for (int j = 0; j < BOARD_SIZE / 3; j++) {
            int centerX = j * 3 + 1;
            int centerY = i * 3 + 1;
            if (!getBlock(centerX, centerY)) continue;

            for(int ii = -1; ii <= 1; ii++) {
                for(int jj = -1; jj <= 1; jj++) {
                    if (!getBlock(centerX + ii, centerY + jj)) goto squareNext;
                }
            }

            dirty = true;
            for(int ii = -1; ii <= 1; ii++) {
                for(int jj = -1; jj <= 1; jj++) {
                    setBlock(centerX + ii, centerY + jj, 0);
                }
            }

            squareNext:;
        }
    }

    foreach (auto&& w, deleteX) {
        for (int i = 0; i < BOARD_SIZE; i++) setBlock(i, w, 0);
    }

    foreach (auto&& w, deleteY) {
        for (int i = 0; i < BOARD_SIZE; i++) setBlock(w, i, 0);
    }

    if (dirty) {
        emit boardChanged();
    }
}
