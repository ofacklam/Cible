#include "student.h"

Student::Student()
{
    m_id = 0;
    m_id_groups = 0;
}

int Student::id() {
    return m_id;
}

int Student::id_groups() {
    return m_id_groups;
}

QString Student::name() {
    return m_name;
}

QString Student::first_name() {
    return m_first_name;
}

QString Student::comments() {
    return m_comments;
}

void Student::setId(int id) {
    m_id = id;
}

void Student::setId_groups(int id_groups) {
    m_id_groups = id_groups;
}

void Student::setName(QString name) {
    m_name = name;
}

void Student::setFirst_name(QString first_name) {
    m_first_name = first_name;
}

void Student::setComments(QString comments) {
    m_comments = comments;
}
