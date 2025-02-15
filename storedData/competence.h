#ifndef COMPETENCE_H
#define COMPETENCE_H

#include <QString>

class Competence
{
public:
    Competence();
    int id();
    int id_domains();
    QString name();
    //int value();
    //QString comments();
    void setId(int id);
    void setId_domains(int id_domains);
    void setName(QString name);
    //void setValue(int value);
    //void setComments(QString comments);

private:
    int m_id;
    int m_id_domains;
    QString m_name;
    int m_value;
    QString m_comments;
};

#endif // COMPETENCE_H
