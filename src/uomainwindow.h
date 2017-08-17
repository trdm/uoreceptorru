#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "uocomonmainwnd.h"

#include "uomditextedit.h"
#include "uoabstractmdichild.h"
#include "uorichtextedit.h"
#include "uodbmanager/uodbmanager.h"
#include "uodbmanager/uodbform.h"

class QAction;
class QMenu;
class QToolBar;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;
class QDockWidget;


class uoMainWindow : public uoComonMainWnd
{
    Q_OBJECT
    
public:
    explicit uoMainWindow(QWidget *parent = 0);
    virtual ~uoMainWindow();
    virtual void setupGui();

    QAction *m_editFile_01, *m_editFile_02, *m_editFile_03;

public slots:
    void onNewRecept();
    void onActivateUrl(const uoUrl& url);
    void editFile_01();
    void editFile_02();
    void editFile_03();


protected:
    void createActions();
    void createMemu();
    virtual void prepareConections();

    uoDbManager* m_dbMan;
    uoDbForm*	 m_docTreeFormRecept;
    uoDbRefTable* m_receptTable;
    uoDbField* m_templField;
    QAction *m_actionShowDocRecept;
    QMenu *m_menuRecept;
    QDockWidget* m_receptTemplDocWidget;

    void createTables();
    virtual void menuFillStep1();
    virtual void menuFillStep2();

};

#endif // MAINWINDOW_H
