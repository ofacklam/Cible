#include "preferences.h"

Preferences::Preferences()
{
    m_file = "";
    read();
}

void Preferences::read() {
    QFile read(QCoreApplication::applicationDirPath() + QDir::separator() + "preferences.pref");
    if(read.exists() && read.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&read);
        while(! in.atEnd()) {
            QString data = in.readLine();
            if(data == "File path:") {
                m_file = in.readLine();
            }
        }
    }
}

void Preferences::write() {
    QFile write(QCoreApplication::applicationDirPath() + QDir::separator() + "preferences.pref");
    if(write.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate)){
        QTextStream out(&write);
        out << "File path:\n" << m_file << "\n\n";
    }
}

QString Preferences::file() {
    read();
    return m_file == "" ? "" : QDir::cleanPath(QDir(QCoreApplication::applicationDirPath()).absoluteFilePath(m_file));
}

void Preferences::setFile(QString file) {
    read();
    m_file = file == "" ? "" : QDir(QCoreApplication::applicationDirPath()).relativeFilePath(file);
    write();
}

QString Preferences::dir() {
    read();
    if(m_file != "") {
        QDir dir = QFileInfo(QDir(QCoreApplication::applicationDirPath()), m_file).absoluteDir();
        if(dir.exists())
            return dir.absolutePath();
        else
            return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    }
    else
        return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

}
