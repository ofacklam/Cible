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
    int part();
    int id_students();
    QString name();
    void setId(int id);
    void setPart(int part);
    void setId_students(int id_students);
    void setName(QString name);
    QList<Competence*> competences(); //Extra
    void add_competence(Competence *c); //Extra
    Competence *take_competence(int pos); //Extra
    void clear_competences(); //Extra

private:
    int m_id;
    int m_part;
    int m_id_students;
    QString m_name;
    QList<Competence*> m_competences; //Extra
};

#endif // DOMAIN_H
