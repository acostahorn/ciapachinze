#include "CharacterManager.h"
#include "GameTypes.h"
#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

// --- QUESTA RIGA DEVE ESSERE QUI (fuori dalle funzioni) ---
QVector<ProfileData> CharacterManager::m_registeredPlayers = {
    {"Baciccia", "avatars/Baciccia.png", false},
    {"Ugo", "avatars/Ugo.png", false},
    {"Mussadiferro", "avatars/Mussadiferro.png", false},
    {"Alberto", "avatars/Alberto.png", true}};
// ---------------------------------------------------------

CharacterManager::CharacterManager()
{
}

QVector<ProfileData> CharacterManager::getPlayers(const QString humanPlayerName)
{
    QString humanPlayerTrimmed = humanPlayerName.trimmed();

    // Cerca il giocatore
    auto it = std::find_if(m_registeredPlayers.begin(), m_registeredPlayers.end(),
                           [&](const ProfileData &p)
                           { return p.name == humanPlayerTrimmed; });

    // Se non trovato, usa il default
    ProfileData profile = (it != m_registeredPlayers.end()) ? *it : ProfileData{"Alberto", "avatars/Alberto.png", true};

    return {
        {"Baciccia", "avatars/Baciccia.png", false},
        {"Ugo", "avatars/Ugo.png", false},
        profile,
        {"Mussadiferro", "avatars/Mussadiferro.png", false}};
}

void CharacterManager::updatePlayerStats(const ProfileData &updatedProfile)
{
    for (auto &p : m_registeredPlayers)
    {
        if (p.name == updatedProfile.name)
        {
            p = updatedProfile;
            saveToDisk();
            return; // Usciamo subito appena trovato, non serve continuare
        }
    }
}

bool CharacterManager::playerExists(const QString &name)
{
    QString nameTrimmed = name.trimmed();
    for (const auto &p : m_registeredPlayers)
    {
        if (p.name == nameTrimmed)
        {
            return true;
        }
    }
    return false;
}

void CharacterManager::createNewPlayer(const QString &name, const QString &avatarPath)
{
    // Creiamo il nuovo profilo
    ProfileData newPlayer;
    newPlayer.name = name.trimmed();
    newPlayer.avatarPath = avatarPath;
    newPlayer.isHuman = true;

    m_registeredPlayers.append(newPlayer);

    saveToDisk();
}

void CharacterManager::saveToDisk()
{
    QJsonArray playersArray;

    for (const auto &p : m_registeredPlayers)
    {
        QJsonObject playerObj;
        playerObj["name"] = p.name;
        playerObj["avatar"] = p.avatarPath;
        playerObj["isHuman"] = p.isHuman;
        playerObj["playedMatches"] = p.playedMatches;
        playerObj["wonMatches"] = p.wonMatches;
        playersArray.append(playerObj);
    }
    QJsonDocument doc(playersArray);
    QFile file("players.json");
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(doc.toJson());
        file.close();
    }
}

void CharacterManager::loadFromDisk()
{
    QFile file("players.json");
    if (!file.open(QIODevice::ReadOnly))
        return;

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonArray array = doc.array();

    m_registeredPlayers.clear();

    for (const auto &val : array)
    {
        QJsonObject obj = val.toObject();
        ProfileData p;
        p.name = obj["name"].toString();
        p.avatarPath = obj["avatar"].toString();
        p.isHuman = obj["isHuman"].toBool();
        p.playedMatches = obj["playedMatches"].toInt();
        p.wonMatches = obj["wonMatches"].toInt();

        m_registeredPlayers.append(p);
    }
    file.close();
}
