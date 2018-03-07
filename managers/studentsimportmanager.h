#ifndef STUDENTSIMPORTMANAGER_H
#define STUDENTSIMPORTMANAGER_H

#include <QList>
#include <QMessageBox>
#include <QTextStream>
#include <QString>
#include <QStringList>
#include <QtSql>
#include "managers/groupsstudentsmanager.h"

class GroupsStudentsManager;

class StudentsImportManager
{
public:
    StudentsImportManager(QSqlDatabase *db, GroupsStudentsManager *p);
    bool load(QString filename);
    bool readFile(QString filename);
    void treatLine(QString line);
    bool testTable();
    void calculateDistinct();
    int treatColumn(int index);
    void findMaxes(int *max1, int *max2);
    void insertStudents(int indexName, int indexFirstName);

private:
    QList<QStringList> m_table;
    QList<int> m_number_distinct;
    GroupsStudentsManager *parent;
    QSqlDatabase *m_db;
};

#endif // STUDENTSIMPORTMANAGER_H
