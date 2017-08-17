#ifndef UODBTREEVIEW_H
#define UODBTREEVIEW_H

#include <QTreeView>
#include <QKeyEvent>

class uoDbTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit uoDbTreeView(QWidget *parent = 0);
    
signals:
    
public slots:
private:
    virtual void	keyPressEvent ( QKeyEvent * event );

};

#endif // UODBTREEVIEW_H
