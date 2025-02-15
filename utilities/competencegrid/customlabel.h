#ifndef CUSTOMLABEL_H
#define CUSTOMLABEL_H

#include <QLabel>

class CustomLabel : public QLabel
{
public:
    CustomLabel(int id_comp);
    int id_comp();

protected:
    void mouseReleaseEvent(QMouseEvent *event);

signals:
    void clicked(CustomLabel*);

private:
    int m_id_comp;
};

#endif // CUSTOMLABEL_H
