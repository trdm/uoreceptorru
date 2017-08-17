/********************************************************************************
** Form generated from reading UI file 'uodbformelement.ui'
**
** Created: Sat 22. Oct 12:36:42 2011
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UODBFORMELEMENT_H
#define UI_UODBFORMELEMENT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_uoDbFormElement
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *m_TitleLabel;
    QGridLayout *m_gridLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *m_actionPB;
    QSpacerItem *horizontalSpacer;
    QPushButton *m_savePB;
    QPushButton *m_OKPB;
    QPushButton *m_ClosePB;

    void setupUi(QWidget *uoDbFormElement)
    {
        if (uoDbFormElement->objectName().isEmpty())
            uoDbFormElement->setObjectName(QString::fromUtf8("uoDbFormElement"));
        uoDbFormElement->resize(422, 171);
        verticalLayout = new QVBoxLayout(uoDbFormElement);
        verticalLayout->setSpacing(2);
        verticalLayout->setContentsMargins(3, 3, 3, 3);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        m_TitleLabel = new QLabel(uoDbFormElement);
        m_TitleLabel->setObjectName(QString::fromUtf8("m_TitleLabel"));
        m_TitleLabel->setMinimumSize(QSize(381, 16));
        m_TitleLabel->setMaximumSize(QSize(381, 16));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        m_TitleLabel->setFont(font);

        verticalLayout->addWidget(m_TitleLabel,0);

        m_gridLayout = new QGridLayout();
        m_gridLayout->setObjectName(QString::fromUtf8("m_gridLayout"));

        verticalLayout->addLayout(m_gridLayout,1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(2);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetMaximumSize);
        m_actionPB = new QPushButton(uoDbFormElement);
        m_actionPB->setObjectName(QString::fromUtf8("m_actionPB"));
        m_actionPB->setMinimumSize(QSize(75, 23));
        m_actionPB->setMaximumSize(QSize(75, 23));
        m_actionPB->setFocusPolicy(Qt::StrongFocus);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/uodb/images/table.png"), QSize(), QIcon::Normal, QIcon::Off);
        m_actionPB->setIcon(icon);

        horizontalLayout->addWidget(m_actionPB);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        m_savePB = new QPushButton(uoDbFormElement);
        m_savePB->setObjectName(QString::fromUtf8("m_savePB"));
        m_savePB->setMinimumSize(QSize(75, 23));
        m_savePB->setMaximumSize(QSize(75, 23));
        m_savePB->setFocusPolicy(Qt::StrongFocus);

        horizontalLayout->addWidget(m_savePB);

        m_OKPB = new QPushButton(uoDbFormElement);
        m_OKPB->setObjectName(QString::fromUtf8("m_OKPB"));
        m_OKPB->setMinimumSize(QSize(75, 23));
        m_OKPB->setMaximumSize(QSize(75, 23));
        m_OKPB->setFocusPolicy(Qt::StrongFocus);

        horizontalLayout->addWidget(m_OKPB);

        m_ClosePB = new QPushButton(uoDbFormElement);
        m_ClosePB->setObjectName(QString::fromUtf8("m_ClosePB"));
        m_ClosePB->setMinimumSize(QSize(75, 23));
        m_ClosePB->setMaximumSize(QSize(75, 23));
        m_ClosePB->setFocusPolicy(Qt::StrongFocus);

        horizontalLayout->addWidget(m_ClosePB);

        horizontalLayout->setStretch(1, 1);

        verticalLayout->addLayout(horizontalLayout,0);

//        verticalLayout->setStretch(0, 0);
//        verticalLayout->setStretch(1, 1);
//        verticalLayout->setStretch(2, 0);

        retranslateUi(uoDbFormElement);

        QMetaObject::connectSlotsByName(uoDbFormElement);
    } // setupUi

    void retranslateUi(QWidget *uoDbFormElement)
    {
        uoDbFormElement->setWindowTitle(QApplication::translate("uoDbFormElement", "Form", 0, QApplication::UnicodeUTF8));
        m_TitleLabel->setText(QApplication::translate("uoDbFormElement", "TextLabel", 0, QApplication::UnicodeUTF8));
        m_actionPB->setText(QApplication::translate("uoDbFormElement", "\320\224\320\265\320\271\321\201\321\202\320\262\320\270\321\217", 0, QApplication::UnicodeUTF8));
        m_savePB->setText(QApplication::translate("uoDbFormElement", "\320\227\320\260\320\277\320\270\321\201\320\260\321\202\321\214", 0, QApplication::UnicodeUTF8));
        m_OKPB->setText(QApplication::translate("uoDbFormElement", "\320\236\320\232", 0, QApplication::UnicodeUTF8));
        m_ClosePB->setText(QApplication::translate("uoDbFormElement", "\320\227\320\260\320\272\321\200\321\213\321\202\321\214", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class uoDbFormElement: public Ui_uoDbFormElement {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UODBFORMELEMENT_H
