#include "uoabstractmdichild.h"
#include "uomdiobserver.h"
#include <QMdiSubWindow>
static int AMC_DOCUM_COUNT = 0;

uoAbstractMDIChild::uoAbstractMDIChild(QString type)
    :m_type(type)
{
}

uoAbstractMDIChild::~uoAbstractMDIChild()
{}


int uoAbstractMDIChild::nextDocumentNumber()
{
    AMC_DOCUM_COUNT += 1;
    return AMC_DOCUM_COUNT;
}



bool uoAbstractMDIChild::isActiveMdiWindow(QWidget *wi){
    bool retVal = false;
    uoComonMainWnd* mwnd = uoMdiObserver::instance()->mainWnd();
    if (mwnd){
        QMdiArea* area = mwnd->area();
        if (area){
            QMdiSubWindow* sw = area->currentSubWindow();
            if (sw && sw->widget() == wi)
                retVal = true;
        }
    }

    return retVal;
}
