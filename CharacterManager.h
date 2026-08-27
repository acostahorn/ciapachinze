#ifndef CHARACTERMANAGER_H
#define CHARACTERMANAGER_H

#include <QVector>
#include <QString>
#include <QJsonArray>

#include "GameTypes.h"

// Definiamo qui la struttura, così è accessibile ovunque includa questo header

class CharacterManager
{
public:
    // Il costruttore di solito non serve se i metodi sono statici
    CharacterManager();

    // Metodo statico per ottenere la lista

    static QVector<ProfileData> getPlayers(QString humanPlayerName);

    static void updatePlayerStats(const ProfileData &updatedProfile);

    static bool playerExists(const QString &name);
    static void createNewPlayer(const QString &name, const QString &avatarPath);

    static void saveToDisk();
    static void loadFromDisk();

    static QString getSaveFilePath();

    static void ensureBotsExist();

    // Nel CharacterManager.h
    static QVector<ProfileData> getAllPlayers()
    {
        CharacterManager manager; // Creiamo un'istanza temporanea
        manager.loadFromDisk();   // Carichiamo i dati dal file
        return m_registeredPlayers;
    }

private:
    static QVector<ProfileData> m_registeredPlayers;
    static QJsonArray serializeRankedMoves(const std::vector<RankedSituation> &rankedMoves);
    static std::vector<RankedSituation> deserializeRankedMoves(const QJsonArray &array);
};

#endif // CHARACTERMANAGER_H