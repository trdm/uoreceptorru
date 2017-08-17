#include "uomdiobserver.h"
#include <QMessageBox>
#include <QDebug>
#include <QSet>
#include <QApplication>
#include <QStringListModel>
#include <QTextCodec>

#include "uodbmanager.h"
#include "uodbmanagerdlg.h"
#include "uodbreftreemodel.h"

uoDbField::uoDbField(QString name, uoDbFieldType fType, int len, int presition)
	:uoDbObject(name, "") {
	m_objType = objType_Field;
	m_numLen = len;
	m_numPresition = presition;
	m_len = m_numLen+1+m_numPresition;
	if (m_len == 1 || (fType == fieldType_Text))
		m_len = 25;
	m_useFild = uoFieldU_All;
	m_fieldType = fType;
}
uoDbField::uoDbField(QString name, uoDbRefTable* table )
	:uoDbObject(name, ""),m_table(table){
	m_objType = objType_Field;
	m_len = 10;
	m_useFild = uoFieldU_All;
	m_fieldType = fieldType_TableRef; 
}

uoDbField::~uoDbField(){
	m_table = 0;
	m_manager = 0;
}

QString uoDbObject::title()
{
	if (!m_descr.isEmpty())
		return m_descr;
	return m_name;
}

QString uoDbField::dbName(){
	QString dbName = QString("sp%1").arg(m_baseNom);
	return dbName;
}

void uoElementData::setDescr(const QString descr){
	if (m_descr != descr){
		m_descr = descr;
		m_dirty = true;
	}

	m_title = descr;
}

void uoElementData::setParentID(const int parent)
{
	if (m_ParentID != parent){
		 m_ParentID = parent;
		 m_dirty = true;
	}
}

void uoElementData::setParent(uoElementData *parent){
	if (m_parent != parent){
		m_parent = parent;
		m_dirty = true;
	}
}

void uoElementData::setData(QString name, QVariant data){
	if (name.isEmpty())
		return;
	if (m_data.contains(name)){
		m_data[name] = data;
		m_dirty = true;
	} else if (name == UO_DB_FIELDNAME_CODE){
		setCode(data.toString());
	} else if (name == UO_DB_FIELDNAME_TITLE || name == UO_DB_FIELDNAME_DESCR){
		setDescr(data.toString());
	} else if (name == UO_DB_FIELDNAME_PARENTID){
		setParentID(data.toInt());
	}
}

void uoElementData::setDataToData(QString name, QVariant data)
{
	if (name.isEmpty())
		return;
	m_data[name] = data;
	m_dirty = true;
}

void uoElementData::setDirty(const bool dir){
	m_dirty = dir;
}

void uoElementData::setFolder(const int fold){
	if (m_isFolder != fold){
		if (fold == 0 || fold == 1)
		m_isFolder = fold;
		setDirty(true);
	}
}

void uoElementData::setTitle(const QString title){
	if (m_title.isEmpty() && !title.isEmpty()){
		m_title = title;
		m_dirty = true;
	}
}

void uoElementData::setTablePtr(uoDbRefTable *tablePtr)
{
	m_tablePtr = tablePtr;
	m_table = tablePtr->dbName();
}

void uoElementData::setCode(const QString code){
	if(m_code != code)	{
		m_code = code;
		m_dirty = true;
	}
	setTitle(code);
}

bool uoElementData::hasData(QString name)
{
	if (!m_tablePtr) return false;
	if (name == UO_DB_FIELDNAME_CODE && m_tablePtr->hasCode()) return true;
	if (name == UO_DB_FIELDNAME_TITLE && m_tablePtr->hasTitle()) return true;
	if (name == UO_DB_FIELDNAME_DESCR && m_tablePtr->hasTitle()) return true;
	if (name == UO_DB_FIELDNAME_PARENTID && m_tablePtr->hasHierarchy()) return true;
	return m_data.contains(name);
}

QVariant uoElementData::data(QString name)
{
	if (hasData(name)){
		if (!m_tablePtr) return QVariant();
		if (name == UO_DB_FIELDNAME_CODE && m_tablePtr->hasCode()) return code();
		if (name == UO_DB_FIELDNAME_TITLE && m_tablePtr->hasTitle()) return descr();
		if (name == UO_DB_FIELDNAME_DESCR && m_tablePtr->hasTitle()) return descr();
		if (name == UO_DB_FIELDNAME_PARENTID && m_tablePtr->hasHierarchy()) return qVariantFromValue(*m_parent);

		return m_data.value(name);
	}
	return QVariant();
}

uoElementData::uoElementData(uoDbRefTable *tabPtr)
	:m_tablePtr(tabPtr)
{
	m_descr = m_code = m_title = "";
	m_id = m_ParentID = UO_EMPTY_ELEMENT;
	m_isFolder = m_isMark = m_verssion = 0;
	m_dirty = false;
	m_parent = 0;
	m_table = m_tablePtr->dbName();
}

bool uoElementData::isFolder(){
	if (m_isFolder == 1)
		return true;
	return false;
}

bool uoElementData::isMark(){
	if (m_isMark == 1)
		return true;
	return false;
}


uoDbField* uoDbRefTable::addField(QString name, uoDbFieldType fType, int len, int presition)
{
	if (fieldExist(name))
		return 0;

	uoDbField* field = new uoDbField(name, fType,  len,  presition);
	m_fields.append(field);
	field->m_manager = m_manager;
	m_manager->registerObj(field);
	return field;
}
uoDbField* uoDbRefTable::addField(QString name, QString m_relTable)
{
	if (fieldExist(name))
		return 0;
	uoDbField* field = new uoDbField(name, fieldType_Number,  8);
	m_fields.append(field);
	m_manager->registerObj(field);
	field->m_manager = m_manager;
	return field;
}

uoDbRefTable::~uoDbRefTable() {
	while(m_fields.takeFirst()){
		delete m_fields.takeFirst();
	}
}


uoDbRefTable::uoDbRefTable(QString name, QString descr, int codelen /* = 4*/, int titleLen/* = 25 */)
	:uoDbObject( name, descr)
{
	m_hasCode = codelen > 0 ? true : false;
	m_objType = objType_RefTable;
	m_hasTitle = titleLen > 0 ? true : false;
	m_codeLen = codelen;
	m_titleLen = titleLen;
	m_codeType = codeType_Number;
	m_hasHierarchy = false;
	m_prefixTab = "sp";
	m_privatTab = false;

}

void uoDbRefTable::setOption(uoDbCodeType codeType, bool hierarchy){
	m_codeType = codeType;
	m_hasHierarchy = hierarchy;
}

///\breaf - ВНИМАНИЕ, ПОСЛЕ ИСПОЛЬЗОВАНИЯ ЭЛЕМЕНТ НУЖНО УДАЛИТЬ ЗАПP АШИВАЮЩЕМУ!!
uoElementData*
uoDbRefTable::elementData(int elem, int level, int parent)
{
	uoElementData* elData = new uoElementData(this);
	if (parent != UO_EMPTY_ELEMENT)
		elData->setParentID(parent);
	if (level < 0)
		return elData;
	level = level - 1;
	if (elem == UO_EMPTY_ELEMENT){
		if (elData->parentId() >0)
			elData->setParent(elementData(elData->parentId(), level));
		return elData;
	}
	QSqlQuery query(m_manager->dataBase());
	QString queStr = QString("SELECT %1, %2 ").arg(UO_DB_FIELDNAME_ISMARK).arg(UO_DB_FIELDNAME_VERSION);
	if (m_hasCode)		queStr.append( QString(" , %1 ").arg(UO_DB_FIELDNAME_CODE) );
	if (m_hasTitle)		queStr.append( QString(" , %1 ").arg(UO_DB_FIELDNAME_TITLE) );
	if (m_hasHierarchy)	queStr.append( QString(" , %1, %2 ").arg(UO_DB_FIELDNAME_ISFOLDER).arg(UO_DB_FIELDNAME_PARENTID) );
	uoDbField* fld = 0;
	for (int i = 0; i<m_fields.count(); i++){
		fld = m_fields.at(i);
		queStr.append( QString(" ,%1 ").arg(fld->dbName()));
	}
	queStr.append(QString("FROM %1 WHERE %2 = %3").arg(dbName()).arg(UO_DB_FIELDNAME_ID).arg(elem));
	query.exec(queStr);
	if (query.lastError().isValid()   )	{
		qDebug() << query.lastError().driverText();
		qDebug() << queStr;
		delete elData;
		return 0;
	}
	int lastFld = 0;
	while (query.next())    {
		elData->setID(elem);
		elData->m_isMark = query.value(0).toInt();
		elData->m_isMarkB = query.value(0).toBool();
		elData->m_verssion = query.value(1).toInt();
		lastFld = 2;
		if (m_hasCode)	 { elData->setCode(query.value(2).toString()); lastFld++; }
		if (m_hasTitle)	 { elData->setDescr(query.value(3).toString()); lastFld++; }
		if (m_hasHierarchy) {
			elData->setFolder(query.value(4).toInt());
			elData->setParentID( query.value(5).toInt());
			if (elData->parentId()>0)
				elData->setParent(elementData(elData->parentId(), level));
			lastFld = lastFld+2;
		}
		lastFld -= 1;
		for (int i = 0; i<m_fields.count(); i++){
			lastFld += 1;
			fld = m_fields.at(i);
			elData->setDataToData(fld->dbName(),query.value(lastFld));
			///\todo а вот тут с агrегатными типами надо подумать...
		}
		elData->setDirty(false);

		break;
	}

	return elData;
}

bool uoDbRefTable::doDeleteElement(uoElementData *elData)
{
	bool retVal = false;
	if (elData && elData->id() != UO_EMPTY_ELEMENT){
//		bool setMark = !elData->isMark();
		int mark = 1-elData->isMarkN();
		QString qStr = "UPDATE ";;
		qStr.append(elData->table()).append(" SET ");
		qStr.append(UO_DB_FIELDNAME_ISMARK).append(" = ?").append(" WHERE ").append(UO_DB_FIELDNAME_ID).append(" = ?");
		QSqlQuery query(manager()->dataBase());
		if (!query.prepare(qStr)){
			manager()->doReportError(query);
			return retVal;
		}
		query.addBindValue(mark);
		query.addBindValue(elData->id());
		if (!query.exec()){
			manager()->doReportError(query);
			return retVal;
		} else {
			retVal = true; // мы уже обновили отметку...
		}
		if (elData->isFolder()){
			QSqlQuery queryF(manager()->dataBase());
			QList<int> dList;
			QSet<int> dSet;
			dList.append(elData->id());
			dSet.insert(elData->id()); // защита от ошибки..
			QString qStrF = "SELECT ";
			qStrF.append(UO_DB_FIELDNAME_ID).append(", ").append(UO_DB_FIELDNAME_ISFOLDER)
					.append(" FROM ").append(elData->table())
					.append(" WHERE ").append(UO_DB_FIELDNAME_PARENTID).append(" = ?");
			if (!queryF.prepare(qStrF)){
				manager()->doReportError(queryF);
				return retVal;
			}
			int IDsel = 0;
			bool isFSel = false;
			while (dList.count()>0){
				int id = dList.at(0); dList.pop_back();
				queryF.addBindValue(id);
				if (queryF.exec()){
					while (queryF.next()){
						IDsel	  = queryF.value(0).toInt();
						isFSel = queryF.value(1).toBool();
						if (isFSel && !dSet.contains(IDsel) && IDsel != UO_EMPTY_ELEMENT){
							dList.append(IDsel);
						}
						query.addBindValue(mark);
						query.addBindValue(IDsel);
						if (!query.exec()){
							manager()->doReportError(query);
							break;
						}
					}
				}
			}
		}
	}
	return retVal;
}

bool uoDbRefTable::save(uoElementData *elementData)
{
	bool retVal = true;
	//INSERT INTO sp1 () VALUES
	if (elementData){
		QString saveQStr = "";
		QString valuesStr = "";
		QString setStr = "";
		QSqlQuery query = QSqlQuery(m_manager->dataBase());
		QList<QVariant> bVal;
		bool needInsert = false;
		if (elementData->id() == UO_EMPTY_ELEMENT){
			setupID(elementData);
			needInsert = true;
			saveQStr.append("INSERT INTO ").append(dbName()).append("(");
			saveQStr.append(UO_DB_FIELDNAME_ID).append(", ");
			valuesStr.append("? ,");
			//query.addBindValue(elementData->id());
			bVal.append(elementData->id());
		} else {
			elementData->m_verssion += 1;
			setStr.append("UPDATE ").append(dbName()).append(" SET ");
		}
		saveQStr.append(UO_DB_FIELDNAME_ISMARK);				valuesStr.append("? "); //.arg(elementData->m_isMark); fldCounter++;
		setStr.append(UO_DB_FIELDNAME_ISMARK).append(" = ?");	bVal.append(elementData->m_isMark);

		saveQStr.append(", ").append(UO_DB_FIELDNAME_VERSION); 	valuesStr.append(", ?");
		setStr.append(", ").append(UO_DB_FIELDNAME_VERSION).append(" = ?");	bVal.append(elementData->m_verssion);
		if (hasCode()) {
			saveQStr.append(", ").append(UO_DB_FIELDNAME_CODE); 	valuesStr.append(", ?");
			setStr.append(", ").append(UO_DB_FIELDNAME_CODE).append(" = ?");			//query.addBindValue(elementData->code());
			bVal.append(elementData->code());
		}
		if (hasTitle()){
			saveQStr.append(", ").append(UO_DB_FIELDNAME_TITLE); 	valuesStr.append(", ?");
			setStr.append(", ").append(UO_DB_FIELDNAME_TITLE).append(" = ?");//			query.addBindValue(elementData->descr());
			bVal.append(elementData->descr());
		}
		if (hasHierarchy()){
			saveQStr.append(", ").append(UO_DB_FIELDNAME_PARENTID); 	valuesStr.append(", ?");
			setStr.append(", ").append(UO_DB_FIELDNAME_PARENTID).append(" = ?");	bVal.append(elementData->parentId());

			saveQStr.append(", ").append(UO_DB_FIELDNAME_ISFOLDER); 	valuesStr.append(", ?");
			setStr.append(", ").append(UO_DB_FIELDNAME_ISFOLDER).append(" = ?");	bVal.append(elementData->isFolderN());
		}
		uoDbField* dbField = 0;
		for (int i = 0; i<m_fields.count(); i++){
			dbField = m_fields.at(i);
			if (dbField){
				saveQStr.append(", ").append(dbField->dbName()); 	valuesStr.append(", ?");
				setStr.append(", ").append(dbField->dbName()).append(" = ?");
				bVal.append(elementData->data(dbField->dbName()));
			}
		}
		if (needInsert){
			saveQStr.append(") VALUES (").append(valuesStr).append(")");
		} else {
			setStr.append("").append(" WHERE ").append(UO_DB_FIELDNAME_ID).append(" = ? ");
			bVal.append(elementData->id());
			saveQStr = setStr;
		}
		query.prepare(saveQStr);

		for (int t=0; t<bVal.count(); t++){
			query.addBindValue(bVal.at(t));
		}
		query.exec();
		if (query.lastError().isValid()   )	{
			//retVal = false;
			QMapIterator<QString, QVariant> i(query.boundValues());
			while (i.hasNext()) {
				i.next();
				qDebug() << i.key().toAscii().data() << ": "			 << i.value().toString().toAscii().data();
			}
			qDebug() << query.lastError().driverText();
			qDebug() << query.lastError().text();
		}
	}

	return retVal;
}

bool uoDbRefTable::setupID(uoElementData *elementData)
{
	if (elementData){
		if (elementData->id() == UO_EMPTY_ELEMENT){
			QString qs = QString("SELECT MAX(%1) as maxId").arg(UO_DB_FIELDNAME_ID);
			if (hasCode())
				qs.append(QString(", MAX(%1) as maxCode").arg(UO_DB_FIELDNAME_CODE));
			qs.append(QString(" FROM %3").arg(dbName()));
			QSqlQuery query = QSqlQuery(m_manager->dataBase());
			query.exec(qs);
			if (query.lastError().isValid()   )	{
				qDebug() << query.lastError().driverText();
				qDebug() << qs;
			}
			int ID = 0, codeInt = 0;
			QString code;
			while (query.next())
			{
			  ID = query.value(0).toInt();
			  if (hasCode())
				code = query.value(1).toString();
			  break;
			}
			if (ID == 0)
				ID = 0;
			if (hasCode())
				codeInt = code.toInt();


			ID = ID + 1;
			codeInt += 1;
			if (hasCode()){
				QString str = QString("%1").arg(codeInt);
				if (m_codeType == codeType_String){
					str = QString("000000000000000000000000000000000000%1").arg(codeInt);
					str = str.left(m_codeLen);
				}

				elementData->setCode(str);
			}
			elementData->setID(ID);
		}
	}


	return true;
}

bool uoDbRefTable::populate(uoTreeRMNode *nodeP, bool withElem){
    if (!nodeP)
        return false;
    uoTreeRMNode* tmpNode0 = nodeP;
    if (nodeP->m_nmID == -2){
        // костыль (((((( нужен узел с наименованием справочника...
        if (nodeP->m_children.count() == 0)
            tmpNode0 = nodeP->createNode(nodeP, -1, m_descr);
        nodeP->m_populated = true;
    }
    if (!m_hasHierarchy)
        return false;
    QSqlQuery query(manager()->dataBase());
    QString strQu = QString("SELECT %1, %2, %3, %4 FROM %5 WHERE  ")
            .arg(UO_DB_FIELDNAME_ID)
            .arg(UO_DB_FIELDNAME_DESCR)
            .arg(UO_DB_FIELDNAME_ISMARK)
            .arg(UO_DB_FIELDNAME_ISFOLDER)
            .arg(dbName())
            ;
    if (!withElem)
        strQu.append(QString(" %6 = 1 AND ").arg(UO_DB_FIELDNAME_ISFOLDER));
    if (tmpNode0){
        if (tmpNode0->m_nmID == -1){
            strQu.append( QString(" %1 = 0").arg(UO_DB_FIELDNAME_PARENTID) );
        } else {
            strQu.append( QString(" %1 = %2").arg(UO_DB_FIELDNAME_PARENTID).arg(tmpNode0->m_nmID) );
        }
    } else {
        strQu.append( QString(" %1 = 0").arg(UO_DB_FIELDNAME_PARENTID) );
    }
    strQu.append(QString(" ORDER BY %1 Desc,  %2 ASC").arg(UO_DB_FIELDNAME_ISFOLDER).arg(UO_DB_FIELDNAME_DESCR));
    //qDebug() << "uoDbRefTableIface::populate" << strQu;
    query.exec(strQu);
    if (query.lastError().isValid()   )	{
        qDebug() << query.lastError().driverText();
        qDebug() << strQu;
    }
    uoTreeRMNode* tmpNode = 0;
    while (query.next())
    {
      int ID = query.value(0).toInt();
      QString title = query.value(1).toString();
      tmpNode = tmpNode0->createNode(tmpNode0, ID, title);
      if (tmpNode){
          tmpNode->m_isMark = query.value(2).toBool();
          tmpNode->m_isFolder = query.value(3).toBool();
      }

    }
    tmpNode0->m_populated = true;

    return true;
}

bool uoDbRefTable::isFolder(int elem)
{
	bool isFolder = false;
	if (elem == UO_EMPTY_ELEMENT)
		return false;
	uoElementData* elData = elementData(elem);
	if (elData)
		isFolder = elData->isFolder();
	delete elData;
	return isFolder;
}

int uoDbRefTable::getParentFrom(const int elem)
{
	int parent = false;
	if (elem == UO_EMPTY_ELEMENT)
		return UO_EMPTY_ELEMENT;
	uoElementData* elData = elementData(elem);
	if (elData)
		parent = elData->parentId();
	delete elData;
	return parent;
}


QString uoDbRefTable::dbName() const{
	QString dbName = QString("%1%2").arg(m_prefixTab).arg(m_baseNom);
	return dbName;
}

QString appendString(QString src, QString add, QString prefix)
{
	QString retVal = src;
	if(src.isEmpty()){
		retVal = add;
	} else {
		retVal.append(prefix).append(add);
	}
	return retVal;
}

QString uoDbRefTable::getUrlByForm(uoFormType fType, int elem)
{
	QString getUrl = "uo:form:";
	getUrl.append(dbName()).append( QString(":formType:%1").arg(fType));
	return	getUrl;
}

uoUrl uoDbRefTable::getUoUrlByForm(uoFormType fType, int elem){
	return uoUrl( dbName(), elem, fType, UO_EMPTY_ELEMENT);
}

QString uoDbRefTable::getUrlByElement(uoFormType fType, int elem, int parent)
{
	QString getUrl = "uo:elem:";	//spNNN:ELEM:ID:type:elemType:parent:ID
	getUrl.append(dbName()).append( QString(":ELEM:%1").arg(elem)).append( QString(":type:%1:parent:%2").arg(fType).arg(parent));
	return	getUrl;
}

uoUrl uoDbRefTable::getUoUrlToElement(uoFormType fType, int elem, int parent)
{
	return uoUrl( dbName(), elem, fType, parent);
}

uoUrl uoDbRefTable::getUrlElement(int elem, bool edit)
{
	uoUrl url = uoUrl(elementData(elem));
	url.setReadOnly(!edit);
	return url;
}


uoDbField* uoDbRefTable::fieldGet(QString name)
{
	uoDbField* retVal = 0;
	for (int i = 0; i < m_fields.size(); ++i) {
		retVal = m_fields.at(i);
		if (retVal->m_name == name || retVal->m_descr == name || retVal->dbName() == name) {
			break;
		}
		retVal = 0;
	}
	return retVal;

}

bool uoDbRefTable::fieldExist(QString name)
{
	bool retVal = false;
	uoDbField* field = fieldGet(name);
	if (field) {
		retVal = true;
	}
	return retVal;
}

uoUiFields::uoUiFields(){}

uoUiFields::uoUiFields(uoDbRefTableIface* parent, QString name, QString qname) :
    m_ifase(parent),m_dbName(name),m_queryName(qname)
{
    if (m_queryName.isEmpty())
        m_queryName = m_dbName;
}

uoDbRefTableIface::uoDbRefTableIface(uoDbGuiFactory *parent, uoDbRefTable *table, uoUrl url, QString filesName)
    :QObject(parent), m_factory(parent), m_table(table),m_urlObj(url)
{
    m_urlstr = url.toString();
    m_isFolderNom = -1;
    m_isMarkNom = -1;
    m_IDColNom = -1;
    m_PictureColNom = 0;
    m_dirty = false;
    m_curentElem = UO_EMPTY_ELEMENT;
    m_curentParent = UO_EMPTY_ELEMENT;

    QStringList list = filesName.split(",");
    bool lookupFromList = false;
    if (list.count() > 0){
        lookupFromList = true;
        if (list.count() == 1 && list.at(0) == "*")
            lookupFromList = false;
    }
    int fldLenPres = 0;
    QString fldTitle = "";
    QString queryName = "";
    uoUiFields* uiFields = 0;
    const int charWidht = 3;
    uoDbField* field = 0;
    if (lookupFromList){
        for (int i = 0; i<list.count(); i++){
            QString name = list.at(i);

            fldLenPres = 0;
            fldTitle = name;
            queryName = name;

            field = m_table->fieldGet(name);
            if (!field){
                if (name == UO_DB_FIELDNAME_CODE){
                    fldTitle = tr("Code"); //fldTitle = "Код";
                    fldLenPres = qMax(m_table->m_codeLen,6) * charWidht;
                } else if (name == UO_DB_FIELDNAME_TITLE){
                    fldTitle = tr("Description"); // fldTitle = "Наименование";
                    fldLenPres = qMin(qMax(m_table->m_titleLen,10),200) * charWidht;
                } else if (name == UO_DB_FIELDNAME_PARENTID){
                    fldTitle = tr("Description"); // fldTitle = "Наименование";
                    fldLenPres = qMax(m_table->m_titleLen,10) * charWidht; // надо pасчитывать по наименованию.
                } else if (name == UO_DB_FIELDNAME_PICTSTATE){
                    fldTitle = "";
                    fldLenPres = 3 * charWidht; // пиктограмма
                    name = "0"; // имя поля у пиктограммы нет, там составное
                } else {
                    queryName = name;
                }
            } else {
                fldTitle = field->m_name;
                fldLenPres = qMax(field->m_len ,3) * charWidht;
            }
            uiFields = new uoUiFields(this,name,queryName);
            uiFields->m_present = fldTitle;
            uiFields->m_length = fldLenPres;
            uiFields->m_queryName = queryName;
            m_tabField.append(uiFields);
        }
    } else {
        uiFields = new uoUiFields(this,"0",UO_DB_FIELDNAME_PICTSTATE);
        uiFields->m_present = "";
        uiFields->m_length = 3 * charWidht;
        m_tabField.append(uiFields);
        if (m_table->m_hasTitle){
            uiFields = new uoUiFields(this,UO_DB_FIELDNAME_DESCR);
            uiFields->m_present = tr("Description"); // uiFields->m_present = "Наименование";
            uiFields->m_length = qMax(m_table->m_titleLen,10) * charWidht;
            m_tabField.append(uiFields);
        }
        if (m_table->m_hasCode){
            uiFields = new uoUiFields(this,UO_DB_FIELDNAME_CODE);
            uiFields->m_present = tr("Code"); //uiFields->m_present = "Код";
            uiFields->m_length = qMax(m_table->m_codeLen,6) * charWidht;
            m_tabField.append(uiFields);
        }
        if (url.formType() == uoFormT_Group || url.formType() == uoFormT_Elem){
            if (m_table->hasHierarchy()){
                uiFields = new uoUiFields(this,UO_DB_FIELDNAME_PARENTID);
                uiFields->m_present = tr("Parent"); // uiFields->m_present = "Pодитель";
                uiFields->m_length = qMax(m_table->m_titleLen,10) * charWidht;
                m_tabField.append(uiFields);
            }
        }
        for(int i=0; i<m_table->m_fields.count(); i++) {
            field = m_table->m_fields.at(i);
            if (field->m_public){
                uiFields = new uoUiFields(this,field->dbName());
                uiFields->m_present = field->m_name;
                uiFields->m_length = qMin(qMax(field->m_len,6),150) * charWidht;
                m_tabField.append(uiFields);
            }
        }
    }
}

uoDbRefTableIface::~uoDbRefTableIface(){
}


uoDbRefTableListIface::uoDbRefTableListIface(uoDbGuiFactory* parent, uoDbRefTable* table, uoUrl url, QString filesName )
    :uoDbRefTableIface(parent,table,url,filesName)
{
    m_treeModel = 0;
    m_tableModel = 0;
    m_hierarchyMode = true;
    m_treeWithElem = false;
    m_curentElem = UO_EMPTY_ELEMENT;
}

QString uoDbRefTableListIface::setupQueryToModel(uoDBRefTableModel* model, int parentElem)
{
    m_hideCols.clear();
    QString retVal = "SELECT ";
    QString retValCols = "";
    model->setParent(parentElem);

    QString tmpStr = "";
    uoUiFields* uiField = 0;
    int colCounter = m_tabField.count() - 1;
    for (int i = 0; i< m_tabField.count(); i++){
        uiField = m_tabField.at(i);
        tmpStr = uiField->m_dbName;
        if (tmpStr == UO_DB_FIELDNAME_DESCR)
            m_curentColTable = i;
        if (tmpStr == UO_DB_FIELDNAME_PICTSTATE)
            m_PictureColNom = i;
        retValCols = appendString(retValCols, QString("%1 as %2").arg(tmpStr).arg(uiField->m_queryName),"," );
    }
    colCounter++;    m_hideCols << colCounter;
    retValCols = appendString(retValCols, UO_DB_FIELDNAME_ID,"," );
    m_IDColNom = colCounter;

    colCounter++;    m_hideCols << colCounter;
    retValCols = appendString(retValCols, UO_DB_FIELDNAME_ISMARK,"," );
    m_isMarkNom = colCounter;


    if (m_table->m_hasHierarchy){
        colCounter++;    m_hideCols << colCounter;
        retValCols = appendString(retValCols, UO_DB_FIELDNAME_ISFOLDER,"," );
        m_isFolderNom = colCounter;
    }

    retVal.append(retValCols);
    retVal.append(QString(" FROM %1").arg(m_table->dbName()));
    QString whereStr = "";
    if (m_table->m_hasHierarchy && m_hierarchyMode){
        if (parentElem == -1 || parentElem == 0){
            whereStr.append(QString(" %1 = %2").arg(UO_DB_FIELDNAME_PARENTID).arg(0));
        } else {
            whereStr.append(QString(" %1 = %2").arg(UO_DB_FIELDNAME_PARENTID).arg(parentElem));
        }
    }
    if (!whereStr.isEmpty()){
        retVal.append(" WHERE ").append(whereStr);
    }
    retVal.append( QString(" ORDER BY %1 ").arg(UO_DB_FIELDNAME_DESCR) );

    model->setQuery(retVal,m_table->manager()->dataBase());
    const QSqlError err = model->lastError();
    if (err.isValid()){
        qDebug() << err.text(); //<< err.driverText();
        //qDebug() << retVal;
    }
    for (int i = 0; i< m_tabField.count(); i++){
        uiField = m_tabField.at(i);
        model->setHeaderData(i, Qt::Horizontal,uiField->m_present);
        QSize sz = QSize(uiField->m_length, 18);
        model->setHeaderData(i, Qt::Horizontal, sz, Qt::SizeHintRole);
    }
    return retVal;

}

int uoDbRefTableIface::getLeghtCol(int colNo)
{
    int retVal = 110;
    if(colNo < m_tabField.count()){
        uoUiFields* fld = m_tabField.at(colNo);
        if (fld)
            retVal = fld->m_length;
    }
    return retVal;
}

void uoDbRefTableIface::setCurentsElement(int elem, int elemP){
    m_curentElem   = elem;
    m_curentParent = elemP;
}

bool uoDbRefTableIface::doCloseForm(uoElementData *dataEl)
{
    bool retVal = true;
    if (dataEl){
        if (dataEl->isDirty()){
            QMessageBox::StandardButton save = m_factory->doQuerySaveElement();
            if (save == QMessageBox::Yes){
                retVal = m_table->save(dataEl);
            } else if (save == QMessageBox::No) {
                dataEl->setDirty(false);
                retVal = true;
            } else if (save == QMessageBox::Cancel) {
                retVal = false;
            }

        }
    }
    return retVal;
}

void uoDbRefTableIface::setUrl(const uoUrl url){
    m_urlObj = url;
}

uoElementData*
uoDbRefTableIface::load(int elem, int level)
{
    return m_table->elementData(m_curentElem,level,m_curentParent);
}

bool uoDbRefTableListIface::populate(uoTreeRMNode *nodeP)
{
    return m_table->populate(nodeP, m_treeWithElem);
}

QIcon uoDbRefTableIface::getIcon(bool isFolder, bool isMark){
    return m_factory->getIcon(isFolder, isMark); //     QList<QIcon> m_icon;
}

bool uoDbRefTableListIface::editStart(int elem)
{
    if (elem == UO_EMPTY_ELEMENT)
        return false;
    return m_factory->onEditStart(elem, m_table);;
}

uoDbRefTreeModel *uoDbRefTableListIface::treeModel()
{
    if (!m_treeModel)
        m_treeModel = new uoDbRefTreeModel( this);
    return m_treeModel;
}

uoDBRefTableModel *uoDbRefTableListIface::tableModel()
{
    if (!m_tableModel)
        m_tableModel = new uoDBRefTableModel(this);
    return m_tableModel;
}

bool uoDbRefTableListIface::setHierarchyMode(bool mode)
{
    bool last = m_hierarchyMode;
    if(m_hierarchyMode != mode){
        m_hierarchyMode = mode;
    }
    return last;
}

bool uoDbRefTableListIface::doNewElement(uoFormType elType, int parent)
{
    int curParent = parent;
    if (parent == UO_EMPTY_ELEMENT){
        curParent = tableModel()->parentCur();
    }
    uoUrl url(m_table->dbName(),UO_EMPTY_ELEMENT, elType,curParent);
    m_factory->showFormUrl(url);
    return true;
}

bool uoDbRefTableListIface::doEditElement(int elem)
{
    uoUrl url2 = m_table->getUrlElement(elem);
    m_factory->parseUrl(url2);
    return true;
}

bool uoDbRefTableListIface::doDeleteElement(int elem)
{
    bool retVal = false;
    if (elem != UO_EMPTY_ELEMENT){
        uoElementData* elData = m_table->elementData(elem);
        if (m_factory->doQueruDeleteElement(elData)) {
            retVal = m_table->doDeleteElement(elData);// uoElementData* elData
        }
        delete elData;
    }
    return retVal;
}

void uoDbRefTableListIface::addAction(QAction *act){
    if (m_actionList.indexOf(act) == -1)
        m_actionList << act;
}

void uoDbRefTableListIface::setWithElem(bool withElem)
{
    if (m_treeWithElem != withElem)
        m_treeWithElem = withElem;
}

void uoDbRefTableListIface::setHierarchy(bool mode)
{
    setHierarchyMode(mode);
}

uoDbManager::uoDbManager(QObject *parent) :
    QObject(parent)
{
	m_counter = 0;
	m_DbConnected  = false;
	m_uiFactory = 0;
	uoElementData_idVariantType = qRegisterMetaType<uoElementData>();
	m_openObjTable = 0;
}

uoDbManager::~uoDbManager()
{
	m_openObjTable = 0;
	if(m_DbConnected)
		m_db.close();
	qDeleteAll(m_tables);
}

uoDbRefTable* uoDbManager::addRefTable(QString name, QString descr, int codelen, int titleLen){
	uoDbRefTable* table = new uoDbRefTable(name,  descr,  codelen, titleLen);
	table->m_manager = this;
	m_tables.append(table);
	registerObj(table);
	return table;
}

void uoDbManager::registerObj(uoDbObject* obj)
{
	m_counter = m_counter + 1;	
	obj->m_baseNom = m_counter;
}

bool uoDbManager::setupDataBase(QString dbName,QString dbUserName,QString dbPassword,QString dbMode,int dbPort,QString dbHost)
{
	m_dbMode = dbMode; /// драйвер
	m_DbConnected  = false;

	m_dbName = dbName;
	m_dbHost = dbHost;
	m_dbPort = dbPort;
	m_dbUserName = dbUserName;
	m_dbPassword = dbPassword;
	return openDataBase();
}

bool uoDbManager::openDataBase(){
	m_db = QSqlDatabase::addDatabase(m_dbMode/*,"main"*/);
	if(m_dbHost.isEmpty())
		m_dbHost ="127.0.0.1";
	m_db.setHostName(m_dbHost);

	if (m_dbPort > 0){
		m_db.setPort(m_dbPort);
	}

	if (!m_dbName.isEmpty()){
		m_db.setDatabaseName(m_dbName);
	}

	if (!m_dbUserName.isEmpty()){
		m_db.setUserName(m_dbUserName);
		m_db.setPassword(m_dbPassword);
	}
	m_DbConnected = m_db.open();
	return m_DbConnected;

}
bool uoDbManager::closeDataBase()
{
	if (m_DbConnected) m_db.close();
	return true;
}


bool uoDbManager::tableExist(QString name)
{
	bool retVal = false;
	if (findTable(name) != 0)
		retVal = true;
	return retVal;
}

uoDbRefTable* uoDbManager::findTable(QString name)
{
	uoDbRefTable* table;
	for (int i = 0; i < m_tables.size(); ++i) {
		table = m_tables.at(i);
		if (table->m_name == name || table->dbName() == name) {
			break;
		} else {
			table = 0;
		}
	}
	return table;
}

QSqlDatabase uoDbManager::dataBase()
{
	if (!m_db.isOpen())
		openDataBase();
	return m_db;
}

// Сообщить об ошибке..
bool uoDbManager::doReportError(const QSqlQuery &query, QString qStr ){
	bool retVal = false;
	if (query.lastError().isValid()   )	{
		retVal = true;
		qDebug() << query.lastError().driverText();
		qDebug() << query.lastError().text();
		qDebug() << query.executedQuery();
		if (!qStr.isEmpty())
			qDebug() << qStr;


	}
	return retVal;
}

QCompleter*
uoDbManager::getComleter()
{
	QCompleter* retVal = new QCompleter();
	QStringList words;
	uoDbRefTable* tab = 0;
	QSqlQuery query = QSqlQuery(dataBase());
	QString qStr = "";
	QString theWord = "";
#ifndef QT_NO_CURSOR
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
#endif
	int refTabID = uoMdiObserver::instance()->m_receptTableId; //= m_receptTable->m_baseNom;
	bool test = false;
	int i = 0;
	for (i = 0; i < m_tables.count(); i++) {
		tab = m_tables.at(i);
		if (tab->hasTitle() && refTabID == tab->m_baseNom){
			qStr = "";
			qStr.append("SELECT ").append(UO_DB_FIELDNAME_DESCR).append(" FROM ").append(tab->dbName()).append(" WHERE ").append(UO_DB_FIELDNAME_ISMARK).append(" = 0");
			if (tab->hasHierarchy())
				qStr.append(" AND ").append(UO_DB_FIELDNAME_ISFOLDER).append(" = 0");
			if (!query.exec(qStr)){
				doReportError(query);
				continue;
			}
			if (test)	qDebug() << "Table: " << tab->m_descr;

			while(query.next()){
				theWord = query.value(0).toString();
				if (!theWord.isEmpty()) {
					theWord = theWord.trimmed();
					words << theWord;
					if (test)	qDebug() << theWord;
				}
			}
		}
	}
	QStringList files;
	files << UO_DB_FILE_ZABOLEVANIE << UO_DB_FILE_LEKARSTVO << UO_DB_FILE_SIMPTOMS;
	for (int t = 0; t < files.count(); t++){
		QString fName = qApp->applicationDirPath()+ "/"+files.at(t);
#ifdef Q_OS_LINUX
		QDir home = QDir::home();
		if (!home.exists(".uoReceptor")) {
			   home.mkpath(".uoReceptor");
		   }
		fName = home.absolutePath() + "/.uoReceptor/" + "/"+files.at(t);

#endif

		QFile fl(fName);
		if (fl.open(QIODevice::ReadOnly)){

			QTextCodec*	c = QTextCodec::codecForName("UTF-8");

			QTextStream t(&fl);
			t.setCodec( c );
			QString line;
			do {
				line = t.readLine();
				if (!line.isEmpty()){
					if (!words.contains(line));
						words << line;
				}
			} while (!line.isNull());
			fl.close();

		}
	}



	QStringListModel* model =  new QStringListModel(words, retVal);
	retVal->setModel(model);
	retVal->setModelSorting(QCompleter::UnsortedModel);
	retVal->setCaseSensitivity(Qt::CaseInsensitive);
	if (test) {
		qDebug() << "================================";
		qDebug() << "Test completation";
		for (int j = 0; j < words.count(); j++){
			theWord = words.at(j);
			if (theWord.length()>3){
				qDebug() << "Test for: " << theWord << theWord.mid(0,3);
				theWord = theWord.mid(0,3);
				retVal->setCompletionPrefix(theWord);
				int cnt = retVal->completionCount();
				for (int i = 0; i < cnt; i++){
					retVal->setCurrentRow(i);
					qDebug() << retVal->currentCompletion() << " is match number " << i;
				}
			}
		}
		qDebug() << "================================";
	}

#ifndef QT_NO_CURSOR
	QApplication::restoreOverrideCursor();
#endif
	return retVal;
}

uoDbForm *uoDbManager::getForm(const uoUrl &url, QWidget* parent)
{
	if (!m_uiFactory)
		return 0;
	return m_uiFactory->getForm(url, parent);
}

uoElementData *uoDbManager::getElement(const uoUrl &url){
	uoElementData * elData = 0;
	uoDbRefTable* tab = findTable(url.table());
	if (tab){
		elData = tab->elementData(url.element());
	}

	return elData;
}


void uoDbManager::showDlg(QWidget* parent)
{
	uoDbManagerDlg* dlg = new uoDbManagerDlg(parent, this);
	dlg->exec();

}

void uoDbManager::createGuiFactory(uoMainWindow *mwind)
{
	delete m_uiFactory;
	m_uiFactory = new uoDbGuiFactory(this, mwind);
	if (m_tables.count() > 0) {
		m_uiFactory->updateGui(this);
	}
}

QActionGroup *uoDbManager::getActions()
{
	QActionGroup* aGroup = 0;
	if (m_uiFactory)
		aGroup = m_uiFactory->getActions();
	return aGroup;
}

int uoDbManager::fillMenu(QMenu *menu){

	if (m_uiFactory) {
		if (QActionGroup* aGroup = m_uiFactory->getActions()){
			QList<QAction *> list = aGroup->actions ();
//			for(int i = 0; i < list.count(); i++){
				menu->addActions(aGroup->actions ());
//			}
		}
	}
	return 0;
}

bool uoDbManager::restructuring()
{
	bool retVal = false;
	if (!m_DbConnected)
		return retVal;
//	m_openObjTable = addRefTable("openJrn","Открытые объекты",10,20);
//	m_openObjTable->setPrefixTab("oj", true);
	QString sqlQuery = "";
	uoDbRefTable* table;
	uoDbField* field;
	QStringList tablesDbExist =  m_db.tables();
	m_db.exec("PRAGMA encoding = \"UTF-8\"");


	QString dbNameOftabl = "";
	for (int i = 0; i < m_tables.size(); ++i) {

		table = m_tables.at(i);
		dbNameOftabl = table->dbName();

		sqlQuery = "CREATE TABLE " + dbNameOftabl + "("
				"ID INTEGER PRIMARY KEY";
		sqlQuery.append("\n,").append( UO_DB_FIELDNAME_ISMARK ).append(" int");
		sqlQuery.append("\n,").append( UO_DB_FIELDNAME_VERSION ).append(" int");
		if (table->m_hasCode){
			sqlQuery.append("\n,").append(UO_DB_FIELDNAME_CODE).append(" ");
			if (table->m_codeType == codeType_Number){
				sqlQuery.append(" INTEGER ");
			} else {
				sqlQuery.append(" TEXT ");
			}
		}
		if (table->m_hasTitle){
			sqlQuery.append(", ").append(UO_DB_FIELDNAME_DESCR).append(" TEXT");
		}
		if (table->m_hasHierarchy){
			sqlQuery.append("\n, ").append(UO_DB_FIELDNAME_ISFOLDER).append( " int, ").append( UO_DB_FIELDNAME_PARENTID).append(" INTEGER \n");
		}
			for(int jj = 0; jj< table->m_fields.size(); ++jj) {
				field = table->m_fields.at(jj);
				sqlQuery.append(" ,");
				sqlQuery.append(field->dbName()).append(" ");
				switch(field->m_fieldType)
				{
				case fieldType_Number:
				case fieldType_Date:
					sqlQuery.append("INT");
					break;

				case fieldType_Float:
					sqlQuery.append("REAL");
					break;

				case fieldType_String:
				case fieldType_Text:
				case fieldType_TextHTML:
					sqlQuery.append("TEXT");
					break;
				case fieldType_TableRef:
					sqlQuery.append("INT");
					break;
				default:
					sqlQuery.append("TEXT");
					break;
				}
			}
		sqlQuery = sqlQuery + ")";
		table->m_createSQL = sqlQuery;
		if (tablesDbExist.indexOf(dbNameOftabl) != -1)
			continue;
		m_db.exec(sqlQuery);
		if (m_db.lastError().isValid()   )	{
			qDebug() << m_db.lastError().driverText();
			retVal = false;
		}
	}
	closeDataBase();
	retVal = openDataBase();
	return retVal;
}
