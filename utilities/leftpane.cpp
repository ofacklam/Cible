#include "leftpane.h"
#include "ui_leftpane.h"

LeftPane::LeftPane(QGraphicsView *gv, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LeftPane)
{
    ui->setupUi(this);
    m_pagesBox = new QToolBox;
    ui->verticalLayout->addWidget(m_pagesBox);
    ui->verticalLayout->setStretch(0, 2);
    ui->verticalLayout->setStretch(1, 3);

    m_graphicsView = gv;
    m_window = parent;

    connect(ui->GroupsList, SIGNAL(itemSelectionChanged()), this, SLOT(updateStudentsList()));
    connect(ui->StudentsList, SIGNAL(itemSelectionChanged()), this, SLOT(updatePagesBox()));
    connect(m_pagesBox, SIGNAL(currentChanged(int)), this, SLOT(updateRadar(int)));
}

LeftPane::~LeftPane()
{
    unset_radars();

    //DELETE SCENE (also deletes domains and competences)
    QGraphicsScene *scene = m_graphicsView->scene();
    if(scene != NULL)
        delete scene;

    delete ui;
    free_pages();
    free_students();
    free_groups();   
}

Group *LeftPane::selectedGroup() {
    Group *g = NULL;

    QList<QListWidgetItem*> gsel = ui->GroupsList->selectedItems();
    if(gsel.length() > 0)
        g = (Group*) gsel[0]->data(Qt::UserRole).toULongLong();

    return g;
}

Student *LeftPane::selectedStudent() {
    Student *s = NULL;

    QList<QListWidgetItem*> ssel = ui->StudentsList->selectedItems();
    if(ssel.length() > 0)
        s = (Student*) ssel[0]->data(Qt::UserRole).toULongLong();

    return s;
}

Page *LeftPane::selectedPage() {
    Page *p = NULL;

    int index = m_pagesBox->currentIndex();
    if(index != -1)
        p = m_displayedPageWidgets[index]->page();

    return p;
}

Tab *LeftPane::selectedTab() {
    Tab *t = NULL;

    int index = m_pagesBox->currentIndex();
    if(index != -1)
        t = m_displayedPageWidgets[index]->tab();

    return t;
}

void LeftPane::updateGroupsList() {
    //Clear what was before
    ui->GroupsList->clear();
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

    //Add groups to ListWidget
    while(q.next()) {
        Group *g = new Group();
        g->setId(q.value(0).toInt());
        g->setName(q.value(1).toString());

        QListWidgetItem *item = new QListWidgetItem(g->name(), ui->GroupsList);
        item->setData(Qt::UserRole, (qulonglong) g);
        m_displayedGroups.enqueue(g);
    }
}

void LeftPane::free_groups() {
    while(!m_displayedGroups.isEmpty()) {
        delete m_displayedGroups.dequeue();
    }
}

void LeftPane::updateStudentsList() {
    //Clear what was before
    ui->StudentsList->clear();
    free_students();

    //Get groups selection
    QList<QListWidgetItem*> gsel = ui->GroupsList->selectedItems();

    bool enable = (gsel.length() > 0);
    ui->StudentsList->setEnabled(enable);
    if(!enable)
        return;

    //Get students from selected group
    Group* g = (Group*) gsel[0]->data(Qt::UserRole).toULongLong();

    QSqlDatabase db = QSqlDatabase::database();
    if(!db.isOpen()) {
        QMessageBox::critical(this, "Erreur", "La connexion à la base de données a échoué.");
        return;
    }

    QSqlQuery q(db);
    q.prepare("SELECT id, id_groups, name, first_name FROM students WHERE id_groups=:id_groups ORDER BY name, first_name");
    q.bindValue(":id_groups", g->id());
    q.exec();

    //Display students
    while(q.next()) {
        Student *s = new Student();
        s->setId(q.value(0).toInt());
        s->setId_groups(q.value(1).toInt());
        s->setName(q.value(2).toString());
        s->setFirst_name(q.value(3).toString());

        QListWidgetItem *item = new QListWidgetItem(s->name() + " " + s->first_name(), ui->StudentsList);
        item->setData(Qt::UserRole, (qulonglong) s);
        m_displayedStudents.enqueue(s);
    }
}

void LeftPane::free_students() {
    while(!m_displayedStudents.isEmpty())
        delete m_displayedStudents.dequeue();
}

void LeftPane::updatePagesBox() {
    //Clear what was before
    free_pages();

    //Get student selection
    Student *s = selectedStudent();

    m_pagesBox->setEnabled(s != NULL);
    if(s == NULL) {
        updateRadar(-1);
        return;
    }

    //Get pages from selected student
    QSqlDatabase db = QSqlDatabase::database();
    if(!db.isOpen()) {
        QMessageBox::critical(this, "Erreur", "La connexion à la base de données a échoué.");
        return;
    }

    QSqlQuery qPages(db);
    qPages.prepare("SELECT id, name, id_students FROM pages WHERE id_students=:id_students ORDER BY id");
    qPages.bindValue(":id_students", s->id());
    qPages.exec();

    //Display pages
    while(qPages.next()) {
        Page *p = new Page();
        p->setId(qPages.value(0).toInt());
        p->setName(qPages.value(1).toString());
        p->setId_students(qPages.value(2).toInt());

        //Get domains of page
        QSqlQuery qDomains(db);
        qDomains.prepare("SELECT id, name, id_pages FROM domains WHERE id_pages=:id_pages ORDER BY id");
        qDomains.bindValue(":id_pages", p->id());
        qDomains.exec();

        while(qDomains.next()) {
            Domain *d = new Domain();
            d->setId(qDomains.value(0).toInt());
            d->setName(qDomains.value(1).toString());
            d->setId_pages(qDomains.value(2).toInt());

            //Get competences of domain
            QSqlQuery qComps(db);
            qComps.prepare("SELECT id, name, id_domains FROM competences WHERE id_domains=:id_domains ORDER BY id");
            qComps.bindValue(":id_domains", d->id());
            qComps.exec();

            while(qComps.next()) {
                Competence *c = new Competence();
                c->setId(qComps.value(0).toInt());
                c->setName(qComps.value(1).toString());
                c->setId_domains(qComps.value(2).toInt());
                d->add_competence(c);
            }

            p->add_domain(d);
        }

        //Add CompetenceTabWidget
        CompetenceTabWidget *ctw = new CompetenceTabWidget(p);
        m_pagesBox->addItem(ctw, p->name());
        m_displayedPageWidgets.enqueue(ctw);
    }
}

void LeftPane::free_pages() {
    while(!m_displayedPageWidgets.isEmpty()) {
        CompetenceTabWidget *ctw = m_displayedPageWidgets.dequeue();
        Page *p = ctw->page();

        QList<Domain*> domains = p->domains();
        while(!domains.isEmpty()) {
            Domain *d = domains.takeFirst();

            QList<Competence*> comps = d->competences();
            while(!comps.isEmpty())
                delete comps.takeFirst();

            delete d;
        }

        delete p;
        delete ctw;
    }
}

void LeftPane::updateRadar(int index) {
    //DONT DELETE DOMAINS AND COMPETENCES
    Radar *old_radar = (Radar*) m_graphicsView->scene();
    unset_radars();

    if(index == -1) {
        Radar *r = new Radar(m_window, m_graphicsView, NULL);
        m_graphicsView->setScene(r);
        delete old_radar;
        return;
    }

    Radar *r = new Radar(m_window, m_graphicsView, m_displayedPageWidgets[index]->page());
    m_graphicsView->setScene(r);
    m_displayedPageWidgets[index]->setRadar(r);
    delete old_radar;
}

void LeftPane::unset_radars() {
    foreach(CompetenceTabWidget *ctw, m_displayedPageWidgets)
        ctw->setRadar(NULL);
}

AffectedStudent LeftPane::question_affected(QString title, QString text, Group *g, Student *s, Page *p, Tab *t) {
    //Construct MessageBox
    QMessageBox msg;
    msg.setWindowTitle(title);
    msg.setText(text);

    QPushButton *btn_all = msg.addButton("Tous les élèves", QMessageBox::AcceptRole);
    QPushButton *btn_group = NULL;
    QPushButton *btn_student = NULL;
    QPushButton *btn_page = NULL;
    QPushButton *btn_tab = NULL;

    //Add selected group
    if(g != NULL) {
        btn_group = new QPushButton();
        btn_group->setText("La classe " + g->name());
        msg.addButton(btn_group, QMessageBox::AcceptRole);
    }

    //Add selected student
    if(s != NULL) {
        btn_student = new QPushButton();
        btn_student->setText("L'élève " + s->name() + " " + s->first_name());
        msg.addButton(btn_student, QMessageBox::AcceptRole);
    }

    //Add selected page
    if(p != NULL) {
        btn_page = new QPushButton();
        btn_page->setText("La page " + p->name());
        msg.addButton(btn_page, QMessageBox::AcceptRole);
    }

    //Add selected tab
    if(t != NULL) {
        btn_tab = new QPushButton();
        btn_tab->setText("L'onglet " + t->name());
        msg.addButton(btn_page, QMessageBox::AcceptRole);
    }

    QPushButton *btn_cancel = msg.addButton(QMessageBox::Cancel);

    msg.exec();

    if(msg.clickedButton() == btn_all)
        return All;
    else if(msg.clickedButton() == btn_group)
        return OneGroup;
    else if(msg.clickedButton() == btn_student)
        return OneStudent;
    else if(msg.clickedButton() == btn_page)
        return OnePage;
    else if(msg.clickedButton() == btn_tab)
        return OneTab;
    else
        return Cancel;
}

void LeftPane::printCompetences() {
    /** To print to PDF **/

    /*Student *s = selectedStudent();
    Group *g = selectedGroup();
    Page *p = selectedPage();
    Tab *t = selectedTab();

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
        QMessageBox::critical(this, "Erreur", "Des problèmes sont survenus lors de la génération des PDF.");*/
}


bool LeftPane::printClass(QString dirname, QString group_name) {
    bool success = true;
    /*for(int i = 0; i < ui->StudentsList->count(); i++) {
        ui->StudentsList->setCurrentRow(i);
        Student *s = (Student*) ui->StudentsList->currentItem()->data(Qt::UserRole).toULongLong();
        if(!PDFCreator::printStudent((Radar*) ui->graphicsView->scene(),
                                     dirname + QDir::separator() + s->name() + "_" + s->first_name() + ".pdf",
                                     s,
                                     group_name))
            success = false;
    }*/
    return success;
}

bool LeftPane::printAll(QString dirname) {
    bool success = true;
    /*for(int i = 0; i < ui->GroupsList->count(); i++) {
        ui->GroupsList->setCurrentRow(i);
        Group *g = (Group*) ui->GroupsList->currentItem()->data(Qt::UserRole).toULongLong();
        if(!printClass(dirname, g->name()))
            success = false;
    }*/
    return success;
}

void LeftPane::resetCompetences() {
    /*//Get student
    Student *s = NULL;
    QList<QListWidgetItem *> ssel = ui->StudentsList->selectedItems();
    if(ssel.length() > 0)
        s = (Student*) ssel[0]->data(Qt::UserRole).toULongLong();

    //Get group
    Group *g = NULL;
    QList<QListWidgetItem *> gsel = ui->GroupsList->selectedItems();
    if(gsel.length() > 0)
        g = (Group*) gsel[0]->data(Qt::UserRole).toULongLong();

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
    updateWindow();*/
}
