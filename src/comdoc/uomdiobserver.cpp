#include "uomdiobserver.h"
#include <QApplication>
#include <QDir>
#include <QFile>

uoMdiObserver* uoMdiObserver::m_self = 0;

uoMdiObserver::uoMdiObserver(){
    setDataPathFolder();
    m_receptTableId = -1;
}

uoMdiObserver::~uoMdiObserver()
{
    //dtor
}

void uoMdiObserver::setMainWnd(uoComonMainWnd *mw){
     m_mWindow = mw;
}

void uoMdiObserver::setDataPathFolder(QString dataPath)
{
    m_dataPathFolder = qApp->applicationDirPath() +"/Data/";
#ifdef Q_OS_LINUX
    QDir home = QDir::home();
    if (!home.exists(".uoReceptor")) {
           home.mkpath(".uoReceptor");
       }
    m_dataPathFolder = home.absolutePath() + "/.uoReceptor/" +"/Data/";
#endif

    if (!QFile::exists(m_dataPathFolder)){
        QDir* dir = new QDir(qApp->applicationDirPath());
#ifdef Q_OS_LINUX
        dir = new QDir(home.absolutePath() + "/.uoReceptor/");
#endif
        if (!dir->mkdir(m_dataPathFolder))
            m_dataPathFolder = qApp->applicationDirPath() +"/";
#ifdef Q_OS_LINUX
            m_dataPathFolder = home.absolutePath() + "/.uoReceptor" +"/";
#endif
    }
}

void uoMdiObserver::setVersion(QString object, QString version){
    m_object = object;
    m_version = version;
    QString ver = QString("%1: %2\n").arg(m_object).arg( m_version);
    m_versionList.prepend(ver);
}

QString uoMdiObserver::version(){
    return m_version;
}

QString uoMdiObserver::versionAll(){
    return m_versionList;
}

QString uoMdiObserver::dataPathFolder(){
    return m_dataPathFolder;
}
uoMdiObserver* uoMdiObserver::instance()
{
    if (!m_self) {
        m_self = new uoMdiObserver();
    }
    return m_self;
}


