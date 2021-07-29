#ifndef CUBEWIDGET_H
#define CUBEWIDGET_H

#include <QWidget>

class CubeWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int boardX READ boardX)
    Q_PROPERTY(int boardY READ boardY)
    Q_PROPERTY(int state MEMBER istate)
    Q_PROPERTY(int source MEMBER src)
public:
    explicit CubeWidget(int x, int y, int src, QWidget *parent = nullptr);

    int boardX();
    int boardY();
    int state();
    void setState(int);
    void usePattern(bool);

signals:
    void pressed(int x, int y, int src);
    void hovered(int x, int y, bool enter);

protected:
    void paintEvent(QPaintEvent*) override;
    void enterEvent(QEvent*) override;
    void leaveEvent(QEvent*) override;
    void mousePressEvent(QMouseEvent *event) override;
    int bx, by;
    int istate;
    int src;
};

#endif // CUBEWIDGET_H
