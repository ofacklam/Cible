#ifndef SIMPLEWORD_H
#define SIMPLEWORD_H

#include <QDialog>
#include <QTextCharFormat>

namespace Ui {
class SimpleWord;
}

class SimpleWord : public QDialog
{
    Q_OBJECT

public:
    explicit SimpleWord(QWidget *parent = 0);
    ~SimpleWord();
    QString text();
    void setText(QString text);

public slots:
    void toggleBold(bool checked);
    void toggleItalic(bool checked);
    void toggleUnderline(bool checked);
    void undoAvailable(bool a);
    void redoAvailable(bool a);
    void formatChanged(QTextCharFormat f);

private:
    Ui::SimpleWord *ui;
    QString m_text;
};

#endif // SIMPLEWORD_H
