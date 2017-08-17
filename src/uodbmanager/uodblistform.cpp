#include "uodblistform.h"
#include "uodbreftableqmodel.h"
#include "uodbreftreemodel.h"
#include <QDebug>



uoDbFormList::uoDbFormList(QWidget *parent, uoDbRefTableListIface *IFace)
: uoDbForm(parent)
, m_IFace(IFace)
{
    prepareData();

    setupUi(this);
    prepareActions();
    prepareUi();
    prepareConnection();
    updateTable();
}

void uoDbFormList::updateTable()
{
    m_tableView->resizeRowsToContents();
    //m_tableView->resizeColumnsToContents();
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    if (m_IFace->tableModel()->rowCount()>0){
        if (m_IFace->curentId() == UO_EMPTY_ELEMENT){
            QModelIndex topLeft = m_IFace->tableModel()->index(0,0,QModelIndex());
            m_tableView->setCurrentIndex(topLeft);
        } else {
            if (!m_tableView->goToCurentElement()){
                QModelIndex topLeft = m_IFace->tableModel()->index(0,0,QModelIndex());
                m_tableView->setCurrentIndex(topLeft);
            }
        }
    }
}

void uoDbFormList::prepareConnection()
{
    connect(m_tableView,     SIGNAL(doubleClicked(const QModelIndex&)),    this, SLOT(onEdit(const QModelIndex&)));
    connect(m_tableView,     SIGNAL(onNewElement(const QModelIndex&)),     this, SLOT(onNewElement(const QModelIndex&)));
    connect(m_tableView,     SIGNAL(onNewGroup(const QModelIndex&)),       this, SLOT(onNewGroup(const QModelIndex&)));
    connect(m_tableView,     SIGNAL(onDelElement(const QModelIndex&)),     this, SLOT(onDelElement(const QModelIndex&)));
    connect(m_tableView,     SIGNAL(onUpToParent(const QModelIndex&)),     this, SLOT(onUpToParent(const QModelIndex&)));
    connect(m_tableView,     SIGNAL(onDownToChildren(const QModelIndex&)), this, SLOT(onDownToChildren(const QModelIndex&)));
    connect(m_tableView,     SIGNAL(onUpdate(const QModelIndex&)),         this, SLOT(onUpdate(const QModelIndex&)));
    connect(m_dataModel,     SIGNAL(refreshComplit()),                     this, SLOT(doRefreshComplit()));
    connect(m_actNewElement, SIGNAL(triggered()),                          this, SLOT(onNewElementAct()));
    connect(m_actNewGroup,   SIGNAL(triggered()),                          this, SLOT(onNewGroupAct()));
    connect(m_actEdit,       SIGNAL(triggered()),                          this, SLOT(onEditAct()));
    connect(m_actView,       SIGNAL(triggered()),                          this, SLOT(onViewAct()));
    connect(m_actDelete,     SIGNAL(triggered()),                          this, SLOT(onDelElementAct()));
    connect(m_actHierarhi,   SIGNAL(triggered(bool)),                      this, SLOT(doHierarhyChange(bool)));
    connect(m_actRefresh,    SIGNAL(triggered(bool)),                      this, SLOT(doRefresh()));
    connect(m_actUpToParent, SIGNAL(triggered()),                           this, SLOT(doUpToParent()));
    connect(m_actDownToChildren, SIGNAL(triggered()),                      this, SLOT(doDownToChildren()));
    connect(m_ClosePB,       SIGNAL(pressed()),                            this, SLOT(doClose()));
}

void uoDbFormList::prepareData()
{
    m_dataModel = m_IFace->tableModel(); // new uoDBSQModelRef(this, IFace);
}

void uoDbFormList::prepareUi()
{
    setWindowTitle(m_IFace->m_table->m_descr);
    m_TitleLabel->setText(m_IFace->m_table->m_descr);
    setFocusProxy(m_tableView);
    m_tableView->setIFace(m_IFace);
    m_tableView->setModel(m_dataModel);
    m_tableView->resizeRowsToContents();
    m_tableView->resizeColumnsToContents();
    for (int y=0;y<m_IFace->m_hideCols.count(); y++)
    {
        m_tableView->hideColumn(m_IFace->m_hideCols.at(y));
    }

    if (m_IFace->m_table->m_hasHierarchy){
        uoDbRefTreeModel* treeModel = m_IFace->treeModel(); // new uoDbRefTreeModel(this, IFace);
        m_treeView->setModel(treeModel);
        connect(m_treeView, SIGNAL(doubleClicked(const QModelIndex&)),this, SLOT(doShowChildsFromTree(const QModelIndex&)));
        connect(m_treeView, SIGNAL(entered(const QModelIndex&)),this, SLOT(doShowChildsFromTree(const QModelIndex&)));
    } else {
        m_treeView->hide();
    }
    m_splitter->setStretchFactor(0, 0);
    m_splitter->setStretchFactor(1, 1);

    m_treeView->resize(m_treeView->width()/2,m_treeView->height());

    //Drake
    //Вот эта инструкция и приводит к тому, что окна остаются подсвеченными
    //m_tableView->setFocus(Qt::OtherFocusReason);

}

void uoDbFormList::prepareActions()
{
    m_actHierarhi->setChecked(m_IFace->hierarchyMode());

    m_IFace->addAction(m_actNewElement);
    m_IFace->addAction(m_actNewGroup);
    m_IFace->addAction(m_actEdit);
    m_IFace->addAction(m_actView);
    m_IFace->addAction(m_actDelete);
    QAction *action = new QAction(this);
    action->setSeparator(true);
    m_IFace->addAction(action);
    m_IFace->addAction(m_actHierarhi);
    m_IFace->addAction(m_actRefresh);
    action = new QAction(this);
    action->setSeparator(true);
    m_IFace->addAction(action);
    m_IFace->addAction(m_actUpToParent);
    m_IFace->addAction(m_actDownToChildren);
}

void uoDbFormList::onEdit(const QModelIndex &index, bool ro)
{
    int elem = m_IFace->tableModel()->getElementByIndex(index);
    if (elem != UO_EMPTY_ELEMENT){
        int col = index.column();
        if (m_IFace->hierarchyMode() ) {
            if (col == m_IFace->colNo_Picture() && m_IFace->colNo_Picture() != -1){
                // Хочу перехватить нафигацию по иконке вверх вниз.
                uoElementData* elData = m_IFace->load(elem);
                if (elData){
                    if (elData->isFolder()) {
                        onDownToChildren(index);
                    } else if (elData->parentId() != UO_EMPTY_ELEMENT){
                        onUpToParent(index);
                    }
                }
                return;
            }
        }

        uoUrl url = m_IFace->m_table->getUrlElement(elem);
        url.setReadOnly(ro);
        url.setOvnerForm(m_IFace->url().toString());
        m_IFace->m_factory->showFormUrl(url);
    }
}

void uoDbFormList::onEditAct(){
    const QModelIndex &index = m_tableView->currentIndex();
    onEdit(index);
}

void uoDbFormList::onViewAct(){
    const QModelIndex &index = m_tableView->currentIndex();
    onEdit(index, true);
}

void uoDbFormList::onNewElement(const QModelIndex &index)
{
//    int elem = m_IFace->tableModel()->getElementByIndex(index);
    m_IFace->doNewElement(uoFormT_Elem, m_IFace->tableModel()->parentCur());
}

void uoDbFormList::onNewElementAct(){
    const QModelIndex &index = m_tableView->currentIndex();
    onNewElement(index);
}

void uoDbFormList::onNewGroup(const QModelIndex &index)
{
    m_IFace->doNewElement(uoFormT_Group, m_IFace->tableModel()->parentCur());
}

void uoDbFormList::onNewGroupAct(){
    m_IFace->doNewElement(uoFormT_Group, m_IFace->tableModel()->parentCur());
}

void uoDbFormList::onDelElement(const QModelIndex &index)
{
    int elem = m_IFace->tableModel()->getElementByIndex(index);
    qDebug() << "uoDbListForm::onDelElement" << index.row() << index.column() << " elem "<<elem;
    if (m_IFace->doDeleteElement( elem))
        doRefresh();
}

void uoDbFormList::onDelElementAct(){
    const QModelIndex &index = m_tableView->currentIndex();
    onDelElement(index);
}

void uoDbFormList::doHierarhyChange(bool check){
    m_IFace->setHierarchy(check);
    onUpdate(m_tableView->currentIndex());
}

void uoDbFormList::doRefresh()
{
    const QModelIndex &index = m_tableView->currentIndex();
    onUpdate(index);
}

void uoDbFormList::doShowChildsFromTree(const QModelIndex &index)
{
    if (!m_IFace->hierarchyMode())
        return;
    int elem = m_IFace->treeModel()->getElementByIndex(index);
    if (elem == -1)
        elem = 0;
    m_IFace->tableModel()->setParentElem(elem);
    updateTable();
}

void uoDbFormList::onUpToParent(const QModelIndex &index)
{
    if (!m_IFace->hierarchyMode())
        return;
    int elem = m_IFace->tableModel()->getElementByIndex(index);
    int parentId = m_IFace->m_table->getParentFrom(elem);
    int curId = parentId;
    parentId = m_IFace->m_table->getParentFrom(parentId);
    m_IFace->setCurentsElement(curId,parentId);
    m_IFace->tableModel()->setParentElem(parentId);
    updateTable();
}

void uoDbFormList::doUpToParent(){
    onUpToParent(m_tableView->currentIndex());
}

void uoDbFormList::onDownToChildren(const QModelIndex &index)
{
    if (!m_IFace->hierarchyMode())
        return;
    int elem = m_IFace->tableModel()->getElementByIndex(index);
    uoElementData* el = m_IFace->m_table->elementData(elem);
    if (el){
        if (el->isFolder()){
            m_IFace->tableModel()->setParentElem(elem);
            updateTable();
        }
        delete el;
    }
}

void uoDbFormList::doDownToChildren(){
    onDownToChildren(m_tableView->currentIndex());
}

void uoDbFormList::onUpdate(const QModelIndex &index)
{
    int elem = m_IFace->tableModel()->parentCur();
    m_IFace->tableModel()->setParentElem(elem);
    m_IFace->treeModel()->refresh(m_treeView->currentIndex());
    updateTable();

}

void uoDbFormList::doClose()
{
    if (m_mdiParent)
        m_mdiParent->close();
}

void uoDbFormList::doRefreshComplit(){
//    m_tableView->goToCurentElement();
}



void uoDbFormList::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
    QWidget::changeEvent(e);
}


