#include "uodbguifactory.h"
#include <QDebug>
#include <QStringList>
#include <QVBoxLayout>
#include "uodblistform.h"
#include "uodbformelement.h"
#include "uodbtreeform.h"
#include "uourl.h"

uoDbGuiFactory::uoDbGuiFactory(QObject *parent, uoMainWindow *mainWnd) :
    QObject(parent),m_mainWnd(mainWnd)
{
    m_aGroup = 0;
    m_maper = new QSignalMapper(this);
    m_icon.append(QIcon(":/uodb/images/folder.png"));
    m_icon.append(QIcon(":/uodb/images/folder_add.png"));
    m_icon.append(QIcon(":/uodb/images/folder_delete.png"));
    m_icon.append(QIcon(":/uodb/images/element.png"));
    m_icon.append(QIcon(":/uodb/images/element_add.PNG"));
    m_icon.append(QIcon(":/uodb/images/element_delete.PNG"));
}

void uoDbGuiFactory::updateGui(uoDbManager *dbMan)
{
    m_dbMan = dbMan;
    delete m_aGroup;
    delete m_maper;
    m_maper = new QSignalMapper(this);
    m_aGroup = new QActionGroup(this);
    for (int i = 0; i<m_dbMan->m_tables.count(); i++){
        uoDbRefTable* refT = m_dbMan->m_tables.at(i);
        if (refT->isPublic()) {
            QAction * act = new QAction(refT->m_descr ,m_aGroup);
            QString data = refT->getUrlByForm(uoFormT_List);
            act->setData(data);
            connect(act, SIGNAL(triggered()), m_maper, SLOT(map()));
            m_maper->setMapping(act, data);
        }
    }

    connect(m_maper, SIGNAL(mapped(const QString &)),
            this, SLOT(parseIUrl(const QString &)));

}

QActionGroup *uoDbGuiFactory::getActions()
{
    return m_aGroup;
}

QIcon uoDbGuiFactory::getIcon(bool isFolder, bool isMark)
{
    if (isFolder){
        if (isMark){
            return m_icon.at(2);
        } else {
            return m_icon.at(0);
        }
    } else {
        if (isMark){
            return m_icon.at(5);
        } else {
            return m_icon.at(3);
        }
    }
    return m_icon.at(3);
}

bool uoDbGuiFactory::onEditStart(int elem, uoDbRefTable *tab)
{
    ///\todo - надо бы запрашивать побольше характеристик из БД за один раз. Возможно свести к структуе какой-то.
    //bool isFolder = tab->isFolder(elem);

    return true;
}


bool uoDbGuiFactory::parseIUrl(const QString & urlstr)
{

    bool sucsess = true;
    QStringList list = urlstr.split(":");

    if (list.size()<3)    {
        return false;
    }
    // uoDbGuiFactory::parseIUrl "uo:form:sp1:formType:0"
    QString tName = list.at(2);
    QString uType = list.at(1);
    uoFormType fTypeVl = uoFormT_Unknown;
    QString fTypeId = "";
    if (list.count()>=5){
        fTypeId = list.at(3);
        fTypeVl = (uoFormType)list.at(4).toInt();
    }
    if (uType == "form"){
        showForm(tName,fTypeVl,0,urlstr);
    } else if (uType == "elem") {
        fTypeVl = (uoFormType)list.at(6).toInt();
        int elem = list.at(4).toInt();
        int elemP = 0;
        if (list.count()>=9)
            elemP = list.at(8).toInt();
        showForm(tName,fTypeVl,elem,urlstr,elemP);
    }
    // uoDbGuiFactory::parseIUrl "uo:elem:sp1:ELEM:-1:type:2:parent:-1"
    return sucsess;
}

bool uoDbGuiFactory::parseUrl(const class uoUrl &url)
{
    showForm(url.table() , url.formType(),url.element() , url.toString() ,url.parent());
    return true;
}

void uoDbGuiFactory::showFormUrl(const class uoUrl& url)
{
    QString urlstr = url.toString();
//    qDebug() << "uoDbGuiFactory::showFormUrl urlstr = " << urlstr;
    if (uoDbRefTable* refTable = m_dbMan->findTable(url.table()))
    {
        if (QWidget* wi = m_mainWnd->findByDbURL(url.toString()))
        {
//            qDebug() << "Widget already exists";
            wi->show();
        }
        else
        {
            uoDbForm* form = getForm(url);
            if (form) {
                form->setObjectName(urlstr);
                QMdiSubWindow* sw = m_mainWnd->addTab(form , refTable->m_descr);
                if (sw)
                    form->setSW(sw);
            }
        }
    }
}

void uoDbGuiFactory::showForm(QString tabNameIntern, uoFormType fType, int IDElem, QString urlstr, int parent)
{
    if (uoDbRefTable* refTable = m_dbMan->findTable(tabNameIntern))
    {
        uoUrl url   = uoUrl(urlstr);
        showFormUrl(url);
    }
}

uoDbForm *uoDbGuiFactory::getForm(const uoUrl &url, QWidget* parent)
{
    uoDbForm* form = 0;
    QWidget* pa = parent;
    if (!pa)
        pa = m_mainWnd;
    if (uoDbRefTable* refTable = m_dbMan->findTable(url.table())) {
        if (url.formType() == uoFormT_List)
        {
            uoDbRefTableListIface* iFace = new uoDbRefTableListIface(this, refTable,url);
            iFace->setUrl(url);
            form = new uoDbFormList(pa, iFace);
        }
        else if (url.formType() == uoFormT_Tree){
            uoDbRefTableListIface* iFace = new uoDbRefTableListIface(this, refTable,url);
            iFace->setWithElem(url.isWithElem());
            iFace->setUrl(url);
            form = new uoDbTreeForm(pa, iFace);
            connect(form, SIGNAL(onClickedElement(const uoUrl&)), m_mainWnd, SLOT(onActivateUrl(const uoUrl&)));
//                    onActivateUrl(const uoUrl& url);
        }
        else if (url.formType() == uoFormT_Group || url.formType() == uoFormT_Elem)
        {
            uoDbRefTableIface* iFace = new uoDbRefTableIface(this, refTable, url, "*");
            iFace->setCurentsElement(url.element(), url.parent());
            form = new uoDbFormElement(pa,iFace);
        }
    }
    if (form)
        form->setObjectName(url.toString());

    return form;
}


QMessageBox::StandardButton uoDbGuiFactory::doQuerySaveElement()
{
    QMessageBox::StandardButton retVal =
            QMessageBox::question(m_mainWnd,QString(tr("Receptor")),tr("Save changes?"),QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    //QMessageBox::question(m_mainWnd,QString(tr("Rецептор")),"Сохранить изменения?",QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    return retVal;
}

bool uoDbGuiFactory::doQueruDeleteElement(uoElementData *elData)
{
    bool retVal = false;
    if (elData){
        if (elData->id() != UO_EMPTY_ELEMENT){
            //QString qString = QString("Пометить на удаление: '%1'").arg(elData->descr());
            QString qString = QString(tr("Mark as deleted: '%1'")).arg(elData->descr());
            if (elData->isMark())
                qString = QString(tr("Unmark deleted elem for: '%1'")).arg(elData->descr());
            //qString = QString("Снять пометку на удаление у: '%1'").arg(elData->descr());
            if (elData->isFolder())
                qString.append(tr("\n With process all childs?"));
                //qString.append(tr("\nС обработкой всех подчиненных?"));
            QMessageBox::StandardButton an = QMessageBox::question(m_mainWnd,QString(tr("Receptor")),qString,QMessageBox::Yes | QMessageBox::No);
            //QMessageBox::StandardButton an = QMessageBox::question(m_mainWnd,QString("Rецептор"),qString,QMessageBox::Yes | QMessageBox::No);
            if (an == QMessageBox::Yes)
                retVal = true;
        }
    }
    return retVal;
}
