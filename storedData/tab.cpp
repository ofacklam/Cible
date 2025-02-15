#include "tab.h"

Tab::Tab()
{
    m_id = 0;
    m_id_pages = 0;
    m_color = Qt::blue;
}

int Tab::id() {
    return m_id;
}

QString Tab::name() {
    return m_name;
}

int Tab::id_pages() {
    return m_id_pages;
}

QString Tab::comments() {
    return m_comments;
}

void Tab::setId(int id) {
    m_id = id;
}

void Tab::setName(QString name) {
    m_name = name;
}

void Tab::setId_pages(int id_pages) {
    m_id_pages = id_pages;
}

void Tab::setComments(QString comments) {
    m_comments = comments;
}

void Tab::insertValue(int id_competence, int value) {
    m_values.insert(id_competence, value);
}

int Tab::value(int id_competence) {
    return m_values.value(id_competence, 0);
}

void Tab::insertComment(int id_competence, QString comment) {
    m_values_comments.insert(id_competence, comment);
}

QString Tab::comment(int id_competence) {
    return m_values_comments.value(id_competence, "");
}

void Tab::clearValues() {
    m_values.clear();
    m_values_comments.clear();
}

QColor Tab::color() {
    return m_color;
}

void Tab::setColor(QColor c) {
    m_color = c;
}
