#include "uomditextedit.h"
#include "uomainwindow.h"
#include "uocomonmainwnd.h"


#include <QDebug>
#include <QMenuBar>
#include "uomdiobserver.h"


uoMdiTextEdit::uoMdiTextEdit(QWidget *parent) :
    QTextEdit(parent),uoAbstractMDIChild("uoMdiTextEdit")
{
    setObjectName("uoMdiTextEdit");
    setAcceptRichText( false );
    setContentsMargins(0, 0, 0, 0);
    setName(QString("text%1.txt").arg(nextDocumentNumber()));
    updateCaption();
    createActions();
    connectActions();
    setTabStopWidth(20);
    connect(this, SIGNAL(textChanged()), this, SLOT(textChanged_()));
    setWindowIcon(QIcon(":/images/new.png"));
}

uoMdiTextEdit::~uoMdiTextEdit()
{}

void uoMdiTextEdit::onActivate(QMainWindow *mWnd){
    fillMenu(mWnd);
    blockSignals(false);
    enableActions(true);
}

void uoMdiTextEdit::onDeactivate(QMainWindow *mWnd){
    blockSignals(true);
    enableActions(false);
}

void uoMdiTextEdit::canCloseClient(){
}

void uoMdiTextEdit::fillMenu(QMainWindow *mWnd)
{
	mWnd->menuBar()->addMenu(m_editMenu);
}

void uoMdiTextEdit::connectActions()
{
	uoComonMainWnd* wnd = uoMdiObserver::instance()->mainWnd();
	connect( this, SIGNAL(copyAvailable(bool)), wnd->m_actCopy, SLOT(setEnabled(bool)) );
	connect( this, SIGNAL(copyAvailable(bool)), wnd->m_actCut, SLOT(setEnabled(bool)) );
	connect( this, SIGNAL(pasteAvailable(bool)), wnd->m_actPaste, SLOT(setEnabled(bool)) );
	connect( this, SIGNAL(undoAvailable(bool)), wnd->m_actUndo, SLOT(setEnabled(bool)) );
	connect( this, SIGNAL(redoAvailable(bool)), wnd->m_actRedo, SLOT(setEnabled(bool)) );

    connect( wnd->m_actUndo, SIGNAL(triggered()), this, SLOT(undo_()) );
    connect( wnd->m_actRedo, SIGNAL(triggered()), this, SLOT(redo_()) );
    connect( wnd->m_actCopy, SIGNAL(triggered()), this, SLOT(copy_()) );
    connect( wnd->m_actCut, SIGNAL(triggered()), this, SLOT(cut_()) );
    connect( wnd->m_actPaste, SIGNAL(triggered()), this, SLOT(paste_()) );
    connect( wnd->m_actSave, SIGNAL(triggered()), this, SLOT(fileSave()) );

}

void uoMdiTextEdit::load(QString fileName){
    if (!fileName.isEmpty())
        openFile(fileName);
}


bool uoMdiTextEdit::fileSave(){
	if (!isActiveMdiWindow(this)) return false;
	if (m_fileName.isEmpty())
		return fileSaveAs();

	return saveFile( m_fileName );
}

bool uoMdiTextEdit::fileSaveAs(){
	QString s = QFileDialog::getSaveFileName(
		NULL,
		tr("Choose a filename to save under"),
				name(),
		"text (*.txt)"
	);

	if (s.isEmpty())
		return false;

	m_fileName = s;
	return saveFile( m_fileName );
}

bool uoMdiTextEdit::fileClose(){
	return true;
}

void uoMdiTextEdit::undo_(){
    if (!isActiveMdiWindow(this)) return;
     undo();
}

void uoMdiTextEdit::redo_(){
    if (!isActiveMdiWindow(this)) return;
    redo();
}

void uoMdiTextEdit::cut_(){
    if (!isActiveMdiWindow(this)) return;
    cut();
}

void uoMdiTextEdit::copy_(){
    if (!isActiveMdiWindow(this)) return;
    copy();

}

void uoMdiTextEdit::paste_(){
    if (!isActiveMdiWindow(this)) return;
    paste();

}

void uoMdiTextEdit::textChanged_(){
    updateCaption();
}

void uoMdiTextEdit::enableActions(bool enable){
    uoComonMainWnd* wnd = uoMdiObserver::instance()->mainWnd();
    if (!enable){
        wnd->enableActions(enable);
        return;
    }
    wnd->m_actUndo->setEnabled(enable);
    wnd->m_actRedo->setEnabled(enable);
    wnd->m_actCopy->setEnabled(enable);
    wnd->m_actCut->setEnabled(enable);
    wnd->m_actPaste->setEnabled(enable);
    wnd->m_actSave->setEnabled(enable);
}

void uoMdiTextEdit::updeteActions()
{
}

bool uoMdiTextEdit::openFile(QString newFile, QTextCodec *c){
    m_fileName = newFile;		// the full path of the loaded file

    // the name of the object for it's mdi server
    // is the file name alone, without the directory
    updateCaption();
    if (newFile.isEmpty())
        return true;

    QFile f( m_fileName );

    if (!f.open( QIODevice::ReadOnly ) )
        return false;

    if (c == NULL)
        c = QTextCodec::codecForName("UTF-8");

    QTextStream t(&f);
    t.setCodec( c );
    setPlainText( t.readAll() );
    f.close();
    updateCaption();
    return true;

}

bool uoMdiTextEdit::saveFile(QString newFile, QTextCodec *c){
	QFile f( newFile );

	if (!f.open( QIODevice::WriteOnly ) )
		return false;

	if (c == NULL)
		c = QTextCodec::codecForName("UTF-8");

	QTextStream t(&f);
	t.setCodec( c );
	t << toPlainText();
	f.close();
	document()->setModified(false);
	updateCaption();
	return true;

}

void uoMdiTextEdit::createActions()
{
	uoComonMainWnd* wnd = uoMdiObserver::instance()->mainWnd();
	m_editMenu = new QMenu(tr("Edit"),this);
	m_editMenu->addAction(wnd->m_actUndo);
	m_editMenu->addAction(wnd->m_actRedo);
	m_editMenu->addSeparator();
	m_editMenu->addAction(wnd->m_actCopy);
	m_editMenu->addAction(wnd->m_actCut);
	m_editMenu->addAction(wnd->m_actPaste);

}

void uoMdiTextEdit::updateCaption(){
	QString capt = m_fileName;
	if (!name().isEmpty())
		capt = name();
	if (document()->isModified())
		capt.append(" *");
	setWindowTitle(capt);
}
