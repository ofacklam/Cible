#include "kscopemanager.h"

KScopeManager::KScopeManager() {
    tables << "groups" << "students" << "pages" << "tabs" << "domains" << "competences" << "values" << "general";
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

    QString old_version = getOldVersion(&db);
    int nbRectifications = checkDBStructure(&db);
    if(nbRectifications < 0)
        QMessageBox::critical(NULL, "Echec", "Votre fichier est corrompu.<br />Néanmoins, il a été chargé pour permettre d'identifier le problème.<br /> Par conséquence, il est possible que le logiciel ne supporte pas ce fichier.");
    else {
        upgradeFile(old_version, &db);
        QMessageBox::information(NULL, "Succès", "Votre fichier a été chargé.<br />Vous pouvez l'utiliser.");
    }


    return true;
}

QString KScopeManager::getOldVersion(QSqlDatabase *db) {
    QSqlQuery version(*db);
    version.exec("SELECT `value` FROM `general` WHERE `key`=\"version\"");

    if(version.next())
        return version.value(0).toString();
    else
        return "0";
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
    if(nameTable == "groups") {
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
    } else if(nameTable == "students") {
        switch(action) {
            case Create:
                qCreate.exec("CREATE TABLE `students` ( "
                             "`id`	INTEGER NOT NULL DEFAULT 0 PRIMARY KEY AUTOINCREMENT UNIQUE, "
                             "`id_groups`	INTEGER NOT NULL DEFAULT 0, "
                             "`name`	TEXT DEFAULT '', "
                             "`first_name`	TEXT DEFAULT '' "
                         ");");
                break;
            case Check:
                columns.insert("id", Id);
                columns.insert("name", Text);
                columns.insert("first_name", Text);
                columns.insert("id_groups", Int);
            break;
        }
    } else if(nameTable == "pages") {
        switch(action) {
            case Create:
            qCreate.exec("CREATE TABLE `pages` ( "
                         "`id`	INTEGER NOT NULL DEFAULT 0 PRIMARY KEY AUTOINCREMENT UNIQUE, "
                         "`name`	TEXT DEFAULT '', "
                         "`id_students`	INTEGER NOT NULL DEFAULT 0 "
                         ");");
            break;
            case Check:
                columns.insert("id", Id);
                columns.insert("name", Text);
                columns.insert("id_students", Int);
            break;
        }
    } else if(nameTable == "tabs") {
        switch(action) {
            case Create:
            qCreate.exec("CREATE TABLE `tabs` ( "
                         "`id`	INTEGER NOT NULL DEFAULT 0 PRIMARY KEY AUTOINCREMENT UNIQUE, "
                         "`name`	TEXT DEFAULT '', "
                         "`id_pages`	INTEGER NOT NULL DEFAULT 0, "
                         "`comments`	TEXT DEFAULT '' "
                         ");");
            break;
            case Check:
                columns.insert("id", Id);
                columns.insert("name", Text);
                columns.insert("id_pages", Int);
                columns.insert("comments", Text);
            break;
        }
    } else if(nameTable == "domains") {
        switch(action) {
            case Create:
            qCreate.exec("CREATE TABLE `domains` ( "
                         "`id`	INTEGER NOT NULL DEFAULT 0 PRIMARY KEY AUTOINCREMENT UNIQUE, "
                         "`name`	TEXT DEFAULT '', "
                         "`id_pages`	INTEGER NOT NULL DEFAULT 0 "
                         ");");
            break;
            case Check:
                columns.insert("id", Id);
                columns.insert("name", Text);
                columns.insert("id_pages", Int);
            break;
        }
    } else if(nameTable == "competences") {
        switch(action) {
            case Create:
            qCreate.exec("CREATE TABLE `competences` ( "
                         "`id`	INTEGER NOT NULL DEFAULT 0 PRIMARY KEY AUTOINCREMENT UNIQUE, "
                         "`id_domains`	INTEGER NOT NULL DEFAULT 0, "
                         "`name`	TEXT DEFAULT '' "
                     ");");
            break;
            case Check:
                columns.insert("id", Id);
                columns.insert("name", Text);
                columns.insert("id_domains", Int);
            break;
        }
    } else if(nameTable == "values") {
        switch(action) {
            case Create:
            qCreate.exec("CREATE TABLE `values` ( "
                         "`id`	INTEGER NOT NULL DEFAULT 0 PRIMARY KEY AUTOINCREMENT UNIQUE, "
                         "`id_competences`	INTEGER NOT NULL DEFAULT 0, "
                         "`id_tabs`	INTEGER NOT NULL DEFAULT 0, "
                         "`value`	INTEGER DEFAULT 0, "
                         "`comments`	TEXT DEFAULT '' "
                         ");");
            break;
            case Check:
                columns.insert("id", Id);
                columns.insert("id_competences", Int);
                columns.insert("id_tabs", Int);
                columns.insert("value", Int);
                columns.insert("comments", Text);
            break;
        }
    } else if(nameTable == "general") {
        switch(action) {
            case Create:
            qCreate.exec("CREATE TABLE `general` ( "
                         "`id`	INTEGER NOT NULL DEFAULT 0 PRIMARY KEY AUTOINCREMENT UNIQUE, "
                         "`key`	TEXT NOT NULL DEFAULT '', "
                         "`value`	TEXT DEFAULT '' "
                         ");");
            break;
            case Check:
                columns.insert("id", Id);
                columns.insert("key", Text);
                columns.insert("value", Text);
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

    if(nameTable == "general") {
        QSqlQuery qUpdate(*db);
        switch(action) {
            case Create:
                qUpdate.prepare("INSERT INTO `general`(`key`, `value`) VALUES(\"version\", :version);");
                qUpdate.bindValue(":version", APP_VERSION);
                qUpdate.exec();
            break;
            case Check:
                qUpdate.prepare("UPDATE `general` SET `value`=:version WHERE `key`=\"version\";");
                qUpdate.bindValue(":version", APP_VERSION);
                qUpdate.exec();
            break;
        }
    }

    return nbRectifications;
}


void KScopeManager::upgradeFile(QString old_version, QSqlDatabase *db) {
    if(old_version == "0") {
        db->transaction();

        QSqlQuery qStudents(*db);
        qStudents.exec("SELECT id, comments FROM students");

        while(qStudents.next()) {
            QSqlQuery qPage(*db);
            qPage.prepare("INSERT INTO pages(name, id_students) VALUES(:name, :id_students);");
            qPage.bindValue(":name", "Page 1");
            qPage.bindValue(":id_students", qStudents.value(0).toInt());
            qPage.exec();
            int id_page = qPage.lastInsertId().toInt();

            QSqlQuery qTab(*db);
            qTab.prepare("INSERT INTO tabs(name, id_pages, comments) VALUES(:name, :id_pages, :comments);");
            qTab.bindValue(":name", "Onglet 1");
            qTab.bindValue(":id_pages", id_page);
            qTab.bindValue(":comments", qStudents.value(1).toString());
            qTab.exec();
            int id_tab = qTab.lastInsertId().toInt();

            QSqlQuery qDomains(*db);
            qDomains.prepare("UPDATE domains SET id_pages=:id_pages WHERE id_students=:id_students;");
            qDomains.bindValue(":id_pages", id_page);
            qDomains.bindValue(":id_students", qStudents.value(0).toInt());
            qDomains.exec();

            QSqlQuery qComps(*db);
            qComps.prepare("SELECT id, value, comments FROM competences WHERE id_domains IN (SELECT id FROM domains WHERE id_pages=:id_pages);");
            qComps.bindValue(":id_pages", id_page);
            qComps.exec();

            while(qComps.next()) {
                QSqlQuery qVals(*db);
                qVals.prepare("INSERT INTO `values`(id_competences, id_tabs, value, comments) VALUES(:id_competences, :id_tabs, :value, :comments);");
                qVals.bindValue(":id_competences", qComps.value(0).toInt());
                qVals.bindValue(":id_tabs", id_tab);
                qVals.bindValue(":value", qComps.value(1).toInt());
                qVals.bindValue(":comments", qComps.value(2).toString());
                qVals.exec();
            }
        }

        db->commit();
    }
}

