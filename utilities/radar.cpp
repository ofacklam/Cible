#include "radar.h"

Radar::Radar(QWidget *win, QGraphicsView *view, Page *p) : QGraphicsScene()
{
    m_window = win;
    m_view = view;
    m_page = p;

    // COLORS TO BE DEFINED
    m_colors << QColor(250, 250, 190) << QColor(240, 220, 240) << QColor(200, 240, 240) << QColor(255, 205, 205);
    m_circles << QColor(255, 100, 100) << QColor(255, 210, 110) << QColor(255, 250, 100) << QColor(130, 255, 150);

    updateScreen();

    m_window->installEventFilter(this);
}

Radar::~Radar() {

}

void Radar::addTab(Tab *t) {
    m_tabs.insert(t->id(), t);
    updateScreen();
}

void Radar::removeTab(int id_tab) {
    m_tabs.remove(id_tab);
    updateScreen();
}

void Radar::clearTabs() {
    m_tabs.clear();
    updateScreen();
}

void Radar::drawZones(float L) {
    qreal min_scale = 1000;
    QList<QGraphicsTextItem*> texts;

    float border = BORDER * L / 500.0;
    int domain_count = m_page->domains().length();

    //Draw borders
    if(domain_count <= 1) {
        QPen borders(QBrush(m_colors[0]), border);
        addLine(-L, -L, -L, L, borders);
        addLine(-L, L, L, L, borders);
        addLine(L, L, L, -L, borders);
        addLine(L, -L, -L, -L, borders);
    }
    else {
        for(int i = 0; i < domain_count; i++) {
            QPen borders(QBrush(m_colors[i%m_colors.length()]), border);

            float a = domain_angle(i, domain_count);
            QPointF b = border_point(L, a);
            addLine(0, 0, b.x, b.y, borders);

            a = next_angle(a);
            while(a < domain_angle(i+1, domain_count)) {
                QPointF tb = border_point(L, a);
                addLine(b.x, b.y, tb.x, tb.y, borders);
                a += M_PI/2;
                b = tb;
            }

            QPointF f = border_point(L, domain_angle(i+1, domain_count));
            addLine(b.x, b.y, f.x, f.y, borders);
            addLine(f.x, f.y, 0, 0, borders);
        }
    }

    //Draw the texts
    for(int i = 0; i < domain_count; i++) {
        float ave_angle = (domain_angle(i, domain_count) + domain_angle(i+1, domain_count)) / 2;
        float start = std::max(domain_angle(i, domain_count), previous_angle(ave_angle));
        float end = std::min(domain_angle(i+1, domain_count), next_angle(ave_angle));
        ave_angle = (start + end) / 2;
        QPointF p_ave = border_point(L, ave_angle);
        QPointF p_start = border_point(L, start);
        QPointF p_end = border_point(L, end);

        QString text = m_page->domains().at(i)->name();
        if(which_border(ave_angle) == Right || which_border(ave_angle) == Left) {
            //Vertical wall => write vertically
            int l = text.length();
            for(int k = l-1; k > 0; k++)
                text.insert(k, "\n");
        }

        QGraphicsTextItem *t = addText(text);
        QFont f = t->font();
        f.setPointSize(10);
        t->setFont(f);

        QPointF pos = p_ave;
        float space_x, space_y;
        switch(which_border(ave_angle)) {
        case Right:
            pos.rx() -= t->boundingRect().width();
            pos.ry() -= t->boundingRect().height() / 2;
            space_x = 0.2 * L;
            space_y = 0.7 * (p_start.y() - p_end.y());
            break;
        case Top:
            pos.rx() -= t->boundingRect().width() / 2;
            space_x = 0.7 * (p_start.x() - p_end.x());
            space_y = 0.2 * L;
            break;
        case Left:
            pos.ry() -= t->boundingRect().height() / 2;
            space_x = 0.2 * L;
            space_y = 0.7 * (p_end.y() - p_start.y());
            break;
        case Bottom:
            pos.rx() -= t->boundingRect().width() / 2;
            pos.ry() -= t->boundingRect().height();
            space_x = 0.7 * (p_end.x() - p_start.x());
            space_y = 0.2 * L;
            break;
        }

        t->setPos(pos);
        t->setTransformOriginPoint(p_ave);
        if(t->boundingRect().width() != 0 && t->boundingRect().height() != 0)
            min_scale = std::min(min_scale, std::min(space_y / t->boundingRect().height(), space_x / t->boundingRect().width()));
        texts.append(t);
    }

    //Set scale for every title
    for(int k = 0; k < texts.length(); k++)
        texts[k]->setScale(min_scale);
}

void Radar::drawCircles(float r) {
    /** Circles **/

    //Create pen
    QPen lines(Qt::black, std::max(1.0, (double) THICK * L / 500));

    for(int k = CIRCLES_COUNT - 1; k >= 0; k--) {
        float rad = (k+1) * r / CIRCLES_COUNT;
        addEllipse(-rad, -rad, 2*rad, 2*rad, lines, QBrush(m_circles[k]));
    }
}

void Radar::drawRayons(float L, float r) {
    //Create pen
    QPen lines(Qt::black, std::max(1.0, (double) THICK * L / 500));

    qreal min_scale = 1000;
    QList<QGraphicsTextItem*> texts;

    QMap<int, QPolygonF> polys;
    int comp_count = 0;

    //Cycle through domains
    int n = m_page->domains().length();
    for(int i = 0; i < n; i++) {
        QList<Competence*> comps = m_page->domains().at(i)->competences();

        //Cycle through competences
        int m = comps.length();
        comp_count += m;
        for(int j = 0; j < m; j++) {
            float a = comp_angle(i, n, j, m);
            float x = r*cos(a);
            float y = -r*sin(a);
            float eps = std::max(1.0, (double) EPSILON * L / 500);

            /** Lines  **/
            addLine(0, 0, x, y, lines);

            /** The Text **/
            QGraphicsTextItem *t = addText(comps[j]->name());
            if(m_tabs.count() == 1 && m_tabs.first()->value(comps[j]->id()) == 0)
                t->setDefaultTextColor(Qt::red);
            QFont f = t->font();
            f.setPointSize(10);
            t->setFont(f);

            //Calculate width
            QFontMetrics fm(f);
            int min_width = fm.width("Une compétence");

            float space_x = L, space_y = 0;
            float d_before = domain_angle(i, n);
            float d_after = domain_angle(i+1, n);
            float a_before = (j-1 >= 0) ? comp_angle(i, n, j-1, m) : d_before;
            float a_after = (j+1 < m) ? comp_angle(i, n, j+1, m) : d_after;
            if(top(a)) {
                if(right(a)) {
                    space_y = y + r*sin(a_after);
                    if(d_before > 0)
                        space_x = std::min(L, -y*tan(M_PI/2 - d_before));
                }
                else {
                    space_y = y + r*sin(a_before);
                    if(d_after < M_PI)
                        space_x = std::min(L, y*tan(M_PI/2 - d_after));
                }
            }
            else {
                if(right(a)) {
                    space_y = -r*sin(a_before) - y;
                    if(d_after < 2*M_PI)
                        space_x = std::min(L, -y*tan(M_PI/2 - d_after));
                }
                else {
                    space_y = -r*sin(a_after) - y;
                    if(d_before > M_PI)
                        space_x = std::min(L, y*tan(M_PI/2 - d_before));
                }
            }
            space_x += x * (right(a) ? -1 : 1);
            space_x *= 0.7;
            space_y *= 0.9;

            int num_lines = 1;
            if(space_x != 0 && t->boundingRect().height() != 0)
                num_lines = std::max(1, round(sqrt((space_y * t->boundingRect().width()) / (space_x * t->boundingRect().height()))));

            t->setTextWidth(std::max(min_width, t->boundingRect().width() / num_lines));


            //Calculate position
            float x_text = x + 2 * eps * (right(a) ? 1 : -1);
            float y_text = y;
            if(top(a))
                y_text -= t->boundingRect().height();
            if(!right(a))
                x_text -= t->boundingRect().width();
            t->setPos(x_text, y_text);

            //Calculate origin point
            t->setTransformOriginPoint(t->boundingRect().width() * !right(a),
                                       t->boundingRect().height() * top(a));

            //Calculate scale
            if(t->boundingRect().width() != 0 && t->boundingRect().height() != 0)
                min_scale = std::min(min_scale, std::min(space_x / t->boundingRect().width(), space_y / t->boundingRect().height()));
            texts.append(t);

            /** Point for value **/
            foreach(Tab *t, m_tabs) {
                if(t->value(comps[j]->id()) != 0) {
                    float l = (t->value(comps[j]->id()) - 0.5) * r/CIRCLES_COUNT;
                    QPointF p(l*cos(a), -l*sin(a));
                    addEllipse(p.x() - eps, p.y() - eps, 2*eps, 2*eps, QPen(), t->color());

                    polys[t->id()].append(p);
                }
                else
                    polys[t->id()].append(QPointF(0, 0));
            }
        }
    }

    //Set scale for every text
    for(int k = 0; k < texts.length(); k++)
        texts[k]->setScale(min_scale);

    foreach(Tab *t, m_tabs) {
        QPolygonF p = polys[t->id()];
        if(p.length() == comp_count) {
            QColor c = t->color();
            c.setAlpha(OPACITY);
            addPolygon(p, QPen(c), QBrush(c));
        }
    }
}

float Radar::domain_angle(int i, int n) {
    return M_PI * 2 * i / (float)n;
}

float Radar::next_angle(float a) {
    switch(which_border(a)) {
    case Right:
        return M_PI/4;
    case Top:
        return 3*M_PI/4;
    case Left:
        return 5*M_PI/4;
    case Bottom:
        return 7*M_PI/4;
    }
}

float Radar::previous_angle(float a) {
    switch(which_border(a)) {
    case Right:
        return 7*M_PI/4;
    case Top:
        return M_PI/4;
    case Left:
        return 3*M_PI/4;
    case Bottom:
        return 5*M_PI/4;
    }
}

QPointF Radar::border_point(float L, float a) {
    if(a < M_PI/4)
        return QPointF(L, -L*tan(a));
    else if(a < 3*M_PI/4)
        return QPointF(L*tan(M_PI/2 - a), -L);
    else if(a < 5*M_PI/4)
        return QPointF(-L, L*tan(a));
    else if(a < 7*M_PI/4)
        return QPointF(-L*tan(M_PI/2 - a), L);
    else
        return QPointF(L, -L*tan(a));
}

float Radar::comp_angle(int i, int n, int j, int m) {
    return M_PI * (2*i + (float)(1 + 2*j) / m) / n;
}

Borders Radar::which_border(float a) {
    if(a < M_PI/4)
        return Right;
    else if(a <= 3*M_PI/4)
        return Top;
    else if(a < 5*M_PI/4)
        return Left;
    else if(a <= 7*M_PI/4)
        return Bottom;
    else
        return Right;
}

bool Radar::top(float a) {
    return (a <= M_PI);
}

bool Radar::right(float a) {
    return (a <= M_PI/2 || a >= 3*M_PI/2);
}

bool Radar::eventFilter(QObject *watched, QEvent *event) {
    if(watched == m_window && (event->type() == QEvent::Resize || event->type() == QEvent::Show)) {
        updateScreen();
    }
    return false;
}

void Radar::updateScreen() {
    /** Draws figure **/

    //Delete what was before
    clear();

    if(m_circles.length() != CIRCLES_COUNT || m_colors.length() == 0)
        return;

    //Calculate dimensions
    float L = std::min(m_view->width(), m_view->height()) / 2;
    float r = L * 0.7;
    setSceneRect(-L, -L, 2*L, 2*L);

    drawZones(L);
    drawCircles(r);
    drawRayons(L, r);

    //Center the view
    m_view->centerOn(0, 0);
}


QList<Domain*> Radar::page() {
    return m_page;
}

