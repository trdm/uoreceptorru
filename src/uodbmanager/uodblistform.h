#ifndef UODBLISTFORM_H
#define UODBLISTFORM_H

#include "ui_uodblistform.h"
#include "uodbmanager.h"
#include "uodbform.h"


class uoDbFormList : public uoDbForm, private Ui::uoDbListForm
{
    Q_OBJECT
    
public:
    explicit uoDbFormList(QWidget *parent = 0, uoDbRefTableListIface* IFace = 0);
public slots:
    void onEdit(const QModelIndex & index, bool ro = false);
    void onEditAct();
    void onViewAct();
    void onNewElement(const QModelIndex &index);
    void onNewElementAct();
    void onNewGroup(const QModelIndex &index);
    void onNewGroupAct();
    void onDelElement(const QModelIndex &index);
    void onDelElementAct();
    void doHierarhyChange(bool check = true);
    void doRefresh();
    void doShowChildsFromTree(const QModelIndex &index);
    void onUpToParent(const QModelIndex &index);
    void doUpToParent();
    void onDownToChildren(const QModelIndex &index);
    void doDownToChildren();
    void onUpdate(const QModelIndex &index);
    void doClose();
    void doRefreshComplit();

protected:
    void changeEvent(QEvent *e);
    uoDbRefTableListIface* m_IFace;
    uoDBRefTableModel*        m_dataModel;
    void updateTable();

private:
    void prepareConnection();
    void prepareData();
    void prepareUi();
    void prepareActions();
};

#endif // UODBLISTFORM_H
