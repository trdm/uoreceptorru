#ifndef UODBGUIFACTORY_H
#define UODBGUIFACTORY_H

#include <QObject>
#include <QMainWindow>
#include <QMessageBox>
#include "uodbmanager.h"
#include <QAction>
#include <QActionGroup>
#include <QList>
#include <QSignalMapper>
#include "uomainwindow.h"
#include "uourl.h"
#include "uodbform.h"

class uoDbGuiFactory : public QObject
{
    Q_OBJECT
public:
    explicit uoDbGuiFactory(QObject *parent, uoMainWindow* mainWnd);
    void updateGui(uoDbManager* dbMan);
    QActionGroup *getActions();
    QIcon getIcon(bool isFolder, bool isMark);
    bool onEditStart(int elem, uoDbRefTable* tab);
    void showFormUrl(const class uoUrl& url);
    void showForm(QString tabNameIntern, uoFormType fType, int IDElem = 0, QString urlstr="", int parent = UO_EMPTY_ELEMENT);
    uoDbForm* getForm(const class uoUrl& url, QWidget *parent = 0);
    QMessageBox::StandardButton doQuerySaveElement();
    bool doQueruDeleteElement(uoElementData* elData); // elData
signals:
    
public slots:
    bool parseIUrl(const QString &urlstr);
    bool parseUrl(const class uoUrl& url);
private:
    uoMainWindow* m_mainWnd;
    uoDbManager* m_dbMan;
    QActionGroup* m_aGroup;
    QSignalMapper* m_maper;



    QList<QIcon> m_icon;
};

#endif // UODBGUIFACTORY_H
