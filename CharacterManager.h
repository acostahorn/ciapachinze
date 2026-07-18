#ifndef CHARACTERMANAGER_H
#define CHARACTERMANAGER_H

#include <QVector>
#include <QString>
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

    // Nel CharacterManager.h
    static QVector<ProfileData> getAllPlayers() { return m_registeredPlayers; }

private:
    static QVector<ProfileData> m_registeredPlayers;
};

#endif // CHARACTERMANAGER_H