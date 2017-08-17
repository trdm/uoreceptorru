#ifndef UODBDEFS_H
#define UODBDEFS_H
#include <QFlags>

class QString;
class uoUrl;
class uoDbField;
class uoDbManager;
class uoDbObject;
class uoElementData;
class uoDbRefTable;
class uoDbRefTableListIface;
class uoDbRefTableIface;
class uoDbRefTreeModel;
class uoDBRefTableModel;
class uoMDITabWidget;
class uoTreeRMNode;
class uoUiFields;
class uoDbGuiFactory;
class uoMainWindow;
//class uoDbForm;

// Пиктограмма
#define UO_DB_FIELDNAME_PICTSTATE	QString("Picture")
#define UO_DB_FIELDNAME_ID			QString("ID")
#define UO_DB_FIELDNAME_CODE		QString("CODE")
#define UO_DB_FIELDNAME_DESCR		QString("Descr")
#define UO_DB_FIELDNAME_TITLE		QString("Descr")
#define UO_DB_FIELDNAME_PARENTID	QString("ParentID")
#define UO_DB_FIELDNAME_ISFOLDER	QString("isFolder")
#define UO_DB_FIELDNAME_ISMARK		QString("isMark")
#define UO_DB_FIELDNAME_VERSION		QString("ver")
#define UO_DB_FIELD_REF_LEN			10
#define UO_EMPTY_ELEMENT			0

#define UO_DB_FILE_RECIPE_TEMPL		QString("recipe.txt")
#define UO_DB_FILE_LEKARSTVO		QString("lek.txt")
#define UO_DB_FILE_SIMPTOMS			QString("simp.txt")
#define UO_DB_FILE_ZABOLEVANIE		QString("disease.txt")
//disease.txt","Заболевания"

// #include "uodefines.h"
enum uoDbTableType {
	uoTabTape_Unknown = 0
	, uoTabTape_Referense = 1
};

enum uoDbCodeType {
	codeType_Number = 0
	, codeType_String = 1
};

enum uoDbObjType {
	objType_Unknown = 0 ,
	objType_Field = 1 ,
	objType_Table = 2 ,
	objType_RefTable = 3 ,
	objType_Doc = 4
};


enum uoDbFieldType {
	fieldType_Unknown = -1
	, fieldType_Number = 0
	, fieldType_Float = 1
	, fieldType_String = 2
	, fieldType_Date = 3
	, fieldType_TableRef = 4
	, fieldType_Enum = 5
	, fieldType_Text = 6
	, fieldType_TextHTML = 7
	, fieldType_Bin = 8
};

enum uoFormType{
	uoFormT_Unknown = -1
	, uoFormT_List = 0
	, uoFormT_Tree
	, uoFormT_Group
	, uoFormT_Elem
};

///\breaf - использование полей
enum uoFieldUsage {
	uoFieldU_All = 0
	, uoFieldU_GruopOnly
	, uoFieldU_ElementOnly
};

enum uoUrlType {
	uoUrlType_Unknown = -1
	, uoUrlType_Form
	, uoUrlType_Element
	, uoUrlType_Group
};

enum uoDbLineEditFlag {
    uoDBLe_None         = 0x00000000,
    uoDBLe_Choice       = 0x00000001,
    uoDBLe_ChoiceArrow  = 0x00000002,
    uoDBLe_Open         = 0x00000004,
    uoDBLe_Clear        = 0x00000008,
    uoDBLe_All          = uoDBLe_Choice | uoDBLe_ChoiceArrow |uoDBLe_Open| uoDBLe_Clear
};

Q_DECLARE_FLAGS(uoDbLineEditFlags, uoDbLineEditFlag)



#endif // UODBDEFS_H
