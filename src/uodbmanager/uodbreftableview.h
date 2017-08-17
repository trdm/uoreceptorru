#ifndef UODBREFTABLEVIEW_H
#define UODBREFTABLEVIEW_H

#include <QTableView>
#include <QKeyEvent>

#include "uodbmanager.h"

class uoDbRefTableView : public QTableView
{
    Q_OBJECT
public:
    explicit uoDbRefTableView(QWidget *parent = 0);
    void setIFace(uoDbRefTableListIface* iFase);
    bool goToCurentElement();
    
signals:
    void onNewElement(const QModelIndex &index);
    void onNewGroup(const QModelIndex &index);
    void onDelElement(const QModelIndex &index);
    void onUpToParent(const QModelIndex &index);
    void onDownToChildren(const QModelIndex &index);
    void onUpdate(const QModelIndex &index);
public slots:
protected Q_SLOTS:
     virtual void currentChanged ( const QModelIndex & current, const QModelIndex & previous );

private:
    uoDbRefTableListIface* m_iFase;
protected:
     void contextMenuEvent( QContextMenuEvent * e );
     void keyPressEvent ( QKeyEvent * event );


    
};

#endif // UODBREFTABLEVIEW_H
