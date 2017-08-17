#include "uodbmanagerdlg.h"
#include <QDir>
#include <QTreeView>
#include <QVBoxLayout>
#include <QSplitter>
#include <QTextEdit>



uoDbManagerDlg::uoDbManagerDlg(QWidget *parent, uoDbManager* dbMan) :
	QDialog(parent)
{
	if (!dbMan)
		m_dbMan = new uoDbManager;
	else
		m_dbMan = dbMan;

	m_model = new uoDbModel;
	m_model->setRootNode(new uoDbModelNode);
	m_model->setManager(m_dbMan);
	QTreeView* view = new QTreeView(this);
	view->setSelectionMode(QAbstractItemView::SingleSelection);
	view->setSelectionBehavior(QAbstractItemView::SelectItems);
	view->setEditTriggers(QAbstractItemView::NoEditTriggers);
	view->setModel(m_model);
	view->expandAll();
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(view);
	resize(300, 300);
}

uoDbManagerDlg::~uoDbManagerDlg()
{
}
