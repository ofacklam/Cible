#include "competencecell.h"
#include "ui_competencecell.h"

CompetenceCell::CompetenceCell(Domain *d, QQueue<CompetenceAction *> *queue, Radar *r, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CompetenceCell)
{
    ui->setupUi(this);

    m_domain = d;
    m_queue = queue;
    m_radar = r;

    connect(ui->edit_domain, SIGNAL(textChanged(QString)), this, SLOT(domainChanged(QString)));
    connect(ui->list_comps, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
    connect(ui->btn_add, SIGNAL(clicked(bool)), this, SLOT(addComp()));
    connect(ui->btn_delete, SIGNAL(clicked(bool)), this, SLOT(deleteComp()));

    //Set domain name
    ui->edit_domain->setText(m_domain->name());

    //Populate list of competences
    QList<Competence *> comps = m_domain->competences();
    for(int i = 0; i < comps.length(); i++) {
        new QListWidgetItem(comps[i]->name(), ui->list_comps);
    }
}

CompetenceCell::~CompetenceCell()
{
    delete ui;
}

void CompetenceCell::domainChanged(QString name) {
    m_domain->setName(name);
    m_radar->updateScreen();
}

void CompetenceCell::selectionChanged() {
    //Get selection
    QList<QListWidgetItem*> selection = ui->list_comps->selectedItems();

    if(selection.length() > 0) {
        int row = ui->list_comps->row(selection[0]);
        ui->edit_comp->setText(m_domain->competences().at(row)->name());
    }
}

void CompetenceCell::addComp() {
    //Get name of competence
    QString name = ui->edit_comp->text();

    if(name == "") {
        QMessageBox::critical(this, "Erreur", "Veuillez renseigner un nom de compétence.");
        return;
    }

    //Create competence locally
    Competence *c = new Competence();
    c->setId_domains(m_domain->id());
    c->setName(name);
    m_domain->add_competence(c);

    //Add to queue
    m_queue->enqueue(new CompetenceAction(ActionType::Add, c));

    //Update visuals
    new QListWidgetItem(c->name(), ui->list_comps);
    m_radar->updateScreen();

    ui->edit_comp->clear();
}

void CompetenceCell::deleteComp() {
    //Get selection
    QList<QListWidgetItem*> selection = ui->list_comps->selectedItems();

    if(selection.length() <= 0) {
        QMessageBox::critical(this, "Erreur", "Veuillez sélectionner une compétence.");
        return;
    }

    //Get competence
    int index = ui->list_comps->row(selection[0]);
    Competence *c = m_domain->take_competence(index);

    //Add action to queue
    m_queue->enqueue(new CompetenceAction(ActionType::Delete, c));

    //and delete visually
    delete selection[0];
    m_radar->updateScreen();
}
