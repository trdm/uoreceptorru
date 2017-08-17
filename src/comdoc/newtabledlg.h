#ifndef NEWTABLEDLG_H
#define NEWTABLEDLG_H

#include "ui_newtabledlg.h"

class NewTableDlg : public QDialog, private Ui::NewTableDlg
{
    Q_OBJECT
    
public:
    explicit NewTableDlg(QWidget *parent = 0);
    int m_columns;
    int m_rows;
    int m_widht;
public slots:
    void accept();
protected:
    void changeEvent(QEvent *e);


};

#endif // NEWTABLEDLG_H
