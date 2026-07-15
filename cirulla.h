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
#include <QStackedWidget>
#include <QPushButton>

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

    // Statistiche persistenti
    int partiteGiocate = 0;
    int partiteVinte = 0;

    // Helper per il calcolo percentuale
    double getWinRate() const
    {
        if (partiteGiocate == 0)
            return 0.0;
        return (static_cast<double>(partiteVinte) / partiteGiocate) * 100.0;
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
    int hand = 0;
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

    QStackedWidget *stackedWidget;
    QWidget *gameArea;
    QWidget *scoreArea;

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

    QFrame *infoOverlay;
    QHBoxLayout *overlayLayout;

    QLabel *infoLabel;
    QPushButton *button;

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

    // SCORE AREA

    QGridLayout scoreGrid;

    QLabel *scoreAvatar[4];
    QLabel *labelPunti[4];

    QWidget *playerArea[4];

    QWidget *manoArea;
    QLabel *manoLabel;
    QHBoxLayout *manoLayout;

    QHBoxLayout *playerScoreLayout[4];

    QLabel *nameLabel[4];
    QLabel *lblScope[4];
    QLabel *lblCarte[4];
    QLabel *lblDenari[4];
    QLabel *lblSettebello[4];
    QLabel *lblPiccola[4];
    QLabel *lblGrande[4];
    QWidget *primieraContainer[4];
    QHBoxLayout *primieraLayout[4];
    QLabel *primieraThumbnails[4][4];
    QLabel *primieraText[4];
    QLabel *lblTotale[4];

    // Variables

    int selectedHandCardIndex = -1;     // -1 means no selected card in hand
    QList<int> selectedTableIndices;    // index of selected cards from the table
    bool isSelectingTableCards = false; // Abilita/disabilita selezione sul tavolo
    int revealedCardIndex = -1;
    int lastPlayerToScore = -1;
    const int ID_CARTA_NULLA = -999;
    QString colBlu = "#00aaff";
    QString colArancio = "#ffaa00";
    bool isWaitingForBuona = false;
    FaseBottone statoAttualeBottone = FaseAvvio;

    // *******TEST SWITCH********

    bool isTestMode = false;
    bool botGame = false;
    int waitTime = botGame == true ? 0 : 1000;

    GamePhase currentGamePhase;

    // ***FUNCTIONS***

    void mainScreen();

    QVector<Carta> generateShuffledDeck();
    QVector<Carta> generateTestDeck();

    Carta generaCarta(int i);

    void setupGame();
    void initialDeal();
    void executeDeal();
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
    void applyRoundedCorners(QLabel *label);

    int calcolaValoreTattico(const Mossa &m);

    void handleEndOfGame();
    Score calcolaPunteggio(const QVector<Carta> &totaleCarte, int totaleScope);
    int valorePrimieraCarta(const Carta &carta);
    Carta trovaCartaMigliorePerSeme(const QVector<Carta> &carteSeme);
    void calcolaPunti(int tipoStat);
    QString cartaSemeToString(Seme seme);

    void continueGame();

    void updateOverlay(const QString &text, const QString &buttonText, bool buttonEnabled);
    void onGlobalOverlayClicked();

    void resetValoreMatta();
};

#endif // CIRULLA_H
