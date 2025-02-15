#include "competencetabwidget.h"
#include "ui_competencetabwidget.h"

CompetenceTabWidget::CompetenceTabWidget(Student *s, Page *p, QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::CompetenceTabWidget)
{
    ui->setupUi(this);
    m_student = s;
    m_page = p;

    connect(this, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));

    getTabs();
}

CompetenceTabWidget::~CompetenceTabWidget()
{
    clearTabs();
    delete ui;
}

Page *CompetenceTabWidget::page() {
    return m_page;
}

Tab *CompetenceTabWidget::tab() {
    Tab *t = NULL;

    int index = this->currentIndex();
    if(index != -1)
        t = m_displayedTabs[index];

    return t;
}

void CompetenceTabWidget::setRadar(Radar *r) {
    m_radar = r;
}

void CompetenceTabWidget::getTabs() {
    clearTabs();

    QSqlDatabase db = QSqlDatabase::database();
    if(!db.isOpen()) {
        QMessageBox::critical(this, "Erreur", "La connexion à la base de données a échoué.");
        return;
    }

    //Get tabs of this page
    QSqlQuery qTabs(db);
    qTabs.prepare("SELECT id, name, id_pages, comments FROM tabs WHERE id_pages=:id_pages ORDER BY id");
    qTabs.bindValue(":id_pages", m_page->id());
    qTabs.exec();

    while(qTabs.next()) {
        Tab *t = new Tab();
        t->setId(qTabs.value(0).toInt());
        t->setName(qTabs.value(1).toString());
        t->setId_pages(qTabs.value(2).toInt());
        t->setComments(qTabs.value(3).toString());

        //Get values associated with this tab for every competence
        QSqlQuery qVals(db);
        qVals.prepare("SELECT id_competences, value, comments FROM `values` WHERE id_tabs=:id_tabs");
        qVals.bindValue(":id_tabs", t->id());
        qVals.exec();

        while(qVals.next()) {
            int id_comp = qVals.value(0).toInt();
            t->insertValue(id_comp, qVals.value(1).toInt());
            t->insertComment(id_comp, qVals.value(2).toString());
        }

        //Display tab
        QScrollArea *sa = new QScrollArea;
        sa->setWidgetResizable(true);
        sa->setWidget(new CompetenceGrid(m_student, m_page, t));
        this->addTab(sa, t->name());
        m_displayedTabs.enqueue(t);
    }
}

void CompetenceTabWidget::clearTabs() {
    while(!m_displayedTabs.isEmpty())
        delete m_displayedTabs.dequeue();
}

void CompetenceTabWidget::tabChanged(int index) {
    if(m_radar != NULL) {
        m_radar->clearTabs();
        m_radar->addTab(m_displayedTabs[index]);
    }
}
