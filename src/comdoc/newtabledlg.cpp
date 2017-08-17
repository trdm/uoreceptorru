#include "newtabledlg.h"

NewTableDlg::NewTableDlg(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    m_columns = 3;
    m_rows = 3;
    m_widht = 100;

}

void NewTableDlg::changeEvent(QEvent *e)
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
void NewTableDlg::accept()
{
    m_columns = m_columnLe->text().toInt();
    m_rows = m_rowLe->text().toInt();
    m_widht = m_widhtSb->text().toInt();
    QDialog::accept();
}
