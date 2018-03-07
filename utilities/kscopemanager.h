#ifndef KSCOPEMANAGER_H
#define KSCOPEMANAGER_H

#include <QDebug>
#include <QString>
#include <QSqlQuery>
#include <QSqlDriver>
#include <QSqlRecord>
#include <QSqlError>
#include <QMessageBox>
#include <QFileInfo>
#include <QVariant>

class KScopeManager
{
public:
    enum ActionType {
        Create, Check
    };
    enum DataType {
        Id, Int, Text
    };

    KScopeManager();
    ~KScopeManager();
    bool createFile(QString path);
    bool openFile(QString path);
    int checkDBStructure(QSqlDatabase* db);
    int tablesStructures(QSqlDatabase* db, QString nameTable, ActionType action);

private:
    QStringList tables;
};

#endif // KSCOPEMANAGER_H
