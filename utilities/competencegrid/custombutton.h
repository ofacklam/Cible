#ifndef CUSTOMBUTTON_H
#define CUSTOMBUTTON_H

#include <QRadioButton>
#include <QTextDocument>
#include <QResizeEvent>
#include <QPainter>

class CustomButton : public QRadioButton
{
public:
    CustomButton(QString text, int value, int id_comp);
    int value();
    int id_comp();

private:
    int m_value;
    int m_id_comp;
};

#endif // CUSTOMBUTTON_H
