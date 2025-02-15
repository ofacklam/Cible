#ifndef PAGE_H
#define PAGE_H

#include <QString>
#include "storedData/domain.h"

class Page
{
public:
    Page();
    int id();
    QString name();
    int id_students();
    void setId(int id);
    void setName(QString name);
    void setId_students(int id_students);
    QList<Domain*> domains(); //Extra
    void add_domain(Domain *d); //Extra
    Domain *take_domain(int pos); //Extra
    void clear_domains(); //Extra

private:
    int m_id;
    QString m_name;
    int m_id_students;
    QList<Domain*> m_domains; //Extra
};

#endif // PAGE_H
