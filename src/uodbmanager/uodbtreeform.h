#ifndef UODBTREEFORM_H
#define UODBTREEFORM_H

#include "ui_uodbtreeform.h"
#include "uodbmanager.h"
#include "uodbreftreemodel.h"
#include "uodbform.h"


class uoDbTreeForm : public uoDbForm, private Ui::uoDbTreeForm
{
    Q_OBJECT
    
public:
    explicit uoDbTreeForm(QWidget *parent = 0, uoDbRefTableListIface* IFace = 0);

public slots:
    void onClickedIndex(const QModelIndex &index);
signals:
    void onClickedElement(const uoUrl& url);

protected:
    void changeEvent(QEvent *e);
    void prepareData();
    void prepareUi();

    uoDbRefTreeModel* m_treeModel;
    uoDbRefTableListIface* m_iFace;
};

#endif // UODBTREEFORM_H
