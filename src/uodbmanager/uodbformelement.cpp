#include "uodbformelement.h"
#include "uodblineedit.h"
#include <QCloseEvent>
#include <QTextEdit>
#include <QDebug>

uoDbFormElement::uoDbFormElement(QWidget *parent, uoDbRefTableIface* refIFace):
    uoDbForm(parent),m_iFace(refIFace)
{
    // пора строить форму.
    setupUi(this);
    m_elData = m_iFace->load(m_iFace->curentId(),1);
    if (m_iFace->curentId() == UO_EMPTY_ELEMENT ) {
        uoUrl url = m_iFace->url();
        if (url.isFolder())
            m_elData->setFolder(1);
    }
    createControls();
    connect(m_ClosePB,SIGNAL(pressed()),this, SLOT(onCloseBtn()));
    connect(m_savePB,SIGNAL(pressed()),this, SLOT(onSaveBtn()));
    connect(m_OKPB,SIGNAL(pressed()),this, SLOT(onOkBtn()));
    setWindowIcon(m_iFace->getIcon(m_elData->isFolder(), m_elData->isMark()));
}

uoDbFormElement::~uoDbFormElement()
{

}

void uoDbFormElement::setTitle(){
    QString titleNew = m_iFace->m_table->m_descr;
    titleNew.append(": ").append(m_elData->title().trimmed());
    if (m_elData->isDirty())
        titleNew.append(" *");
    setWindowTitle(titleNew);
}

void uoDbFormElement::createControls()
{
    m_TitleLabel->setText(m_iFace->m_table->m_descr);
    setTitle();
    uoUiFields* uiFlds = 0;
    QString dbName = "";
    QString labelTitle = "";
    QList<QWidget*> wiList; // for widgets with fokus
    bool readOnly = m_iFace->url().isReadOnly();
    int maxLen = 0;
    for (int i = 0; i< m_iFace->m_tabField.count() ; i++){
        uiFlds = m_iFace->m_tabField.at(i);
        if (uiFlds){
            uoDbFieldType fldType = fieldType_String;
            dbName = uiFlds->dbName();
            uoElementData* elData = 0;
            if (dbName == UO_DB_FIELDNAME_CODE){
                labelTitle = "Код";
                maxLen = m_iFace->m_table->m_codeLen;
            } else if (dbName == UO_DB_FIELDNAME_TITLE){
                labelTitle = "Наименование";
                maxLen = m_iFace->m_table->m_titleLen;

            } else if (dbName == UO_DB_FIELDNAME_PARENTID){
                labelTitle = "Родитель";
                maxLen = m_iFace->m_table->m_titleLen;
                elData = m_elData->parent();
                fldType = fieldType_TableRef;
            } else {
                uoDbField* dbField = m_iFace->m_table->fieldGet(dbName);
                if (dbField){
                    labelTitle = dbField->title();
                    maxLen = dbField->m_len;
                    fldType = dbField->m_fieldType;
                } else {
                    continue;
                    // поле не существует.
                }
            }
            QLabel* lab = new QLabel(this);
            lab->setAlignment(Qt::AlignLeft|Qt::AlignJustify|Qt::AlignTop);
            lab->setText(labelTitle);
            lab->setEnabled(!readOnly);
            m_gridLayout->addWidget(lab, i, 0);
            if (fldType == fieldType_String || fldType == fieldType_Number || fldType == fieldType_Date  ){
                QLineEdit* lineEdit = new QLineEdit(this);
                lineEdit->setObjectName(QString("QLineEdit/%1").arg(dbName));

                lineEdit->setMaxLength(maxLen);
                if (m_elData->hasData(dbName)){
                    lineEdit->setText(m_elData->data(dbName).toString());
                }
                m_gridLayout->addWidget(lineEdit, i, 1);
                m_mapWidget.insert(dbName, lineEdit);
                lab->setBuddy(lineEdit);
                wiList.append(lineEdit);
                connect(lineEdit, SIGNAL(textChanged(const QString&)),this,SLOT(dataChangeLE()) );
                lineEdit->setEnabled(!readOnly);
            } else if (fldType == fieldType_TableRef ){
                uoDbLineEdit* lineEdit = new uoDbLineEdit(this);
                lineEdit->setData(elData);
                m_gridLayout->addWidget(lineEdit, i, 1);
                m_mapWidget.insert(dbName, lineEdit);
                lab->setBuddy(lineEdit);
                wiList.append(lineEdit);
                lineEdit->setEnabled(!readOnly);
            } else if (fldType == fieldType_Text || fldType == fieldType_TextHTML){
                uoTextEdit * lineEdit = new uoTextEdit(this);
                lineEdit->setObjectName(QString("uoTextEdit/%1").arg(dbName));
                if (fldType == fieldType_Text)
                    lineEdit->setAcceptRichText(false);
                if (m_elData->hasData(dbName)){
                    if (fldType == fieldType_Text)
                        lineEdit->setPlainText(m_elData->data(dbName).toString());
                    else
                        lineEdit->setHtml(m_elData->data(dbName).toString());
                }
                m_gridLayout->addWidget(lineEdit, i, 1);
                m_mapWidget.insert(dbName, lineEdit);
                lab->setBuddy(lineEdit);
                connect(lineEdit, SIGNAL(textChanged()),this,SLOT(dataChangeTE()) );
                wiList.append(lineEdit);
                lineEdit->setEnabled(!readOnly);
            }

        }
    }
    QWidget* wi = 0;
    for (int i = 0; i<wiList.count(); i++ ){
        if (wi){
            setTabOrder(wi,wiList.at(i));
        }
        wi = wiList.at(i);
        if (i == 0)
            setFocusProxy(wi);
    }

    if (wi){
        setTabOrder(wi,m_OKPB);
        setTabOrder(m_OKPB,m_ClosePB);
        setTabOrder(m_ClosePB,m_actionPB);
        setTabOrder(m_actionPB, m_savePB);
        //setTabOrder(m_savePB, wi);
    }
//    qDebug() << focusWidget();
}


void uoDbFormElement::onCloseBtn(){
    bool ok = true;
    if (m_iFace){
        ok = m_iFace->doCloseForm(m_elData);
        if (ok){
            m_elData->setDirty(false);
            if (m_mdiParent)
                m_mdiParent->close();
        }
    }
}

void uoDbFormElement::onOkBtn(){
    bool ok = true;
    if (m_iFace){
        if (m_elData->isDirty()) {
            ok = m_iFace->m_table->save(m_elData);
            if (ok){
                m_elData->setDirty(false);
            }
        }
        setTitle();
        if (m_mdiParent)
            m_mdiParent->close();
    }

}

void uoDbFormElement::onSaveBtn(){
    bool ok = true;
    if (m_iFace){
        if (m_elData->isDirty()) {
            ok = m_iFace->m_table->save(m_elData);
            if (ok){
                m_elData->setDirty(false);
            }
            setTitle();
        }
    }
}

void uoDbFormElement::dataChangeLE(){
    QObject* obg = sender();
    if (obg && m_elData){
        QWidget* wi = (QWidget*)obg;
        if (wi){
            QMapIterator<QString, QWidget*> i(m_mapWidget);
            while (i.hasNext()) {
                i.next();
                if (wi == i.value()){
                    wi = i.value();
                    if (!wi)
                        return;
                    QLineEdit* le = (QLineEdit*)wi;
                    if (le){
                        m_elData->setData(i.key(),le->text() );
                        setTitle(); // Нужно еще символ модификации обновить
                    }
                    break;
                }
            }
        }
    }
}

void uoDbFormElement::dataChangeTE(){
    QObject* obg = sender();
    if (obg && m_elData){
        QWidget* wi = (QWidget*)obg;
        if (wi){
            QMapIterator<QString, QWidget*> i(m_mapWidget);
            while (i.hasNext()) {
                i.next();
                if (wi == i.value()){
                    wi = i.value();
                    if (!wi)
                        return;

                    uoTextEdit* te = (uoTextEdit*)wi;
                    if (te){
                        if (te->acceptRichText())
                            m_elData->setData(i.key(),te->toHtml() );
                        else
                            m_elData->setData(i.key(),te->toPlainText() );
                        setTitle(); // Нужно еще символ модификации обновить
                    }
                    break;
                }
            }
        }
    }
}


void uoDbFormElement::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

void uoDbFormElement::closeEvent(QCloseEvent *cev)
{
    bool ok = true;
    if (m_iFace){
        ok = m_iFace->doCloseForm(m_elData);
    }
    cev->setAccepted(ok);
    if (ok)
        QWidget::closeEvent(cev);
}

void uoDbFormElement::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    if (key == Qt::Key_Escape){
        bool ok = true;
        if (m_iFace){
            ok = m_iFace->doCloseForm(m_elData);
            if (ok){
                m_elData->setDirty(false);
                if (m_mdiParent)
                    m_mdiParent->close();
            }
        }
    } else if (key == Qt::Key_Enter || key == Qt::Key_Return){
        if (qApp->keyboardModifiers() & Qt::ControlModifier){
            QWidget::keyPressEvent(event);
            onOkBtn();
            return;
        } else {
//            // не пашет..
//            QKeyEvent *eventN = new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab,qApp->keyboardModifiers());
//            QWidget::keyPressEvent(eventN);
//            return;
        }
    }
    QWidget::keyPressEvent(event);
}

void uoDbFormElement::focusInEvent(QFocusEvent *event){
    QWidget::focusInEvent(event);
}

