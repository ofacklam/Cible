#include "groupsstudentsmanager.h"
#include "ui_groupsstudentsmanager.h"

GroupsStudentsManager::GroupsStudentsManager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GroupsStudentsManager)
{
    ui->setupUi(this);

    connect(ui->btn_add_groups, SIGNAL(clicked(bool)), this, SLOT(add_group()));
    connect(ui->btn_delete_groups, SIGNAL(clicked(bool)), this, SLOT(delete_group()));
    connect(ui->btn_add_students, SIGNAL(clicked(bool)), this, SLOT(add_student()));
    connect(ui->btn_delete_students, SIGNAL(clicked(bool)), this, SLOT(delete_student()));
    connect(ui->list_groups, SIGNAL(itemSelectionChanged()), this, SLOT(groups_selection_change()));
    connect(ui->list_students, SIGNAL(itemSelectionChanged()), this, SLOT(students_selection_change()));
    connect(ui->btn_import, SIGNAL(clicked(bool)), this, SLOT(import_students()));

    update_groups();
    update_students();
}

GroupsStudentsManager::~GroupsStudentsManager()
{
    delete ui;
    free_groups();
    free_students();
}

void GroupsStudentsManager::update_groups() {
    //Clear what was before
    ui->list_groups->clear();
    free_groups();

    //Get the groups with SQL request
    QSqlDatabase db = QSqlDatabase::database();
    if(!db.isOpen()) {
        QMessageBox::critical(this, "Erreur", "La connexion à la base de données a échoué.");
        return;
    }

    QSqlQuery q(db);
    q.prepare("SELECT id, name FROM groups ORDER BY name");
    q.exec();

    //Add groups to list
    while(q.next()) {
        Group *g = new Group();
        g->setId(q.value(0).toInt());
        g->setName(q.value(1).toString());

        QListWidgetItem *item = new QListWidgetItem(g->name(), ui->list_groups);
        item->setData(Qt::UserRole, (qulonglong) g);
        m_displayedGroups.enqueue(g);
    }
}

void GroupsStudentsManager::free_groups() {
    while(!m_displayedGroups.isEmpty()) {
        delete m_displayedGroups.dequeue();
    }
}

void GroupsStudentsManager::update_students() {
    //Clear what was before
    ui->list_students->clear();
    free_students();

    //Get groups selection
    QList<QListWidgetItem*> selection = ui->list_groups->selectedItems();

    bool enable = (selection.length() > 0);

    ui->list_students->setEnabled(enable);
    ui->btn_add_students->setEnabled(enable);
    ui->btn_delete_students->setEnabled(enable);
    ui->edit_first_name->setEnabled(enable);
    ui->edit_name->setEnabled(enable);
    ui->btn_import->setEnabled(enable);

    if(!enable)
        return;

    //Get students from selected group
    Group* g = (Group*) selection[0]->data(Qt::UserRole).toULongLong();

    QSqlDatabase db = QSqlDatabase::database();
    if(!db.isOpen()) {
        QMessageBox::critical(this, "Erreur", "La connexion à la base de données a échoué.");
        return;
    }

    QSqlQuery q(db);
    q.prepare("SELECT id, id_groups, name, first_name, comments FROM students WHERE id_groups=:id_groups ORDER BY name, first_name");
    q.bindValue(":id_groups", g->id());
    q.exec();

    //Display students
    while(q.next()) {
        Student *s = new Student();
        s->setId(q.value(0).toInt());
        s->setId_groups(q.value(1).toInt());
        s->setName(q.value(2).toString());
        s->setFirst_name(q.value(3).toString());
        s->setComments(q.value(4).toString());

        QListWidgetItem *item = new QListWidgetItem(s->name() + " " + s->first_name(), ui->list_students);
        item->setData(Qt::UserRole, (qulonglong) s);
        m_displayedStudents.enqueue(s);
    }

}

void GroupsStudentsManager::free_students() {
    while(!m_displayedStudents.isEmpty()) {
        delete m_displayedStudents.dequeue();
    }
}

void GroupsStudentsManager::groups_selection_change() {
    //Get groups selection
    QList<QListWidgetItem*> selection = ui->list_groups->selectedItems();

    if(selection.length() > 0) {
        Group *g = (Group*) selection[0]->data(Qt::UserRole).toULongLong();
        ui->edit_groups->setText(g->name());
    }

    update_students();
}

void GroupsStudentsManager::students_selection_change() {
    //Get students selection
    QList<QListWidgetItem*> selection = ui->list_students->selectedItems();

    if(selection.length() > 0) {
        Student *s = (Student*) selection[0]->data(Qt::UserRole).toULongLong();
        ui->edit_name->setText(s->name());
        ui->edit_first_name->setText(s->first_name());
    }
}

void GroupsStudentsManager::add_group() {
    //Get name of group
    QString name = ui->edit_groups->text();

    if(name == "") {
        QMessageBox::critical(this, "Erreur", "Veuillez renseigner un nom de classe.");
        return;
    }

    //Add group to DB
    QSqlDatabase db = QSqlDatabase::database();
    if(!db.isOpen()) {
        QMessageBox::critical(this, "Erreur", "La connexion à la base de données a échoué.");
        return;
    }

    QSqlQuery q(db);
    q.prepare("INSERT INTO groups(name) VALUES(:name)");
    q.bindValue(":name", name);
    q.exec();

    ui->edit_groups->clear();
    update_groups();
}

void GroupsStudentsManager::delete_group() {
    //Get group selection
    QList<QListWidgetItem*> selection = ui->list_groups->selectedItems();

    if(selection.length() <= 0) {
        QMessageBox::critical(this, "Erreur", "Veuillez sélectionner une classe.");
        return;
    }

    Group *g = (Group*) selection[0]->data(Qt::UserRole).toULongLong();

    //Verification
    int first = QMessageBox::question(this, "Suppression", "Etes-vous sûr de vouloir supprimer la classe <strong>" + g->name() + "</strong> ? <br/>"
                                                           "Cela supprimera aussi tous les élèves de la classe ainsi que leurs cibles de compétences !");
    if(first == QMessageBox::Yes) {
        int second = QMessageBox::question(this, "Suppression", "Etes-vous vraiment sûr de vouloir supprimer la classe <strong>" + g->name() + "</strong> ainsi que <strong> toutes les données </strong> associées ?");

        if(second == QMessageBox::Yes) {
            /** DELETE GROUP **/
            //Get DB
            QSqlDatabase db = QSqlDatabase::database();
            if(!db.isOpen()) {
                QMessageBox::critical(this, "Erreur", "La connexion à la base de données a échoué.");
                return;
            }

            //Delete scores
            /*QSqlQuery scores(db);
            scores.prepare("DELETE FROM scores WHERE id_competences IN "
                           "(SELECT competences.id FROM "
                           "competences JOIN domains ON competences.id_domains=domains.id "
                           "JOIN students ON domains.id_students=students.id "
                           "WHERE students.id_groups=:id_group)");
            scores.bindValue(":id_group", g->id());
            scores.exec();*/

            //Delete competences
            QSqlQuery comp(db);
            comp.prepare("DELETE FROM competences WHERE id_domains IN "
                         "(SELECT domains.id FROM "
                         "domains JOIN students ON domains.id_students=students.id "
                         "WHERE students.id_groups=:id_group)");
            comp.bindValue(":id_group", g->id());
            comp.exec();

            //Delete domains
            QSqlQuery domains(db);
            domains.prepare("DELETE FROM domains WHERE id_students IN "
                            "(SELECT id FROM students WHERE id_groups=:id_group)");
            domains.bindValue(":id_group", g->id());
            domains.exec();

            //Delete students
            QSqlQuery students(db);
            students.prepare("DELETE FROM students WHERE id_groups=:id_group");
            students.bindValue(":id_group", g->id());
            students.exec();

            //Delete group
            QSqlQuery group(db);
            group.prepare("DELETE FROM groups WHERE id=:id_group");
            group.bindValue(":id_group", g->id());
            group.exec();

            update_groups();
        }
    }
}

void GroupsStudentsManager::add_student() {
    //Get name of student
    QString name = ui->edit_name->text();
    QString first_name = ui->edit_first_name->text();

    if(name == "" || first_name == "") {
        QMessageBox::critical(this, "Erreur", "Veuillez renseigner le nom et le prénom de l'élève.");
        return;
    }

    QSqlDatabase db = QSqlDatabase::database();
    if(!db.isOpen()) {
        QMessageBox::critical(this, "Erreur", "La connexion à la base de données a échoué.");
        return;
    }

    student_to_DB(&db, name, first_name);

    ui->edit_first_name->clear();
    ui->edit_name->clear();
    update_students();
}

void GroupsStudentsManager::student_to_DB(QSqlDatabase *db, QString name, QString first_name) {
    //Get selected group
    QList<QListWidgetItem*> selection = ui->list_groups->selectedItems();
    if(selection.length() <= 0) {
        QMessageBox::critical(this, "Erreur", "Aucune classe n'est sélectionnée. Comment est-ce possible ?");
        return;
    }
    Group *g = (Group*) selection[0]->data(Qt::UserRole).toULongLong();

    //Add student to DB
    QSqlQuery q(*db);
    q.prepare("INSERT INTO students(name, first_name, id_groups) VALUES(:name, :first_name, :id_groups)");
    q.bindValue(":name", name);
    q.bindValue(":first_name", first_name);
    q.bindValue(":id_groups", g->id());
    q.exec();
    int id_student = q.lastInsertId().toInt();

    //Add domains to DB
    QSqlQuery domains(*db);
    domains.prepare("INSERT INTO domains(id_students, part) VALUES(:id_student, 0), (:id_student, 1), (:id_student, 2), (:id_student, 3)");
    domains.bindValue(":id_student", id_student);
    domains.exec();
}

void GroupsStudentsManager::delete_student() {
    //Get student selection
    QList<QListWidgetItem*> selection = ui->list_students->selectedItems();

    if(selection.length() <= 0) {
        QMessageBox::critical(this, "Erreur", "Veuillez sélectionner un élève.");
        return;
    }

    Student *s = (Student*) selection[0]->data(Qt::UserRole).toULongLong();

    //Verification
    int first = QMessageBox::question(this, "Suppression", "Etes-vous sûr de vouloir supprimer l'élève <strong>" + s->name() + " " + s->first_name() + "</strong> ? <br/>"
                                                           "Cela supprimera aussi sa cible de compétences !");
    if(first == QMessageBox::Yes) {
        int second = QMessageBox::question(this, "Suppression", "Etes-vous vraiment sûr de vouloir supprimer l'élève <strong>" + s->name() + " " + s->first_name() + "</strong> ainsi que <strong> toutes les données </strong> associées ?");

        if(second == QMessageBox::Yes) {
            /** DELETE STUDENT **/
            //Get DB
            QSqlDatabase db = QSqlDatabase::database();
            if(!db.isOpen()) {
                QMessageBox::critical(this, "Erreur", "La connexion à la base de données a échoué.");
                return;
            }

            //Delete scores
            /*QSqlQuery scores(db);
            scores.prepare("DELETE FROM scores WHERE id_competences IN "
                           "(SELECT competences.id FROM "
                           "competences JOIN domains ON competences.id_domains=domains.id "
                           "WHERE domains.id_students=:id_student)");
            scores.bindValue(":id_student", s->id());
            scores.exec();*/

            //Delete competences
            QSqlQuery comp(db);
            comp.prepare("DELETE FROM competences WHERE id_domains IN "
                         "(SELECT domains.id FROM domains WHERE domains.id_students=:id_student)");
            comp.bindValue(":id_student", s->id());
            comp.exec();

            //Delete domains
            QSqlQuery domains(db);
            domains.prepare("DELETE FROM domains WHERE id_students=:id_student");
            domains.bindValue(":id_student", s->id());
            domains.exec();

            //Delete student
            QSqlQuery students(db);
            students.prepare("DELETE FROM students WHERE id=:id_student");
            students.bindValue(":id_student", s->id());
            students.exec();

            update_students();
        }
    }
}

void GroupsStudentsManager::import_students() {
    //Try to load directory preferences
    Preferences pref;
    QString pref_path = pref.dir();

    //Get file name
    QString filename = QFileDialog::getOpenFileName(this, "Ouvrir un fichier", pref_path + QDir::separator(), "CSV (*.csv)");

    if(filename == "")
        return;

    //Get DB
    QSqlDatabase db = QSqlDatabase::database();
    if(!db.isOpen()) {
        QMessageBox::critical(this, "Erreur", "La connexion à la base de données a échoué.");
        return;
    }

    //Use StudentsImportManager
    StudentsImportManager manager(&db, this);
    if(manager.load(filename))
        QMessageBox::information(this, "Importation", "Importation des étudiants réussie.");
    update_students();
}
