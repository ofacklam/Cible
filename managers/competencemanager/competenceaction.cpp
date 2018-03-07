#include "competenceaction.h"

CompetenceAction::CompetenceAction(ActionType type, Competence *c)
{
    m_actiontype = type;
    m_comp = c;
}

ActionType CompetenceAction::type() {
    return m_actiontype;
}

Competence *CompetenceAction::competence() {
    return m_comp;
}
