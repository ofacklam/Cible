#include "custombutton.h"

CustomButton::CustomButton(QString text, int value, int id_comp) : QRadioButton(text)
{
    m_value = value;
    m_id_comp = id_comp;
}

int CustomButton::value() {
    return m_value;
}

int CustomButton::id_comp() {
    return m_id_comp;
}
