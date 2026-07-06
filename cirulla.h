#ifndef CIRULLA_H
#define CIRULLA_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QTextEdit>
#include <QGroupBox>
#include <vector>
#include <QKeyEvent>
#include <QLabel>

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

// --- Core Structural Objects ---
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

struct PlayerState
{
    int id;
    QString name;
    SeatType type = SeatType::Empty;

    QVector<Carta> hand;
    QVector<Carta> prese;
    QVector<Carta> scope;
};

struct GameConfig
{
    GameMode mode = GameMode::Offline;
    int humanSeatIndex = 0;
    int botCount = 3;
    QVector<QString> playerNames;
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
};

class Cirulla : public QWidget
{
    Q_OBJECT

public:
    explicit Cirulla(QWidget *parent = nullptr);
    ~Cirulla();

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void startGame();

private:
    // Engine state wrappers
    GameConfig config;
    GameState state;

    // Layouts
    QVBoxLayout *mainLayout;
    QHBoxLayout *topZone;
    QHBoxLayout *middleZone;
    QGridLayout *tableLayout; // Used by showTable()
    QHBoxLayout *handLayout;

    // --- Containers/Widgets ---
    QTextEdit *outputArea;
    QGroupBox *tableContainer;
    QGroupBox *handContainer;

    QWidget *p2StatsContainer;

    // --- main player taken and scope labels

    QLabel *mazzoPreseIcon;
    QLabel *mazzoPreseText;

    QLabel *mazzoScopeIcon;
    QLabel *mazzoScopeText;

    // Assuming player containers are widgets (e.g., QFrame or QGroupBox)
    QWidget *player0Container;
    QWidget *p0StatsContainer;
    QLabel *mazzoPreseP0Label;
    QLabel *mazzoScopeP0Label;

    QWidget *player1Container;
    QWidget *player3Container;
    QLabel *mazzoPreseP1Label; // Giocatore destra
    QLabel *mazzoScopeP1Label;
    QLabel *mazzoPreseP3Label; // Giocatore sinistra
    QLabel *mazzoScopeP3Label;

    int selectedHandCardIndex = -1;     // -1 means no selected card in hand
    QList<int> selectedTableIndices;    // index of selected cards from the table
    bool isSelectingTableCards = false; // Abilita/disabilita selezione sul tavolo
    int revealedCardIndex = -1;
    int lastPlayerToScore = -1;

    QVector<Carta> generateShuffledDeck();

    void setupGame();
    void initialDeal();
    void dealNextRound();
    void showTable();
    void renderHandToLayout(const PlayerState &p, QLayout *targetLayout);
    void showHands();
    int selectDealer();
    int getLocalSeat(int player_id, int human_seat_index, int total_players);
    int cardValue(const Carta &carta);
    void dealersChance();
    bool all_cards_same(const QVector<Carta> &carte);
    int cards_sum(const QVector<Carta> &carte);

    void enableHandInteraction(bool enabled);
    void processTurn();
    bool eventFilter(QObject *obj, QEvent *event);
    void playCard(int handIndex, QList<int> &tableIndices);

    bool isCurrentPlayerBot() const;
    void botPlay();
    void executeBotMove(const Mossa &m);

    void resetSelection();
    void validateAndPlay();
    void makeMove(int handIndex, QList<int> &tableIndices);
    int numeroAssiInTavola() const;
    void updatePlayerUI(int playerIndex);
    void aggiornaMazzoPrese(PlayerState &p);
    void aggiornaMazzoScope(PlayerState &p, Carta &c);
    QVector<Mossa> possibiliPrese(int handIndex);
    QVector<Mossa> trovaTutteLePrese(int handIndex);

    int calcolaValoreTattico(const Mossa &m);

    void handleEndOfGame();
};

#endif // CIRULLA_H
