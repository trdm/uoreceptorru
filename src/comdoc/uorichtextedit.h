#ifndef UORICHTEXTEDIT_H
#define UORICHTEXTEDIT_H

#include "uoabstractmdichild.h"
#include "uotexteditcomplit.h"
#include <QAction>
#include <QActionGroup>
#include <QCloseEvent>
#include <QColorDialog>
#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFontComboBox>
#include <QKeySequence>
#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>
#include <QPixmap>
#include <QPrintDialog>
#include <QPrinter>
#include <QPrintPreviewDialog>
#include <QTextCharFormat>
#include <QTextCodec>
#include <QTextDocumentFragment>
#include <QTextEdit>
#include <QTextList>
#include <QTextStream>



class uoMdiRichTextEdit : public uoTextEditComplit, public uoAbstractMDIChild
{
    Q_OBJECT
public:
    explicit uoMdiRichTextEdit(QWidget *parent = 0, int type=0);
    virtual ~uoMdiRichTextEdit();
    virtual void onActivate(QMainWindow* mWnd );
    virtual void onDeactivate(QMainWindow* mWnd );
    virtual void canCloseClient();
    void fillMenu(QMainWindow* mWnd );
    void connectActions();
    QString fileName() {return m_fileName; }
    void load(QString fileName);

static	int instanceCount();
signals:

public slots:
	bool fileSave();
	bool fileSaveAs();
	bool fileClose();

	void undo_();
	void redo_();
	void cut_();
	void copy_();
	void paste_();
	void textAlign_(QAction *a);
	void onSetHTML(QString html);

	void textChanged_();
	void markBold();
	void markItalic();
	void markUnderline();

private slots:
	void removeLink();
	void updateLink(QString lText, QString lLocation);

//	void on_richText_Modified();
	void alignmentChanged(Qt::Alignment a);
	void cursorPositionChanged_();
	void currentCharFormatChanged_(const QTextCharFormat &format);
	void setList_(QAction *a);
	void fontChanged(const QFont &f);
	void colorChanged_(const QColor &c);
	void textFamily(const QString &f);
	void textSize( QString p );
	void textColor_();
	void textColorBg_();
	void textHiperLinkEdit();

    void nup();
    void tableInsert();
    void tableEdit();
    void MergeCellByCursorPosition();
    void removeRowByCursorPosition();
    void removeCoolByCursorPosition();
    void appendTableRows();
    void appendTableCools();

//	void save();
//	void setDirty();
	void filePrint();
	void filePrintPreview();
	void printPreview(QPrinter *);
	void filePrintPdf();
	void fileClear();


private:
	QTextList* getList();
	void setList( int type ); //void setList( QTextListFormat::Style type );

private:
	void enableActions(bool enable);
	void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
	bool openFile( QString newFile, QTextCodec *c=NULL  );
	bool saveFile( QString newFile, QTextCodec *c=NULL  );
	QMenu	*m_editMenu;
	QTextTable* m_curTable;

	int m_selCur, m_selStart, m_selEnd;

	void createActions();
	QString m_fileName;
	void updateCaption();
	void printerSetup(QPrinter* printer );
	void printerSetupSave(QPrinter* printer );
	int m_tipe; //0-html, 1-html/recept
	QString m_lastAnchor; //url for document in helpwindow
};

#endif // UORICHTEXTEDIT_H
