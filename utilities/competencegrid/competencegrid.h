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

class MainWindow;
class Radar;

namespace Ui {
class CompetenceGrid;
}

class CompetenceGrid : public QWidget
{
    Q_OBJECT

public:
    explicit CompetenceGrid(Student *s, QList<Domain*> domains, Radar *radar, QWidget *parent = 0);
    ~CompetenceGrid();
    void addHSep(int row, int column, int span);
    void addVSep(int row, int column, int span);

public slots:
    void buttonClicked(int id);
    void textClicked(QAbstractButton *btn);
    void studentCommentClicked(QAbstractButton *btn);

private:
    Ui::CompetenceGrid *ui;
    Radar *m_radar;
    QList<Domain*> m_domains;
    Student *m_student;
};

#endif // COMPETENCEGRID_H
