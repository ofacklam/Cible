#include "customlabel.h"

CustomLabel::CustomLabel(int id_comp)
{
    m_id_comp = id_comp;
}

void CustomLabel::mouseReleaseEvent(QMouseEvent *event) {
    emit clicked(this);
    return QLabel::mouseReleaseEvent(event);
}

int CustomLabel::id_comp() {
    return m_id_comp;
}
