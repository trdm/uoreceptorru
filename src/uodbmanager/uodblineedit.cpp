#include "uodblineedit.h"

uoDbLineEdit::uoDbLineEdit(QWidget *parent, uoDbLineEditFlags flags) :
    QWidget(parent),m_flags(flags)
{
    setupUi(this);
    setFlags(flags);
}

QString uoDbLineEdit::text() const {
    return m_lineEdit->text();
}

void uoDbLineEdit::setText(const QString &txt){
    m_lineEdit->setText(txt);
}

void uoDbLineEdit::setFlags(const uoDbLineEditFlags &flags){
    m_flags = flags;
    if (!(m_flags & uoDBLe_Choice))         {m_chooseTB->hide();     } else {        m_chooseTB->show();    }
    if (!(m_flags & uoDBLe_ChoiceArrow ))   {m_chooseTBA->hide();    } else {        m_chooseTBA->show();    }
    if (!(m_flags & uoDBLe_Open ))          {m_openTB->hide();       } else {        m_openTB->show();    }
    if (!(m_flags & uoDBLe_Clear ))         {m_clearTB->hide();      } else {        m_clearTB->show();    }
}

void uoDbLineEdit::setData(uoElementData *elData){
    m_elData = elData;
    m_lineEdit->setReadOnly(true);
    if (m_elData){
        setText(m_elData->title());
    } else {
        setText("");
    }
}

void uoDbLineEdit::changeEvent(QEvent *e)
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
