#include "page.h"

Page::Page()
{
    m_id = 0;
    m_id_students = 0;
}

int Page::id() {
    return m_id;
}

QString Page::name() {
    return m_name;
}

int Page::id_students() {
    return m_id_students;
}

void Page::setId(int id) {
    m_id = id;
}

void Page::setName(QString name) {
    m_name = name;
}

void Page::setId_students(int id_students) {
    m_id_students = id_students;
}

QList<Domain*> Page::domains() {
    return m_domains;
}

void Page::add_domain(Domain *d) {
    m_domains.append(d);
}

Domain *Page::take_domain(int pos) {
    return m_domains.takeAt(pos);
}

void Page::clear_domains() {
    m_domains.clear();
}
