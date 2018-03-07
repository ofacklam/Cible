#include "group.h"

Group::Group()
{
    m_id = 0;
}

int Group::id() {
    return m_id;
}

QString Group::name() {
    return m_name;
}

void Group::setId(int id) {
    m_id = id;
}

void Group::setName(QString name) {
    m_name = name;
}
