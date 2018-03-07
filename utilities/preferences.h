#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QString>
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QTextStream>

class Preferences
{
public:
    Preferences();
    void read();
    void write();
    QString file();
    void setFile(QString file);
    QString dir();

private:
    QString m_file;
};

#endif // PREFERENCES_H
