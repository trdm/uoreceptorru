#ifndef UOABSTRACTMDICHILD_H
#define UOABSTRACTMDICHILD_H

class QMainWindow;
#include <QString>
#include <QMap>
#include <QAction>
#include <QMainWindow>


class uoAbstractMDIChild
{
public:
    uoAbstractMDIChild(QString type);
    virtual ~uoAbstractMDIChild();
    virtual void onActivate(QMainWindow* mWnd ) = 0;
    virtual void onDeactivate(QMainWindow* mWnd ) = 0;
    virtual void canCloseClient() = 0;
    static int nextDocumentNumber();
    bool isActiveMdiWindow(QWidget* wi);
    void setName(QString name) {  m_name = name; }
    QString name() {return m_name;}

private:
    QMap<int, QAction*> m_acttionMap;
    QString m_name;
    QString m_type;

};

#endif // UOABSTRACTMDICHILD_H
