#include "radar.h"

Radar::Radar(QWidget *win, QGraphicsView *view, QList<Domain*> domains) : QGraphicsScene()
{
    if(domains.length() != DOMAIN_COUNT) {
        QMessageBox::critical(NULL, "Erreur fatale", "Mauvais nombre de domaines.");
        return;
    }

    m_window = win;
    m_view = view;
    m_domains = domains;

    // COLORS TO BE DEFINED
    QList<QColor> colors;
    colors << QColor(250, 250, 190) << QColor(240, 220, 240) << QColor(200, 240, 240) << QColor(255, 205, 205);

    QList<QColor> circles;
    circles << QColor(255, 100, 100) << QColor(255, 210, 110) << QColor(255, 250, 100) << QColor(130, 255, 150);

    m_colors = colors;
    m_circles = circles;

    drawRayons(domains, colors, circles);

    m_window->installEventFilter(this);
}

Radar::~Radar() {
    while(!m_domains.isEmpty()) {
        Domain *d = m_domains.takeFirst();
        QList<Competence *> comp = d->competences();

        while(!comp.isEmpty()) {
            delete comp.takeFirst();
        }
        delete d;
    }
}

void Radar::drawRayons(QList<Domain*> domains, QList<QColor> colors, QList<QColor> circles) {
    /** Draws figure **/

    //Delete what was before
    clear();

    if(domains.length() != DOMAIN_COUNT || colors.length() != DOMAIN_COUNT || circles.length() != CIRCLES_COUNT)
        return;

    //Calculate dimensions
    float L = std::min(m_view->width(), m_view->height()) / 2;
    float r = L * 0.7;
    setSceneRect(-L, -L, 2*L, 2*L);

    //Create pen
    QPen lines(Qt::black, std::max(1.0, (double) THICK * L / 500));

    qreal min_scale = 1000;
    QList<QGraphicsTextItem*> texts;

    /** Zones **/
    for(int i = 0; i < DOMAIN_COUNT; i++) {
        float x = -L * (right(i) ? 0 : 1);
        float y = -L * (top(i) ? 1 : 0);
        float border = BORDER * L / 500.0;
        addRect(x+border/2, y+border/2, L-border, L-border, QPen(QBrush(colors[i]), border));

        //The Text
        QGraphicsTextItem *t = addText(domains[i]->name());
        QFont f = t->font();
        f.setPointSize(10);
        t->setFont(f);
        x = right(i) ? (L - t->boundingRect().width()) : -L;
        y = top(i) ? -L : (L - t->boundingRect().height());
        t->setPos(x, y);
        t->setTransformOriginPoint(t->boundingRect().width() * (right(i) ? 1 : 0),
                                   t->boundingRect().height() * (top(i) ? 0 : 1));
        if(t->boundingRect().width() != 0)
            min_scale = std::min(min_scale, std::min(0.2 * L / t->boundingRect().height(), 0.7 * L / t->boundingRect().width()));
        texts.append(t);
    }

    //Set scale for every title
    for(int k = 0; k < texts.length(); k++)
        texts[k]->setScale(min_scale);

    /** Circles **/
    for(int k = CIRCLES_COUNT - 1; k >= 0; k--) {
        float rad = (k+1) * r / CIRCLES_COUNT;
        addEllipse(-rad, -rad, 2*rad, 2*rad, lines, QBrush(circles[k]));
    }


    QPolygonF poly;
    int comp_count = 0;
    min_scale = 1000;
    texts.clear();

    for(int i = 0; i < DOMAIN_COUNT; i++) {
        QList<Competence*> comp = domains[i]->competences();

        int n = comp.length();
        comp_count += n;

        for(int j = 0; j < n; j++) {
            float x = r*cos(alpha(i, j, n));
            float y = -r*sin(alpha(i, j, n));
            float eps = std::max(1.0, (double) EPSILON * L / 500);

            /** Lines  **/
            addLine(0, 0, x, y, lines);

            /** The Text **/

            QGraphicsTextItem *t = addText(comp[j]->name());
            t->setDefaultTextColor(comp[j]->value() == 0 ? Qt::red : Qt::black);
            QFont f = t->font();
            f.setPointSize(10);
            t->setFont(f);

            //Calculate position
            float x_text = x + 2 * eps * (right(i) ? 1 : -1);
            float y_text = y;
            if(top(i))
                y_text -= t->boundingRect().height();
            if(!right(i))
                x_text -= t->boundingRect().width();
            t->setPos(x_text, y_text);

            //Calculate origin point
            t->setTransformOriginPoint(t->boundingRect().width() * (right(i) ? 0 : 1),
                                       t->boundingRect().height() * (top(i) ? 1 : 0));

            //Calculate scale
            float w = L + x * (right(i) ? -1 : 1);
            w *= 0.7;
            if(t->boundingRect().width() != 0)
                min_scale = std::min(min_scale, w / t->boundingRect().width());
            texts.append(t);

            /** Point for value **/
            if(comp[j]->value() != 0) {
                float l = (comp[j]->value() - 0.5) * r/CIRCLES_COUNT;
                QPointF p(l*cos(alpha(i, j, n)), -l*sin(alpha(i, j, n)));
                addEllipse(p.x() - eps, p.y() - eps, 2*eps, 2*eps, QPen(), Qt::blue);

                poly.append(p);
            }
            else
                poly.append(QPointF(0, 0));
        }
    }

    //Set scale for every text
    for(int k = 0; k < texts.length(); k++)
        texts[k]->setScale(min_scale);

    if(poly.length() == comp_count) {
        QColor blue(0, 0, 255, OPACITY);
        addPolygon(poly, QPen(blue), QBrush(blue));
    }

    //Center the view
    m_view->centerOn(0, 0);
}

float Radar::alpha(int i, int j, int n) {
    return M_PI * (i + (float)(1 + 2*j) / (2*n)) / 2;
}

bool Radar::top(int i) {
    return (trunc(i/2) == 0);
}

bool Radar::right(int i) {
    return (i % 3 == 0);
}

bool Radar::eventFilter(QObject *watched, QEvent *event) {
    if(watched == m_window && (event->type() == QEvent::Resize || event->type() == QEvent::Show)) {
        updateScreen();
    }
    return false;
}

void Radar::updateScreen() {
    drawRayons(m_domains, m_colors, m_circles);
}


QList<Domain*> Radar::domains() {
    return m_domains;
}

