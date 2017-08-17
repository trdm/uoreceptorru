#ifndef UOLHSETTINGTHDLG_H
#define UOLHSETTINGTHDLG_H

#include "ui_uolhsettingthdlg.h"

class uoLHSettingthDlg : public QDialog, private Ui::uoLHSettingthDlg
{
    Q_OBJECT
    
public:
    explicit uoLHSettingthDlg(QWidget *parent = 0);
    
protected:
    void changeEvent(QEvent *e);
};

#endif // UOLHSETTINGTHDLG_H
