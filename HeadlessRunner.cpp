#include "HeadlessRunner.h"
#include <QCoreApplication>
#include <QDebug>
#include "cirulla.h"

HeadlessRunner::HeadlessRunner()
{
}

void HeadlessRunner::runBatch(int numGames, bool giocoACoppie)
{
    fprintf(stderr, "=== [DEBUG] AVVIO RUNBATCH (Partite totali: %d) ===\n", numGames);
    fflush(stderr);

    Cirulla engine;
    engine.setTestGameMode(true);

    for (int game = 0; game < numGames; ++game)
    {
        fprintf(stderr, "\n--- [DEBUG] Inizio iterazione partita %d/%d ---\n", game + 1, numGames);
        fflush(stderr);

        bool matchEnded = false;

        QMetaObject::Connection connection = QObject::connect(
            &engine, &Cirulla::gameFinished,
            [&matchEnded](const QVector<ProfileData> &updatedPlayers)
            {
                fprintf(stderr, ">>> [DEBUG] SEGNALE gameFinished RICEVUTO! Partita completata. <<<\n");
                fflush(stderr);
                matchEnded = true;
            });

        fprintf(stderr, "[DEBUG] Chiamata a setupGame()...\n");
        fflush(stderr);

        engine.setupGame(engine.getPlayers("Alberto"), giocoACoppie);

        int safetyCounter = 0;
        const int maxTries = 500;

        fprintf(stderr, "[DEBUG] Entro nel ciclo while di attesa (processEvents)...\n");
        fflush(stderr);

        while (!matchEnded && safetyCounter < maxTries)
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

            // Un piccolo controllo di sicurezza per evitare loop infiniti muti durante il debug
            safetyCounter++;
        }

        if (!matchEnded)
        {
            fprintf(stderr, "[ERRORE CRITICO] Timeout sulla partita %d! Il segnale gameFinished non è arrivato.\n", game + 1);
            fflush(stderr);
        }
        else
        {
            fprintf(stderr, "[DEBUG] Partita %d terminata correttamente.\n", game + 1);
            fflush(stderr);
        }
        QObject::disconnect(connection);
    }

    fprintf(stderr, "=== [DEBUG] SIMULAZIONE COMPLETATA CON SUCCESSO ===\n");
    fflush(stderr);
}