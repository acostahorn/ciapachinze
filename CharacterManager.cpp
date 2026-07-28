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
#include <random>

// --- QUESTA RIGA DEVE ESSERE QUI (fuori dalle funzioni) ---
QVector<ProfileData> CharacterManager::m_registeredPlayers = {
    {"Metirde", "avatars/Metirde.png", false},
    {"Cesarin", "avatars/Cesarin.png", false},
    {"Pippetto", "avatars/Pippetto.png", false},
    {"Steva", "avatars/Steva.png", false},
    {"Giggia", "avatars/Giggia.png", false},
    {"Colomba", "avatars/Colomba.png", false},
    {"Venanzio", "avatars/Venanzio.png", false},
    {"Alberto", "avatars/Alberto.png", true}};
// ---------------------------------------------------------

CharacterManager::CharacterManager()
{
    loadFromDisk();
    ensureBotsExist();
}

QVector<ProfileData> CharacterManager::getPlayers(const QString humanPlayerName)
{
    QVector<ProfileData> botPlayers;
    ProfileData humanPlayer;
    bool humanFound = false;

    for (const auto &p : m_registeredPlayers)
    {
        if (p.name == humanPlayerName.trimmed())
        {
            humanPlayer = p;
            humanFound = true;
        }
        else if (!p.isHuman)
        {
            botPlayers.append(p);
        }
    }

    // Se l'umano non esiste nel database, crealo al volo
    if (!humanFound)
    {
        humanPlayer = {humanPlayerName.trimmed(), "avatars/default.png", true};
    }

    // 2. Costruiamo il tavolo
    QVector<ProfileData> table;

    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(botPlayers.begin(), botPlayers.end(), g);

    table.append(botPlayers[0]);
    table.append(botPlayers[1]);
    table.append(humanPlayer);
    table.append(botPlayers[2]);

    return table;
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
    QString dataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    QDir dir(dataLocation);
    if (!dir.exists())
    {
        dir.mkpath(".");
    }

    return dataLocation + "/players.json";
}

void CharacterManager::ensureBotsExist()
{

    for (const QString &name : botNames)
    {
        if (!playerExists(name))
        {
            ProfileData bot;
            bot.name = name;
            bot.avatarPath = "avatars/" + name + ".png";
            bot.isHuman = false;
            bot.playedMatches = 0;
            bot.wonMatches = 0;

            m_registeredPlayers.append(bot);
        }
    }
    // Una volta aggiunti, salviamo lo stato iniziale su disco
    saveToDisk();
}
