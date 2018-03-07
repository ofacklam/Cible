#include "competence.h"

Competence::Competence()
{
    m_value = 0;
    m_id = 0;
    m_id_domains = 0;
}

int Competence::id() {
    return m_id;
}

int Competence::id_domains() {
    return m_id_domains;
}

QString Competence::name() {
    return m_name;
}

int Competence::value() {
    return m_value;
}

QString Competence::comments() {
    return m_comments;
}

void Competence::setId(int id) {
    m_id = id;
}

void Competence::setId_domains(int id_domains) {
    m_id_domains = id_domains;
}

void Competence::setName(QString name) {
    m_name = name;
}

void Competence::setValue(int value) {
    m_value = value;
}

void Competence::setComments(QString comments) {
    m_comments = comments;
}
