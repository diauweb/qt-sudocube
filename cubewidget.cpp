#include "cubewidget.h"
#include <QDebug>
#include <QMouseEvent>
#include <QStyleOption>
#include <QPainter>

CubeWidget::CubeWidget(int x, int y, int src, QWidget *parent) : QWidget(parent), bx(x), by(y), src(src)
{
    this->setState(0);
    this->setProperty("preview", false);
}

int CubeWidget::boardX() {
    return this->bx;
}

int CubeWidget::boardY() {
    return this->by;
}

int CubeWidget::state() {
    return this->istate;
}

void CubeWidget::setState(int newState) {
    this->istate = newState;
}

void CubeWidget::usePattern(bool b) {
    if (b) {
        this->setProperty("CheckerStyle", (int(by / 3) * 3 + int(bx / 3)) % 2);
    } else {
        this->setProperty("CheckerStyle", 0);
    }
}

void CubeWidget::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CubeWidget::enterEvent(QEvent *) {
    emit hovered(bx, by, true);
}

void CubeWidget::leaveEvent(QEvent *) {
    emit hovered(bx, by, false);
}

void CubeWidget::mousePressEvent(QMouseEvent *event) {
    emit pressed(bx, by, src);
}
