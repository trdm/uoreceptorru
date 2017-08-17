#include "uodbreftableview.h"
#include "uodbreftableqmodel.h"
#include <QMenu>
#include <QDebug>
#include <QApplication>

uoDbRefTableView::uoDbRefTableView(QWidget *parent) :
    QTableView(parent)
{
}

void uoDbRefTableView::setIFace(uoDbRefTableListIface *iFase){
    m_iFase = iFase;
}

bool uoDbRefTableView::goToCurentElement(){
    int curElem  = m_iFase->curentId();
    bool retVal = false;
    if (curElem == UO_EMPTY_ELEMENT)
        return false;
    QModelIndexList matchIndexes = model()->match(model()->index(0, m_iFase->colNo_ID()), Qt::DisplayRole, curElem, 1, Qt::MatchFixedString);

    if (!matchIndexes.isEmpty()){
        QModelIndex idx = matchIndexes.at(0);
        if (idx.isValid()) {
            // колонка с ID у нас спрятана, надо перепозиционировать на видимый индекс...
            QModelIndex idxSel = model()->index(idx.row(), m_iFase->curentCol());
            if (idxSel.isValid() && idxSel.column() != -1) {
                setCurrentIndex(idxSel);
                QItemSelectionModel *selectionM = selectionModel();
                QItemSelection selection(idxSel, idxSel);
                selectionM->select(selection, QItemSelectionModel::Select);
                retVal = true;
            }
        }
     }
    return retVal;
}

void uoDbRefTableView::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu *popup = new QMenu(this);
    const QModelIndex idx = currentIndex();

    QList<QAction *> actions = m_iFase->actionsList();
    popup->addActions(actions);
    QAction *result = popup->exec(e->globalPos());
    Q_UNUSED(result);
    int iresult;
    Q_UNUSED(iresult);
    delete popup;

}

void uoDbRefTableView::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    int cols = model()->columnCount();
    int lastCol = 0;
    int firstCol = -1;
    for (int i=0; i<cols; i++){
        if (!isColumnHidden(i)){
            lastCol = i;
            if (firstCol == -1)
                firstCol = i;
        }
    }
    const QModelIndex idx = currentIndex();
    switch (key){
    case Qt::Key_Delete:   {
        emit onDelElement(currentIndex());
        event->accept();
        break;
    }
    case Qt::Key_Insert:    {
       emit onNewElement(currentIndex());
       event->accept();
       break;
    }
    case Qt::Key_Enter:
    case Qt::Key_Return:
    {
        emit doubleClicked(idx);
        event->accept();
        return;
    }
    case Qt::Key_Plus:{
        if (qApp->keyboardModifiers() & Qt::ControlModifier){
            emit onNewGroup(idx);
            event->accept();
        }
        break;
       }
    case Qt::Key_Up:{
        if (qApp->keyboardModifiers() & Qt::ControlModifier){
            emit onUpToParent(idx);
            event->accept();
            return; //<< селекшин модель иксовничает...
        }
        break;
    }
    case Qt::Key_Tab:{
        // Обеспечим нафигацию табом...
        if (lastCol == idx.column()){
            QWidget::keyPressEvent(event);
            return;
        }
        break;
    }
    case Qt::Key_Backtab:{
        // Обеспечим нафигацию табом...
        if (firstCol == idx.column()){
            QWidget::keyPressEvent(event);
            return;
        }
        break;
    }
    case Qt::Key_F5:{
        event->accept();
        onUpdate(idx);
        break;
    }
    case Qt::Key_Down:{
        if (qApp->keyboardModifiers() & Qt::ControlModifier){
            event->accept();
            emit onDownToChildren(idx);
            return; //<< селекшин модель иксовничает...
        }
        break;
    }
    default:
        break;
    }
    QTableView::keyPressEvent(event);
}

void uoDbRefTableView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    int curentElem = UO_EMPTY_ELEMENT;
    if (m_iFase){
        if (m_iFase->tableModel() && current.isValid()){
            m_iFase->setCurentCol(current.column());
            curentElem = m_iFase->tableModel()->getElementByIndex(current);
            if (curentElem>0){
                m_iFase->setCurentsElement(curentElem, m_iFase->curentParent());
            }
        }
    }
    QTableView::currentChanged(current, previous);
}
