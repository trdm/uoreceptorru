#include "uodbreftreemodel.h"


uoTreeRMNode::uoTreeRMNode(uoTreeRMNode *parent, int ID, QString title)
	:m_nmID(ID),m_parent(parent),m_title(title),m_level(-1)
{
	m_modified = false;
	m_populated= false; /// измененный
	m_isMark = false;
	m_isFolder = true;
	m_parent = parent;
	if (m_parent){
		m_level = m_parent->m_level +1;
	}

	m_childrenCount = 0;
}

uoTreeRMNode::~uoTreeRMNode()
{
	m_parent = 0;
	qDeleteAll(m_children);
	m_children.clear();
	m_childrenCount = 0;
}

/// перехват создания узлов с проверкой
uoTreeRMNode* uoTreeRMNode::createNode(uoTreeRMNode *parent, int ID, QString title)
{
	uoTreeRMNode* pNode = NULL;
	if (!parent){
		pNode = new uoTreeRMNode(NULL, ID, title);
	} else {
		pNode = new uoTreeRMNode(parent, ID, title);
		parent->m_children.append(pNode);
		parent->m_childrenCount = parent->m_children.count();
	}
	return pNode;

}

uoDbRefTreeModel::uoDbRefTreeModel(uoDbRefTableListIface *ifase) :
	QAbstractItemModel(ifase),m_uoIFase(ifase)
{
	m_rootNode = new uoTreeRMNode(0,-2, m_uoIFase->m_table->m_descr);
	reset();
}

uoDbRefTreeModel::~uoDbRefTreeModel()
{
	delete m_rootNode;
}

QModelIndex uoDbRefTreeModel::index(int row, int col, const QModelIndex &parent) const
{
	uoTreeRMNode* nodeP = nodeFromIndex(parent);
	if (!nodeP)
		return QModelIndex();
	if(!nodeP->m_populated && m_uoIFase)
		m_uoIFase->populate(nodeP);
	uoTreeRMNode* child = nodeP->m_children.value(row);
	if (child)
		return createIndex(row, col, child);
	return QModelIndex();

}
uoTreeRMNode* uoDbRefTreeModel::nodeFromIndex( const QModelIndex &index) const
{
	if (index.isValid()) {
		return static_cast<uoTreeRMNode*>(index.internalPointer());
	}	else {
		return m_rootNode;
	}
}


QModelIndex uoDbRefTreeModel::parent(const QModelIndex &child) const
{
	uoTreeRMNode* chNode = nodeFromIndex(child);
	if (!chNode)		return QModelIndex();

	uoTreeRMNode* parentNode = chNode->m_parent;
	if (!parentNode)		return QModelIndex();

	uoTreeRMNode* grandParentNode = parentNode->m_parent;
	if (!grandParentNode)		return QModelIndex();

	int row = grandParentNode->m_children.indexOf(parentNode);
	return createIndex(row, child.column(), parentNode);


}

int uoDbRefTreeModel::rowCount(const QModelIndex &parent) const
{
	uoTreeRMNode* parNode = nodeFromIndex(parent);
	if (!parNode)
		return 0;
	if (!parNode->m_populated && m_uoIFase)
		m_uoIFase->populate(parNode);

	return parNode->m_children.count();

}
int uoDbRefTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
	return 1;
}
QVariant uoDbRefTreeModel::data(const QModelIndex &index, int role) const
{

	uoTreeRMNode* pNode = nodeFromIndex(index);
	if (!pNode)	return QVariant();
	int col = index.column();

	if (role == Qt::DisplayRole){
		return pNode->m_title;
	} else if (role == Qt::DecorationRole){
		if (col == 0){
			if (pNode){
				return m_uoIFase->getIcon(pNode->m_isFolder, pNode->m_isMark == 1 ? true : false );
				return QIcon(":/uodb/images/element.png");
			}
		}
		return m_uoIFase->getIcon(true, false);
	}

	return QVariant();

}
QVariant uoDbRefTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(role);
    Q_UNUSED(section);
	if(orientation == Qt::Horizontal && m_uoIFase)
		return m_uoIFase->m_table->m_descr;
	return QVariant();
}

int uoDbRefTreeModel::getElementByIndex(const QModelIndex &index)
{
	int retVal = UO_EMPTY_ELEMENT;
	uoTreeRMNode* pNode = nodeFromIndex(index);
	if (pNode)
		return pNode->m_nmID;
	return retVal;
}

int uoDbRefTreeModel::refresh(const QModelIndex &index)
{
	uoTreeRMNode* pNode = nodeFromIndex(index);
//	int	ID = pNode->m_nmID;
	pNode = 0;
	beginResetModel();
	if (m_rootNode)
		m_rootNode->m_populated = false;
		qDeleteAll(m_rootNode->m_children);
		m_rootNode->m_children.clear();
		m_rootNode->m_childrenCount = 0;
		m_rootNode->m_parent = 0;
		delete m_rootNode;
		m_rootNode = new uoTreeRMNode(0,-2, m_uoIFase->m_table->m_descr);
	reset();
	endResetModel();
    return 0;
}
