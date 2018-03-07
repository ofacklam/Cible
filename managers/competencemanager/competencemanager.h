#ifndef COMPETENCEMANAGER_H
#define COMPETENCEMANAGER_H

#include <QDialog>
#include <QQueue>
#include "storedData/group.h"
#include "storedData/student.h"
#include "storedData/domain.h"
#include "storedData/competence.h"
#include "competenceaction.h"
#include "competencecell.h"
#include "mainwindow.h"

namespace Ui {
class CompetenceManager;
}

class CompetenceManager : public QDialog
{
    Q_OBJECT

public:
    explicit CompetenceManager(Group *g, Student *s, QWidget *parent = 0);
    ~CompetenceManager();
    int addCompetence(QSqlDatabase db, int id_domain, QString name);
    void deleteCompetence(QSqlDatabase db, int id_comp);
    void updateDomains(QSqlDatabase db, int id_student);
    void treatStudent(QSqlDatabase db, int id_student);
    void treatGroup(QSqlDatabase db, int id_group);
    void treatAll(QSqlDatabase db);

public slots:
    void applyAll();
    void applyGroup();
    void applyStudent();

private:
    Ui::CompetenceManager *ui;
    Group *m_group;
    Student *m_student;
    QQueue<CompetenceAction*> m_queue;
    QList<Domain*> m_domains;
};

#endif // COMPETENCEMANAGER_H
