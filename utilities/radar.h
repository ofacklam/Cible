#ifndef RADAR_H
#define RADAR_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsSimpleTextItem>
#include <QMessageBox>
#include <QEvent>
#include "mainwindow.h"
#include "storedData/domain.h"
#include "storedData/competence.h"
#include <cmath>

#define EPSILON 8
#define THICK 4
#define OPACITY 60
#define BORDER 16

class Radar : public QGraphicsScene
{

    Q_OBJECT

public:
    Radar(QWidget *win, QGraphicsView *view, QList<Domain*> domains);
    ~Radar();
    void drawRayons(QList<Domain*> domains, QList<QColor> colors, QList<QColor> circles);
    float alpha(int i, int j, int n);
    bool top(int i);
    bool right(int i);
    bool eventFilter(QObject *watched, QEvent *event);

    QList<Domain*> domains();

public slots:
    void updateScreen();

private:
    QWidget *m_window;
    QGraphicsView *m_view;
    QList<Domain*> m_domains;
    QList<QColor> m_colors;
    QList<QColor> m_circles;

};

#endif // RADAR_H
