#include "uotextedit.h"

uoTextEdit::uoTextEdit(QWidget *parent) :
    QTextEdit(parent)
{
    //setTabOrder();
    setMaximumWidth(16555);
    setMaximumHeight(16555);
}

void uoTextEdit::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    if (key == Qt::Key_Tab || key == Qt::Key_Backtab){
        QWidget::keyPressEvent(event);
        return;
    }
    QTextEdit::keyPressEvent(event);
}
