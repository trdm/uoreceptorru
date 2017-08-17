#include "uodbreftableqmodel.h"
#include <QtGui>
#include <QtSql>
#include <QFont>
#include <QFontMetrics>
#include "uodbmanager.h"


uoDBRefTableModel::uoDBRefTableModel(uoDbRefTableListIface *ifase)
    : QSqlQueryModel(ifase), m_uoIFase(ifase) {
    m_parent = UO_EMPTY_ELEMENT;
    refresh();
}

Qt::ItemFlags uoDBRefTableModel::flags(
        const QModelIndex &index) const {

    Qt::ItemFlags flags = QSqlQueryModel::flags(index);
    return flags;
}

QVariant uoDBRefTableModel::data(
            const QModelIndex &index,
            int role) const {

    QVariant value = QSqlQueryModel::data(index, role);
    bool isMark = false;
    bool isFolder = false;
    int col = index.column();
    int row = index.row();
    if (Qt::DecorationRole == role){
        if (m_uoIFase->colNo_isFolder() != -1){
            isFolder = QSqlQueryModel::data( createIndex(row, m_uoIFase->colNo_isFolder()) , Qt::EditRole).toBool();
        }
        if (m_uoIFase->colNo_isMark() != -1){
            isMark = QSqlQueryModel::data( createIndex(row, m_uoIFase->colNo_isMark()) , Qt::EditRole).toBool();
        }
    }

    switch (role) {

    case Qt::DisplayRole: // Данные для отображения
    case Qt::EditRole:    // Данные для редактирования
        if (col == 0)
            return QString("");
        return value;
    case Qt::FontRole: // Шрифт
    case Qt::BackgroundColorRole:
    case Qt::CheckStateRole:
    case Qt::TextColorRole: // Цвет текста
        return value;

    case Qt::TextAlignmentRole: // Выравнивание
         return int(Qt::AlignLeft | Qt::AlignVCenter);
         // Размер ячейки
    case Qt::DecorationRole:
    {
        if (col == 0)
            return m_uoIFase->getIcon(isFolder,  isMark);
        break;
    }
    case Qt::SizeHintRole:{
        int width = m_uoIFase->getLeghtCol(index.column());
        int height = QFontMetrics(qApp->font()).height();
        return QSize(width, height);
        }
    }
    return value;
}

bool uoDBRefTableModel::setData(
            const QModelIndex &index,
            const QVariant &value,
            int /* role */) {
    if (index.column() < 1 || index.column() > 4)
        return false;

    QModelIndex primaryKeyIndex = QSqlQueryModel::index(
                index.row(), 0);
    bool ok = true;
    refresh();
    return ok;
}

int uoDBRefTableModel::getElementByIndex(const QModelIndex &index)
{
	int retVal = QSqlQueryModel::data( createIndex(index.row() , m_uoIFase->colNo_ID()) , Qt::EditRole).toInt();
	//if (retVal == 0)		retVal = -1;
	return retVal;
}

void uoDBRefTableModel::setParentElem(const int parent)
{
	m_parent = parent;
	refresh();
}

void uoDBRefTableModel::setParent(int parent){
	m_parent = parent;
}



void uoDBRefTableModel::refresh() {
    if (!m_uoIFase)
        return;
    m_uoIFase->setupQueryToModel(this, m_parent);
    //emit refreshComplit();

}
