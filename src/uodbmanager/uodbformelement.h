#ifndef UODBELEMENTFORM_H
#define UODBELEMENTFORM_H

#include <QWidget>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>

#include "uodefines.h"
#include "uodbform.h"
#include "uodbmanager.h"
#include "uotextedit.h"
#include "../ui_uodbformelement.h"

class uoDbFormElement : public uoDbForm, private Ui::uoDbFormElement
{
    Q_OBJECT
public:
    explicit uoDbFormElement(QWidget *parent, uoDbRefTableIface* refIFace);
    virtual ~uoDbFormElement();
public:
    void setTitle();
    void createControls();

signals:
    
public slots:
    void onCloseBtn();
    void onOkBtn();
    void onSaveBtn();
    void dataChangeLE();
    void dataChangeTE();

private:
    uoDbRefTableIface*      m_iFace;
    QMap<QString, QWidget*> m_mapWidget;
    uoElementData*     m_elData;
    //QWidget*                m_firstWi;
    QString*                m_descr;
protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *cev);
    void keyPressEvent(QKeyEvent * event);
    void focusInEvent ( QFocusEvent * event );
};

#endif // UODBELEMENTFORM_H
