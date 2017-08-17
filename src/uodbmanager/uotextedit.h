#ifndef UOTEXTEDIT_H
#define UOTEXTEDIT_H

#include <QTextEdit>
#include <QKeyEvent>

class uoTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit uoTextEdit(QWidget *parent = 0);
    
signals:
    
public slots:
protected:
    void keyPressEvent ( QKeyEvent * event );
};

#endif // UOTEXTEDIT_H
