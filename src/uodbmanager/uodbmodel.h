#ifndef UODBMODEL_H
#define UODBMODEL_H

#include <QAbstractItemModel>
#include <QFileIconProvider>

#include "uodefines.h"
#include "uodbmanager.h"

class uoDbModelNode;
class uoDbRefTable;
class uoDbField;

class uoDbModelNode {
public:
	uoDbModelNode(uoDbModelNode *parentNode = 0,
		const QString& s1 = "",
		const QString& s2 = "",
		const QString& s3 = "",
		const QString& s4 = "");
	//uoDbModelNode(){}
	~uoDbModelNode();
	QString str1, str2, str3, strSQL;
	uoDbModelNode *parent;
	QList<uoDbModelNode*> children;
	uoDbRefTable* m_table;
	uoDbField* m_fields;
};


class uoDbModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit uoDbModel(QObject *parent = 0);
	void setManager(uoDbManager* dbManager);
	void setRootNode(uoDbModelNode *node);
	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
	bool hasChildren(const QModelIndex& parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex& index) const;
	int rowCount(const QModelIndex& index = QModelIndex()) const;
	int columnCount(const QModelIndex& index = QModelIndex()) const;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	Qt::ItemFlags flags(const QModelIndex& index) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

private:
	uoDbManager* m_dbManager;
	uoDbModelNode* rootNode;
	uoDbModelNode *nodeFromIndex(const QModelIndex& index) const;
	QFileIconProvider ip;

signals:

public slots:

};

#endif // UODBMODEL_H
