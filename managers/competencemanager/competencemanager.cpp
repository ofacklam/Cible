#include "competencemanager.h"
#include "ui_competencemanager.h"

CompetenceManager::CompetenceManager(Group *g, Student *s, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CompetenceManager)
{
    ui->setupUi(this);

    m_group = g;
    m_student = s;

    //Get list of domains and competences for current student
    if(m_student == NULL) {
        for(int i = 0; i < DOMAIN_COUNT; i++)
            m_domains << new Domain();
    }
    else {
        QSqlDatabase db = QSqlDatabase::database();
        if(!db.isOpen()) {
            QMessageBox::critical(this, "Erreur", "La connexion à la base de données a échoué.");
            return;
        }

        QSqlQuery domains(db);
        domains.prepare("SELECT id, part, name, id_students FROM domains WHERE id_students=:id_student ORDER BY part");
        domains.bindValue(":id_student", m_student->id());
        domains.exec();

        while(domains.next()) {
            Domain *d = new Domain();
            d->setId(domains.value(0).toInt());
            d->setPart(domains.value(1).toInt());
            d->setName(domains.value(2).toString());
            d->setId_students(domains.value(3).toInt());

            QSqlQuery comps(db);
            comps.prepare("SELECT id, id_domains, name, value, comments FROM competences WHERE id_domains=:id_domain ORDER BY id");
            comps.bindValue(":id_domain", d->id());
            comps.exec();

            while(comps.next()) {
                Competence *c = new Competence();
                c->setId(comps.value(0).toInt());
                c->setId_domains(comps.value(1).toInt());
                c->setName(comps.value(2).toString());
                c->setValue(comps.value(3).toInt());
                c->setComments(comps.value(4).toString());

                d->add_competence(c);
            }

            m_domains.append(d);
        }
    }

    if(m_domains.length() != DOMAIN_COUNT)
        return;

    //Create Radar
    Radar *r = new Radar(this, ui->graphicsView, m_domains);
    ui->graphicsView->setScene(r);

    //Create CompetenceCells
    for(int i = DOMAIN_COUNT - 1; i >= 0; i--) {
        ui->layout_main->insertWidget(0, new CompetenceCell(m_domains[i], &m_queue, r), 1);
    }

    //Create Buttons
    QPushButton *btn_all = new QPushButton("Appliquer à tous les élèves");
    connect(btn_all, SIGNAL(clicked(bool)), this, SLOT(applyAll()));
    ui->layout_buttons->addWidget(btn_all);

    if(m_group != NULL) {
        QPushButton *btn_group = new QPushButton("Appliquer à la classe " + m_group->name());
        connect(btn_group, SIGNAL(clicked(bool)), this, SLOT(applyGroup()));
        ui->layout_buttons->addWidget(btn_group);
    }

    if(m_student != NULL) {
        QPushButton *btn_student = new QPushButton("Appliquer à l'élève " + m_student->name() + " " + m_student->first_name());
        connect(btn_student, SIGNAL(clicked(bool)), this, SLOT(applyStudent()));
        ui->layout_buttons->addWidget(btn_student);
    }

    ui->layout_main->setStretch(DOMAIN_COUNT, DOMAIN_COUNT);
}

CompetenceManager::~CompetenceManager()
{
    //Delete Radar
    if(ui->graphicsView->scene() != NULL)
        delete ui->graphicsView->scene();

    //Delete CompetenceActions from Queue and competences not in domains (the ones to be deleted)
    while(!m_queue.isEmpty()) {
        CompetenceAction *action = m_queue.dequeue();
        if(action->type() == ActionType::Delete)
            delete action->competence();
        delete action;
    }

    delete ui;
}

void CompetenceManager::applyAll() {
    int res = QMessageBox::question(this, "Modification des compétences",
                                    "Cette opération va supprimer <strong>toutes les compétences</strong>"
                                    " des élèves ainsi que"
                                    " <strong>les valeurs et appréciations</strong> associées.<br/>"
                                    "Etes-vous sûr de vouloir continuer ?");

    if(res == QMessageBox::Yes) {
        QSqlDatabase db = QSqlDatabase::database();
        if(!db.isOpen()) {
            QMessageBox::critical(this, "Erreur", "La connexion à la base de données a échoué.");
            return;
        }
        db.transaction();

        treatAll(db);

        db.commit();
        accept();
    }
}

void CompetenceManager::treatAll(QSqlDatabase db) {
    //Get all students
    QSqlQuery students(db);
    students.prepare("SELECT id FROM students");
    students.exec();

    while(students.next()) {
        treatStudent(db, students.value(0).toInt());
    }
}

void CompetenceManager::applyGroup() {
    if(m_group == NULL)
        return;

    int res = QMessageBox::question(this, "Modification des compétences",
                                    "Cette opération va supprimer <strong>toutes les compétences</strong>"
                                    " des élèves de la classe " + m_group->name() + " ainsi que"
                                    " <strong>les valeurs et appréciations</strong> associées.<br/>"
                                    "Etes-vous sûr de vouloir continuer ?");

    if(res == QMessageBox::Yes) {
        QSqlDatabase db = QSqlDatabase::database();
        if(!db.isOpen()) {
            QMessageBox::critical(this, "Erreur", "La connexion à la base de données a échoué.");
            return;
        }
        db.transaction();

        treatGroup(db, m_group->id());

        db.commit();
        accept();
    }
}

void CompetenceManager::treatGroup(QSqlDatabase db, int id_group) {
    //Get all students from this group
    QSqlQuery students(db);
    students.prepare("SELECT id FROM students WHERE id_groups=:id_group");
    students.bindValue(":id_group", id_group);
    students.exec();

    while(students.next()) {
        treatStudent(db, students.value(0).toInt());
    }
}

void CompetenceManager::treatStudent(QSqlDatabase db, int id_student) {
    //Delete everything related to this student
    QSqlQuery deletequery(db);
    deletequery.prepare("DELETE FROM competences WHERE id_domains IN (SELECT id FROM domains WHERE id_students=:id_student)");
    deletequery.bindValue(":id_student", id_student);
    deletequery.exec();

    //Add all competences
    for(int part = 0; part < m_domains.length(); part++) {
        //Get domain id for this student and part
        QSqlQuery domain(db);
        domain.prepare("SELECT id FROM domains WHERE id_students=:id_student AND part=:part");
        domain.bindValue(":id_student", id_student);
        domain.bindValue(":part", part);
        domain.exec();

        if(domain.next()) {
            int id_domain = domain.value(0).toInt();
            QList<Competence*> comps = m_domains[part]->competences();

            for(int j = 0; j < comps.length(); j++) {
                int id = addCompetence(db, id_domain, comps[j]->name());
                comps[j]->setId(id);
            }
        }
    }

    //Update domain names
    updateDomains(db, id_student);
}

void CompetenceManager::applyStudent() {
    QSqlDatabase db = QSqlDatabase::database();
    if(!db.isOpen()) {
        QMessageBox::critical(this, "Erreur", "La connexion à la base de données a échoué.");
        return;
    }
    db.transaction();

    //Dequeue the action queue
    while(!m_queue.isEmpty()) {
        CompetenceAction *action = m_queue.dequeue();
        Competence *c = action->competence();

        switch(action->type()) {
            case ActionType::Add: {
                int id_comp = addCompetence(db, c->id_domains(), c->name());
                c->setId(id_comp);
                break;
            }
            case ActionType::Delete: {
                deleteCompetence(db, c->id());
                delete c;
                break;
            }
            default:
                break;
        }
        delete action;
    }

    //Update the domain names
    if(m_student != NULL)
        updateDomains(db, m_student->id());

    db.commit();
    accept();
}

int CompetenceManager::addCompetence(QSqlDatabase db, int id_domain, QString name) {
    if(id_domain == 0)
        return 0;

    QSqlQuery add(db);
    add.prepare("INSERT INTO competences(id_domains, name) VALUES(:id_domain, :name)");
    add.bindValue(":id_domain", id_domain);
    add.bindValue(":name", name);
    add.exec();

    return add.lastInsertId().toInt();
}

void CompetenceManager::deleteCompetence(QSqlDatabase db, int id_comp) {
    QSqlQuery del(db);
    del.prepare("DELETE FROM competences WHERE id=:id");
    del.bindValue(":id", id_comp);
    del.exec();
}

void CompetenceManager::updateDomains(QSqlDatabase db, int id_student) {
    for(int i = 0; i < m_domains.length(); i++) {
        QSqlQuery edit(db);
        edit.prepare("UPDATE domains SET name=:name WHERE id_students=:id_student AND part=:part");
        edit.bindValue(":name", m_domains[i]->name());
        edit.bindValue(":id_student", id_student);
        edit.bindValue(":part", i);
        edit.exec();
    }
}
