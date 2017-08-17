#ifndef UOMDITEXTEDIT_H
#define UOMDITEXTEDIT_H

#include <QTextEdit>
#include <QAction>
#include <QMenu>
#include <QMainWindow>
#include <QTextCodec>
#include <QTextStream>
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>
#include <QCloseEvent>

#include "uoabstractmdichild.h"

class uoMdiTextEdit : public QTextEdit, public uoAbstractMDIChild
{
    Q_OBJECT
public:
    explicit uoMdiTextEdit(QWidget *parent = 0);
    virtual ~uoMdiTextEdit();
    virtual void onActivate(QMainWindow* mWnd );
    virtual void onDeactivate(QMainWindow* mWnd );
    virtual void canCloseClient();
    void fillMenu(QMainWindow* mWnd );
    void connectActions();
    void load(QString fileName);
    QString fileName() {return m_fileName; }
signals:
    
public slots:
	bool fileSave();
	bool fileSaveAs();
	bool fileClose();

	void undo_();
	void redo_();
	void cut_();
	void copy_();
	void paste_();
	void textChanged_();

private:
	void enableActions(bool enable);
	void updeteActions();
	bool openFile( QString newFile, QTextCodec *c=NULL  );
	bool saveFile( QString newFile, QTextCodec *c=NULL  );
	QMenu	*m_editMenu;


	void createActions();
	QString m_fileName;
	void updateCaption();
};

#endif // UOMDITEXTEDIT_H
