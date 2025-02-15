#ifndef COMPETENCETABWIDGET_H
#define COMPETENCETABWIDGET_H

#include <QTabWidget>
#include "storedData/page.h"
#include "storedData/tab.h"
#include "utilities/radar.h"

namespace Ui {
class CompetenceTabWidget;
}

class CompetenceTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit CompetenceTabWidget(Student *s, Page *p, QWidget *parent = 0);
    ~CompetenceTabWidget();
    Page *page();
    Tab *tab();
    void setRadar(Radar *r);

public slots:
    void tabChanged(int index);

private:
    void getTabs();
    void clearTabs();

private:
    Ui::CompetenceTabWidget *ui;
    Student *m_student;
    Page *m_page;
    Radar *m_radar;
    QQueue<Tab*> m_displayedTabs;
};

#endif // COMPETENCETABWIDGET_H
