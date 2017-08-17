#ifndef TABLEFORMATDLG_H
#define TABLEFORMATDLG_H
#include <QTextCursor>
#include <QTextTable>
#include <QTextTableCell>
#include <QTextTableFormat>
#include <QColor>
#include <QAction>
#include <QCloseEvent>

#include <QPixmap>


#include "ui_tableformatdlg.h"

class TableFormatDlg : public QDialog, private Ui::TableFormatDlg
{
    Q_OBJECT
    
public:
    explicit TableFormatDlg(QWidget *parent = 0);
    void initFormat(QTextTable *existingtable);
    QTextTableFormat GetNewFormats();
public slots:
    void ChoisColor();
    void UpdateFormats();

protected:
    void changeEvent(QEvent *e);
    void closeEvent( QCloseEvent* e );
private:
    QTextTable* m_curTable;
    QTextTableFormat m_curTableFrm;
    qreal m_borderWth[2];
    qreal m_pading[2];
    qreal m_spasing[2];

    QColor m_bgColor;
    QAction *m_actTableColor;
    QPixmap m_tabColPix;


};

#endif // TABLEFORMATDLG_H
