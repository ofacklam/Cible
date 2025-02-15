#include "competencegrid.h"
#include "ui_competencegrid.h"

CompetenceGrid::CompetenceGrid(Student *s, Page *p, Tab *t, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CompetenceGrid)
{
    ui->setupUi(this);

    if(s == NULL || p == NULL || t == NULL)
        return;

    m_tab = t;
    m_page = p;
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
    int offset = 2;
    int domainrow = offset + 3;
    for(int i = 0; i < p.length(); i++) {
        Domain *d = p[i];
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
                CustomButton *b = new CustomButton(QString::number(k), k, c->id());
                group->addButton(b, k);
                ui->gridLayout->addWidget(b, domainrow, 5 + (k-1), Qt::AlignHCenter);
            }
            CustomButton *special = new CustomButton("NE", 0, c->id());
            group->addButton(special, 0);
            ui->gridLayout->addWidget(special, domainrow, CIRCLES_COUNT + 5, Qt::AlignHCenter);

            QAbstractButton *toCheck = group->button(t->value(c->id()));
            if(toCheck != NULL)
                toCheck->click();

            connect(group, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(buttonClicked(QAbstractButton*)));

            //Add comments
            CustomLabel *lbl_comment = new CustomLabel(c->id());
            lbl_comment->setText(t->comment(c->id()));
            connect(lbl_comment, SIGNAL(clicked(CustomLabel*)), this, SLOT(textClicked(CustomLabel*)));
            ui->gridLayout->addWidget(lbl_comment, domainrow, 6 + CIRCLES_COUNT);

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
    CustomLabel *customlabel = new CustomLabel(-1);
    customlabel->setText(m_tab->comments());
    connect(customlabel, SIGNAL(clicked(CustomLabel*)), this, SLOT(tabCommentClicked(CustomLabel*)));
    ui->gridLayout->addWidget(customlabel, domainrow, 5, 1, CIRCLES_COUNT + 2);
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

void CompetenceGrid::buttonClicked(QAbstractButton *btn) {
    CustomButton *c_btn = (CustomButton *) btn;
    int value = c_btn->value();
    int id_comp = c_btn->id_comp();

    //Update value in local variables
    m_tab->insertValue(id_comp, value);

    //Update value in DB
    QSqlDatabase db = QSqlDatabase::database();
    if(!db.isOpen()) {
        QMessageBox::critical(this, "Erreur", "La connexion à la base de données a échoué.");
        return;
    }

    QSqlQuery q(db);
    q.prepare("UPDATE values SET `value`=:value WHERE id_competences=:id_competences AND id_tabs=:id_tabs");
    q.bindValue(":id_competences", id_comp);
    q.bindValue(":id_tabs", m_tab->id());
    q.bindValue(":value", value);
    q.exec();

    //Update visuals
    emit valuesUpdated();
}

void CompetenceGrid::textClicked(CustomLabel *lbl) {
    int id_comp = lbl->id_comp();

    SimpleWord editor;
    editor.setText(m_tab->comment(id_comp));
    if(editor.exec() == QDialog::Accepted) {
        QString text = editor.text();

        //Update locally
        m_tab->insertComment(id_comp, text);

        //Update in DB
        QSqlDatabase db = QSqlDatabase::database();
        if(!db.isOpen()) {
            QMessageBox::critical(this, "Erreur", "La connexion à la base de données a échoué.");
            return;
        }

        QSqlQuery q(db);
        q.prepare("UPDATE values SET comments=:text WHERE id_competences=:id_competences AND id_tabs=:id_tabs");
        q.bindValue(":id_competences", id_comp);
        q.bindValue(":id_tabs", m_tab->id());
        q.bindValue(":text", text);
        q.exec();

        //Update visuals
        lbl->setText(text);
    }
}

void CompetenceGrid::tabCommentClicked(CustomLabel *lbl) {
    SimpleWord editor;
    editor.setText(m_tab->comments());
    if(editor.exec() == QDialog::Accepted) {
        QString text = editor.text();

        //Update locally
        m_tab->setComments(text);

        //Update in DB
        QSqlDatabase db = QSqlDatabase::database();
        if(!db.isOpen()) {
            QMessageBox::critical(this, "Erreur", "La connexion à la base de données a échoué.");
            return;
        }

        QSqlQuery q(db);
        q.prepare("UPDATE tabs SET comments=:text WHERE id=:id");
        q.bindValue(":id", m_tab->id());
        q.bindValue(":text", m_tab->comments());
        q.exec();

        //Update visuals
        lbl->setText(m_tab->comments());
    }
}
