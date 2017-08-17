#ifndef UOCOMONMAINWND_H
#define UOCOMONMAINWND_H

#include <QAction>
#include <QMdiArea>
#include <QMainWindow>
#include <QMap>
#include <QDir>
#include <QFontComboBox>

#include "uomditextedit.h"
#include "uoabstractmdichild.h"
#include "uorichtextedit.h"
#include "linkproperty.h"
class QAction;
class QMenu;
class QToolBar;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;

class uoComonMainWnd : public QMainWindow
{
    Q_OBJECT
public:
    explicit uoComonMainWnd(QWidget *parent = 0);
    virtual ~uoComonMainWnd();

    QActionGroup*   m_actGrpFile;
    QAction*        m_actNew, *m_actNewRecept, *m_actNewQHTML, *m_actOpen, *m_actSave, *m_actQuit, *m_actHelp, *m_actAbout;
    QAction         *m_clear, *m_filePrint, *m_filePrintPreview, *m_fileExportPdf;
    QMenu           *m_menuNewFile;
    QActionGroup*   m_actGrpEditSimple;
    QAction    *m_actUndo,    *m_actRedo,    *m_actCopy,    *m_actCut,       *m_actPaste;

    QActionGroup*   m_actGrpFind;
    QAction *m_actFind,    *m_actFindUp,    *m_actFindDown;
    QComboBox *m_searchCombo;

    QActionGroup*   m_actEditFont;
    QAction      *m_actFontBold,    *m_actFontItalic, *m_actFontUnderline;

	QActionGroup *m_alignGroup;
	QAction *m_actionAlignRight,*m_actionAlignLeft, *m_actionAlignCenter, *m_actionAlignJustify;

	QActionGroup *m_listGroup;
	QAction *m_actionListDisc, *m_actionListCircle, *m_actionListSquare, *m_actionListDecmial, *m_actionListLowerAlpha, *m_actionListUpperAlpha;
	QAction *m_actionTextColor, *m_actionBGColor, *m_actionHiperLink;
	QAction *m_actionTableInsert, *m_actionTableEdit, *m_actionTableMergeCell, *m_actionTableRemoveRow, *m_actionTableRemoveCool, *m_actionTableAppendRows, *m_actionTableAppendCools;
	QFontComboBox* m_fontComboBox;
	QComboBox *m_comboSize;


	QAction *m_closeAct, *m_closeAllAct, *m_tileAct, *m_cascadeAct, *m_nextAct, *m_previousAct,*m_separatorAct;

	virtual void setupGui();

	void enabledActFont(bool ena);
	void enableActions(bool enable);
	QMdiArea* area() { return m_area; }
	QWidget* findByDbURL(QString url);
	void editText(QString fName, QString caption);
	void connectinsForRichTextEdit(uoMdiRichTextEdit* te);
	QMdiSubWindow *addTab(QWidget* wi, QString name);
virtual
	LinkProperties* getLinkPropDlg();

signals:
	void updateRecept(QString html);

public slots:
    void subWindowActivated(QMdiSubWindow* sw);
    void onNewFile();
    void onNewQHTML();
    void onOpenFile();
    void onShowHelp();
    void onShowAbout();
private slots:
    void updateWindowMenu();
    void setActiveSubWindow(QWidget *window);

protected:
    void deactivateOther(QMdiSubWindow* sw);
    void activateDeactivate(QMdiSubWindow* sw, bool activate = true);
    QWidget* findByFileName(QString file, bool show = false);
    QMdiArea* m_area;
    QMenu *m_menuFile;
    QMenu *m_menuWindows;
    QMenu *m_menuToolBars;
    QMenu *m_menuHelp;
    QToolBar *m_tbFile;
    QToolBar *m_tbEditSimple;
    QToolBar *m_tbEditFont;
    QToolBar *m_tbEditTable;
    QToolBar *m_tbEditLists;
    QToolBar *m_tbSearch;
    QAction *m_tbFileTVAct, *m_tbEditSimpleTVAct, *m_tbEditFontTVAct, *m_tbEditTableTVAct, *m_tbEditListsTVAct, *m_tbSearchTVAct;

    QSignalMapper* m_windowMapper;
    QMdiSubWindow *addSubWindow(QWidget* wi);

	LinkProperties* m_linkprop;

    void createActions();
    void preCreateGui();
    void createMemu();
    void prepareConections();
    virtual void menuFillStep1();
    virtual void menuFillStep2();

};

#endif // UOCOMONMAINWND_H
