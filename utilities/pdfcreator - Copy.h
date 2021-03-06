#ifndef PDFCREATOR_H
#define PDFCREATOR_H

#include <QString>
#include <QPdfWriter>
#include <QMessageBox>
#include <QPainter>
#include <QGraphicsView>
#include "storedData/competence.h"
#include "storedData/domain.h"
#include "utilities/radar.h"

#define CIRCLE 100
#define SQUARE 25

class PDFCreator
{
public:
    PDFCreator();
    static bool printStudent(Radar *radar, QString filename, Student *s, QString group_name);
    static void max_width(QList<Domain*> domains, QFontMetrics m, float& domain_width, float& comp_width);
    static void new_page(QPdfWriter *w, QPainter *p, float& offset, float domain_width, float comp_width, float eval_width);
    static void end_page(QPdfWriter *w, QPainter *p, float& offset, float domain_width, float comp_width, float eval_width);
    static bool draw_domain(QPdfWriter *w, QPainter *p, Domain *d, float& offset, float domain_width, float comp_width, float eval_width);
    static bool draw_comp(QPdfWriter *w, QPainter *p, Competence *c, float& offset, float domain_width, float comp_width, float eval_width);
    static bool draw_footer(QPdfWriter *w, QPainter *p, Student *s, float& offset, float desc_width);
};

#endif // PDFCREATOR_H
