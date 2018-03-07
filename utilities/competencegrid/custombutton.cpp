#include "custombutton.h"

CustomButton::CustomButton() : QToolButton()
{
    setMinimumSize(0, 0);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    setAutoRaise(true);
    //resize(0, 0);
}

void CustomButton::setHtml(QString html) {
    m_html = html;
    updateHtml();
}

void CustomButton::updateHtml() {
    QTextDocument doc;
    doc.setHtml(m_html);
    if(doc.toPlainText() == "")
        doc.setHtml("Cliquer ici pour ajouter une appréciation ou des objectifs...");
    doc.setTextWidth(0.95 * width());

    QPixmap pic(doc.size().width(), doc.size().height());
    pic.fill(Qt::transparent);
    QPainter painter(&pic);
    doc.drawContents(&painter, pic.rect());

    QIcon icon(pic);
    setIcon(icon);
    setIconSize(pic.rect().size());
}

void CustomButton::resizeEvent(QResizeEvent *e) {
    updateHtml();
    return QToolButton::resizeEvent(e);
}

QSize CustomButton::sizeHint() {
    return QSize(0, 0);
}
