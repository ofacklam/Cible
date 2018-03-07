#ifndef COMPETENCECELL_H
#define COMPETENCECELL_H

#include <QWidget>
#include <QMessageBox>
#include <QQueue>
#include "storedData/competence.h"
#include "storedData/domain.h"
#include "competenceaction.h"
#include "utilities/radar.h"

class Radar;

namespace Ui {
class CompetenceCell;
}

class CompetenceCell : public QWidget
{
    Q_OBJECT

public:
    explicit CompetenceCell(Domain *d, QQueue<CompetenceAction*> *queue, Radar *r, QWidget *parent = 0);
    ~CompetenceCell();

public slots:
    void domainChanged(QString name);
    void selectionChanged();
    void addComp();
    void deleteComp();

private:
    Ui::CompetenceCell *ui;
    Domain *m_domain;
    QQueue<CompetenceAction*> *m_queue;
    Radar *m_radar;
};

#endif // COMPETENCECELL_H
