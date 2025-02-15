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
#include "storedData/page.h"
#include "storedData/tab.h"
#include <cmath>

#define EPSILON 8
#define THICK 4
#define OPACITY 60
#define BORDER 16

enum Borders {Top, Left, Right, Bottom};

class Radar : public QGraphicsScene
{

    Q_OBJECT

public:
    Radar(QWidget *win, QGraphicsView *view, Page *p);
    ~Radar();
    void addTab(Tab *t);
    void removeTab(int id_tab);
    void clearTabs();
    bool eventFilter(QObject *watched, QEvent *event);

    Page *page();

public slots:
    void updateScreen();

private:
    void drawZones(float L);
    void drawCircles(float r);
    void drawRayons(float L, float r);
    float domain_angle(int i, int n);
    float next_angle(float a);
    float previous_angle(float a);
    QPointF border_point(float L, float a);
    float comp_angle(int i, int n, int j, int m);
    Borders which_border(float a);
    bool top(float a);
    bool right(float a);

private:
    QWidget *m_window;
    QGraphicsView *m_view;
    Page* m_page;
    QMap<int, Tab*> m_tabs;
    QList<QColor> m_colors;
    QList<QColor> m_circles;

};

#endif // RADAR_H
