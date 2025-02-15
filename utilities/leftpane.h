#ifndef LEFTPANE_H
#define LEFTPANE_H

#include <QtSql>
#include <QWidget>
#include <QToolBox>
#include <QScrollArea>
#include <QGraphicsScene>
#include <QMessageBox>
#include "utilities/radar.h"
#include "utilities/competencetabwidget.h"
#include "storedData/group.h"
#include "storedData/student.h"
#include "storedData/page.h"
#include "storedData/domain.h"
#include "storedData/competence.h"

enum AffectedStudent {All, OneGroup, OneStudent, OnePage, OneTab, Cancel};

namespace Ui {
class LeftPane;
}

class LeftPane : public QWidget
{
    Q_OBJECT

public:
    explicit LeftPane(QGraphicsView *gv, QWidget *parent = 0);
    ~LeftPane();

    void updateGroupsList();
    Group *selectedGroup();
    Student *selectedStudent();
    Page *selectedPage();
    Tab *selectedTab();

public slots:
    void updateStudentsList();
    void updatePagesBox();
    void updateRadar(int index);

    void printCompetences();
    void resetCompetences();

private:
    void free_groups();
    void free_students();
    void free_pages();
    void unset_radars();

    AffectedStudent question_affected(QString title, QString text, Group *g, Student *s, Page *p, Tab *t);
    bool printClass(QString dirname, QString group_name);
    bool printAll(QString dirname);

private:
    Ui::LeftPane *ui;
    QToolBox *m_pagesBox;
    QGraphicsView *m_graphicsView;
    QWidget *m_window;
    QQueue<Group*> m_displayedGroups;
    QQueue<Student*> m_displayedStudents;
    QQueue<CompetenceTabWidget*> m_displayedPageWidgets;
};

#endif // LEFTPANE_H
