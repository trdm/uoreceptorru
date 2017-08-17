#include "uomainwindow.h"
#include "uomdiobserver.h"
#include <QApplication>
#include <QDockWidget>
#include <QMenuBar>
#include <QMdiSubWindow>
#include <QSettings>

uoMainWindow::uoMainWindow(QWidget *parent) :
    uoComonMainWnd(parent)
{
    uoMdiObserver::instance()->setVersion(tr("uoReceptor"),"0.5 (a)"); //

    QString ttl = QString(tr("uoReceptor"));
    ttl.append(" ").append(uoMdiObserver::instance()->version());
    setWindowTitle(ttl);
    m_dbMan = new uoDbManager;
    createTables();
    setupGui();


}

uoMainWindow::~uoMainWindow(){
    QSettings set("trdmSoft", "uoReceptor");
    set.setValue("receptTemplDocWidget_visible",m_receptTemplDocWidget->isVisible());

}

void uoMainWindow::setupGui(){
    createActions();
    createMemu();
    prepareConections();
    menuFillStep1();
    menuFillStep2();
    QSettings set("trdmSoft", "uoReceptor");
    m_receptTemplDocWidget->setVisible(set.value("receptTemplDocWidget_visible",false).toBool());

}

void uoMainWindow::onNewRecept(){
    uoMdiRichTextEdit * te = new uoMdiRichTextEdit(this,1);
    connect(this, SIGNAL(updateRecept(QString)), te, SLOT(onSetHTML(QString)));
    te->setCompleter(m_dbMan->getComleter());
    addSubWindow(te);
    te->show();
//    sw->resize(500, 400);
}

void uoMainWindow::createTables()
{
	QString dbName = qApp->applicationDirPath() + QDir::separator()+"main.db";
#ifdef Q_OS_LINUX
	QDir home = QDir::home();
	if (!home.exists(".uoReceptor")) {
		home.mkpath(".uoReceptor");
	}
	dbName = home.absolutePath() + "/.uoReceptor/" + QDir::separator()+"main.db";
#endif


	m_dbMan->setupDataBase(dbName);
	m_receptTable = m_dbMan->addRefTable("ШаблоныPецептов","Шаблоны рецептов",6,120);
	m_receptTable->setOption(codeType_Number, true);
	m_receptTable->addField("Комментарий", fieldType_String,150);
	m_templField = m_receptTable->addField("Шаблон", fieldType_TextHTML);
	uoMdiObserver::instance()->m_receptTableId = m_receptTable->m_baseNom;

	uoDbRefTable* tmlpTabl = m_dbMan->addRefTable("Лекарства","Лекарства",6, 70);
	tmlpTabl->addField("ЛатИмя",fieldType_String,70)->m_descr = "Латинское наименование";
	tmlpTabl->addField("Описание",fieldType_Text);
	tmlpTabl->setOption(codeType_Number, true);

	tmlpTabl = m_dbMan->addRefTable("Симптомы","Симптомы",6, 100);
	tmlpTabl->setOption(codeType_Number, true);
	tmlpTabl->addField("Описание",fieldType_Text);

	tmlpTabl = m_dbMan->addRefTable("Заболевания","Заболевания",6, 100);
	tmlpTabl->setOption(codeType_Number, true);
	tmlpTabl->addField("Описание",fieldType_Text);

	m_dbMan->restructuring();//  Реструктуризация
	m_dbMan->createGuiFactory(this);
	m_receptTemplDocWidget = new QDockWidget(tr("Recept template"), this);
	m_receptTemplDocWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

	uoUrl url = m_receptTable->getUoUrlByForm(uoFormT_Tree);
	url.setWithElem(true);
	m_docTreeFormRecept = m_dbMan->getForm(url);
	if (m_docTreeFormRecept) {
		m_docTreeFormRecept->setParent(m_receptTemplDocWidget);
		m_receptTemplDocWidget->setWidget(m_docTreeFormRecept);
		addDockWidget(Qt::LeftDockWidgetArea, m_receptTemplDocWidget);
		m_actionShowDocRecept = m_receptTemplDocWidget->toggleViewAction();
	}

}


void uoMainWindow::onActivateUrl(const uoUrl &url){
    uoElementData* elData = m_dbMan->getElement(url);
    if (elData ) {
        if (url.table() == m_receptTable->dbName()){
            QString templ = elData->data(m_templField->dbName()).toString();
            if (!templ.isEmpty()){
                if (uoMdiRichTextEdit::instanceCount() == 0)
                    onNewRecept();
                emit updateRecept(templ);
            }
        }
    }
}

void uoMainWindow::editFile_01(){
	editText(UO_DB_FILE_LEKARSTVO,"Лекарства"); // "lek.txt"
}
void uoMainWindow::editFile_02(){
	editText(UO_DB_FILE_SIMPTOMS,"Симптомы"); // "simp.txt"
}
void uoMainWindow::editFile_03(){
	editText(UO_DB_FILE_ZABOLEVANIE,"Заболевания"); // "disease.txt"
}

void uoMainWindow::createActions()
{
	m_editFile_01	= new QAction(tr("Лекарства (текст)"), this );
	m_editFile_02	= new QAction(tr("Симптомы (текст)"), this );
	m_editFile_03	= new QAction(tr("Заболевания (текст)"), this );

	connect( m_editFile_01, SIGNAL(triggered()), this, SLOT(editFile_01()) );
	connect( m_editFile_02, SIGNAL(triggered()), this, SLOT(editFile_02()) );
	connect( m_editFile_03, SIGNAL(triggered()), this, SLOT(editFile_03()) );
}

void uoMainWindow::createMemu()
{
    m_menuRecept = new QMenu(tr("&References"), this);
    m_dbMan->fillMenu(m_menuRecept);
    //m_menuRecept->addActions(m_dbMan->getActions());
    m_menuRecept->addSeparator();
    m_menuRecept->addAction(m_actionShowDocRecept);
    m_menuRecept->addSeparator();
    m_menuRecept->addAction(m_editFile_01);
    m_menuRecept->addAction(m_editFile_02);
    m_menuRecept->addAction(m_editFile_03);

}


void uoMainWindow::prepareConections()
{
    connect(m_actNewRecept, SIGNAL(triggered()), this, SLOT(onNewRecept()));
}

void uoMainWindow::menuFillStep1()
{
    uoComonMainWnd::menuFillStep1();
}

void uoMainWindow::menuFillStep2()
{
    menuBar()->addMenu(m_menuRecept);
    uoComonMainWnd::menuFillStep2();
}
