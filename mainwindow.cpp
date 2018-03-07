#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->statusBar->addPermanentWidget(ui->lbl_path);

    QAction *action = ui->menuBar->addAction("A propos de...");
    connect(action, SIGNAL(triggered(bool)), this, SLOT(openAboutDialog()));

    connect(ui->actionNew, SIGNAL(triggered(bool)), this, SLOT(createFile()));
    connect(ui->actionOpen, SIGNAL(triggered(bool)), this, SLOT(openFile()));
    connect(ui->btn_groups, SIGNAL(clicked(bool)), this, SLOT(openGroupsStudentsManager()));
    connect(ui->btn_competences, SIGNAL(clicked(bool)), this, SLOT(openCompetenceManager()));
    connect(ui->btn_reset, SIGNAL(clicked(bool)), this, SLOT(resetCompetences()));
    connect(ui->btn_pdf, SIGNAL(clicked(bool)), this, SLOT(printCompetences()));
    connect(ui->GroupsBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateStudentsList()));
    connect(ui->StudentsList, SIGNAL(itemSelectionChanged()), this, SLOT(updateRadar()));

    bool file_opened = false;
    // Ouvrir directement un fichier (double-clic)
    QStringList args = QCoreApplication::arguments();
    if(args.count() > 1) {
        QString suffix = QFileInfo(args[1]).suffix().toUpper();
        // Check the file suffix
        if(suffix == "CIBLE") {
            openFile(args[1]); // Try to open the file
            file_opened = true;
        } else {
            QMessageBox::critical(this, "Fichier non pris en charge", "Erreur : Fichier " + QFileInfo(args[1]).suffix().toUpper() + " non pris en charge.");
        }
    }

    //Try to load the default file and update window
    if(!file_opened) {
        Preferences pref;
        QString filename = pref.file();
        if(filename != "" && QFileInfo(filename).exists()) {
            openFile(filename);
        }
    }

    updateWindow();
}

MainWindow::~MainWindow()
{
    //DELETE SCENE (also deletes domains and competences)
    QGraphicsScene *scene = ui->graphicsView->scene();
    if(scene != NULL)
        delete scene;

    delete ui;
    free_groups();
    free_students();
}

void MainWindow::createFile() {
    //Try to load directory preferences
    Preferences pref;
    QString pref_path = pref.dir();

    //Get file name
    QString filename = QFileDialog::getSaveFileName(this, "Enregistrer sous...",
                                                    pref_path + QDir::separator() + "donnees",  "CIBLE (*.cible)");

    if(filename == "") {
        updateWindow();
        return;
    }

    //Save file in preferences
    pref.setFile(filename);

    if(kscopemanager.createFile(filename))
        QMessageBox::information(this, "Succès", "Votre fichier a été créé.<br />Vous pouvez dès maintenant l'utiliser.");

    updateWindow();
}

void MainWindow::openFile() {
    //Try to load directory preferences
    Preferences pref;
    QString pref_path = pref.dir();

    //Get file name
    QString fileDB = QFileDialog::getOpenFileName(this, "Ouvrir un fichier", pref_path + QDir::separator(), "CIBLE (*.cible)");

    if(fileDB == "") {
        updateWindow();
        return;
    }

    QString suffix = QFileInfo(fileDB).suffix().toUpper();
    // Check the file suffix
    if(suffix == "CIBLE")
        openFile(fileDB); // Try to open the file
    else
        QMessageBox::critical(this, "Fichier non pris en charge", "Erreur : Fichier " + QFileInfo(fileDB).suffix().toUpper() + " non pris en charge.");

    return;
}

void MainWindow::openFile(QString filename) {
    //Save directory in preferences
    Preferences pref;
    pref.setFile(filename);

    kscopemanager.openFile(filename);
    updateWindow();
}

void MainWindow::openGroupsStudentsManager() {
    GroupsStudentsManager mng(this);
    mng.exec();
    updateWindow();
}

void MainWindow::openAboutDialog() {
    AboutDialog dlg(this);
    dlg.exec();
}

void MainWindow::openCompetenceManager() {
    Group *g = NULL;
    Student *s = NULL;

    //Get current group
    QVariant data = ui->GroupsBox->currentData();
    if(data.isValid())
        g = (Group*) data.toULongLong();

    //Get current student
    QList<QListWidgetItem*> selection = ui->StudentsList->selectedItems();
    if(selection.length() > 0)
        s = (Student*) selection[0]->data(Qt::UserRole).toULongLong();

    CompetenceManager mng(g, s, this);
    mng.exec();
    updateRadar();
}

//To complete !!!
void MainWindow::updateWindow() {
    //Check if DataBase is open
    QSqlDatabase db = QSqlDatabase::database();

    //Enable buttons and lists
    ui->btn_groups->setEnabled(db.isOpen());
    ui->btn_competences->setEnabled(db.isOpen());
    ui->btn_reset->setEnabled(db.isOpen());
    ui->btn_pdf->setEnabled(db.isOpen());
    ui->GroupsBox->setEnabled(db.isOpen());
    ui->StudentsList->setEnabled(db.isOpen());

    //Update label
    ui->lbl_path->setText(db.isOpen() ? "Fichier chargé : " + db.databaseName() : "Aucun fichier chargé.");

    //Update screen
    if(db.isOpen()) {
        updateGroupsBox();
        updateStudentsList();
    }
    updateRadar();
}

void MainWindow::updateGroupsBox() {
    //Clear what was before
    ui->GroupsBox->clear();
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

    //Add groups to ComboBox
    while(q.next()) {
        Group *g = new Group();
        g->setId(q.value(0).toInt());
        g->setName(q.value(1).toString());

        ui->GroupsBox->addItem(g->name(), (qulonglong) g);
        m_displayedGroups.enqueue(g);
    }
}

void MainWindow::free_groups() {
    while(!m_displayedGroups.isEmpty()) {
        delete m_displayedGroups.dequeue();
    }
}

void MainWindow::updateStudentsList() {
    //Clear what was before
    ui->StudentsList->clear();
    free_students();

    //Get groups selection
    QVariant data = ui->GroupsBox->currentData();

    bool enable = data.isValid();
    ui->StudentsList->setEnabled(enable);
    if(!enable)
        return;

    //Get students from selected group
    Group* g = (Group*) data.toULongLong();

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

        QListWidgetItem *item = new QListWidgetItem(s->name() + " " + s->first_name(), ui->StudentsList);
        item->setData(Qt::UserRole, (qulonglong) s);
        m_displayedStudents.enqueue(s);
    }
}

void MainWindow::free_students() {
    while(!m_displayedStudents.isEmpty())
        delete m_displayedStudents.dequeue();
}

void MainWindow::updateRadar() {
    //DONT DELETE DOMAINS AND COMPETENCES
    Radar *old = (Radar*) ui->graphicsView->scene();

    //Get selected student
    QList<QListWidgetItem*> selection = ui->StudentsList->selectedItems();

    if(selection.length() <= 0) {
        QList<Domain*> empty;
        for(int i = 0; i < DOMAIN_COUNT; i++)
            empty << new Domain();

        Radar *r = new Radar(this, ui->graphicsView, empty);
        ui->graphicsView->setScene(r);
        ui->scrollArea->setWidget(new CompetenceGrid(NULL, QList<Domain*>(), r));
        delete old;
        return;
    }

    Student *s = (Student*) selection[0]->data(Qt::UserRole).toULongLong();

    QSqlDatabase db = QSqlDatabase::database();
    if(!db.isOpen()) {
        QMessageBox::critical(this, "Erreur", "La connexion à la base de données a échoué.");
        return;
    }

    QList<Domain*> doms;

    QSqlQuery domains(db);
    domains.prepare("SELECT id, part, name, id_students FROM domains WHERE id_students=:id_student ORDER BY part");
    domains.bindValue(":id_student", s->id());
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

        doms.append(d);
    }

    Radar *r = new Radar(this, ui->graphicsView, doms);
    ui->graphicsView->setScene(r);
    ui->scrollArea->setWidget(new CompetenceGrid(s, doms, r));
    delete old;
}

AffectedStudent MainWindow::question_affected(QString title, QString text, Student *s, Group *g) {
    //Construct MessageBox
    QMessageBox msg;
    msg.setWindowTitle(title);
    msg.setText(text);

    QPushButton *btn_all = msg.addButton("Tous les élèves", QMessageBox::AcceptRole);
    QPushButton *btn_group = new QPushButton();
    QPushButton *btn_student = new QPushButton();

    //Add selected group
    if(g != NULL) {
        btn_group->setText("La classe " + g->name());
        msg.addButton(btn_group, QMessageBox::AcceptRole);
    }

    //Add selected student
    if(s != NULL) {
        btn_student->setText("L'élève " + s->name() + " " + s->first_name());
        msg.addButton(btn_student, QMessageBox::AcceptRole);
    }

    QPushButton *btn_cancel = msg.addButton(QMessageBox::Cancel);

    msg.exec();

    if(msg.clickedButton() == btn_all)
        return All;
    if(msg.clickedButton() == btn_group)
        return OneGroup;
    if(msg.clickedButton() == btn_student)
        return OneStudent;
    if(msg.clickedButton() == btn_cancel)
        return Cancel;

    return Cancel;
}

void MainWindow::printCompetences() {
    /** To print to PDF **/
    //Get student
    Student *s = NULL;
    QList<QListWidgetItem *> selection = ui->StudentsList->selectedItems();
    if(selection.length() > 0)
        s = (Student*) selection[0]->data(Qt::UserRole).toULongLong();

    //Get group
    Group *g = NULL;
    QVariant data = ui->GroupsBox->currentData();
    if(data.isValid())
        g = (Group*) data.toULongLong();

    AffectedStudent affected = question_affected("Génération de PDF...", "Pour quel(s) élève(s) voulez-vous générer un fichier PDF ?", s, g);

    if(affected == Cancel)
        return;

    //Try to load directory preferences
    Preferences pref;
    QString pref_path = pref.dir();

    bool success = false;

    //Get file name
    if(affected == OneStudent) {
        if(s == NULL || g == NULL || ui->graphicsView->scene() == NULL)
            return;

        QString filename = QFileDialog::getSaveFileName(this, "Enregistrer sous...",
                                                        pref_path + QDir::separator() + s->name() + "_" + s->first_name(),  "PDF (*.pdf)");
        if(filename == "")
            return;
        success = PDFCreator::printStudent((Radar*) ui->graphicsView->scene(), filename, s, g->name());
    }
    else {
        QString dirname = QFileDialog::getExistingDirectory(this, "Enregistrer sous...", pref_path + QDir::separator());
        if(dirname == "")
            return;

        int res = QMessageBox::question(this, "Génération de PDF...", "Cette opération peut éventuellement écraser d'anciens fichiers PDF contenus dans le répertoire sélectionné ! <br/>"
                                                                      "Voulez-vous vraiment continuer ?");
        if(res == QMessageBox::Yes) {
            if(affected == All)
                success = printAll(dirname);
            if(affected == OneGroup && g != NULL)
                success = printClass(dirname, g->name());
        }
    }

    if(success)
        QMessageBox::information(this, "Succès", "Fichier(s) PDF généré(s).");
    else
        QMessageBox::critical(this, "Erreur", "Des problèmes sont survenus lors de la génération des PDF.");
}


bool MainWindow::printClass(QString dirname, QString group_name) {
    bool success = true;
    for(int i = 0; i < ui->StudentsList->count(); i++) {
        ui->StudentsList->setCurrentRow(i);
        Student *s = (Student*) ui->StudentsList->currentItem()->data(Qt::UserRole).toULongLong();
        if(!PDFCreator::printStudent((Radar*) ui->graphicsView->scene(),
                                     dirname + QDir::separator() + s->name() + "_" + s->first_name() + ".pdf",
                                     s,
                                     group_name))
            success = false;
    }
    return success;
}

bool MainWindow::printAll(QString dirname) {
    bool success = true;
    for(int i = 0; i < ui->GroupsBox->count(); i++) {
        ui->GroupsBox->setCurrentIndex(i);
        Group *g = (Group*) ui->GroupsBox->currentData().toULongLong();
        if(!printClass(dirname, g->name()))
            success = false;
    }
    return success;
}

void MainWindow::resetCompetences() {
    //Get student
    Student *s = NULL;
    QList<QListWidgetItem *> selection = ui->StudentsList->selectedItems();
    if(selection.length() > 0)
        s = (Student*) selection[0]->data(Qt::UserRole).toULongLong();

    //Get group
    Group *g = NULL;
    QVariant data = ui->GroupsBox->currentData();
    if(data.isValid())
        g = (Group*) data.toULongLong();

    //Get affected...
    AffectedStudent affected = question_affected("Remise à zéro...", "De quel(s) élève(s) voulez-vous réinitialiser les compétences et commentaires ?", s, g);

    if(affected == Cancel)
        return;

    //Verification
    QString message = "Cette opération va, pour ";
    switch(affected) {
    case All:
        message += "tous les élèves";
        break;
    case OneGroup:
        message += "la classe " + g->name();
        break;
    case OneStudent:
        message += "l'élève " + s->name() + " " + s->first_name();
        break;
    default:
        break;
    }
    message += ", remettre les compétences en 'Non Evalué' et supprimer les commentaires.<br/>Voulez-vous vraiment continuer ?";
    int res = QMessageBox::question(this, "Remise à zéro...", message);

    if(res == QMessageBox::Yes) {
        //Get DB
        QSqlDatabase db = QSqlDatabase::database();
        if(!db.isOpen()) {
            QMessageBox::critical(this, "Erreur", "La connexion à la base de données a échoué.");
            return;
        }

        QSqlQuery comp_query(db);
        QSqlQuery students_query(db);

        if(affected == All) {
            comp_query.prepare("UPDATE competences SET value=0, comments=''");
            students_query.prepare("UPDATE students SET comments=''");
        }
        if(affected == OneGroup) {
            comp_query.prepare("UPDATE competences SET value=0, comments='' WHERE id_domains IN "
                               "(SELECT domains.id FROM domains JOIN students ON domains.id_students=students.id "
                               "WHERE students.id_groups=:id_group)");
            comp_query.bindValue(":id_group", g->id());

            students_query.prepare("UPDATE students SET comments='' WHERE id_groups=:id_group");
            students_query.bindValue(":id_group", g->id());
        }
        if(affected == OneStudent) {
            comp_query.prepare("UPDATE competences SET value=0, comments='' WHERE id_domains IN "
                               "(SELECT id FROM domains WHERE id_students=:id_student)");
            comp_query.bindValue(":id_student", s->id());

            students_query.prepare("UPDATE students SET comments='' WHERE id=:id_student");
            students_query.bindValue(":id_student", s->id());
        }

        comp_query.exec();
        students_query.exec();
    }
    updateWindow();
}
