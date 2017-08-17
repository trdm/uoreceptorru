#include <QtGui/QApplication>
#include <QTranslator>
#include "uomainwindow.h"
#include "uomdiobserver.h"
#include "uocomonmainwnd.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator qtTranslator;
    qtTranslator.load("qt_ru.qm");
    a.installTranslator(&qtTranslator);

    QTranslator myappTranslator;
    myappTranslator.load("uoReceptor_ru.qm");
    a.installTranslator(&myappTranslator);

    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	uoMainWindow w;
	//uoComonMainWnd w;
    w.show();
    
    return a.exec();
}
