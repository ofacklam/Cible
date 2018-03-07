#ifndef CUSTOMBUTTON_H
#define CUSTOMBUTTON_H

#include <QToolButton>
#include <QTextDocument>
#include <QResizeEvent>
#include <QPainter>

class CustomButton : public QToolButton
{
public:
    CustomButton();
    void setHtml(QString html);
    void updateHtml();
    void resizeEvent(QResizeEvent *e);
    QSize sizeHint();

private:
    QString m_html;
};

#endif // CUSTOMBUTTON_H
