#include "domain.h"

Domain::Domain()
{
    m_id = 0;
    m_id_students = 0;
    m_part = 0;
}

int Domain::id() {
    return m_id;
}

int Domain::part() {
    return m_part;
}

int Domain::id_students() {
    return m_id_students;
}

QString Domain::name() {
    return m_name;
}

void Domain::setId(int id) {
    m_id = id;
}

void Domain::setPart(int part) {
    m_part = part;
}

void Domain::setId_students(int id_students) {
    m_id_students = id_students;
}

void Domain::setName(QString name) {
    m_name = name;
}

QList<Competence*> Domain::competences() {
    return m_competences;
}

void Domain::add_competence(Competence *c) {
    m_competences.append(c);
}

Competence *Domain::take_competence(int pos) {
    return m_competences.takeAt(pos);
}

void Domain::clear_competences() {
    m_competences.clear();
}
