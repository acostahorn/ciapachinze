#include "cirulla.h"
#include "CharacterManager.h"
#include "HeadlessRunner.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#if defined(Q_OS_WIN)
#include <shobjidl.h>
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QStringList arguments = a.arguments();

#if defined(Q_OS_WIN)
    // ... e nel punto in cui richiami la funzione:
    SetCurrentProcessExplicitAppUserModelID(L"Genova.Cirulla.Game");
#endif
    if (arguments.contains("--headless"))
    {
        fprintf(stderr, ">>> [DEBUG] Trovato --headless nel main! Entro nel blocco.\n");
        fflush(stderr);

        CharacterManager::loadFromDisk();

        bool giocoACoppie = arguments.contains("--giocoACoppie");
        int numGames = 1; // Default
        for (int i = 0; i < arguments.size(); ++i)
        {
            if (arguments[i] == "--numGames" && i + 1 < arguments.size())
            {
                numGames = arguments[i + 1].toInt();
                break;
            }
        }

        HeadlessRunner runner;
        runner.runBatch(numGames, giocoACoppie);
        return 0; // Termina l'applicazione dopo l'esecuzione batch
    }

    QCoreApplication::setOrganizationName("Genova");
    QCoreApplication::setApplicationName("cirulla");

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
