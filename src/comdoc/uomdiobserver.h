#ifndef UOMDIOBSERVER_H
#define UOMDIOBSERVER_H
#include "uomainwindow.h"
#include "uocomonmainwnd.h"

class uoMdiObserver
{
public:
    static uoMdiObserver* instance();
    ~uoMdiObserver();
    void setMainWnd(uoComonMainWnd* mw);
    uoComonMainWnd* mainWnd() {return m_mWindow; }
    void    setDataPathFolder(QString dataPathFolder = "");
    void    setVersion(QString object, QString version);
    QString version();
    QString versionAll();
    QString dataPathFolder();
    int m_receptTableId;
private:
    uoMdiObserver();
    uoComonMainWnd* m_mWindow;
protected:
    QString m_dataPathFolder;
    QString m_object, m_version;
    QString m_versionList;
    static uoMdiObserver* m_self;
};




#endif // UOMDIOBSERVER_H
