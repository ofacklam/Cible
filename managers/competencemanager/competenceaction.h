#ifndef COMPETENCEACTION_H
#define COMPETENCEACTION_H

#include "storedData/competence.h"

enum ActionType {Add, Delete};

class CompetenceAction
{
public:
    CompetenceAction(ActionType type, Competence *c);
    ActionType type();
    Competence *competence();

private:
    ActionType m_actiontype;
    Competence *m_comp;
};

#endif // COMPETENCEACTION_H
