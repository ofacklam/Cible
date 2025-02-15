#ifndef DOMAIN_H
#define DOMAIN_H

#include <QString>
#include <QList>
#include "competence.h"

class Domain
{
public:
    Domain();
    int id();
    int id_pages();
    QString name();
    void setId(int id);
    void setId_pages(int id_pages);
    void setName(QString name);
    QList<Competence*> competences(); //Extra
    void add_competence(Competence *c); //Extra
    Competence *take_competence(int pos); //Extra
    void clear_competences(); //Extra

private:
    int m_id;
    int m_id_pages;
    QString m_name;
    QList<Competence*> m_competences; //Extra
};

#endif // DOMAIN_H
