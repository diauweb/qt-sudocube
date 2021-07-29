#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <QObject>

class GameLogic : public QObject
{

    Q_OBJECT
public:
    explicit GameLogic(QObject *parent = nullptr);
    ~GameLogic();

    const int BOARD_SIZE = 9;

    void findClear();
    int getBlock(int x, int y);
    int setBlock(int x, int y, int v);
    int* getPattern(int id);
    void newGame();


    void place(int patId, int x, int y);
    void updateClear();
    bool canPlace(int patId, int x, int y);

    int selectedSlot();
    void setSelectedSlot(int);

    int getCandidateId(int p);
    void setCandidateId(int p, int v);

    int getSelectedPatternId();

    void spawnCandidates();

signals:
    void boardChanged();
    void blockCandidateChanged();

private:
    int *board;
    int flatSize;
    QList<int*> patterns;
    void parsePatterns();

    int iSelectedSlot;
    int* candidateIds;
};

#endif // GAMELOGIC_H
