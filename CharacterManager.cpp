#include "CharacterManager.h"
#include "GameTypes.h"
#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QVector>
#include <QDir>
#include <QStandardPaths>

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
    bool found = false;

    // Debug: cosa stiamo cercando?
    fprintf(stderr, "DEBUG: Cerco '%s' (isHuman: %d) tra %zu giocatori in memoria.\n",
            qPrintable(updatedProfile.name), updatedProfile.isHuman, m_registeredPlayers.size());

    for (auto &p : m_registeredPlayers)
    {
        // Debug: vediamo cosa c'è in memoria
        // fprintf(stderr, "   Confronto con: '%s'\n", qPrintable(p.name));

        if (p.name == updatedProfile.name)
        {
            found = true;
            p = updatedProfile;
            saveToDisk();
            fprintf(stderr, "DEBUG: '%s' trovato e aggiornato!\n", qPrintable(updatedProfile.name));
            return;
        }
    }

    if (!found)
    {
        fprintf(stderr, "ERRORE: Profilo '%s' NON TROVATO!\n", qPrintable(updatedProfile.name));
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
    QFile file(getSaveFilePath());
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(doc.toJson());
        file.close();
    }
}

void CharacterManager::loadFromDisk()
{
    QFile file(getSaveFilePath());

    if (!file.exists())
    {
        fprintf(stderr, "Nessun file di salvataggio trovato in: %s. Creo una lista vuota.\n", qPrintable(file.fileName()));
        return;
    }

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

QString CharacterManager::getSaveFilePath()
{
    // 1. Ottieni il percorso della cartella dati dell'utente (es. ~/.local/share/Cirulla su Linux)
    QString dataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    // 2. Assicurati che la cartella esista
    QDir dir(dataLocation);
    if (!dir.exists())
    {
        dir.mkpath(".");
    }

    // 3. Ritorna il percorso completo al file
    return dataLocation + "/players.json";
}