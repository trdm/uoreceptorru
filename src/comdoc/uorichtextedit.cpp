#include "uorichtextedit.h"
#include "uomdiobserver.h"
#include "uomainwindow.h"
#include "uocomonmainwnd.h"
#include "newtabledlg.h"
#include "tableformatdlg.h"
#include "linkproperty.h"
#include <QDebug>
#include <QMenuBar>
#include <QTextDocumentFragment>
#include <QInputDialog>
#include <QTextTable>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QSettings>
#include <QPrintEngine>
#include "linkproperty.h"

static int instanceCountOfuoMdiRichTextEdit = 0;

uoMdiRichTextEdit::uoMdiRichTextEdit(QWidget *parent, int type) :
    uoTextEditComplit(parent),uoAbstractMDIChild("uoMdiRichTextEdit"),m_tipe(type)
{
    setObjectName("uoMdiRichTextEdit");
    if (type == 0) {
        setName(QString("text%1.html").arg(nextDocumentNumber()));
        setWindowIcon(QIcon(":/images/html.png"));
    } else {
        setName(QString("Recept%1.html").arg(nextDocumentNumber()));
        setWindowIcon(QIcon(":/images/newrecept.png"));

    }
    setTabStopWidth(20);
    m_curTable = 0;
    updateCaption();
    createActions();
    connectActions();
    if (m_tipe == 1) // onli for resepts
        instanceCountOfuoMdiRichTextEdit += 1;
    connect(this, SIGNAL(textChanged()), this, SLOT(textChanged_()));
	m_selStart = m_selEnd = -1;

}

uoMdiRichTextEdit::~uoMdiRichTextEdit(){
    if (m_tipe == 1)
        instanceCountOfuoMdiRichTextEdit -= 1;
}

void uoMdiRichTextEdit::onActivate(QMainWindow *mWnd){
    blockSignals(false);
    fillMenu(mWnd);
    uoComonMainWnd* wnd = uoMdiObserver::instance()->mainWnd();
    wnd->enabledActFont(true);
    enableActions(true);


}

void uoMdiRichTextEdit::onDeactivate(QMainWindow *mWnd){
    blockSignals(true);
    uoComonMainWnd* wnd = uoMdiObserver::instance()->mainWnd();
    wnd->enabledActFont(false);
    enableActions(false);
}

void uoMdiRichTextEdit::canCloseClient()
{
}

void uoMdiRichTextEdit::fillMenu(QMainWindow *mWnd)
{
	mWnd->menuBar()->addMenu(m_editMenu);
}

void uoMdiRichTextEdit::connectActions()
{
	uoComonMainWnd* wnd = uoMdiObserver::instance()->mainWnd();
	connect( this, SIGNAL(copyAvailable(bool)), wnd->m_actCopy, SLOT(setEnabled(bool)) );
	connect( this, SIGNAL(copyAvailable(bool)), wnd->m_actCut, SLOT(setEnabled(bool)) );
//	connect( this, SIGNAL(pasteAvailable(bool)), wnd->m_actPaste, SLOT(setEnabled(bool)) );
	connect( this, SIGNAL(undoAvailable(bool)), wnd->m_actUndo, SLOT(setEnabled(bool)) );
	connect( this, SIGNAL(redoAvailable(bool)), wnd->m_actRedo, SLOT(setEnabled(bool)) );

    connect( wnd->m_actUndo, SIGNAL(triggered()), this, SLOT(undo_()) );
    connect( wnd->m_actRedo, SIGNAL(triggered()), this, SLOT(redo_()) );
    connect( wnd->m_actCopy, SIGNAL(triggered()), this, SLOT(copy_()) );
    connect( wnd->m_actCut, SIGNAL(triggered()), this, SLOT(cut_()) );
    connect( wnd->m_actPaste, SIGNAL(triggered()), this, SLOT(paste_()) );
    connect( wnd->m_actSave, SIGNAL(triggered()), this, SLOT(fileSave()) );

    connect( wnd->m_actFontBold, SIGNAL(triggered()), this, SLOT( markBold()) );
    connect( wnd->m_actFontItalic, SIGNAL(triggered()), this, SLOT( markItalic()) );
    connect( wnd->m_actFontUnderline, SIGNAL(triggered()), this, SLOT( markUnderline()) );
    connect( this	, SIGNAL(currentCharFormatChanged(const QTextCharFormat &)), this, SLOT(currentCharFormatChanged_(const QTextCharFormat &)));
    connect( wnd->m_alignGroup	, SIGNAL(triggered(QAction *)), this, SLOT(textAlign_(QAction *)));
    connect( this	, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged_()));
    connect( wnd->m_listGroup	, SIGNAL(triggered(QAction *)), this, SLOT(setList_(QAction *)));

	connect( wnd->m_fontComboBox	, SIGNAL(activated(const QString &)), this, SLOT(textFamily(const QString &)));
	connect( wnd->m_comboSize	, SIGNAL(activated(const QString &)), this, SLOT(textSize(const QString &)));
	connect( wnd->m_actionTextColor, SIGNAL(triggered()), this, SLOT(textColor_()));
	connect( wnd->m_actionBGColor, SIGNAL(triggered()), this, SLOT(textColorBg_()));
	connect( wnd->m_actionHiperLink, SIGNAL(triggered()), this, SLOT(textHiperLinkEdit()));

	connect(wnd->m_actionTableInsert, SIGNAL(triggered()), this, SLOT(tableInsert()));
	connect(wnd->m_actionTableEdit, SIGNAL(triggered()), this, SLOT(tableEdit()));
	connect(wnd->m_actionTableMergeCell, SIGNAL(triggered()), this, SLOT(MergeCellByCursorPosition()));
	connect(wnd->m_actionTableRemoveRow , SIGNAL(triggered()), this, SLOT(removeRowByCursorPosition()));
	connect(wnd->m_actionTableRemoveCool , SIGNAL(triggered()), this, SLOT(removeCoolByCursorPosition()));

	connect(wnd->m_actionTableAppendRows , SIGNAL(triggered()), this, SLOT(appendTableRows()));
	connect(wnd->m_actionTableAppendCools , SIGNAL(triggered()), this, SLOT(appendTableCools()));
#ifndef QT_NO_PRINTER
    connect(wnd->m_filePrint, SIGNAL(triggered()), this, SLOT(filePrint()));;
    connect(wnd->m_filePrintPreview, SIGNAL(triggered()), this, SLOT(filePrintPreview()));
    connect(wnd->m_fileExportPdf, SIGNAL(triggered()), this, SLOT(filePrintPdf()));
#endif

    connect(wnd->m_clear, SIGNAL(triggered()), this, SLOT(fileClear()));
}

void uoMdiRichTextEdit::load(QString fileName)
{
    openFile(fileName);
}

int uoMdiRichTextEdit::instanceCount()
{
    return instanceCountOfuoMdiRichTextEdit;
}

void uoMdiRichTextEdit::enableActions(bool enable)
{
	uoComonMainWnd* wnd = uoMdiObserver::instance()->mainWnd();
	wnd->m_actCopy->setEnabled(enable);
	wnd->m_actCut->setEnabled(enable);
	wnd->m_actPaste->setEnabled(enable);
	wnd->m_actUndo->setEnabled(enable);
	wnd->m_actRedo->setEnabled(enable);
	wnd->m_actSave->setEnabled(enable);
	wnd->m_actFontBold->setEnabled(enable);
	wnd->m_actFontItalic->setEnabled(enable);
	wnd->m_actFontUnderline->setEnabled(enable);
	wnd->m_alignGroup->setEnabled(enable);
	wnd->m_listGroup->setEnabled(enable);
	wnd->m_fontComboBox->setEnabled(enable);
	wnd->m_comboSize->setEnabled(enable);
	wnd->m_actionTextColor->setEnabled(enable);
	wnd->m_actionBGColor->setEnabled(enable);
	wnd->m_actionHiperLink->setEnabled(enable);
	wnd->m_actionTableInsert->setEnabled(enable);
	wnd->m_actionTableEdit->setEnabled(enable);
	wnd->m_actionTableMergeCell->setEnabled(enable);
	wnd->m_actionTableRemoveRow->setEnabled(enable);
	wnd->m_actionTableRemoveCool->setEnabled(enable);
	wnd->m_actionTableAppendRows->setEnabled(enable);
	wnd->m_actionTableAppendCools->setEnabled(enable);
#ifndef QT_NO_PRINTER
	wnd->m_filePrint->setEnabled(enable);
	wnd->m_filePrintPreview->setEnabled(enable);
	wnd->m_fileExportPdf->setEnabled(enable);
#endif
	wnd->m_clear->setEnabled(enable);
	wnd->m_searchCombo->setEnabled(enable);
	wnd->m_actFind->setEnabled(enable);

}


void uoMdiRichTextEdit::createActions()
{
	uoComonMainWnd* wnd = uoMdiObserver::instance()->mainWnd();
	m_editMenu = new QMenu(tr("Edit"),this);
	m_editMenu->addAction(wnd->m_actUndo);
	m_editMenu->addAction(wnd->m_actRedo);
	m_editMenu->addSeparator();
	m_editMenu->addAction(wnd->m_actCopy);
	m_editMenu->addAction(wnd->m_actCut);
	m_editMenu->addAction(wnd->m_actPaste);
	m_editMenu->addSeparator();
	m_editMenu->addAction(wnd->m_actFontBold);
	m_editMenu->addAction(wnd->m_actFontItalic);
	m_editMenu->addAction(wnd->m_actFontUnderline);

}

void uoMdiRichTextEdit::updateCaption(){
	QString capt = m_fileName;
	if (!name().isEmpty())
		capt = name();
	if (document()->isModified())
		capt.append(" *");
	setWindowTitle(capt);
}




bool uoMdiRichTextEdit::fileSave(){
	if (!isActiveMdiWindow(this)) return false;
	if (m_fileName.isEmpty())
		return fileSaveAs();

	return saveFile( m_fileName );
}

bool uoMdiRichTextEdit::fileSaveAs(){
	QString s = QFileDialog::getSaveFileName(NULL, tr("Choose a filename to save under"), name(), "qt-html (*.html)");

	if (s.isEmpty())
		return false;

	m_fileName = s;
	QFileInfo info(s);
	setName(info.fileName());
	return saveFile( m_fileName );
}

bool uoMdiRichTextEdit::fileClose(){
	return true;
}

void uoMdiRichTextEdit::undo_(){
    if (!isActiveMdiWindow(this)) return;
     undo();
}

void uoMdiRichTextEdit::redo_(){
    if (!isActiveMdiWindow(this)) return;
    redo();
}

void uoMdiRichTextEdit::cut_(){
    if (!isActiveMdiWindow(this)) return;
    cut();
}

void uoMdiRichTextEdit::copy_(){
    if (!isActiveMdiWindow(this)) return;
    copy();

}

void uoMdiRichTextEdit::paste_(){
    if (!isActiveMdiWindow(this)) return;
    paste();

}

void uoMdiRichTextEdit::textAlign_(QAction *a){
    if (!isActiveMdiWindow(this)) return;
    uoComonMainWnd* wnd = uoMdiObserver::instance()->mainWnd();
    if (a == wnd->m_actionAlignLeft)        setAlignment(Qt::AlignLeft);
    else if (a == wnd->m_actionAlignCenter) setAlignment(Qt::AlignHCenter);
    else if (a == wnd->m_actionAlignRight)  setAlignment(Qt::AlignRight);
    else if (a == wnd->m_actionAlignJustify)  setAlignment(Qt::AlignJustify);
}

void uoMdiRichTextEdit::onSetHTML(QString html){
    if (!isActiveMdiWindow(this)) return;
    setHtml(html);
}

void uoMdiRichTextEdit::textChanged_(){
    updateCaption();
}

void uoMdiRichTextEdit::markBold()
{
	if (!isActiveMdiWindow(this)) return;
	QTextCharFormat fmt;
	uoComonMainWnd* mwnd = uoMdiObserver::instance()->mainWnd();
	fmt.setFontWeight( mwnd->m_actFontBold->isChecked() ? QFont::Bold : QFont::Normal );
	mergeFormatOnWordOrSelection( fmt );
}

void uoMdiRichTextEdit::markItalic()
{
	if (!isActiveMdiWindow(this)) return;
	QTextCharFormat fmt;
	uoComonMainWnd* mwnd = uoMdiObserver::instance()->mainWnd();
	fmt.setFontItalic( mwnd->m_actFontItalic->isChecked() );
	mergeFormatOnWordOrSelection( fmt );
}

void uoMdiRichTextEdit::markUnderline()
{
	if (!isActiveMdiWindow(this)) return;
	QTextCharFormat fmt;
	uoComonMainWnd* mwnd = uoMdiObserver::instance()->mainWnd();
	fmt.setFontUnderline( mwnd->m_actFontUnderline->isChecked() );
	mergeFormatOnWordOrSelection( fmt );
}

void uoMdiRichTextEdit::removeLink(){
	if (!isActiveMdiWindow(this)) return;
	// from Research Assistant -> https://sourceforge.net/projects/rassistant/
	QTextCursor cursor = textCursor();
	cursor.setPosition(m_selStart, QTextCursor::MoveAnchor);
	cursor.setPosition(m_selEnd, QTextCursor::KeepAnchor);
	QString s =	cursor.selectedText();;
	cursor.removeSelectedText();
	cursor.insertText(s);
	//!+! убрать форматирование текста
}

void uoMdiRichTextEdit::updateLink(QString lText, QString lLocation){
	if (!isActiveMdiWindow(this)) return;
	// from Research Assistant -> https://sourceforge.net/projects/rassistant/
	QTextCursor cursor = textCursor();
	QString s =	lText;
	if (lLocation.isEmpty()){
		removeLink();
	}else{
		s = "<a href=\"" + lLocation +"\">"+ lText +"</a>";
		qDebug() << "s = " << s;
		QTextDocumentFragment fragment = QTextDocumentFragment::fromHtml(s);
		cursor.setPosition(m_selStart, QTextCursor::MoveAnchor);
		cursor.setPosition(m_selEnd, QTextCursor::KeepAnchor);
		cursor.removeSelectedText();
		cursor.insertFragment(fragment);
		textCursor().setPosition(m_selCur, QTextCursor::MoveAnchor);
   }

}

void uoMdiRichTextEdit::alignmentChanged(Qt::Alignment a)
{
	uoComonMainWnd* wnd = uoMdiObserver::instance()->mainWnd();
	if (a & Qt::AlignLeft) {			wnd->m_actionAlignLeft->setChecked(true);
	} else if (a & Qt::AlignHCenter) {	wnd->m_actionAlignCenter->setChecked(true);
	} else if (a & Qt::AlignRight) {	wnd->m_actionAlignRight->setChecked(true);
	} else if (a & Qt::AlignJustify) {	wnd->m_actionAlignJustify->setChecked(true);
	}

}

QTextList* uoMdiRichTextEdit::getList()
{
	QTextCursor cursor = textCursor();
	return cursor.currentList();
}

void uoMdiRichTextEdit::setList(int type){
	QTextCursor cursor = textCursor();

	if (type != 0) {
		cursor.beginEditBlock();
		QTextBlockFormat blockFmt = cursor.blockFormat();
		QTextListFormat listFmt;

		if (cursor.currentList()) {
			listFmt = cursor.currentList()->format();
		} else {
			listFmt.setIndent(blockFmt.indent() + 1);
			blockFmt.setIndent(0);
			cursor.setBlockFormat(blockFmt);
		}

		listFmt.setStyle((QTextListFormat::Style)type);
		cursor.createList(listFmt);
		cursor.endEditBlock();
	} else {
		QTextBlockFormat bfmt;
		bfmt.setObjectIndex(-1);
		//setProperty // bfmt.setProperty**		QTextFormat::ListStyle
		cursor.mergeBlockFormat(bfmt);
	}

}


void uoMdiRichTextEdit::cursorPositionChanged_(){
	if (!isActiveMdiWindow(this)) return;
	alignmentChanged( alignment() );
	uoComonMainWnd* wnd = uoMdiObserver::instance()->mainWnd();
	QTextList *l = getList();
	if (l == NULL)
	{
		wnd->m_actionListDisc->setChecked( false );
		wnd->m_actionListCircle->setChecked( false );
		wnd->m_actionListSquare->setChecked( false );
		wnd->m_actionListDecmial->setChecked( false );
		wnd->m_actionListLowerAlpha->setChecked( false );
		wnd->m_actionListUpperAlpha->setChecked( false );
	}
	else
	{
		QTextListFormat::Style style = l->format().style();

		if (style ==  QTextListFormat::ListDisc)
			wnd->m_actionListDisc->setChecked( true ); else
		if (style ==  QTextListFormat::ListCircle)
			wnd->m_actionListCircle->setChecked( true ); else
		if (style ==  QTextListFormat::ListSquare)
			wnd->m_actionListSquare->setChecked( true ); else
		if (style ==  QTextListFormat::ListDecimal)
			wnd->m_actionListDecmial->setChecked( true ); else
		if (style ==  QTextListFormat::ListLowerAlpha)
			wnd->m_actionListLowerAlpha->setChecked( true ); else
		if (style ==  QTextListFormat::ListUpperAlpha)
			wnd->m_actionListUpperAlpha->setChecked( true );
		else
		{
			wnd->m_actionListDisc->setChecked( false );
			wnd->m_actionListCircle->setChecked( false );
			wnd->m_actionListSquare->setChecked( false );
			wnd->m_actionListDecmial->setChecked( false );
			wnd->m_actionListLowerAlpha->setChecked( false );
			wnd->m_actionListUpperAlpha->setChecked( false );
		}
	}


}

void uoMdiRichTextEdit::currentCharFormatChanged_(const QTextCharFormat &format)
{
	if (!isActiveMdiWindow(this)) return;
	fontChanged( format.font() );
	colorChanged_( format.foreground().color() );

}

void uoMdiRichTextEdit::setList_(QAction *a)
{
	uoComonMainWnd* mwnd = uoMdiObserver::instance()->mainWnd();
	if (a == mwnd->m_actionListDisc)		setList( QTextListFormat::ListDisc); else
	if (a == mwnd->m_actionListCircle)		setList( QTextListFormat::ListCircle); else
	if (a == mwnd->m_actionListSquare)		setList( QTextListFormat::ListSquare); else
	if (a == mwnd->m_actionListDecmial)		setList( QTextListFormat::ListDecimal); else
	if (a == mwnd->m_actionListLowerAlpha)	setList( QTextListFormat::ListLowerAlpha); else
	if (a == mwnd->m_actionListUpperAlpha)	setList( QTextListFormat::ListUpperAlpha);

}

void uoMdiRichTextEdit::fontChanged(const QFont &f)
{
	uoComonMainWnd* mwnd = uoMdiObserver::instance()->mainWnd();
	mwnd->m_fontComboBox->setCurrentIndex(mwnd->m_fontComboBox->findText(QFontInfo(f).family()));
	mwnd->m_comboSize->setCurrentIndex(mwnd->m_comboSize->findText(QString::number(f.pointSize())));
	mwnd->m_actFontBold->setChecked(f.bold());
	mwnd->m_actFontItalic->setChecked(f.italic());
	mwnd->m_actFontUnderline->setChecked(f.underline());

}

void uoMdiRichTextEdit::colorChanged_(const QColor &c){
}

void uoMdiRichTextEdit::textFamily(const QString &f){
	if (!isActiveMdiWindow(this)) return;
	QTextCharFormat fmt;
	fmt.setFontFamily(f);
	mergeFormatOnWordOrSelection(fmt);
}

void uoMdiRichTextEdit::textSize(QString p){
	if (!isActiveMdiWindow(this)) return;

	QTextCharFormat fmt;
	fmt.setFontPointSize(p.toFloat());
	mergeFormatOnWordOrSelection(fmt);

}

void uoMdiRichTextEdit::textColor_(){
	if (!isActiveMdiWindow(this)) return;
	QColor col = QColorDialog::getColor(textColor(), this);
	if (!col.isValid())
		return;

	QTextCharFormat fmt;
	fmt.setForeground(col);
	mergeFormatOnWordOrSelection(fmt);
	colorChanged_(col);
}

void uoMdiRichTextEdit::textColorBg_()
{
	if (!isActiveMdiWindow(this)) return;
	QColor col = QColorDialog::getColor(textBackgroundColor(), this);
	if (!col.isValid())
		return;
	setTextBackgroundColor(col);
}

void uoMdiRichTextEdit::textHiperLinkEdit()
{
	///todo - normalno sdelat href/
	// - добавить обработку вставки/редактирования гиперссылки m_actionHiperLink (void Base_Edit::MakehrefLink())

	if (!isActiveMdiWindow(this)) return;
	QString s = "";
	QTextCursor cursor = textCursor();
	QTextCharFormat cf = cursor.charFormat();
	m_selCur = cursor.position();
	if (cf.isAnchor()){		//select whole text of the link.
		while (cf.isAnchor()){	//move anchor to the start of hypertext link
			cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor);
			cf = cursor.charFormat();
			if (cursor.position() == 0)
				break;
		}
		cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
		cf = cursor.charFormat();
		int chrCount = document()->characterCount();
		while (cf.isAnchor()){ //move cursor to the end of hypertext link
			cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
			cf = cursor.charFormat();
			if (cursor.position() == chrCount-1)
				break;
		}
		cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
	}
	if (!cursor.hasSelection())//if there is no selection, select the word under cursor
		cursor.select(QTextCursor::WordUnderCursor);
	// Выделение текста остаётся только в этой процедуре, поэтому используем глоб переменные.
	// Cделать cursor глобальным нельзя, т.к. почему-то курсор всегда и в конце текста и на выделении. Баг qt? Использовать указатель на raEdit::textCursor() не помогло - таже ошибка. Подставил Edit::textCursor() вместо cursor - вообще прога зависать стала при просмотре свойств ссылки.
	m_selStart = cursor.selectionStart();
	m_selEnd = cursor.selectionEnd();
	s = cursor.selectedText();
	LinkProperties* linkprop = uoMdiObserver::instance()->mainWnd()->getLinkPropDlg();
	if (linkprop){
		linkprop->setProperties(s, m_lastAnchor);
		linkprop->show();
	}
}

void uoMdiRichTextEdit::nup(){
}

void uoMdiRichTextEdit::tableInsert()
{
	if (!isActiveMdiWindow(this)) return;
	NewTableDlg* dlg = new NewTableDlg(this);
	if (dlg->exec() == QDialog::Accepted){
		QString subtext, collx, rowx;
		int colonne = dlg->m_columns;
		int righe = dlg->m_rows;
		int largo = dlg->m_widht;
        if (colonne > 0 && righe > 0 && largo > 0) {
		QStringList tables;
		tables.clear();
		tables.append(QString("<table border=\"1\" align=\"center\" width=\"%1%\" cellspacing=\"0\" cellpadding=\"0\" bgcolor=\"#ffffff\">").arg(largo));
		for (int i=0;i<righe;i++){
			tables.append(QString("<tr>"));
			   for (int o=0;o<colonne;o++){
				   tables.append(QString("<td><p></p></td>"));
			   }
			tables.append(QString("</tr>"));
		}
		tables.append(QString("</table>"));

		subtext = tables.join("\n");
		QTextDocumentFragment fragment = QTextDocumentFragment::fromHtml(subtext);
		textCursor().insertFragment(fragment);
		}
	}

}

void uoMdiRichTextEdit::tableEdit(){
	if (!isActiveMdiWindow(this)) return;
	QTextCursor findercursor(textCursor());
	QTextTable* curTable = findercursor.currentTable();
	if (curTable) {
		TableFormatDlg* tfDlg = new TableFormatDlg(this);
		tfDlg->initFormat(curTable);
		if (tfDlg->exec() == QDialog::Accepted){
			QTextTableFormat newformine = tfDlg->GetNewFormats();
			if (newformine.isValid()) {
				curTable->setFormat(newformine);
			}
		}
		delete tfDlg;
	 }

}

void uoMdiRichTextEdit::MergeCellByCursorPosition(){
	if (!isActiveMdiWindow(this)) return;
	QTextCursor findercursor(textCursor());
	m_curTable = findercursor.currentTable();
	if (m_curTable) {
		m_curTable->mergeCells(findercursor);
	}

}

void uoMdiRichTextEdit::removeRowByCursorPosition(){
	if (!isActiveMdiWindow(this)) return;
	QTextCursor findercursor(textCursor());
	QTextTable* m_curTable = findercursor.currentTable();
	if (m_curTable) {
		QTextTableCell existingcell =  m_curTable->cellAt(findercursor);
		int cellrowcursoris = existingcell.row(); /* int value start from zero */
		m_curTable->removeRows(cellrowcursoris,1);
	}
}

void uoMdiRichTextEdit::removeCoolByCursorPosition(){
	if (!isActiveMdiWindow(this)) return;
	QTextCursor findercursor(textCursor());
	m_curTable = findercursor.currentTable();
	if (m_curTable) {
		QTextTableCell existingcell =  m_curTable->cellAt(findercursor);
		int cellcoolcursoris = existingcell.column(); /* int value start from zero */
//		int cellrowcursoris = existingcell.row(); /* int value start from zero */
		m_curTable->removeColumns(cellcoolcursoris,1);
	}

}

void uoMdiRichTextEdit::appendTableRows(){
	if (!isActiveMdiWindow(this)) return;
	bool ok = false;
	QTextCursor findercursor(textCursor());
	m_curTable = findercursor.currentTable();
	if (m_curTable) {
	QTextTableCell existingcell =  m_curTable->cellAt(findercursor);
//    int cellcoolcursoris = existingcell.column(); /* int value start from zero */
    int cellrowcursoris = existingcell.row(); /* int value start from zero */
    int approwtot = QInputDialog::getInteger(this, tr("Append NR. line row"),tr("Row:"), 1, 1, 100, 1, &ok);
        if (ok && approwtot > 0) {
          m_curTable->insertRows(cellrowcursoris + 1,approwtot);
        }
    }

}

void uoMdiRichTextEdit::appendTableCools(){
	if (!isActiveMdiWindow(this)) return;
	bool ok = false;
	QTextCursor findercursor(textCursor());
	m_curTable = findercursor.currentTable();
	if (m_curTable) {
		QTextTableCell existingcell =  m_curTable->cellAt(findercursor);
		int cellcoolcursoris = existingcell.column(); /* int value start from zero */
//        int cellrowcursoris = existingcell.row(); /* int value start from zero */
        int appcooltot = QInputDialog::getInteger(this, tr("Tabella appendi colonne"),tr("Cool:"), 1, 1, 10, 1, &ok);
            if (ok && appcooltot > 0) {
              m_curTable->insertColumns(cellcoolcursoris + 1,appcooltot);
            }
    }
}

void uoMdiRichTextEdit::filePrint(){
    if (!isActiveMdiWindow(this)) return ;

#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (textCursor().hasSelection())
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    dlg->setWindowTitle(tr("Print Document"));
    if (dlg->exec() == QDialog::Accepted) {
        print(&printer);
    }
    delete dlg;
#endif

}

void uoMdiRichTextEdit::filePrintPreview(){
    if (!isActiveMdiWindow(this)) return ;

#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    printerSetup(&printer);
    QPrintPreviewDialog preview(&printer, this);
    preview.setWindowFlags( preview.windowFlags() | Qt::WindowMaximizeButtonHint);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(printPreview(QPrinter*)));
    preview.exec();
#endif
}

void uoMdiRichTextEdit::printPreview(QPrinter *printer)
{
#ifdef QT_NO_PRINTER
    Q_UNUSED(printer);
#else
    print(printer);
    printerSetupSave(printer);
#endif
}

void uoMdiRichTextEdit::printerSetup(QPrinter *printer)
{
    QSettings set("trdmSoft", "uoReceptor");
    qreal pX = set.value("printpageleft",20).toReal();
    qreal pY = set.value("printpagetop",10).toReal();
    qreal pXX = set.value("printpageright",10).toReal();
    qreal pYY = set.value("printpagebottom",10).toReal();

    printer->setPageMargins(pX, pY, pXX , pYY, QPrinter::Millimeter);
}

void uoMdiRichTextEdit::printerSetupSave(QPrinter *printer)
{
    QSettings set("trdmSoft", "uoReceptor");
    qreal pX = 0;
    qreal pY = 0;
    qreal pXX = 0;
    qreal pYY = 0;
    printer->getPageMargins(&pX, &pY, &pXX , &pYY, QPrinter::Millimeter);
    set.setValue("printpageleft",pX);
    set.setValue("printpagetop",pY);
    set.setValue("printpageright",pXX);
    set.setValue("printpagebottom",pYY);

}

void uoMdiRichTextEdit::filePrintPdf(){
    if (!isActiveMdiWindow(this)) return ;

#ifndef QT_NO_PRINTER
//! [0]
    QString fileName = QFileDialog::getSaveFileName(this, "Export PDF",
                                                    QString(), "*.pdf");
    if (!fileName.isEmpty()) {
        if (QFileInfo(fileName).suffix().isEmpty())
            fileName.append(".pdf");
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        document()->print(&printer);
    }
//! [0]
#endif
}
void uoMdiRichTextEdit::fileClear(){
	if (!isActiveMdiWindow(this)) return ;
	clear();
}

void uoMdiRichTextEdit::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
	QTextCursor cursor = textCursor();
	if (!cursor.hasSelection())
		cursor.select(QTextCursor::WordUnderCursor);
	cursor.mergeCharFormat(format);
	mergeCurrentCharFormat(format);
}




bool uoMdiRichTextEdit::openFile(QString newFile, QTextCodec *c){
    m_fileName = newFile;		// the full path of the loaded file


    // the name of the object for it's mdi server
    // is the file name alone, without the directory
    int i = newFile.lastIndexOf( '/' );
    QString s;
    if ( i != -1 )
        s = newFile.mid( i+1 );
    else
    {
        i = newFile.lastIndexOf( '\\' );
        if ( i != -1 )
            s = newFile.mid( i+1 );
        else
            s = newFile;
    }
        setName(s);


    if (newFile.isEmpty())
        return true;

    QFile f( m_fileName );

    if (!f.open( QIODevice::ReadOnly ) )
        return false;

    if (c == NULL)
        c = QTextCodec::codecForLocale();

    QTextStream t(&f);
    t.setCodec( c );
    setHtml( t.readAll() );
    f.close();

    return true;

}

bool uoMdiRichTextEdit::saveFile(QString newFile, QTextCodec *c){
	QFile f( newFile );

	if (!f.open( QIODevice::WriteOnly ) )
		return false;

	if (c == NULL)
		c = QTextCodec::codecForLocale();

	QTextStream t(&f);
	t.setCodec( c );
	t << toHtml();
	f.close();
	document()->setModified(false);
	updateCaption();
	return true;

}

