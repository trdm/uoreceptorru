#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "uodbmanager.h"
#include "uodbmodel.h"

//namespace Ui {
//    class Dialog;
//}

class uoDbManagerDlg : public QDialog
{
    Q_OBJECT

public:
    explicit uoDbManagerDlg(QWidget *parent = 0, uoDbManager* dbMan = 0);
    ~uoDbManagerDlg();

private:
//    Ui::Dialog *ui;
	uoDbManager* m_dbMan;
	uoDbModel* m_model;

};

#endif // DIALOG_H
