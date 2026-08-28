#ifndef GAMETYPES_H
#define GAMETYPES_H

#include <QtCore/QString>
#include <QVector>
#include <vector>
#include <QList>

enum class GameMode
{
    Offline,
    Online
};

enum class SeatType
{
    Human,
    Bot,
    Empty
};

enum class MatchPhase
{
    Configuration,
    Dealing,
    Playing,
    Scoring,
    MatchOver
};

enum class Seme
{
    Picche,
    Cuori,
    Denari,
    Fiori
};

inline QString semeToString(Seme s)
{
    switch (s)
    {
    case Seme::Picche:
        return "S";
    case Seme::Cuori:
        return "H";
    case Seme::Denari:
        return "D";
    case Seme::Fiori:
        return "C";
    }
    return "Unknown";
}

enum class Mood
{
    Normal,
    Annoyed,
    Happy
};

enum GamePhase
{
    STATE_SELECTING_DEALER,
    STATE_READY_TO_START
};

enum FaseBottone
{
    FaseAvvio,
    FaseSmazzata,
    DopoScopaMazziere,
    FaseDopoBuona,
    FaseFineTurno,
    RitornoHomeScreen
};

// --- Core Structural Objects ---

struct Carta
{
    int id = -1; // Unique ID (0 to 39), -1 means unset/empty
    Seme seme = Seme::Picche;
    int faceValue = 0; // 1 to 10, 0 means unset/empty

    bool isMatta() const
    {
        return (faceValue == 7 && seme == Seme::Cuori); // 7 of Cups custom rule
    }
    QString toString() const
    {
        return QString("<%1>").arg(id);
    }
    bool operator==(const Carta &other) const
    {
        return id == other.id;
    }
};

struct Mossa
{
    int handIndex;
    QList<int> tableIndices;
    int ranking;
    bool operator==(const Mossa &other) const
    {
        return handIndex == other.handIndex && tableIndices == other.tableIndices;
    }
};

struct Situation
{
    Carta played;
    QVector<Carta> taken;
    QVector<Carta> hand;
    QVector<Carta> table;
    QVector<Carta> alreadyPlayed;

    bool isEmpty() const
    {
        return played.id == -1 && played.faceValue == 0 &&
               taken.isEmpty() && hand.isEmpty() &&
               table.isEmpty() && alreadyPlayed.isEmpty();
    }

    void normalize()
    {
        std::sort(hand.begin(), hand.end(), [](const Carta &a, const Carta &b)
                  { return a.id < b.id; });
        std::sort(table.begin(), table.end(), [](const Carta &a, const Carta &b)
                  { return a.id < b.id; });
    }
    QString toString() const
    {
        QString result = "Move: played card=" + played.toString() + ", taken cards=[";
        for (int i = 0; i < taken.size(); ++i)
        {
            result += taken[i].toString();
            if (i < taken.size() - 1)
                result += ", ";
        }
        result += "]\n| Hand: ";
        for (const auto &c : hand)
        {
            result += c.toString() + " ";
        }
        result += "\n| Table: ";
        for (const auto &c : table)
        {
            result += c.toString() + " ";
        }
        result += "\n| Played: ";
        for (const auto &c : alreadyPlayed)
        {
            result += c.toString() + " ";
        }
        return result;
    }
    bool operator==(const Situation &other) const
    {
        int limit = 10;
        auto recentThis = alreadyPlayed.size() <= limit
                              ? alreadyPlayed
                              : alreadyPlayed.mid(alreadyPlayed.size() - limit);

        auto recentOther = other.alreadyPlayed.size() <= limit
                               ? other.alreadyPlayed
                               : other.alreadyPlayed.mid(other.alreadyPlayed.size() - limit);

        return played.id == other.played.id &&
               table == other.table &&
               taken == other.taken &&
               recentThis == recentOther;
    }
};

struct RankedSituation
{

    Situation situation;
    int rank;
};

struct ProfileData
{
    QString name;
    QString avatarPath;

    bool isHuman = false;

    // Statistiche persistenti
    int playedMatches = 0;
    int wonMatches = 0;

    // Helper per il calcolo percentuale
    double getWinRate() const
    {
        if (playedMatches == 0)
            return 0.0;
        return (static_cast<double>(wonMatches) / playedMatches) * 100.0;
    }
    double getRating() const
    {
        if (playedMatches == 0)
            return 0.0;

        return (static_cast<double>(wonMatches) + 2.5) / (playedMatches + 10) * 100.0;
    }
    std::vector<Situation> moves;             // Elenco delle mosse effettuate dal giocatore durante la singola partita, utile per l'analisi delle strategie, il confronto con lo storico delle mosse e per il miglioramento dell'IA.
    std::vector<RankedSituation> rankedMoves; // Storico delle mosse con ranking, utile per l'analisi delle strategie e per il miglioramento dell'IA.

    QString rankedMovesToString() const
    {
        QString result;
        for (const auto &rankedSituation : rankedMoves)
        {
            result += QString("Situation: %1\nRank: %2\n___________________________________________\n").arg(rankedSituation.situation.toString()).arg(rankedSituation.rank);
        }
        return result;
    };
};

struct Score
{
    int scope = 0;
    int carte = 0;
    int denari = 0;
    int primiera = 0;
    int settebello = 0;
    int grande = 0;
    int piccola = 0;
    int primieraValore = 0;
    Carta primieraCarte[4];
    int cartePunto = 0;
    int denariPunto = 0;
    int primieraPunto = 0;

    int calcolaTotale() const
    {
        return scope + cartePunto + denariPunto + settebello + piccola + grande + primieraPunto;
    }
};

struct PlayerState
{
    int id;
    QString name;
    Mood mood = Mood::Normal;
    SeatType type = SeatType::Empty;

    QVector<Carta> hand;
    QVector<Carta> prese;
    QVector<Carta> scope;
    int totaleScope;
    bool carteScoperte = false;
    QVector<Score> punteggi = {};
    std::vector<int> puntiMano;
    int puntiPartita = 0;
};

struct GameConfig
{
    GameMode mode = GameMode::Offline;
    int humanSeatIndex = 0;
    int botCount = 3;
    bool giocoACoppie = true;
    QVector<ProfileData> players;
};

struct GameState
{
    QVector<Carta> deck;
    QVector<Carta> tableCards;
    QVector<Carta> playedCards;
    int currentTurnIndex = 0;
    int dealerIndex = 0;
    int deckIndex = 0;
    MatchPhase phase = MatchPhase::Configuration;
    QVector<PlayerState> seats;
    int hand = 0;
};

static QStringList botNames = {"Steva", "Giggia", "Colomba", "Venanzio", "Metirde", "Pippetto", "Cesarin"};

#endif
