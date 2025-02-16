#ifndef STUDENT_H
#define STUDENT_H

#include <QString>

class Student
{
public:
    Student();
    int id();
    int id_groups();
    QString name();
    QString first_name();
    void setId(int id);
    void setId_groups(int id_groups);
    void setName(QString name);
    void setFirst_name(QString first_name);

private:
    int m_id;
    int m_id_groups;
    QString m_name;
    QString m_first_name;
};

#endif // STUDENT_H
