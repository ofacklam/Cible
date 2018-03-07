#include "kscopemanager.h"

KScopeManager::KScopeManager() {
    tables << "competences" << "domains" << "groups" << "students";
}

KScopeManager::~KScopeManager() {

}

bool KScopeManager::createFile(QString path) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);
    if (!db.open()) {
        QMessageBox::critical(NULL, "Echec", "Impossible d'ouvrir la base de données générée...");
        return false;
    }

    db.transaction();
    for(int i=0; i<tables.count(); i++)
        tablesStructures(&db, tables[i], Create);
    db.commit();

    return true;
}

bool KScopeManager::openFile(QString path) {
    // Open the QSQLite database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);
    if (!db.open()) {
        QMessageBox::critical(NULL, "Echec", "Impossible d'ouvrir la base de données...");
        return false;
    }
    int nbRectifications = checkDBStructure(&db);
    if(nbRectifications < 0)
        QMessageBox::critical(NULL, "Echec", "Votre fichier est corrompu.<br />Néanmoins, il a été chargé pour permettre d'identifier le problème.<br /> Par conséquence, il est possible que le logiciel ne supporte pas ce fichier.");
    else
        QMessageBox::information(NULL, "Succès", "Votre fichier a été chargé.<br />Vous pouvez l'utiliser.");


    return true;
}

int KScopeManager::checkDBStructure(QSqlDatabase* db) {
    int nbRectifications = 0;
    if(db->driverName() == "QSQLITE") {
        QStringList tab = db->tables();
        for(int i=0; i<tables.count(); i++) {
            if(tab.contains(tables[i])) {
                int res = tablesStructures(db, tables[i], Check);
                if(res >= 0)
                    nbRectifications += res;
                else return -1;
            }else {
                tablesStructures(db, tables[i], Create);
                nbRectifications++;
            }

        }
    }
    return nbRectifications;
}

int KScopeManager::tablesStructures(QSqlDatabase* db, QString nameTable, ActionType action) {
    QSqlQuery qCreate(*db);
    QMap<QString, DataType> columns;
    int nbRectifications = 0;
    if(nameTable == "competences") {
        switch(action) {
            case Create:
                qCreate.exec("CREATE TABLE `competences` ( "
                             "`id`	INTEGER NOT NULL DEFAULT 0 PRIMARY KEY AUTOINCREMENT UNIQUE, "
                             "`id_domains`	INTEGER NOT NULL DEFAULT 0, "
                             "`name`	TEXT DEFAULT '', "
                             "`value`	INTEGER DEFAULT 0, "
                             "`comments`	TEXT DEFAULT '' "
                         ");");
                break;
            case Check:
                columns.insert("id", Id);
                columns.insert("name", Text);
                columns.insert("id_domains", Int);
                columns.insert("value", Int);
                columns.insert("comments", Text);
                break;
        }
    } else if(nameTable == "domains") {
        switch(action) {
            case Create:
                qCreate.exec("CREATE TABLE `domains` ( "
                             "`id`	INTEGER NOT NULL DEFAULT 0 PRIMARY KEY AUTOINCREMENT UNIQUE, "
                             "`part`	INTEGER DEFAULT 0, "
                             "`id_students`	INTEGER NOT NULL DEFAULT 0, "
                             "`name`	TEXT DEFAULT '' "
                         ");");
                break;
            case Check:
                columns.insert("id", Id);
                columns.insert("part", Int);
                columns.insert("id_students", Int);
                columns.insert("name", Text);
            break;
        }
    } else if(nameTable == "groups") {
        switch(action) {
            case Create:
            qCreate.exec("CREATE TABLE `groups` ( "
                         "`id`	INTEGER NOT NULL DEFAULT 0 PRIMARY KEY AUTOINCREMENT UNIQUE, "
                         "`name`	TEXT DEFAULT '' "
                     ");");
            break;
            case Check:
                columns.insert("id", Id);
                columns.insert("name", Text);
            break;
        }
    } else if(nameTable == "scores") {
        switch(action) {
            case Create:
            qCreate.exec("CREATE TABLE `scores` ( "
                         "`id`	INTEGER NOT NULL DEFAULT 0 PRIMARY KEY AUTOINCREMENT UNIQUE, "
                         "`id_competences`	INTEGER NOT NULL DEFAULT 0, "
                         "`score`	INTEGER DEFAULT 0 "
                     ");");
            break;
            case Check:
                columns.insert("id", Id);
                columns.insert("id_competences", Int);
                columns.insert("score", Int);
            break;
        }
    } else if(nameTable == "students") {
        switch(action) {
            case Create:
            qCreate.exec("CREATE TABLE `students` ( "
                         "`id`	INTEGER NOT NULL DEFAULT 0 PRIMARY KEY AUTOINCREMENT UNIQUE, "
                         "`id_groups`	INTEGER NOT NULL DEFAULT 0, "
                         "`name`	TEXT DEFAULT '', "
                         "`first_name`	TEXT DEFAULT '', "
                         "`comments`	TEXT DEFAULT '' "
                     ");");
            break;
            case Check:
                columns.insert("id", Id);
                columns.insert("name", Text);
                columns.insert("first_name", Text);
                columns.insert("id_groups", Int);
                columns.insert("comments", Text);
            break;
        }
    } else {
        QMessageBox::critical(NULL, "Erreur", "Table SQL inexistante : " + nameTable);
        return -1;
    }

    if(action == Check && columns.count()>0) {
        QSqlRecord rec = db->record(nameTable);
        QSqlQuery qCheck(*db);
        for(int i=0; i<rec.count(); i++)
            columns.remove(rec.fieldName(i));
        QMapIterator<QString, DataType> i(columns);
        while(i.hasNext()) {
            i.next();
            switch(i.value()) {
                case Id:
                    QMessageBox::critical(NULL, "Fichier corrompu", "Identifiant de la table " + nameTable + " est manquante <br /> Le problème ne peut être résolu.");
                    return -1;
                    break;
                case Int:
                    qCheck.exec("ALTER TABLE `"+nameTable+"` ADD `"+i.key()+"` INTEGER NOT NULL DEFAULT 0;");
                    nbRectifications++;
                    break;
                case Text:
                    qCheck.exec("ALTER TABLE `"+nameTable+"` ADD `"+i.key()+"` TEXT NOT NULL DEFAULT '';");
                    nbRectifications++;
                    break;
            }
        }
    }

    return nbRectifications;
}

