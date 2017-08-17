#ifndef UODBMANAGER_H
#define UODBMANAGER_H

#include <QObject>
#include <QtSql/QSqlQueryModel>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlTableModel>
#include <QMainWindow>
#include <QActionGroup>
#include <QIcon>
#include <QMetaType>
#include <QStringList>
#include <QList>
#include <QVariant>
#include <QCompleter>

#include "uodefines.h"
#include "uourl.h"
#include "uodbform.h"
#include "uodbguifactory.h"
#include "uodbreftreemodel.h"
#include "uodbreftableqmodel.h"

/// Общий, создающийся в бд объект, таблица или поле.
class uoDbObject {
public:
	uoDbObject(QString name, QString descr) : m_name(name), m_descr(descr),m_objType(objType_Unknown),m_baseNom(0),m_public(true)
	{}
	QString m_name;
	QString m_descr;
	QString title();
	uoDbObjType m_objType; // objType_Unknown
	qint32 m_baseNom;
	bool m_public; ///< публичное. Для того, что-бы исключить служебные поля.
};

class uoDbField : public uoDbObject{
public:
	uoDbField(QString name, uoDbFieldType fType, int len = 0, int presition = 0 );
	uoDbField(QString name, uoDbRefTable* table );
	~uoDbField();
	QString dbName();
public:
	friend class uoDbRefTable;
	friend class uoDbManager;
	uoDbFieldType m_fieldType;
	int m_len;		///< Общая длина поля.
	int m_numLen;	///< Длина цифр.
	int m_numPresition;
	uoFieldUsage m_useFild;
private:
	uoDbRefTable* m_table;
	uoDbManager* m_manager;
};

class uoDbRefTable;
///\brief - просто описание элемента.
class uoElementData {
public:
	uoElementData() {
		m_table = m_descr = m_code = m_title = "";
		m_id = m_ParentID = 0;
		m_isFolder = m_isMark = m_verssion = 0;
		m_dirty = false;
		m_parent = 0;
		m_tablePtr = 0;
	}
	uoElementData(class uoDbRefTable* tabPtr);
	QString table() {return m_table;}
	QString title(){return m_title;}
	QString descr(){return m_descr;}
	int		id(){return m_id;}
	QString code(){return m_code;}
	int		parentId(){return m_ParentID;}
	bool	isFolder();
	int		isFolderN() {return m_isFolder; }
	bool	isMark();
	int		isMarkN() {return m_isMark; }
	bool	isDirty() { return m_dirty; }

	bool	 hasData(QString name);
	QVariant data(QString name);

	class uoElementData* parent(){return m_parent;}

	void  setTitle(const QString title);
	void  setTablePtr(uoDbRefTable* tablePtr);
	void  setCode(const QString code);
	void  setDescr(const QString descr);
	void  setID(const int id) {m_id = id;}
	void  setParentID(const int parent);
	void  setParent(uoElementData* parent);
	void  setData(QString name, QVariant data);
	void  setDataToData(QString name, QVariant data);
	void  setDirty(const bool dir);
	void  setFolder(const int fold);

	int	 m_isMark;
	bool m_isMarkB;
	int  m_verssion;
private:
	QString m_table;
	QString m_title;  // наименование или код, или представление из другого поля, если нет кода или наименования
	QString m_code;
	QString m_descr; // наименование
	int m_isFolder;
	int m_id;
	int m_ParentID;
	uoElementData* m_parent;
	QMap<QString, QVariant> m_data;
	bool  m_dirty;
	class uoDbRefTable* m_tablePtr;
};
Q_DECLARE_METATYPE(uoElementData)

class uoDbRefTable  : public uoDbObject {

private:
	uoDbRefTable(QString name, QString descr = "", int codelen = 4, int titleLen = 25 );
	friend class uoDbManager;
public:
	~uoDbRefTable();
	uoDbField* addField(QString name, uoDbFieldType fType, int len = 0, int presition = 0);
	uoDbField* addField(QString name, QString m_relTable);
	void setOption(uoDbCodeType codeType = codeType_Number, bool hierarchy = false);
	bool isFolder(int elem);
	int getParentFrom(const int elem);
	QString dbName() const;
//	QString queryString();
	QString queryStringCreate() {return m_createSQL;}
	QString getUrlByForm(uoFormType fType, int elem = UO_EMPTY_ELEMENT);
	uoUrl	getUoUrlByForm(uoFormType fType, int elem = UO_EMPTY_ELEMENT);
	QString getUrlByElement(uoFormType fType, int elem = UO_EMPTY_ELEMENT, int parent = UO_EMPTY_ELEMENT);
	uoUrl	getUoUrlToElement(uoFormType fType, int elem = UO_EMPTY_ELEMENT, int parent = UO_EMPTY_ELEMENT);
	uoUrl	getUrlElement(int elem, bool edit = true );
	uoDbManager* manager() {return m_manager;}
	uoElementData* elementData(int elem, int level = 0, int parent =UO_EMPTY_ELEMENT); //level = 0 - означает текущий уровень..
	bool doDeleteElement(uoElementData* elData);// uoElementData* elData
	bool save(uoElementData* elementData);
	bool setupID(uoElementData* elementData);

	bool hasHierarchy() {return m_hasHierarchy; }
	bool hasCode() {return m_hasCode; }
	bool hasTitle() {return m_hasTitle; }
	void setPrefixTab(QString perf, bool privat = true){m_prefixTab = perf; m_privatTab = privat; }
	bool isPrivate() { return m_privatTab; }
	bool isPublic() { return !m_privatTab; }
	bool populate(uoTreeRMNode* nodeP, bool withElem = false);
public:

	bool m_hasCode;
	bool m_hasTitle;
	int m_codeLen;
	int m_titleLen;
	uoDbCodeType m_codeType;
	bool m_hasHierarchy;
	QList<uoDbField*> m_fields;
	QString m_createSQL;
	bool fieldExist(QString name);
	uoDbField* fieldGet(QString name);
private:
	uoDbManager* m_manager;
	QString m_prefixTab;
	bool m_privatTab; //служебная табла
};

class uoDbRefTableIface;

class uoUiFields {
public:
	uoUiFields();
	uoUiFields(uoDbRefTableIface* parent, QString dbName, QString qname="");
private:
	uoDbRefTableIface* m_ifase;
public:
	QString m_dbName;
	QString m_queryName;
	const QString dbName() {return m_dbName;}
	const QString queryName() {return m_queryName;}

	QString m_present;
	int m_length;
	bool m_useCondition;
	QString m_condCompSign; /// '=','<', '>' и т.п.
	QVariant m_condValue;
};

class uoDbRefTableIface  : public QObject
{
	Q_OBJECT
public:
	uoDbRefTableIface(uoDbGuiFactory* parent, uoDbRefTable* table, uoUrl url , QString filesName = "*" );
	virtual ~uoDbRefTableIface();
	QIcon getIcon(bool isFolder, bool isMark);

public:
    int getLeghtCol(int colNo);
    QList<int> m_hideCols;
    int colNo_isFolder(){return m_isFolderNom;}
    int colNo_isMark()  {return m_isMarkNom;}
    int colNo_ID()      {return m_IDColNom;}
    int colNo_Picture()      {return m_PictureColNom;}
    void setCurentsElement(int elem, int elemP);
    bool doCloseForm(uoElementData* dataEl);

	int curentId(){return m_curentElem;}
	int curentParent(){return m_curentParent;}
	void setUrl(const uoUrl url);
	uoUrl url() { return m_urlObj;}

	uoDbGuiFactory* m_factory;
	uoDbRefTable* m_table;

    uoElementData* load(int elem = UO_EMPTY_ELEMENT, int level = 0);
    QList<uoUiFields*> m_tabField;
protected:
    int m_isFolderNom;
    int m_isMarkNom;
    int m_IDColNom;
    int m_PictureColNom;
    QString m_urlstr;
    int m_curentElem;
    int m_curentParent;
    bool m_dirty;
    uoUrl m_urlObj;

};


///\breaf управление интерфейсом представления для uoDbRefTable
class uoDbRefTableListIface : public uoDbRefTableIface
{
    Q_OBJECT
public:
    uoDbRefTableListIface(uoDbGuiFactory* parent, uoDbRefTable* table, uoUrl url ,QString filesName = "*" );

    QString setupQueryToModel(uoDBRefTableModel *model, int parentElem=UO_EMPTY_ELEMENT);
    bool populate(uoTreeRMNode* nodeP);

    bool editStart(int elem);
    uoDbRefTreeModel* treeModel();
    uoDBRefTableModel* tableModel();
    bool setHierarchyMode(bool mode);
    bool hierarchyMode() {return m_hierarchyMode; }
    bool doNewElement(uoFormType elType, int parent = UO_EMPTY_ELEMENT);
    bool doEditElement(int elem);
    bool doDeleteElement(int elem);
    void setCurentCol(int cc){m_curentColTable = cc;}
    int curentCol(){return m_curentColTable;}
    QList<QAction*> actionsList() {return m_actionList;}
    void addAction(QAction* act);
    void setWithElem(bool withElem);

public slots:
    void setHierarchy(bool mode);
private:
    bool m_hierarchyMode;
    bool m_treeWithElem;
    uoDbRefTreeModel* m_treeModel;
    uoDBRefTableModel* m_tableModel;
    int m_curentColTable;
    QList<QAction*> m_actionList;
};
class uoDbManager : public QObject
{
    Q_OBJECT
public:
	friend class uoDbRefTable;
    explicit uoDbManager(QObject *parent = 0);
	~uoDbManager();
	uoDbRefTable* addRefTable(QString name, QString descr = "", int codelen = 4, int titleLen = 25 );
	void registerObj(uoDbObject* obj);
	bool setupDataBase(QString dbName,QString dbUserName="",QString dbPassword="",QString dbMode="QSQLITE",int dbPort=0,QString dbHost = "" );
	bool openDataBase();
	bool closeDataBase();
	bool restructuring();
	QList<uoDbRefTable*> m_tables;
	void showDlg(QWidget* parent);
	void createGuiFactory(uoMainWindow *mwind);
	QActionGroup* getActions();
	int fillMenu(QMenu* menu);
	uoDbRefTable* findTable(QString name);
	QSqlDatabase dataBase();
	bool doReportError(const QSqlQuery& query, QString qStr = "");
	QCompleter* getComleter();
	uoDbForm* getForm(const class uoUrl& url, QWidget* parent=0);
	uoElementData* getElement(const uoUrl& url);

private:
	qint32 m_counter;
	bool tableExist(QString name);

	bool m_DbConnected;
	QSqlDatabase m_db;
	QString m_dbMode; /// драйвер
	uoDbRefTable* m_openObjTable; // открытые обекты. в коде ID в названии имя таблицы

	QString m_dbName;
	QString m_dbHost;
	int m_dbPort;
	QString m_dbUserName;
	QString m_dbPassword;
	uoDbGuiFactory* m_uiFactory;
	int uoElementData_idVariantType; // =  qRegisterMetaType<uoDbRefDataElement>();
	uoDbRefTable* m_receptTable;
signals:

public slots:

};

#endif // UODBMANAGER_H
