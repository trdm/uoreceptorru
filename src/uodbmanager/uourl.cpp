#include <QStringList>
#include "uourl.h"
#include "uodbmanager.h"

uoUrl::uoUrl(){
    initEmpty();
}

uoUrl::uoUrl(uoElementData *el){
    initEmpty();
    if (el) {
        m_type = uoUrlType_Element;
        m_formType = uoFormT_Elem;
        if (el->isFolder()){
            m_type = uoUrlType_Group;
            m_formType = uoFormT_Group;
        }
        m_elem = el->id();
        m_elemParent = el->parentId();
        m_table = el->table();
    }
}

QString uoUrl::toString(bool withIID) const{
    QString retVal = "uo:elem:";
    if (m_formType == uoFormT_List || m_formType == uoFormT_Tree){
        retVal = "uo:form:";
    }
    int ro = m_readOnly ? 1 : 0;
    retVal.append(m_table);
    if (m_formType == uoFormT_List || m_formType == uoFormT_Tree){
        retVal.append(QString(":formType:%1").arg(m_formType));
        retVal.append(QString(":parent:%1").arg(m_elemParent));
        retVal.append(QString(":readOnly:%1").arg(ro));

    } else if (m_formType != uoFormT_Unknown){
        retVal.append( QString(":ELEM:%1").arg(m_elem)).append( QString(":type:%1:parent:%2:readOnly:%3").arg(m_formType).arg(m_elemParent).arg( ro ));
    }
    QString ouf = m_ovnerUrlForm;
    ouf = ouf.replace(":","/");
    if (withIID)
        retVal.append(QString(":IID:%1").arg(m_internalID));
    retVal.append(QString(":ovnerUrl:%2").arg(m_ovnerUrlForm));
    return retVal;
}

uoUrl::uoUrl(QString url)
{
    initEmpty();
    parseUrl(url);
}

uoUrl::uoUrl(QString table, int elem, uoFormType formtype, int parent)
{
    initEmpty();
    m_type = uoUrlType_Element;
    if (formtype == uoFormT_Group)
        m_type = uoUrlType_Group;
    m_formType = formtype ;
    m_elem = elem;
    m_elemParent = parent;
    m_table = table;

}

void uoUrl::initEmpty()
{
    m_type = uoUrlType_Unknown;
    m_formType = uoFormT_Unknown ;
    m_elem = -1;
    m_elemParent = UO_EMPTY_ELEMENT;
    m_table = "";
    m_ovnerUrlForm = "";
    m_selection = false;
    static long g_nextUrlNo = 0;
    m_internalID = ++g_nextUrlNo;
    m_readOnly = false;
    m_withElem = false;
}

void uoUrl::parseUrl(QString url)
{
    QStringList list = url.split(":");

    if (list.size()<3)    {
        return;
    }
    m_type = (uoUrlType)list.at(1).toInt();
    m_table = list.at(2);
    int pos = 0;

    if ("form" == list.at(1)){
        m_formType = (uoFormType) list.at(4).toInt();
    } else if ("elem" == list.at(1)){
        pos = list.indexOf("ELEM");
        if (pos == 3) {
            m_elem = list.at(4).toInt();
            m_formType = (uoFormType) list.at(6).toInt();
            m_elemParent = list.at(8).toInt();
        }
    }
    pos = list.indexOf("parent");
    if (pos != -1) {
        m_elemParent = list.at(pos+1).toInt();
    }

    pos = list.indexOf("IID");
    if (pos != -1) {
        m_internalID = list.at(pos+1).toInt();
    }
    pos = list.indexOf("ovnerUrl");
    if (pos != -1) {
        m_ovnerUrlForm = list.at(pos+1);
        m_ovnerUrlForm = m_ovnerUrlForm.replace("/",":");
    }
    pos = list.indexOf("readOnly");
    if (pos != -1) {
        int readOnly = list.at(pos+1).toInt();
        m_readOnly = (readOnly == 1) ? true : false;
    }

    // uoDbGuiFactory::parseIUrl "uo:elem:sp1:ELEM:-1:type:2:parent:-1"
    //    qDebug() << map;
}
