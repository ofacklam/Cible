#include "pdfcreator.h"

PDFCreator::PDFCreator()
{

}

bool PDFCreator::printStudent(Radar *radar, QString filename, Student *s, QString group_name) {
    if(radar == NULL)
        return false;

    bool success = true;
    //Create the PDF Writer
    QPdfWriter writer(filename);
    writer.setPageSize(QPdfWriter::A4);
    writer.setPageOrientation(QPageLayout::Portrait);
    writer.setPageMargins(QMarginsF(20, 20, 20, 20), QPageLayout::Millimeter);
    writer.setCreator("Créateur de cibles");

    QPainter painter;
    if(!painter.begin(&writer)) {
        QMessageBox::critical(NULL, "Erreur", "Erreur lors de l'écriture du fichier " + filename + ". Le fichier est peut-être ouvert ?");
        return false;
    }

    QList<QColor> colors;
    colors << QColor(255, 100, 100) << QColor(255, 210, 110) << QColor(255, 250, 100) << QColor(130, 255, 150) << QColor(0, 0, 0);

    // The logo
    float cell = writer.width() / 6;
    QImage AA(":/images/AA.jpg");
    float image_height = AA.height() * cell / AA.width();
    painter.drawImage(QRectF(0, 0, cell, image_height), AA);

    //The school name
    QFont f = painter.font();
    f.setPointSize(12);
    painter.setFont(f);
    QFontMetrics fm(f);
    painter.drawText(0, image_height + fm.lineSpacing(), SCHOOLNAME);

    f = painter.font();
    f.setPointSize(10);
    painter.setFont(f);
    painter.drawText(0, image_height + 2*fm.lineSpacing(), SCHOOLADRESS1);
    painter.drawText(0, image_height + 3*fm.lineSpacing(), SCHOOLADRESS2);

    //Student name
    QString student_name = s->name() + " " + s->first_name();
    f = painter.font();
    f.setPointSize(20);
    while(QFontMetrics(f).width(student_name) > 4*cell) {
        f.setPointSize(f.pointSize() - 1);
    }
    painter.setFont(f);
    painter.drawText(QRectF(cell, 0, 4*cell, image_height), Qt::AlignCenter, student_name);

    //Group name
    group_name = "Classe : " + group_name;
    f.setPointSize(14);
    while(QFontMetrics(f).width(group_name) > 2*cell) {
        f.setPointSize(f.pointSize() - 1);

    }
    painter.setFont(f);
    painter.drawText(QRectF(4*cell, 0, 2*cell, image_height), Qt::AlignRight | Qt::AlignTop, group_name);

    //Radar
    int w = writer.width();
    float y = (writer.height() + image_height - w) / 2;
    radar->render(&painter, QRectF(0, y, w, w));

    //Legend
    QStringList legend;
    legend << "Maîtrise insuffisante" << "Maîtrise fragile " << "Maîtrise satisfaisante" << "Très bonne maîtrise";

    y += w;
    f = painter.font();
    f.setPointSize(10);
    painter.setFont(f);
    fm = QFontMetrics(f);
    painter.drawText(0, y + 2*fm.lineSpacing(), "Légende : ");
    int x = 1.1 * fm.width("Légende : ");

    for(int i = 0; i < legend.length(); i++) {
    painter.setBrush(QBrush(colors[i]));
    painter.drawRect(x, y + (1.5*i+1)*fm.lineSpacing() + fm.descent(), x/2, fm.lineSpacing());
    }

    x *= 1.6;
    painter.setBrush(Qt::black);
    for(int i = 0; i < legend.length(); i++)
        painter.drawText(x, y + (1.5*i+2)*fm.lineSpacing(), legend[i]);


    //Evaluation grid
    QList<Domain*> domains = radar->domains();

    f.setPointSize(10);
    painter.setFont(f);
    QFontMetrics m(f);

    float domain_width, comp_width;
    max_width(domains, m, domain_width, comp_width);
    domain_width = std::min((float) writer.width() / 5, domain_width);
    comp_width = std::min((float) writer.width() / 5, comp_width);

    float eval_width = writer.width() / SQUARE;
    float offset;


    //Header
    writer.setPageMargins(QMarginsF(10, 20, 10, 20), QPageLayout::Millimeter);
    new_page(&writer, &painter, offset, domain_width, comp_width, eval_width);

    //Competences
    for(int k = 0; k < domains.length(); k++) {
        ErrorType res = draw_domain(&writer, &painter, domains[k], offset, domain_width, comp_width, eval_width, colors);
        if(res == NewPageError) {
            end_page(&writer, &painter, offset, domain_width, comp_width, eval_width);
            new_page(&writer, &painter, offset, domain_width, comp_width, eval_width);

            if(draw_domain(&writer, &painter, domains[k], offset, domain_width, comp_width, eval_width, colors) != NoError)
                success = false;
        }
        if(res == FatalError)
            success = false;
    }

    //Draw vertical lines
    end_page(&writer, &painter, offset, domain_width, comp_width, eval_width);

    //Footer
    if(!draw_footer(&writer, &painter, s, offset, domain_width + comp_width)) {
        writer.newPage();
        offset = 0;
        painter.drawLine(0, offset, writer.width(), offset);
        if(!draw_footer(&writer, &painter, s, offset, domain_width + comp_width))
            success = false;
    }

    painter.end();
    return success;
}

void PDFCreator::max_width(QList<Domain *> domains, QFontMetrics m, float &domain_width, float &comp_width) {
    //Don't forget to include "Domaines" and "Compétences" and "Appréciation générale :"
    domain_width = 0;
    comp_width = 0;

    for(Domain *d : domains) {
        domain_width = std::max(domain_width, (float) m.width(d->name()));

        for(Competence *c : d->competences()) {
            comp_width = std::max(comp_width, (float) m.width(c->name()));
        }
    }

    domain_width = std::max(domain_width, (float) m.width("Domaines"));
    comp_width = std::max(comp_width, std::max((float) m.width("Compétences"), m.width("Appréciation générale :") - domain_width));

    domain_width *= 1.05;
    comp_width *= 1.05;
}

void PDFCreator::new_page(QPdfWriter *w, QPainter *p, float &offset, float domain_width, float comp_width, float eval_width) {
    w->newPage();
    offset = 0;

    QFontMetrics m(p->font());
    p->drawText(0, 0, "MI : maîtrise insuffisante ; "
                      "MF : maîtrise fragile ; "
                      "MS : maîtrise satisfaisante ; "
                      "TBM : très bonne maîtrise ; "
                      "NE : non évalué");
    offset += m.lineSpacing();

    //HLine
    p->drawLine(0, offset, w->width(), offset);

    //Text
    p->drawText(QRectF(0, offset, domain_width, m.lineSpacing()), Qt::AlignCenter, "Domaines");
    p->drawText(QRectF(domain_width, offset, comp_width, m.lineSpacing()), Qt::AlignCenter, "Compétences");
    QStringList caption;
    caption << "MI" << "MF" << "MS" << "TBM" << "NE";
    for(int k = 0; k < caption.length(); k++)
        p->drawText(QRectF(domain_width + comp_width + k*eval_width, offset, eval_width, m.lineSpacing()), Qt::AlignCenter, caption[k]);
    float x = domain_width + comp_width + (CIRCLES_COUNT+1) * eval_width;
    p->drawText(QRectF(x, offset, w->width() - x, m.lineSpacing()), Qt::AlignCenter, "Appréciations / Objectifs");
    offset += m.lineSpacing();

    //HLine
    p->drawLine(0, offset, w->width(), offset);
}

void PDFCreator::end_page(QPdfWriter *w, QPainter *p, float &offset, float domain_width, float comp_width, float eval_width) {
    QFontMetrics m(p->font());
    p->drawLine(0, m.lineSpacing(), 0, offset);
    p->drawLine(domain_width, m.lineSpacing(), domain_width, offset);
    for(int k = 0; k <= CIRCLES_COUNT + 1; k++)
        p->drawLine(domain_width + comp_width + k * eval_width, m.lineSpacing(), domain_width + comp_width + k * eval_width, offset);
    p->drawLine(w->width(), m.lineSpacing(), w->width(), offset);
}

ErrorType PDFCreator::draw_domain(QPdfWriter *w, QPainter *p, Domain *d, float &offset, float domain_width, float comp_width, float eval_width, QList<QColor> colors) {
    QRectF necessary = p->boundingRect(0, offset, domain_width, w->height(), Qt::TextWordWrap, d->name());
    if(necessary.bottom() > w->height())
        return NewPageError;

    ErrorType err = NoError;
    float old_offset = offset;

    //Cycle through competences
    QList<Competence*> comps = d->competences();
    for(int k = 0; k < comps.length(); k++) {
        if(!draw_comp(w, p, comps[k], offset, domain_width, comp_width, eval_width, colors)) {
            if(offset != old_offset) {
                offset = std::max(old_offset + necessary.height(), (qreal) offset);
                p->drawText(QRectF(0, old_offset, domain_width, offset-old_offset), Qt::AlignCenter | Qt::TextWordWrap, d->name());
                p->drawLine(0, offset, w->width(), offset);
            }
            end_page(w, p, offset, domain_width, comp_width, eval_width);
            new_page(w, p, offset, domain_width, comp_width, eval_width);
            old_offset = offset;

            if(!draw_comp(w, p, comps[k], offset, domain_width, comp_width, eval_width, colors))
                err = FatalError;
        }
    }

    //Name
    offset = std::max(old_offset + necessary.height(), (qreal) offset);
    p->drawText(QRectF(0, old_offset, domain_width, offset-old_offset), Qt::AlignCenter | Qt::TextWordWrap, d->name());

    //HLine
    p->drawLine(0, offset, w->width(), offset);

    return err;
}

bool PDFCreator::draw_comp(QPdfWriter *w, QPainter *p, Competence *c, float &offset, float domain_width, float comp_width, float eval_width, QList<QColor> colors) {
    float x = domain_width + comp_width + (CIRCLES_COUNT+1) * eval_width;

    QTextDocument td;
    td.documentLayout()->setPaintDevice(w);
    QFont f = td.defaultFont();
    f.setPointSize(10);
    td.setDefaultFont(f);
    td.setTextWidth(w->width() - x);
    td.setHtml(c->comments());

    QRectF necessary_name = p->boundingRect(domain_width, offset, comp_width, w->height(), Qt::TextWordWrap, c->name());
    float min_height = std::max(td.size().height(), necessary_name.height());

    if(offset + min_height > w->height())
        return false;

    //Comp name
    p->drawText(domain_width, offset, comp_width, min_height, Qt::AlignCenter | Qt::TextWordWrap, c->name());

    //The value circle
    float radius = w->width() / CIRCLE;
    int value = c->value();
    if(value == 0)
        value = CIRCLES_COUNT + 1;
    p->setBrush(QBrush(colors[value-1]));
    p->drawEllipse(QPointF(domain_width + comp_width + (value - 0.5) * eval_width, offset + min_height / 2), radius, radius);
    p->setBrush(Qt::black);

    //Comp comments
    p->save();
    p->translate(x, offset);
    td.drawContents(p, QRectF(0, 0, w->width()-x, min_height));
    p->restore();

    //HLine
    offset += min_height;
    p->drawLine(domain_width, offset, w->width(), offset);

    return true;
}

bool PDFCreator::draw_footer(QPdfWriter *w, QPainter *p, Student *s, float &offset, float desc_width) {
    QTextDocument td;
    td.documentLayout()->setPaintDevice(w);
    QFont f = td.defaultFont();
    f.setPointSize(10);
    td.setDefaultFont(f);
    td.setTextWidth(w->width()-desc_width);
    td.setHtml(s->comments());

    QRectF necessary_name = p->boundingRect(0, offset, desc_width, w->height(), Qt::TextWordWrap, "Appréciation générale :");
    float min_height = std::max(td.size().height(), necessary_name.height());

    if(offset + min_height > w->height())
        return false;

    //Draw Text
    p->drawText(0, offset, desc_width, min_height, Qt::AlignCenter | Qt::TextWordWrap, "Appréciation générale :");
    p->save();
    p->translate(desc_width, offset);
    td.drawContents(p, QRectF(0, 0, w->width() - desc_width, min_height));
    p->restore();

    //HLine
    float old_offset = offset;
    offset += min_height;
    p->drawLine(0, offset, w->width(), offset);

    //VLines
    p->drawLine(0, old_offset, 0, offset);
    p->drawLine(desc_width, old_offset, desc_width, offset);
    p->drawLine(w->width(), old_offset, w->width(), offset);

    return true;
}
