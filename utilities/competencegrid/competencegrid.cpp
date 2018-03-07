#include "competencegrid.h"
#include "ui_competencegrid.h"

CompetenceGrid::CompetenceGrid(Student *s, QList<Domain *> domains, Radar *radar, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CompetenceGrid)
{
    ui->setupUi(this);

    if(s == NULL)
        return;

    if(domains.length() != DOMAIN_COUNT) {
        QMessageBox::critical(NULL, "Erreur fatale", "Mauvais nombre de domaines.");
        return;
    }

    m_radar = radar;
    m_domains = domains;
    m_student = s;

    //Student name
    QLabel *student_lbl = new QLabel();
    student_lbl->setText(s->name() + " " + s->first_name());
    QFont f = student_lbl->font();
    f.setPointSize(16);
    student_lbl->setFont(f);
    ui->gridLayout->addWidget(student_lbl, 0, 0, 1, CIRCLES_COUNT + 8, Qt::AlignHCenter);

    //Explanation text
    QLabel *expl_label = new QLabel();
    expl_label->setText("MI : maîtrise insuffisante; "
                        "MF : maîtrise fragile ; "
                        "MS : maîtrise satisfaisante; "
                        "TBM : très bonne maîtrise; "
                        "NE : non évalué");
    f = expl_label->font();
    f.setPointSize(8);
    expl_label->setFont(f);
    ui->gridLayout->addWidget(expl_label, 1, 0, 1, CIRCLES_COUNT + 8);

    //Explanation bar
    addHSep(2, 0, CIRCLES_COUNT + 8);
    int explanation_row = 3;
    ui->gridLayout->addWidget(new QLabel("Domaines"), explanation_row, 1);
    ui->gridLayout->addWidget(new QLabel("Compétences"), explanation_row, 3);
    ui->gridLayout->addWidget(new QLabel("MI"), explanation_row, 5, Qt::AlignHCenter);
    ui->gridLayout->addWidget(new QLabel("MF"), explanation_row, 6, Qt::AlignHCenter);
    ui->gridLayout->addWidget(new QLabel("MS"), explanation_row, 7, Qt::AlignHCenter);
    ui->gridLayout->addWidget(new QLabel("TBM"), explanation_row, 8, Qt::AlignHCenter);
    ui->gridLayout->addWidget(new QLabel("NE"), explanation_row, 9, Qt::AlignHCenter);
    addHSep(4, 0, CIRCLES_COUNT + 8);

    //For each domain
    QButtonGroup *comment_group = new QButtonGroup(this);
    connect(comment_group, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(textClicked(QAbstractButton*)));

    int offset = 2;
    int domainrow = offset + 3;
    for(int i = 0; i < domains.length(); i++) {
        Domain *d = domains[i];
        QList<Competence *> comps = d->competences();
        //Name of domain
        ui->gridLayout->addWidget(new QLabel(d->name()), domainrow, 1, std::max(1, 2 * comps.length()), 1, Qt::AlignVCenter);

        //For each competence
        for(int j = 0; j < comps.length(); j++, domainrow += 2) {
            Competence *c = comps[j];
            //Add name
            ui->gridLayout->addWidget(new QLabel(c->name()), domainrow, 3, 2, 1, Qt::AlignVCenter);

            //Add RadioButtons
            QButtonGroup *group = new QButtonGroup(this);

            for(int k = 1; k <= CIRCLES_COUNT; k++) {
                QRadioButton *b = new QRadioButton(QString::number(k));
                group->addButton(b, j * 100 + i * 10 + k);
                ui->gridLayout->addWidget(b, domainrow, 5 + (k-1), Qt::AlignHCenter);
            }
            QRadioButton *special = new QRadioButton("NE");
            group->addButton(special, j * 100 + i * 10);
            ui->gridLayout->addWidget(special, domainrow, CIRCLES_COUNT + 5, Qt::AlignHCenter);

            QAbstractButton *toCheck = group->button(j *100 + i * 10 + c->value());
            if(toCheck != NULL)
                toCheck->click();

            connect(group, SIGNAL(buttonClicked(int)), this, SLOT(buttonClicked(int)));

            //Add comments
            CustomButton *btn_comment = new CustomButton();
            btn_comment->setHtml(c->comments());
            comment_group->addButton(btn_comment, j * 10 + i);
            ui->gridLayout->addWidget(btn_comment, domainrow, 6 + CIRCLES_COUNT);

            addHSep(domainrow + 1, 2, CIRCLES_COUNT + 6);
        }

        if(comps.length() > 0) {
            addHSep(domainrow - 1, 0, CIRCLES_COUNT + 8);
        }
        else {
            addHSep(domainrow + 1, 0, CIRCLES_COUNT + 8);
            domainrow += 2;
        }

    }

    //General comment box
    ui->gridLayout->addWidget(new QLabel("Appréciation générale : "), domainrow, 1, 1, 3);
    CustomButton *custombutton = new CustomButton();
    custombutton->setHtml(m_student->comments());
    QButtonGroup *group = new QButtonGroup(this);
    group->addButton(custombutton);
    connect(group, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(studentCommentClicked(QAbstractButton*)));
    ui->gridLayout->addWidget(custombutton, domainrow, 5, 1, CIRCLES_COUNT + 2);
    addHSep(domainrow + 1, 0, CIRCLES_COUNT + 8);
    domainrow += 2;

    addVSep(offset, 0, domainrow - offset);
    addVSep(offset, 2, domainrow - offset - 2);
    addVSep(offset, 4, domainrow - offset);
    addVSep(offset, CIRCLES_COUNT + 7, domainrow - offset);
}

CompetenceGrid::~CompetenceGrid()
{
    delete ui;

    //DONT DELETE DOMAINS AND COMPETENCES
}

void CompetenceGrid::addHSep(int row, int column, int span) {
    QFrame *f = new QFrame();
    f->setLineWidth(1);
    f->setFrameStyle(QFrame::HLine);
    ui->gridLayout->addWidget(f, row, column, 1, span);
}

void CompetenceGrid::addVSep(int row, int column, int span) {
    QFrame *f = new QFrame();
    f->setLineWidth(1);
    f->setFrameStyle(QFrame::VLine);
    ui->gridLayout->addWidget(f, row, column, span, 1);
}

void CompetenceGrid::buttonClicked(int id) {
    int value = id % 10;
    int index_dom = (id / 10) % 10;
    int index_comp = (id / 100);

    //Update value in local variables
    QList<Competence*> comps = m_domains[index_dom]->competences();
    Competence *c = comps[index_comp];
    c->setValue(value);

    //Update value in DB
    QSqlDatabase db = QSqlDatabase::database();
    if(!db.isOpen()) {
        QMessageBox::critical(this, "Erreur", "La connexion à la base de données a échoué.");
        return;
    }

    QSqlQuery q(db);
    q.prepare("UPDATE competences SET value=:value WHERE id=:id");
    q.bindValue(":id", c->id());
    q.bindValue(":value", c->value());
    q.exec();

    //Update visuals
    m_radar->updateScreen();
}

void CompetenceGrid::textClicked(QAbstractButton *btn) {
    int id = btn->group()->id(btn);
    int index_dom = id % 10;
    int index_comp = id / 10;

    Competence *c = m_domains[index_dom]->competences().at(index_comp);

    SimpleWord editor;
    editor.setText(c->comments());
    if(editor.exec() == QDialog::Accepted) {
        QString text = editor.text();

        //Update locally
        c->setComments(text);

        //Update in DB
        QSqlDatabase db = QSqlDatabase::database();
        if(!db.isOpen()) {
            QMessageBox::critical(this, "Erreur", "La connexion à la base de données a échoué.");
            return;
        }

        QSqlQuery q(db);
        q.prepare("UPDATE competences SET comments=:text WHERE id=:id");
        q.bindValue(":id", c->id());
        q.bindValue(":text", c->comments());
        q.exec();

        //Update visuals
        ((CustomButton*) btn)->setHtml(c->comments());
    }
}

void CompetenceGrid::studentCommentClicked(QAbstractButton *btn) {
    SimpleWord editor;
    editor.setText(m_student->comments());
    if(editor.exec() == QDialog::Accepted) {
        QString text = editor.text();

        //Update locally
        m_student->setComments(text);

        //Update in DB
        QSqlDatabase db = QSqlDatabase::database();
        if(!db.isOpen()) {
            QMessageBox::critical(this, "Erreur", "La connexion à la base de données a échoué.");
            return;
        }

        QSqlQuery q(db);
        q.prepare("UPDATE students SET comments=:text WHERE id=:id");
        q.bindValue(":id", m_student->id());
        q.bindValue(":text", m_student->comments());
        q.exec();

        //Update visuals
        ((CustomButton*) btn)->setHtml(m_student->comments());
    }
}
