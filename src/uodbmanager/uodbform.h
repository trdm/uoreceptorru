#ifndef UODBFORM_H
#define UODBFORM_H

#include <QWidget>
#include <QDialog>
#include <QMdiSubWindow>
#include <QKeyEvent>

class uoDbForm : public QWidget
{
public:
    explicit uoDbForm(QWidget *parent = 0);
    virtual ~uoDbForm();
    void setSW(QMdiSubWindow* mdi) {m_mdiParent = mdi; }
signals:
    
public slots:

protected:
    virtual void	keyPressEvent ( QKeyEvent * event );
    QMdiSubWindow* m_mdiParent;

    
};

#endif // UODBFORM_H
