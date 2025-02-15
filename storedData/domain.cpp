#include "domain.h"

Domain::Domain()
{
    m_id = 0;
    m_id_pages = 0;
}

int Domain::id() {
    return m_id;
}

int Domain::id_pages() {
    return m_id_pages;
}

QString Domain::name() {
    return m_name;
}

void Domain::setId(int id) {
    m_id = id;
}

void Domain::setId_pages(int id_pages) {
    m_id_pages = id_pages;
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
