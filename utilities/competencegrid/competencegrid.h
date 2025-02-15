#ifndef COMPETENCEGRID_H
#define COMPETENCEGRID_H

#include <QWidget>
#include <QLabel>
#include <QRadioButton>
#include <QPushButton>
#include <QToolButton>
#include <QTextDocument>
#include "storedData/student.h"
#include "storedData/domain.h"
#include "storedData/competence.h"
#include "mainwindow.h"
#include "utilities/radar.h"
#include "simpleword.h"
#include "custombutton.h"
#include "customlabel.h"

class MainWindow;
class Radar;

namespace Ui {
class CompetenceGrid;
}

class CompetenceGrid : public QWidget
{
    Q_OBJECT

public:
    explicit CompetenceGrid(Student *s, Page *p, Tab *t, QWidget *parent = 0);
    ~CompetenceGrid();
    void addHSep(int row, int column, int span);
    void addVSep(int row, int column, int span);

public slots:
    void buttonClicked(QAbstractButton *btn);
    void textClicked(CustomLabel *lbl);
    void tabCommentClicked(CustomLabel *lbl);

signals:
    void valuesUpdated();

private:
    Ui::CompetenceGrid *ui;
    Student *m_student;
    Page *m_page;
    Tab *m_tab;
};

#endif // COMPETENCEGRID_H
