#ifndef GROUPSSTUDENTSMANAGER_H
#define GROUPSSTUDENTSMANAGER_H

#include <QDialog>
#include <QtSql>
#include <QMessageBox>
#include <QFileDialog>
#include "storedData/group.h"
#include "storedData/student.h"
#include "utilities/preferences.h"
#include "studentsimportmanager.h"

namespace Ui {
class GroupsStudentsManager;
}

class GroupsStudentsManager : public QDialog
{
    Q_OBJECT

public:
    explicit GroupsStudentsManager(QWidget *parent = 0);
    ~GroupsStudentsManager();
    void update_groups();
    void free_groups();
    void update_students();
    void free_students();

public slots:
    void groups_selection_change();
    void students_selection_change();
    void add_group();
    void delete_group();
    void add_student();
    void student_to_DB(QSqlDatabase *db, QString name, QString first_name);
    void delete_student();
    void import_students();



private:
    Ui::GroupsStudentsManager *ui;
    QQueue<Group *> m_displayedGroups;
    QQueue<Student *> m_displayedStudents;
};

#endif // GROUPSSTUDENTSMANAGER_H
