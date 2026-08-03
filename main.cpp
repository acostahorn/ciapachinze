#include "cirulla.h"
#include "CharacterManager.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#if defined(Q_OS_WIN)
#include <shobjidl.h>
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
#if defined(Q_OS_WIN)
#include <shobjidl.h>
    // ... e nel punto in cui richiami la funzione:
    SetCurrentProcessExplicitAppUserModelID(L"Genova.Cirulla.Game");
#endif

    a.setStyleSheet("QPushButton { border: 1px solid #8f8f91; border-radius: 4px; background-color: #f0f0f0; padding: 5px}"
                    "QComboBox { border: 1px solid #8f8f91; border-radius: 4px; background-color: #f0f0f0; padding: 5px; }");

    CharacterManager::loadFromDisk();

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages)
    {
        const QString baseName = "cirulla_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName))
        {
            a.installTranslator(&translator);
            break;
        }
    }
    Cirulla c;
    c.show();
    return a.exec();
}
