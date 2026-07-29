#include "cirulla.h"
#include "CharacterManager.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


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
