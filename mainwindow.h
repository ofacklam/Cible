#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QtSql>
#include <QPdfWriter>
#include "managers/groupsstudentsmanager.h"
#include "managers/competencemanager/competencemanager.h"
#include "utilities/preferences.h"
#include "utilities/kscopemanager.h"
#include "utilities/radar.h"
#include "utilities/competencegrid/competencegrid.h"
#include "utilities/pdfcreator.h"
#include "aboutdialog.h"
#include "storedData/group.h"
#include "storedData/student.h"

#define DOMAIN_COUNT 4
#define CIRCLES_COUNT 4

enum AffectedStudent {All, OneGroup, OneStudent, Cancel};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void updateWindow();
    void updateGroupsBox();
    void free_groups();
    void free_students();
    AffectedStudent question_affected(QString title, QString text, Student *s, Group *g);
    bool printClass(QString dirname, QString group_name);
    bool printAll(QString dirname);

public slots:
    void createFile();
    void openFile();
    void openFile(QString filename);
    void openGroupsStudentsManager();
    void openCompetenceManager();
    void openAboutDialog();
    void updateStudentsList();
    void updateRadar();
    void printCompetences();
    void resetCompetences();


private:
    Ui::MainWindow *ui;
    KScopeManager kscopemanager;
    QQueue<Group*> m_displayedGroups;
    QQueue<Student*> m_displayedStudents;

};

#endif // MAINWINDOW_H
