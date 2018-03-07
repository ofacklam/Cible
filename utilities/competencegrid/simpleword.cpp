#include "simpleword.h"
#include "ui_simpleword.h"

SimpleWord::SimpleWord(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SimpleWord)
{
    ui->setupUi(this);
    connect(ui->btn_bold, SIGNAL(toggled(bool)),this, SLOT(toggleBold(bool)));
    connect(ui->btn_italic, SIGNAL(toggled(bool)),this, SLOT(toggleItalic(bool)));
    connect(ui->btn_underline, SIGNAL(toggled(bool)),this, SLOT(toggleUnderline(bool)));
    connect(ui->btn_undo, SIGNAL(clicked(bool)), ui->textEdit, SLOT(undo()));
    connect(ui->btn_redo, SIGNAL(clicked(bool)), ui->textEdit, SLOT(redo()));
    connect(ui->textEdit, SIGNAL(undoAvailable(bool)), this, SLOT(undoAvailable(bool)));
    connect(ui->textEdit, SIGNAL(redoAvailable(bool)), this, SLOT(redoAvailable(bool)));
    connect(ui->textEdit, SIGNAL(currentCharFormatChanged(QTextCharFormat)), this, SLOT(formatChanged(QTextCharFormat)));

    connect(ui->btn_save, SIGNAL(clicked(bool)), this, SLOT(accept()));
    connect(ui->btn_cancel, SIGNAL(clicked(bool)), this, SLOT(reject()));

}

SimpleWord::~SimpleWord()
{
    delete ui;
}

void SimpleWord::toggleBold(bool checked) {
    ui->textEdit->setFontWeight(checked ? QFont::Bold : QFont::Normal);
}

void SimpleWord::toggleItalic(bool checked) {
    ui->textEdit->setFontItalic(checked);
}

void SimpleWord::toggleUnderline(bool checked) {
    ui->textEdit->setFontUnderline(checked);
}

void SimpleWord::undoAvailable(bool a) {
    ui->btn_undo->setEnabled(a);
}

void SimpleWord::redoAvailable(bool a) {
    ui->btn_redo->setEnabled(a);
}

void SimpleWord::formatChanged(QTextCharFormat f) {
    ui->btn_underline->setChecked(f.fontUnderline());
    ui->btn_italic->setChecked(f.fontItalic());
    ui->btn_bold->setChecked(f.fontWeight() == QFont::Bold);
}

QString SimpleWord::text() {
    return ui->textEdit->toHtml();
}

void SimpleWord::setText(QString text) {
    ui->textEdit->setHtml(text);
}
