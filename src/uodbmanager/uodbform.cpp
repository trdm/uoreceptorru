#include <QApplication>
#include <QDebug>
#include "uodbform.h"

uoDbForm::uoDbForm(QWidget *parent)
    : QWidget(parent)
{
    m_mdiParent = 0;
}

uoDbForm::~uoDbForm()
{
    m_mdiParent = 0;
}

void uoDbForm::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    if (key == Qt::Key_Tab || key == Qt::Key_Backtab){
        if (qApp->keyboardModifiers() & Qt::ControlModifier){
//            qDebug() << "uoDbForm::keyPressEvent ctrl+Tab || ctrl+bakTab";
        }
    }
    QWidget::keyPressEvent(event);
}
