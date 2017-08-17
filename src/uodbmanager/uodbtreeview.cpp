#include "uodbtreeview.h"

uoDbTreeView::uoDbTreeView(QWidget *parent) :
    QTreeView(parent)
{
}

void uoDbTreeView::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    const QModelIndex idx = currentIndex();

    switch (key){
    case Qt::Key_Enter:
    case Qt::Key_Return:
    {
        emit doubleClicked(idx);
        event->accept();
        return;
    }
    default:
        break;
    }
    QTreeView::keyPressEvent(event);

}
