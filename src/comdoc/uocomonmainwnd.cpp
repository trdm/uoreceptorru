#include "uocomonmainwnd.h"
#include "uoabstractmdichild.h"
#include "uomdiobserver.h"
#include <QApplication>
#include <QDockWidget>
#include <QEvent>
#include <QMenuBar>
#include <QToolBar>
#include <QMenu>
#include <QSignalMapper>
#include <QSettings>

uoComonMainWnd::uoComonMainWnd(QWidget *parent) :
    QMainWindow(parent)
{
    uoMdiObserver::instance()->setMainWnd(this);
    uoMdiObserver::instance()->setVersion(tr("uoComonEditor"),"0.6(a)");

    QString ttl = QString(tr("uoComonEditor"));
    ttl.append(" ").append(uoMdiObserver::instance()->version());
    setWindowTitle(ttl);

    m_menuNewFile = 0;
    m_area = new QMdiArea;
    m_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_area->setActivationOrder(QMdiArea::ActivationHistoryOrder);
    //m_area->setViewMode(QMdiArea::TabbedView);
    m_area->setViewMode(QMdiArea::SubWindowView);
    setCentralWidget(m_area);
    connect(m_area, SIGNAL(subWindowActivated(QMdiSubWindow*)),this, SLOT(subWindowActivated(QMdiSubWindow*)));
    m_windowMapper = new QSignalMapper(this);
    connect(m_windowMapper, SIGNAL(mapped(QWidget*)),this, SLOT(setActiveSubWindow(QWidget*)));
    setupGui();
	m_linkprop = 0;
}

uoComonMainWnd::~uoComonMainWnd(){
    QSettings set("trdmSoft", "uoReceptor");
    set.setValue("tbFile_visible",m_tbFileTVAct->isChecked());
    set.setValue("tbEditSimple_visible",m_tbEditSimpleTVAct->isChecked());
    set.setValue("tbEditFont_visible",m_tbEditFontTVAct->isChecked());
    set.setValue("tbEditTable_visible",m_tbEditTableTVAct->isChecked());
    set.setValue("tbEditLists_visible",m_tbEditListsTVAct->isChecked());
    set.setValue("tbSearch_visible",m_tbSearchTVAct->isChecked());
}

void uoComonMainWnd::setupGui(){
    createActions();
    createMemu();
    prepareConections();
    menuFillStep1();
    menuFillStep2();
    enableActions(false);
    QSettings set("trdmSoft", "uoReceptor");
    m_tbFile->setVisible(set.value("tbFile_visible",true).toBool());
    m_tbEditSimple->setVisible(set.value("tbEditSimple_visible",true).toBool());
    m_tbEditFont->setVisible(set.value("tbEditFont_visible",true).toBool());
    m_tbEditTable->setVisible(set.value("tbEditTable_visible",true).toBool());
    m_tbEditLists->setVisible(set.value("tbEditLists_visible",true).toBool());
    m_tbSearch->setVisible(set.value("tbSearch_visible",true).toBool());
}

void uoComonMainWnd::enabledActFont(bool ena)
{
    m_actFontBold->setEnabled(ena);
    m_actFontItalic->setEnabled(ena);
    m_actFontUnderline->setEnabled(ena);
}

void uoComonMainWnd::enableActions(bool enable){
    m_actCopy->setEnabled(enable);
    m_actCut->setEnabled(enable);
    m_actPaste->setEnabled(enable);
    m_actUndo->setEnabled(enable);
    m_actRedo->setEnabled(enable);
    m_actSave->setEnabled(enable);
    m_actFontBold->setEnabled(enable);
    m_actFontItalic->setEnabled(enable);
    m_actFontUnderline->setEnabled(enable);
    m_alignGroup->setEnabled(enable);
    m_listGroup->setEnabled(enable);
    m_fontComboBox->setEnabled(enable);
    m_comboSize->setEnabled(enable);
    m_actionTextColor->setEnabled(enable);
    m_actionBGColor->setEnabled(enable);
    m_actionHiperLink->setEnabled(enable);
    m_actionTableInsert->setEnabled(enable);
    m_actionTableEdit->setEnabled(enable);
    m_actionTableMergeCell->setEnabled(enable);
    m_actionTableRemoveRow->setEnabled(enable);
    m_actionTableRemoveCool->setEnabled(enable);
    m_actionTableAppendRows->setEnabled(enable);
    m_actionTableAppendCools->setEnabled(enable);
    m_searchCombo->setEnabled(enable);
    m_actFind->setEnabled(enable);
    m_actFindUp->setEnabled(enable);
    m_actFindDown->setEnabled(enable);
#ifndef QT_NO_PRINTER
    m_filePrint->setEnabled(enable);
    m_filePrintPreview->setEnabled(enable);
    m_fileExportPdf->setEnabled(enable);
#endif
    m_clear->setEnabled(enable);
    m_actSave->setEnabled(m_area->currentSubWindow());

}

QMdiSubWindow* uoComonMainWnd::addTab(QWidget *wi, QString name){
	QMdiSubWindow* sw = addSubWindow(wi);
	wi->show();
	return sw;
}

QWidget *uoComonMainWnd::findByDbURL(QString url){
	QMdiSubWindow *sw = 0;
	QWidget* wi = 0;
	if (!m_area->subWindowList().isEmpty()) {
		for (int i = 0; i<m_area->subWindowList().count(); i++){
			sw = m_area->subWindowList().at(i);
			if (sw){
				wi = sw->widget();
				if (wi){
					if (wi->objectName() == url){
						m_area->setActiveSubWindow(sw);
						break;
					}
					wi = 0;
				}
			}
			wi = 0;
		}
	}
	if (wi)
		sw->show();
	return wi;

}

void uoComonMainWnd::editText(QString fName, QString caption){
    if (fName.isEmpty())
        return;
    QString fNameFull = qApp->applicationDirPath() +"/"+ fName;
#ifdef Q_OS_LINUX
    QDir home = QDir::home();
    if (!home.exists(".uoReceptor")) {
           home.mkpath(".uoReceptor");
       }
    fNameFull = home.absolutePath() + "/.uoReceptor" + "/"+ fName;
#endif

    QMdiSubWindow* sw = 0;
    QWidget* wi = 0;
    uoMdiTextEdit* te = 0;
    QString on;
    for (int i = 0; i<m_area->subWindowList().count(); i++){
        sw = m_area->subWindowList().at(i);
        if (sw){ wi = sw->widget();
            if (wi){
                on = wi->objectName();
                if (on == "uoMdiTextEdit"){
                    if (te = (uoMdiTextEdit*)wi){
                        if (te->name() == caption){
                            m_area->setActiveSubWindow(sw);
                            break;
                        }
                        te = 0;
                    }
                }
            }
        }
    }
    if (!te) {
        te = new uoMdiTextEdit( this);
        te->setName(caption);
        te->load(fNameFull);
        addSubWindow(te);
        te->show();
	}
}

void uoComonMainWnd::connectinsForRichTextEdit(uoMdiRichTextEdit *te)
{
	getLinkPropDlg();
	connect(m_linkprop, SIGNAL(removeLink()), te, SLOT(removeLink()));
	connect(m_linkprop, SIGNAL(updateLink(QString,QString)), te, SLOT(updateLink(QString,QString)));
}

void uoComonMainWnd::subWindowActivated(QMdiSubWindow *sw)
{
    if (!sw)
        return;
    activateDeactivate(sw);
}

void uoComonMainWnd::onNewFile(){
    uoMdiTextEdit* te = new uoMdiTextEdit(this);
   addSubWindow(te);
    te->show();
//    sw->resize(500, 400);
}


void uoComonMainWnd::onNewQHTML()
{
    uoMdiRichTextEdit * te = new uoMdiRichTextEdit(this);
    addSubWindow(te);
	connectinsForRichTextEdit(te);
    te->show();
//    sw->resize(500, 400);

}



void uoComonMainWnd::onOpenFile(){
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"",
        tr("All types (*.txt *.htm *.html *.qhtml);;Text (*.txt);;html (*.htm *.html *.qhtml)"));
    if (fileName.isEmpty())
        return;
    QFileInfo fInfo(fileName);
    QString suffix = fInfo.suffix().toLower();
    QWidget* wi = findByFileName(fileName, true);
    if (!wi){
        if (suffix == "htm" || suffix == "html" || suffix == "qhtml"){
            uoMdiRichTextEdit* rte = new uoMdiRichTextEdit(this);
            rte->load(fileName);
            addSubWindow(rte);
			connectinsForRichTextEdit(rte);
            rte->show();
        } else if (suffix == "txt"){
            uoMdiTextEdit* rte = new uoMdiTextEdit(this);
            rte->load(fileName);
            addSubWindow(rte);
            rte->show();
        }
    }
}

void uoComonMainWnd::onShowHelp(){
    ///\todo prodolgit tut
    QString fileName = "index.html";
//    QFileInfo inf(fileName);
//    fileName = inf.absoluteFilePath() + QDir::separator()+inf.absolute()
    //    QWidget* wi = findByFileName(inf.absoluteFilePath());
}

void uoComonMainWnd::onShowAbout(){
    QString text = QString("%1\nver\n %2").arg(windowTitle()).arg(uoMdiObserver::instance()->versionAll());
    text.append("\n (c) trdm");
    text.append("\n Troshin Dmitry <trdmval@gmail.com>");
    text.append("\n ICQ 308-779-620");
    text.append("\n jabber trdm@jabber.ru");
    QMessageBox::information(this,windowTitle(),text);
}

void uoComonMainWnd::updateWindowMenu(){
    m_menuWindows->clear();
    m_menuWindows->addMenu(m_menuToolBars);

    m_menuWindows->addAction(m_closeAct);
    m_menuWindows->addAction(m_closeAllAct);
    m_menuWindows->addSeparator();
    m_menuWindows->addAction(m_tileAct);
    m_menuWindows->addAction(m_cascadeAct);
    m_menuWindows->addSeparator();
    m_menuWindows->addAction(m_nextAct);
    m_menuWindows->addAction(m_previousAct);
    m_menuWindows->addAction(m_separatorAct);

    QList<QMdiSubWindow *> windows = m_area->subWindowList();
    m_separatorAct->setVisible(!windows.isEmpty());
    QMdiSubWindow *curent = m_area->currentSubWindow();
    QMdiSubWindow *next = 0;
    for (int i = 0; i < windows.size(); ++i) {
        next = windows.at(i);
        QWidget *child = qobject_cast<QWidget *>(next->widget());
        if (!child)
            continue;

        QString text;
        if (i < 9) {
            text = tr("&%1 %2").arg(i + 1).arg(child->windowTitle());
        } else {
            text = tr("%1 %2").arg(i + 1).arg(child->windowTitle());
        }
        QAction *action  = m_menuWindows->addAction(text);
        action->setCheckable(true);
        action ->setChecked(curent == next);
        connect(action, SIGNAL(triggered()), m_windowMapper, SLOT(map()));
        m_windowMapper->setMapping(action, windows.at(i));
    }

}



void uoComonMainWnd::deactivateOther(QMdiSubWindow *sw)
{
    enableActions(false);
    QList<QMdiSubWindow *> list = m_area->subWindowList ();
    QMdiSubWindow *swn = 0;
    for(int i=0; i<list.count(); i++) {
        swn = list.at(i);
        if (swn != sw)
            activateDeactivate(swn, false);
    }
}

void uoComonMainWnd::activateDeactivate(QMdiSubWindow *sw, bool activate)
{
    if (!sw)
        return;

    QWidget* wi = sw->widget();
    if (!wi)
        return;
    QString on = wi->objectName();
    menuFillStep1();
    if (activate) {
        deactivateOther(sw);
    }
    if (on == "uoMdiTextEdit"){
        if (uoMdiTextEdit* ob = (uoMdiTextEdit*)wi) {
            if (activate)  ob->onActivate(this);
            else           ob->onDeactivate(this);
         }
    }
    if (on == "uoMdiRichTextEdit"){
        if (uoMdiRichTextEdit* ob = (uoMdiRichTextEdit*)wi) {
            if (activate)  ob->onActivate(this);
            else           ob->onDeactivate(this);
         }
    }


    if (activate)
        menuFillStep2();

}

QWidget *uoComonMainWnd::findByFileName(QString file, bool show)
{
    QList<QMdiSubWindow *> list = m_area->subWindowList ();
    QMdiSubWindow *swn = 0;
    QWidget* wi = 0;
    for(int i=0; i<list.count(); i++) {
        if (swn = list.at(i)){
            if (wi = swn->widget()){
                if (uoMdiTextEdit* te = qobject_cast<uoMdiTextEdit*>(wi)){
                    if (file == te->fileName())
                        break;
                }
                if (uoMdiRichTextEdit* te = qobject_cast<uoMdiRichTextEdit*>(wi)){
                    if (file == te->fileName())
                        break;
                }
                wi = 0;
                swn = 0;
            }
        }
        wi = 0;
        swn = 0;
    }
    if (show && swn)
        m_area->setActiveSubWindow(swn);
    return wi;

}


QMdiSubWindow* uoComonMainWnd::addSubWindow(QWidget *wi){
    QMdiSubWindow* sw = m_area->addSubWindow(wi);
    Q_ASSERT(sw);
	return sw;
}

LinkProperties *uoComonMainWnd::getLinkPropDlg()
{
	if (!m_linkprop){
		m_linkprop = new LinkProperties(this);
	}
	return m_linkprop;
}


void uoComonMainWnd::createActions()
{
    setIconSize(QSize(16,16));
    m_actGrpFile = new QActionGroup(this);
    m_actNew = new QAction(QIcon(":images/new.png"), tr("New"), m_actGrpFile);
    m_actNewRecept = new QAction(QIcon(":images/newrecept.png"), tr("New recept"), m_actGrpFile);
    m_actNewQHTML = new QAction(QIcon(":images/html_add.png"), tr("New qhtml"), m_actGrpFile);
    m_actOpen = new QAction(QIcon(":images/open.png"),tr("Open"), m_actGrpFile);
    m_actOpen->setShortcut(QKeySequence("Ctrl+O")); //   onOpenFile()
    m_actSave = new QAction(QIcon(":images/save.png"),tr("Save"), m_actGrpFile);
    m_actSave->setShortcut( QKeySequence("Ctrl+S") );
    m_actQuit = new QAction(tr("Quit"), m_actGrpFile);
    m_actHelp = new QAction(tr("Help"), m_actGrpFile);
    m_actAbout = new QAction(tr("About"), m_actGrpFile);

    m_actGrpEditSimple = new QActionGroup(this);

    m_actUndo = new QAction(QIcon(":images/undo.png"), tr("Undo"), m_actGrpEditSimple);
    m_actRedo = new QAction(QIcon(":images/redo.png"), tr("Redo"), m_actGrpEditSimple);
    m_actCopy = new QAction(QIcon(":images/copy.png"), tr("Copy"), m_actGrpEditSimple);
    m_actCut = new QAction(QIcon(":images/cut.png"), tr("Cut"), m_actGrpEditSimple);
    m_actPaste = new QAction(QIcon(":images/paste.png"), tr("Paste"), m_actGrpEditSimple);

    m_actGrpFind = new QActionGroup(this);
    m_actFind = new QAction(QIcon(":images/Search.png"), tr("Undo"), m_actGrpEditSimple);
    m_actFindUp = new QAction(QIcon(":images/SearchUp.png"), tr("Undo"), m_actGrpEditSimple);
    m_actFindDown = new QAction(QIcon(":images/SearchDown.png"), tr("Undo"), m_actGrpEditSimple);

    m_actEditFont = new QActionGroup(this);
    m_actFontBold = new QAction(QIcon(":images/text_bold.png"), tr("Bold"), this);
    m_actFontItalic = new QAction(QIcon(":images/text_italic.png"), tr("Italic"), this);
    m_actFontUnderline  = new QAction(QIcon(":images/text_underline.png"), tr("Underline"), this);

    m_actFontBold->setCheckable(true);
    m_actFontItalic->setCheckable(true);
    m_actFontUnderline->setCheckable(true);
    m_actFontBold->setShortcut( QKeySequence("Ctrl+B") );
    m_actFontItalic->setShortcut( QKeySequence("Ctrl+I") );
    m_actFontUnderline->setShortcut( QKeySequence("Ctrl+U") );

	m_alignGroup		= new QActionGroup(this);
	m_actionAlignLeft		= new QAction( tr("Left"), m_alignGroup );
	m_actionAlignCenter	= new QAction( tr("Center"), m_alignGroup );
	m_actionAlignJustify	= new QAction( tr("Justify"), m_alignGroup );
	m_actionAlignRight	= new QAction( tr("Right"), m_alignGroup );

	m_actionAlignRight->setCheckable( true );
	m_actionAlignLeft->setCheckable( true );
	m_actionAlignCenter->setCheckable( true );
	m_actionAlignJustify->setCheckable( true );

	m_actionAlignCenter	->setIcon( QIcon(":/images/text_align_center.png") );
	m_actionAlignJustify	->setIcon( QIcon(":/images/text_align_justify.png") );
	m_actionAlignLeft		->setIcon( QIcon(":/images/text_align_left.png") );
	m_actionAlignRight	->setIcon( QIcon(":/images/text_align_right.png") );

	m_actionAlignRight->setShortcut( QKeySequence("Ctrl+R") );
	m_actionAlignLeft->setShortcut( QKeySequence("Ctrl+L") );
	m_actionAlignCenter->setShortcut( QKeySequence("Ctrl+E") );
	m_actionAlignJustify->setShortcut( QKeySequence() );

	m_listGroup		= new QActionGroup(this);
	m_actionListDisc	= new QAction( "*)", m_listGroup );
	m_actionListCircle	= new QAction( "()", m_listGroup );
	m_actionListSquare	= new QAction( "[]", m_listGroup );
	m_actionListDecmial	= new QAction( "1)", m_listGroup );
	m_actionListLowerAlpha	= new QAction( "a)", m_listGroup );
	m_actionListUpperAlpha	= new QAction( "A)", m_listGroup );

	m_actionListDisc->setCheckable( true );
	m_actionListCircle->setCheckable( true );
	m_actionListSquare->setCheckable( true );
	m_actionListDecmial->setCheckable( true );
	m_actionListLowerAlpha->setCheckable( true );
	m_actionListUpperAlpha->setCheckable( true );

	m_fontComboBox = new QFontComboBox;
	m_fontComboBox->setObjectName("Choose font family");
	m_fontComboBox->setFixedWidth(150);

	m_searchCombo = new QComboBox;
	m_searchCombo->setEditable(true);
	m_searchCombo->setFixedWidth(150);
	m_actFind = new QAction( tr("Search"), this );
	m_actFind->setIcon(QIcon(":/images/Search.png"));

	m_comboSize = new QComboBox;
	QFontDatabase db;
	m_comboSize->setObjectName("Choose font size");
	m_comboSize->setEditable(true);
	foreach(int size, db.standardSizes())
		m_comboSize->addItem(QString::number(size));
	m_comboSize->setFixedWidth(50);

	m_actionTextColor	= new QAction(tr("&Color..."), this);
	m_actionTextColor	->setIcon( QIcon(":/images/colorfont.png") );

	m_actionBGColor	= new QAction(tr("&Color..."), this);
	m_actionBGColor	->setIcon( QIcon(":/images/colorbgraund.png") );
	m_actionHiperLink = new QAction(tr("&Hyperlink..."), this);
	m_actionHiperLink	->setIcon( QIcon(":images/hyperlink.png") );

	m_actionTableInsert = new QAction(QIcon(":images/table_add.png"),tr("Table add"),this);
	m_actionTableEdit = new QAction(QIcon(":images/table_edit.png"),tr("Table edit"),this);
	m_actionTableMergeCell = new QAction(QIcon(":images/table_merge_cell.png"),tr("Merge cell"),this);

	m_actionTableRemoveRow = new QAction(QIcon(":images/table_row_delete.png"),tr("RemoveRow"),this);
	m_actionTableAppendRows = new QAction(QIcon(":images/table_row_insert.png"),tr("Append rows"),this);

	m_actionTableRemoveCool = new QAction(QIcon(":images/table_col_delete.png"),tr("Remove cool"),this);
	m_actionTableAppendCools = new QAction(QIcon(":images/table_col_insert.png"),tr("AppendCools"),this);

	m_filePrint = 0;
	m_filePrintPreview = 0;
#ifndef QT_NO_PRINTER
	m_filePrint = new QAction(tr("&Print..."), this);
	m_filePrint->setShortcut(QKeySequence::Print);

	m_filePrintPreview = new QAction(tr("Print Preview..."), this);

	m_filePrint			->setIcon( QIcon(":/images/printer.png") );
	m_filePrintPreview	->setIcon( QIcon(":/images/preview.png") );

#endif
	m_fileExportPdf = new QAction(tr("&Export PDF..."), this);
	m_fileExportPdf		->setIcon( QIcon(":/images/exportpdf.png") );

	m_clear = new QAction(tr("Clear"), this);
	m_clear			->setIcon( QIcon(":/images/receptclear.png") );
	// windows menu
	m_closeAct = new QAction(tr("Cl&ose"), this);
	m_closeAct->setStatusTip(tr("Close the active window"));

    m_closeAllAct = new QAction(tr("Close &All"), this);
    m_closeAllAct->setStatusTip(tr("Close all the windows"));

    m_tileAct = new QAction(tr("&Tile"), this);
    m_tileAct->setStatusTip(tr("Tile the windows"));

    m_cascadeAct = new QAction(tr("&Cascade"), this);
    m_cascadeAct->setStatusTip(tr("Cascade the windows"));

    m_nextAct = new QAction(tr("Ne&xt"), this);
    m_nextAct->setShortcuts(QKeySequence::NextChild);
    m_nextAct->setStatusTip(tr("Move the focus to the next window"));

    m_previousAct = new QAction(tr("Pre&vious"), this);
    m_previousAct->setShortcuts(QKeySequence::PreviousChild);
    m_previousAct->setStatusTip(tr("Move the focus to the previous "
                                 "window"));

    m_separatorAct = new QAction(this);
    m_separatorAct->setSeparator(true);
}

void uoComonMainWnd::createMemu()
{
    m_menuFile = new QMenu(tr("&File"), this);
    m_menuNewFile = new QMenu(tr("&FileNew"), this);
    m_menuNewFile->addAction(m_actNew);
    m_menuNewFile->addAction(m_actNewQHTML);
    m_menuNewFile->addAction(m_actNewRecept);
    m_menuFile->addMenu(m_menuNewFile);
    m_menuFile->addSeparator();
    m_menuFile->addAction(m_actOpen);
    m_menuFile->addAction(m_actSave);
    m_menuFile->addAction(m_fileExportPdf);
    m_menuFile->addSeparator();
    m_menuFile->addAction(m_clear);
    m_menuFile->addSeparator();
    m_menuFile->addAction(m_filePrint);
    m_menuFile->addAction(m_filePrintPreview);
    m_menuFile->addSeparator();
    m_menuFile->addAction(m_actQuit);

    m_menuWindows = new QMenu(tr("&Windows"), this);
    m_menuToolBars = new QMenu(tr("&ToolBars"), this);
    updateWindowMenu();

    m_menuHelp = new QMenu(tr("&Help"), this);
    m_menuHelp->addAction(m_actHelp);
    m_menuHelp->addAction(m_actAbout);

    m_tbFile = new QToolBar(tr("File"),this);
    m_tbFileTVAct = m_tbFile->toggleViewAction();
    addToolBar(m_tbFile);
    m_tbFile->addAction(m_actNew);
    m_tbFile->addSeparator();
    m_tbFile->addAction(m_actNewQHTML);
    m_tbFile->addAction(m_actNewRecept);
    m_tbFile->addAction(m_clear);
    m_tbFile->addAction(m_actOpen);
    m_tbFile->addAction(m_actSave);
    m_tbFile->addSeparator();
    m_tbFile->addAction(m_filePrint);
    m_tbFile->addAction(m_filePrintPreview);
    m_tbFile->addAction(m_fileExportPdf);

    m_tbEditSimple = new QToolBar(tr("TextEdit"),this);
    addToolBar(m_tbEditSimple);
    m_tbEditSimpleTVAct = m_tbEditSimple->toggleViewAction();
    m_tbEditSimple->addAction(m_actUndo);
    m_tbEditSimple->addAction(m_actRedo);
    m_tbEditSimple->addSeparator();
    m_tbEditSimple->addAction(m_actCopy);
    m_tbEditSimple->addAction(m_actCut);
    m_tbEditSimple->addAction(m_actPaste);

    m_tbSearch = new QToolBar(tr("Search"),this);
    m_tbSearchTVAct = m_tbSearch->toggleViewAction();
    m_tbSearch->addAction(m_actFind);
    m_tbSearch->addWidget(m_searchCombo);
    m_tbSearch->addAction(m_actFindDown);
    m_tbSearch->addAction(m_actFindUp);
    addToolBar(m_tbSearch);

    addToolBarBreak();

    m_tbEditFont = new QToolBar(tr("Font"),this);
    addToolBar(m_tbEditFont);
    m_tbEditFontTVAct = m_tbEditFont->toggleViewAction();
    m_tbEditFont->addAction(m_actFontBold);
    m_tbEditFont->addAction(m_actFontItalic);
    m_tbEditFont->addAction(m_actFontUnderline);
    m_tbEditFont->addSeparator();
    m_tbEditFont->addAction(m_actionAlignLeft);
    m_tbEditFont->addAction(m_actionAlignCenter);
    m_tbEditFont->addAction(m_actionAlignRight);
    m_tbEditFont->addAction(m_actionAlignJustify);
    m_tbEditFont->addSeparator();
    m_tbEditFont->addWidget( (QWidget*)m_fontComboBox );
    m_tbEditFont->addWidget( (QWidget*)m_comboSize );
    m_tbEditFont->addSeparator();
    m_tbEditFont->addAction(m_actionTextColor);
    m_tbEditFont->addAction(m_actionBGColor);
    m_tbEditFont->addAction(m_actionHiperLink);

    m_tbEditLists = new QToolBar(tr("Edit list"),this);
    addToolBar(m_tbEditLists);
    m_tbEditListsTVAct =  m_tbEditLists->toggleViewAction();
    m_tbEditLists->addAction(m_actionListDisc);
    m_tbEditLists->addAction(m_actionListCircle);
    m_tbEditLists->addAction(m_actionListSquare);
    m_tbEditLists->addAction(m_actionListDecmial);
    m_tbEditLists->addAction(m_actionListLowerAlpha);
    m_tbEditLists->addAction(m_actionListUpperAlpha);

    m_tbEditTable = new QToolBar(tr("Edit table"),this);
    addToolBar(m_tbEditTable);
    m_tbEditTableTVAct = m_tbEditTable->toggleViewAction();

    m_tbEditTable->addAction(m_actionTableInsert);
    m_tbEditTable->addAction(m_actionTableEdit);
    m_tbEditTable->addAction(m_actionTableMergeCell);
    m_tbEditTable->addAction(m_actionTableRemoveRow);
    m_tbEditTable->addAction(m_actionTableRemoveCool);
    m_tbEditTable->addAction(m_actionTableAppendRows);
    m_tbEditTable->addAction(m_actionTableAppendCools);

    m_menuToolBars->addAction(m_tbFileTVAct);
    m_menuToolBars->addAction(m_tbEditSimpleTVAct);
    m_menuToolBars->addAction(m_tbSearchTVAct);
    m_menuToolBars->addAction(m_tbEditFontTVAct);
    m_menuToolBars->addAction(m_tbEditTableTVAct);
    m_menuToolBars->addAction(m_tbEditListsTVAct);

}

void uoComonMainWnd::prepareConections()
{
    connect(m_actNew,       SIGNAL(triggered()), this, SLOT(onNewFile()));
    connect(m_actNewQHTML, SIGNAL(triggered()), this, SLOT(onNewQHTML()));
    connect(m_actOpen, SIGNAL(triggered()), this, SLOT(onOpenFile()));
    connect(m_actHelp, SIGNAL(triggered()), this, SLOT(onShowHelp()));
    connect(m_actAbout, SIGNAL(triggered()), this, SLOT(onShowAbout()));
    connect(m_actQuit, SIGNAL(triggered()), this, SLOT(close()));
    //    connect( actionSave, SIGNAL(triggered()), this, SLOT(fileSave()) );
    connect(m_closeAct, SIGNAL(triggered()),  m_area, SLOT(closeActiveSubWindow()));
    connect(m_closeAllAct, SIGNAL(triggered()), m_area, SLOT(closeAllSubWindows()));
    connect(m_tileAct, SIGNAL(triggered()), m_area, SLOT(tileSubWindows()));
    connect(m_cascadeAct, SIGNAL(triggered()), m_area, SLOT(cascadeSubWindows()));
    connect(m_nextAct, SIGNAL(triggered()),      m_area, SLOT(activateNextSubWindow()));
    connect(m_previousAct, SIGNAL(triggered()),m_area, SLOT(activatePreviousSubWindow()));
    connect(m_menuWindows, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

}

void uoComonMainWnd::menuFillStep1(){
    menuBar()->clear();
    menuBar()->addMenu(m_menuFile);
}
void uoComonMainWnd::menuFillStep2(){
    menuBar()->addMenu(m_menuWindows);
    menuBar()->addMenu(m_menuHelp);
}

void uoComonMainWnd::setActiveSubWindow(QWidget *window)
{
    if (!window)
        return;
    m_area->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

