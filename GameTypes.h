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
};

struct Carta
{
    int id; // Unique ID (0 to 39)
    Seme seme;
    int faceValue; // 1 to 10

    bool isMatta() const
    {
        return (faceValue == 7 && seme == Seme::Cuori); // 7 of Cups custom rule
    }
};

struct Mossa
{
    int handIndex;
    QList<int> tableIndices;
    int ranking;
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
    int currentTurnIndex = 0;
    int dealerIndex = 0;
    int deckIndex = 0;
    MatchPhase phase = MatchPhase::Configuration;
    QVector<PlayerState> seats;
    int hand = 0;
};

#endif
