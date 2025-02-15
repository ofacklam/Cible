#ifndef TAB_H
#define TAB_H

#include <QString>
#include <QMap>
#include <QColor>

class Tab
{
public:
    Tab();
    int id();
    QString name();
    int id_pages();
    QString comments();
    void setId(int id);
    void setName(QString name);
    void setId_pages(int id_pages);
    void setComments(QString comments);
    void insertValue(int id_competence, int value); //Extra
    int value(int id_competence); //Extra
    void insertComment(int id_competence, QString comment); //Extra
    QString comment(int id_competence); //Extra
    void clearValues(); //Extra
    QColor color(); //Extra
    void setColor(QColor c); //Extra

private:
    int m_id;
    QString m_name;
    int m_id_pages;
    QString m_comments;
    QMap<int, int> m_values; //Extra
    QMap<int, QString> m_values_comments; //Extra
    QColor m_color; //Extra
};

#endif // TAB_H
