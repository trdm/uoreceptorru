#include "uolhsettingthdlg.h"

uoLHSettingthDlg::uoLHSettingthDlg(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
}

void uoLHSettingthDlg::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}
