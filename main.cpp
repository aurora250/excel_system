#include "mainwindow.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "setting.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "excel_system_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    MainWindow w;
    w.setWindowTitle("Excel");
    w.setWindowIcon(QIcon(setting::title_icon));
    w.showMaximized();
    w.setMinimumSize(700, 400);
    return a.exec();
}
