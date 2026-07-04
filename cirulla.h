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

    // --- main player taken and scope labels
    QLabel *mazzoPreseLabel;
    QLabel *mazzoScopeLabel;

    // Assuming player containers are widgets (e.g., QFrame or QGroupBox)
    QWidget *player0Container;
    QWidget *player1Container;
    QWidget *player3Container;

    int selectedHandCardIndex = -1;     // -1 means no selected card in hand
    QList<int> selectedTableIndices;    // index of selected cards from the table
    bool isSelectingTableCards = false; // Abilita/disabilita selezione sul tavolo

    QVector<Carta> generateShuffledDeck();

    void setupPlayerDashboard(QWidget *container, Qt::Orientation orientation);

    void setupGame();
    void initialDeal();
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
    void botPlay();
    void resetSelection();
    void validateAndPlay();
    void makeMove(int handIndex, QList<int> &tableIndices);
    int numeroAssiInTavola() const;

    QVector<Mossa> possibiliPrese(int handIndex);
    QVector<Mossa> trovaPresePerSomma(int handIndex);
    QVector<Mossa> trovaPresePer15(int handIndex);
};

#endif // CIRULLA_H
