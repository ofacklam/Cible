#ifndef GROUP_H
#define GROUP_H

#include <QString>

class Group
{
public:
    Group();
    int id();
    QString name();
    void setId(int id);
    void setName(QString name);

private:
    int m_id;
    QString m_name;
};

#endif // GROUP_H
