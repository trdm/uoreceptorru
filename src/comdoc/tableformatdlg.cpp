#include "tableformatdlg.h"
#include <QColorDialog>

TableFormatDlg::TableFormatDlg(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
}

void TableFormatDlg::initFormat(QTextTable *existingtable)
{
    m_curTableFrm = existingtable->format();
    m_tWidhtSb->setValue(m_curTableFrm.width().rawValue());
    m_bWidhtSb->setValue(m_curTableFrm.border());
    m_cellPaddingSb->setValue(m_curTableFrm.cellPadding());
    m_cellSpasingSb->setValue(m_curTableFrm.cellSpacing());
    m_bgColor = m_curTableFrm.background().color();

    QPixmap pix(22, 22);
    pix.fill(m_bgColor);

    m_actTableColor = new QAction(pix, tr("Table Background Color"),this);
    connect(m_actTableColor, SIGNAL(triggered()), this, SLOT(ChoisColor()));
    m_colorTB->setDefaultAction(m_actTableColor);
    connect(m_tWidhtSb, SIGNAL(valueChanged(int)), this, SLOT(UpdateFormats()));
    connect(m_cellPaddingSb, SIGNAL(valueChanged(double)), this, SLOT(UpdateFormats()));
    connect(m_cellSpasingSb, SIGNAL(valueChanged(double)), this, SLOT(UpdateFormats()));
    connect(m_bWidhtSb, SIGNAL(valueChanged(double)), this, SLOT(UpdateFormats()));

}

void TableFormatDlg::UpdateFormats()
{
    QBrush newcolorsett(m_bgColor,Qt::SolidPattern);
    m_curTableFrm.setBackground(newcolorsett);
    m_curTableFrm.setCellPadding(m_cellPaddingSb->value());
    m_curTableFrm.setCellSpacing(m_cellSpasingSb->value());
    QTextLength newlargos(QTextLength::PercentageLength,m_tWidhtSb->value());
    m_curTableFrm.setWidth(newlargos);
    m_curTableFrm.setBorder(m_bWidhtSb->value() );

}

QTextTableFormat TableFormatDlg::GetNewFormats()
{
    return m_curTableFrm;
}

void TableFormatDlg::ChoisColor()
{
    QColor bcol = QColorDialog::getColor(m_bgColor, this);
    if (bcol.isValid()) {
        m_bgColor = bcol;
        QPixmap pix(22, 22);
        pix.fill(bcol);
        m_actTableColor->setIcon(pix);
        UpdateFormats();
    }
}
void TableFormatDlg::changeEvent(QEvent *e)
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

void TableFormatDlg::closeEvent( QCloseEvent* e )
{
	UpdateFormats();
	e->accept();
}
