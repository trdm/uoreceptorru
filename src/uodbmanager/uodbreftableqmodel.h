#ifndef UODBSQMODELREF_H
#define UODBSQMODELREF_H

#include <QSqlQueryModel>
#include "uodbmanager.h"

// uodbsqmodelref.h >> uodbreftableqmodel.h
class uoDBRefTableModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    uoDBRefTableModel(uoDbRefTableListIface* ifase = 0);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index,
                 const QVariant &value, int role);
    int getElementByIndex(const QModelIndex &index);
    void setParentElem(const int parent = -1);
    int parentCur(){ return m_parent;}
    void setParent(int parent);
    void resetMy() {reset();}

signals:
    void refreshComplit();
private:
    void refresh();
    uoDbRefTableListIface* m_uoIFase;
    int m_parent;
};

#endif // UODBSQMODELREF_H
