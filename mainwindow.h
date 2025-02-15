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
#include "utilities/leftpane.h"
#include "aboutdialog.h"
#include "storedData/group.h"
#include "storedData/student.h"

#define CIRCLES_COUNT 4

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

public slots:
    void createFile();
    void openFile();
    void openFile(QString filename);
    void openGroupsStudentsManager();
    void openCompetenceManager();
    void openAboutDialog();

private:
    Ui::MainWindow *ui;
    KScopeManager kscopemanager;
    LeftPane *m_leftPane;
};

#endif // MAINWINDOW_H
