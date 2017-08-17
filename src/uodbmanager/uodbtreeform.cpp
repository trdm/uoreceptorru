#include "uodbtreeform.h"
#include <QDebug>

uoDbTreeForm::uoDbTreeForm(QWidget *parent, uoDbRefTableListIface *IFace) :
    uoDbForm(parent),m_iFace(IFace)
{
    prepareData();
    setupUi(this);
    prepareUi();
}

void uoDbTreeForm::prepareData(){
    if (m_iFace)
        m_treeModel = m_iFace->treeModel();
}

void uoDbTreeForm::prepareUi()
{
    if (!m_iFace)
        return;
    setWindowTitle(m_iFace->m_table->m_descr);
    m_TitleLabel->setText(m_iFace->m_table->m_descr);
    setFocusProxy(m_treeView);

    if (m_iFace->m_table->m_hasHierarchy){
        m_treeModel = m_iFace->treeModel(); // new uoDbRefTreeModel(this, IFace);
        m_treeView->setModel(m_treeModel);
        connect(m_treeView, SIGNAL(doubleClicked(const QModelIndex&)),this, SLOT(onClickedIndex(const QModelIndex&)));
        connect(m_treeView, SIGNAL(entered(const QModelIndex&)),this, SLOT(onClickedIndex(const QModelIndex&)));
    } else {
        m_treeView->hide();
    }
}

void uoDbTreeForm::onClickedIndex(const QModelIndex &index){
    int elem = m_treeModel->getElementByIndex(index);
    qDebug() << "uoDbTreeForm::onClickedIndex" << elem;
    if (elem != UO_EMPTY_ELEMENT){
        uoUrl url = m_iFace->m_table->getUrlElement(elem);
        if (!url.isFolder())
            emit onClickedElement(url);
    }

}

void uoDbTreeForm::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

