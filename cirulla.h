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
    int totaleScope;
    bool carteScoperte = false;
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
    QLabel *mazzoPreseIconArr[4];
    QLabel *avatarArr[4];

    QLabel *mazzoScopeIconArr[4];
    QLabel *mazzoTextArr[4];

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

    QLabel *PGAvatar;
    QLabel *mazzoPreseIcon;
    QLabel *mazzoScopeIcon;
    QLabel *mazzoText;

    // Assuming player containers are widgets (e.g., QFrame or QGroupBox)
    QWidget *player0Container;
    QWidget *p0StatsContainer;
    QLabel *P0Avatar;

    QLabel *mazzoPreseP0Icon;
    QLabel *mazzoScopeP0Icon;
    QLabel *mazzoP0Text;

    QWidget *player1Container;
    QWidget *player3Container;

    QLabel *P1Avatar;
    QLabel *mazzoPreseP1Icon; // Giocatore a destra
    QLabel *mazzoScopeP1Icon;

    QLabel *mazzoP1Text;

    QLabel *P3Avatar;
    QLabel *mazzoPreseP3Icon; // Giocatore sinistra
    QLabel *mazzoScopeP3Icon;
    QLabel *mazzoP3Text;

    int selectedHandCardIndex = -1;     // -1 means no selected card in hand
    QList<int> selectedTableIndices;    // index of selected cards from the table
    bool isSelectingTableCards = false; // Abilita/disabilita selezione sul tavolo
    int revealedCardIndex = -1;
    int lastPlayerToScore = -1;

    // TEST SWITCH
    bool isTestMode = false;

    QVector<Carta> generateShuffledDeck();
    QVector<Carta> generateTestDeck();

    Carta generaCarta(int i);

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

    bool checkBuonaDaDieci(int playerIndex);
    void applyBuonaDaDieci(int playerIndex);

    bool checkBuonaDaTre(int playerIndex);
    void applyBuonaDaTre(int playerIndex);

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
    bool contieneMatta(const QVector<Carta> &tableCards);
    void aggiornaStats(int playerIndex);
    void applyCircularMask(QLabel *label);

    int calcolaValoreTattico(const Mossa &m);

    void handleEndOfGame();
};

#endif // CIRULLA_H
