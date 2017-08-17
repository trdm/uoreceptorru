#ifndef UODBREFTREEMODEL_H
#define UODBREFTREEMODEL_H

#include <QAbstractItemModel>
#include "uodefines.h"
#include "uodbmanager.h"

class uoTreeRMNode {
	friend class uoDbRefTreeModel;

	public:
		uoTreeRMNode* createNode(uoTreeRMNode* parent, int ID, QString title);
		virtual ~uoTreeRMNode();
	public:

		int				m_nmID;
		uoTreeRMNode* 	m_parent;
		QString   		m_title;
		int				m_level;

		bool 			m_modified; /// измененный
		bool 			m_populated; /// заполненный
		bool 			m_isFolder; /// заполненный

		QString   		m_strOther;	/// комментарий например..
		QList<uoTreeRMNode*> m_children;
		bool m_isMark;
	protected:
		uoTreeRMNode( uoTreeRMNode* parent, int ID, QString title);
	private:
		int m_childrenCount;

};

class uoDbRefTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit uoDbRefTreeModel(uoDbRefTableListIface* ifase = 0);
    virtual ~uoDbRefTreeModel();
public:
	QModelIndex index(int row, int col, const QModelIndex &parent) const;
	QModelIndex parent(const QModelIndex &child) const;
	int rowCount(const QModelIndex &parent) const;
	int columnCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	int getElementByIndex(const QModelIndex &index);
	int refresh(const QModelIndex &index);
private:
	uoTreeRMNode* nodeFromIndex( const QModelIndex &index) const;
	mutable uoTreeRMNode* m_rootNode;
signals:

public slots:

private:
    uoDbRefTableListIface* m_uoIFase;

    
};

#endif // UODBREFTREEMODEL_H
