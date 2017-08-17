#ifndef UODBLINEEDIT_H
#define UODBLINEEDIT_H

#include "ui_uodblineedit.h"
#include "uodefines.h"
#include "uodbmanager.h"


class uoDbLineEdit : public QWidget, private Ui::uoDbLineEdit
{
    Q_OBJECT
    
public:
    explicit uoDbLineEdit(QWidget *parent = 0, uoDbLineEditFlags flags = uoDBLe_Choice);
    QString	text () const;
    void	setText ( const QString & txt);
    void	setFlags ( const uoDbLineEditFlags &flags = uoDBLe_Choice);
    void	setData ( uoElementData* elData );

protected:
    void changeEvent(QEvent *e);
private:
    uoDbLineEditFlags m_flags;
    uoElementData* m_elData;
};

#endif // UODBLINEEDIT_H
