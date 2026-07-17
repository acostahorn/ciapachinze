#include "CharacterManager.h"
#include "GameTypes.h"
#include <QString>

// --- QUESTA RIGA DEVE ESSERE QUI (fuori dalle funzioni) ---
QVector<ProfileData> CharacterManager::m_registeredPlayers = {
    {"Baciccia", "avatars/Baciccia.png", false},
    {"Ugo", "avatars/Ugo.png", false},
    {"Mussadiferro", "avatars/Mussadiferro.png", false},
    {"Alberto", "avatars/Alberto.png", true}};
// ---------------------------------------------------------

CharacterManager::CharacterManager() {}

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

    // Lo aggiungiamo alla lista in memoria
    m_registeredPlayers.append(newPlayer);

    saveToDisk();
}

void CharacterManager::saveToDisk()
{
}
