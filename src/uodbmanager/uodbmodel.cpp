#include "uodbmodel.h"
#include "uodefines.h"
#include <QFont>

uoDbModelNode::uoDbModelNode(uoDbModelNode *parentNode,
		   const QString& s1,
		   const QString& s2,
		   const QString& s3,
		 const QString& s4 )
	: str1(s1), str2(s2), str3(s3),strSQL(s4),
	  parent(parentNode),m_table(0),m_fields(0)
{
}

uoDbModelNode::~uoDbModelNode(){
	qDeleteAll(children);
}

uoDbModel::uoDbModel(QObject *parent) :
    QAbstractItemModel(parent)
{
	rootNode = 0;
}


void uoDbModel::setManager(uoDbManager* dbManager)
{
	m_dbManager = dbManager;
	uoDbModelNode *root = new uoDbModelNode();
	rootNode = root;

	uoDbRefTable* table;
	uoDbField* field;
	uoDbModelNode *group1 = 0;
	uoDbModelNode *group2 = 0;
	QString dbNameOftabl;
	for (int i = 0; i < m_dbManager->m_tables.size(); ++i) {

		table = m_dbManager->m_tables.at(i);
		dbNameOftabl = table->dbName();
		group1 = new uoDbModelNode(root, dbNameOftabl,		table->m_name, table->m_descr, table->m_createSQL );
		group1->m_table = table;
		root->children.append(group1);

		for(int jj = 0; jj< table->m_fields.size(); ++jj) {
			field = table->m_fields.at(jj);
			dbNameOftabl = field->dbName();
			group2 = new uoDbModelNode(group1, dbNameOftabl,		field->m_name, field->m_descr );
			group2->m_fields = field;
			group1->children.append(group2);
		}
	}
}


//uoDbModel::~uoDbModel(){
//	delete rootNode;
//}

QModelIndex uoDbModel::index(int row, int column,
							 const QModelIndex& parent) const {
	if (rootNode) {
		uoDbModelNode *parentNode = nodeFromIndex(parent);
		if (parentNode)
			if (parentNode->children.size()>0)
			return createIndex(row, column,
							   parentNode->children[row]);
	}
	return QModelIndex();
};

bool uoDbModel::hasChildren(const QModelIndex& parent) const {
	uoDbModelNode *parentNode = nodeFromIndex(parent);
	if (!parentNode) return false;
	else return (parentNode->children.count() > 0);
}

QModelIndex uoDbModel::parent(const QModelIndex& index) const {
	uoDbModelNode *node = nodeFromIndex(index);
	if (!node)
		return QModelIndex();

	uoDbModelNode *parentNode = node->parent;
	if (!parentNode)
		return QModelIndex();

	uoDbModelNode *grandparentNode = parentNode->parent;
	if (!grandparentNode)
		return QModelIndex();

	int row = grandparentNode->children.indexOf(parentNode);
	return createIndex(row, index.column(), parentNode);
}

int uoDbModel::rowCount(const QModelIndex& index) const {
	uoDbModelNode *node = nodeFromIndex(index);
	if ( node )
		return node->children.count();
	else
		return 0;
}

int uoDbModel::columnCount(const QModelIndex& /* index */ ) const {
	return 4;
}

QVariant uoDbModel::data(const QModelIndex& index, int role) const {
	int col = index.column();
	uoDbModelNode *node = nodeFromIndex(index);
	switch (role) {
		case Qt::DisplayRole: // Данные для отображения
		case Qt::ToolTipRole:
		case Qt::EditRole: {   // Данные для редактирования
			if (!node) return QVariant();
			else if (col == 0) return node->str1;
			else if (col == 1) return node->str2;
			else if (col == 2) return node->str3;
			else if (col == 3) return node->strSQL;
			else return QVariant();
		}
		case Qt::TextColorRole: // Цвет текста
			return qVariantFromValue(QColor(0, 0, 0));

		case Qt::TextAlignmentRole: // Выравнивание
			if (col == 0)
				return int(Qt::AlignLeft | Qt::AlignVCenter);
			else if (col == 1)
				return int(Qt::AlignLeft | Qt::AlignVCenter);
			//else if (col == 2)	return int(Qt::AlignRight | Qt::AlignVCenter);
			else return QVariant();

		case Qt::FontRole: { // Шрифт
			QFont font = QFont("Helvetica", 9);
			if (node->children.count() > 0)
				return qVariantFromValue(
					QFont("Helvetica", 10, QFont::Bold));
			else
				return qVariantFromValue(QFont("Helvetica", 9));
		}
		case Qt::BackgroundColorRole:  { // Цвет фона
			if (node->children.count() > 0)
				return qVariantFromValue(QColor(200, 230, 240));
			else
				return qVariantFromValue(QColor(255, 255, 255));
		}
		case Qt::CheckStateRole:  // Галочка
			return QVariant();

		case Qt::SizeHintRole:  //  Размер ячейки
			return QSize(150, 18);

		case Qt::DecorationRole: { // Иконка
			if (col == 0 && node->children.count() > 0)
				return ip.icon(QFileIconProvider::Folder);
			else
				return QVariant();
		}
	}
	return QVariant();
}

void uoDbModel::setRootNode(uoDbModelNode *node){
	delete rootNode;
	rootNode = node;
	reset();
}

uoDbModelNode* uoDbModel::nodeFromIndex(const QModelIndex& index) const {
	if (index.isValid()) {
		return static_cast<uoDbModelNode*>(index.internalPointer());
	}else{
		return rootNode;
	}
}

Qt::ItemFlags uoDbModel::flags(const QModelIndex& index) const {
	if ( index.isValid() )
		return Qt::ItemIsEnabled | Qt::ItemIsEditable |
			   Qt::ItemIsSelectable;
	else
		return 0;
}

QVariant uoDbModel::headerData(int section,
							   Qt::Orientation orientation,
							   int role) const {
	if (orientation == Qt::Horizontal &&
			   role == Qt::DisplayRole) {
		if (section == 0)
			return tr("ID поля");
		else if (section == 1)
			return tr("Имя поля");
		else if (section == 2)
			return tr("Представление");
		else if (section == 3)
			return tr("SQL");
	}
	return QVariant();
}

bool uoDbModel::setData(
			const QModelIndex& index,
			const QVariant& value,
			int role) {
	if ( role != Qt::EditRole )
		return false;

	int col = index.column();
	uoDbModelNode *node = nodeFromIndex(index);
	if (col == 0)
		node->str1 = value.toString();
	else if (col == 1)
		node->str2 = value.toString();
	else if (col == 2)
		node->str3 = value.toString();

	return true;
}

