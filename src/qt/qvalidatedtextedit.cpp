#include "qvalidatedtextedit.h"

#include "guiconstants.h"

#include <QMessageBox>
QValidatedTextEdit::QValidatedTextEdit(QWidget *parent) :
    QPlainTextEdit(parent), valid(true)
{}

void QValidatedTextEdit::setValid(bool valid)
{
    //setStyleSheet(valid ? "border: 1px solid#f26522;background-color: #1c1c1c;" : STYLE_INVALID);
    setStyleSheet(valid ? "" : "border: 1px solid#ff0000;background-color: #1c1c1c;");

    if(valid)
    {
        if(toPlainText() == this->errorText)
            setPlainText("");
    }
    else if(toPlainText() == "")
        setPlainText(this->errorText);
}

void QValidatedTextEdit::setErrorText(QString errorText)
{
    this->errorText = errorText;
}

void QValidatedTextEdit::focusInEvent(QFocusEvent *evt)
{
    // Clear invalid flag on focus
    setValid(true);
    QPlainTextEdit::focusInEvent(evt);
}

void QValidatedTextEdit::markValid()
{
    setValid(true);
}

void QValidatedTextEdit::clear()
{
    setValid(true);
    QPlainTextEdit::clear();
}
