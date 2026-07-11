#include "cirulla.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QFont>
#include <QMessageBox>
#include <iostream>
#include <QTextEdit>
#include <array>
#include <numeric>
#include <random>
#include <algorithm>
#include <QLabel>
#include <QGroupBox>
#include <vector>
#include <QMessageBox>
#include <QCoreApplication>
#include <QTimer>
#include <QVector>
#include <QDirIterator>
#include <QKeyEvent>
#include <QPainter>
#include <QPainterPath>

Cirulla::Cirulla(QWidget *parent) : QWidget(parent)
{

    this->setFixedSize(1400, 1000);
    stackedWidget = new QStackedWidget(this);

    // Nel costruttore della tua classe principale
    this->setObjectName("MainWindow"); // Importante per referenziarlo

    this->setStyleSheet(
        "QWidget#MainWindow { "
        "   background-color: #000c18; " // Blu notte
        "} "
        "* { " // Questo asterisco applica a OGNI widget esistente nella finestra
        "   background-color: transparent; "
        "   color: #f2f3f6; "                      // Testo grigio-azzurro
        "   font-family: 'Segoe UI', sans-serif; " // Font pulito
        "   font-size: 20; "                       // Font pulito
        "} "
        "QLabel#PrimieraText { "
        "   border: none; "
        "   background-color: transparent; "
        "} "
        "QLabel { "
        "   font-size: 16px; " // Aumenta da 14 o 15 a 16 o 18
        "   font-weight: 500; "
        "} "
        "QGroupBox { "
        "   border: 2px solid #3db0d3; "
        "   border-radius: 10px; "
        "   margin-top: 10px; "
        "} "
        "QGroupBox::title { "
        "   color: #ffffff; "
        "   font-weight: bold; "
        "}");

    // playerstyle
    QString playerStyle =
        "QWidget { "

        "   border: 1px solid #34495e; " // Bordo discreto, non elettrico
        "   border-radius: 12px; "       // Arrotondamento più morbido
        "}";

    // CONTAINERS

    // Top Player (centered)
    player0Container = new QWidget(this);
    p0StatsContainer = new QWidget(this);

    QHBoxLayout *player0Layout = new QHBoxLayout(player0Container);
    player0Layout->setAlignment(Qt::AlignCenter);
    player0Container->setMinimumSize(100, 150);

    // Side Players

    // Per il Giocatore 1
    player1Container = new QWidget(this);
    QVBoxLayout *layout1 = new QVBoxLayout(player1Container); // Questo è NECESSARIO per evitare crash
    layout1->setContentsMargins(5, 5, 5, 5);

    player1Container->setMinimumSize(100, 400);
    player1Container->setStyleSheet(playerStyle);

    // Per il Giocatore 3
    player3Container = new QWidget(this);
    player3Container->setStyleSheet(playerStyle);
    QVBoxLayout *layout3 = new QVBoxLayout(player3Container); // Questo è NECESSARIO per evitare crash
    layout3->setContentsMargins(5, 5, 5, 5);
    player3Container->setMinimumSize(100, 400);

    // Apply size policies for all
    auto setContainerStyle = [](QWidget *container)
    {
        container->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    };
    setContainerStyle(player0Container);
    setContainerStyle(player1Container);
    setContainerStyle(player3Container);

    // 2. Main Layout (Vertical)
    mainLayout = new QVBoxLayout(this);

    // 3. Output Area (Separate, full-width row)
    outputArea = new QTextEdit(this);
    outputArea->setReadOnly(true);
    outputArea->setMaximumHeight(100);
    mainLayout->addWidget(outputArea);

    gameArea = new QWidget(this);

    QVBoxLayout *gameLayout = new QVBoxLayout(gameArea);
    gameLayout->setContentsMargins(0, 0, 0, 0); // Nessun margine extra

    // 4. Top Row (Player 0 centered)
    // 1. Il contenitore delle statistiche
    p0StatsContainer = new QWidget(this);

    QVBoxLayout *statsLayoutP0 = new QVBoxLayout(p0StatsContainer); // Assegnato qui!

    mazzoPreseP0Icon = new QLabel(this);
    mazzoScopeP0Icon = new QLabel(this);
    mazzoPreseP0Icon->setFixedHeight(70);
    mazzoScopeP0Icon->setFixedHeight(70);
    mazzoP0Text = new QLabel("Prese: 0\nScope: 0", this);
    P0Avatar = new QLabel(this);
    P0Avatar->setFixedSize(100, 130);

    // 2. Aggiungi le label al layout del contenitore
    statsLayoutP0->addWidget(mazzoScopeP0Icon);
    statsLayoutP0->addWidget(mazzoPreseP0Icon);
    statsLayoutP0->addWidget(mazzoP0Text);

    // 3. Ora p0StatsContainer è "pieno" di statistiche.
    // Possiamo metterlo nel Grid insieme al contenitore delle carte (player0Container)
    QGridLayout *middleTopGrid = new QGridLayout();
    QWidget *p0Wrapper = new QWidget(this);
    p0Wrapper->setStyleSheet(playerStyle);
    QHBoxLayout *wrapper0Layout = new QHBoxLayout(p0Wrapper);
    wrapper0Layout->setContentsMargins(5, 5, 5, 5);
    wrapper0Layout->setSpacing(10);
    p0StatsContainer->setFixedWidth(120);
    p0StatsContainer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    p0Wrapper->setFixedWidth(800);
    // player0Container->setFixedHeight(200);

    wrapper0Layout->addWidget(P0Avatar);
    wrapper0Layout->addWidget(p0StatsContainer);
    wrapper0Layout->addWidget(player0Container);
    middleTopGrid->addWidget(p0Wrapper, 0, 0, Qt::AlignCenter);

    gameLayout->addLayout(middleTopGrid, 1);

    // 5. Middle Zone (Using GridLayout for better table control)
    QGridLayout *middleGrid = new QGridLayout();

    mazzoPreseP3Icon = new QLabel(this);
    mazzoScopeP3Icon = new QLabel(this);
    mazzoP3Text = new QLabel(this);
    P3Avatar = new QLabel(this);
    P3Avatar->setFixedSize(100, 130);

    mazzoPreseP1Icon = new QLabel(this);
    mazzoScopeP1Icon = new QLabel(this);
    mazzoP1Text = new QLabel(this);
    P1Avatar = new QLabel(this);
    P1Avatar->setFixedSize(100, 130);

    mazzoScopeP1Icon->setFixedHeight(100);
    mazzoScopeP1Icon->setFixedHeight(100);

    layout3->addWidget(mazzoPreseP3Icon);
    layout3->addWidget(mazzoScopeP3Icon);
    layout3->addWidget(mazzoP3Text);
    layout3->addWidget(P3Avatar);

    layout1->addWidget(mazzoPreseP1Icon);
    layout1->addWidget(mazzoScopeP1Icon);
    layout1->addWidget(mazzoP1Text);
    layout1->addWidget(P1Avatar);

    middleGrid->setColumnStretch(0, 1);
    middleGrid->setColumnStretch(1, 1);
    middleGrid->setColumnStretch(2, 8); // Il tavolo domina
    middleGrid->setColumnStretch(3, 1);
    middleGrid->setColumnStretch(4, 1);

    middleGrid->addWidget(mazzoScopeP1Icon, 1, 0);
    middleGrid->addWidget(P1Avatar, 0, 0);
    middleGrid->addWidget(mazzoPreseP1Icon, 2, 0);
    middleGrid->addWidget(mazzoP1Text, 3, 0);

    middleGrid->addWidget(P3Avatar, 0, 4);
    middleGrid->addWidget(mazzoScopeP3Icon, 1, 4);
    middleGrid->addWidget(mazzoPreseP3Icon, 2, 4);
    middleGrid->addWidget(mazzoP3Text, 3, 4);

    player1Container->setFixedWidth(100); // Scegli una misura che ti piace
    mazzoScopeP1Icon->setFixedWidth(100);
    mazzoP1Text->setFixedWidth(100);
    player3Container->setFixedWidth(100);
    mazzoScopeP3Icon->setFixedWidth(100);
    mazzoP3Text->setFixedWidth(100);

    // Forza la policy a Fixed per non permettere espansioni
    mazzoScopeP1Icon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mazzoScopeP3Icon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    player1Container->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    player3Container->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Add side players to the sides
    middleGrid->addWidget(player3Container, 0, 1);
    middleGrid->addWidget(player1Container, 0, 3);

    // Table
    QGroupBox *tableContainer = new QGroupBox();

    tableContainer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    // Forza il widget a ignorare le dimensioni minime calcolate dai figli
    tableContainer->setFixedHeight(400);
    tableContainer->setFixedWidth(800);
    tableContainer->setStyleSheet(
        "QGroupBox { "
        "   background-color: qradialgradient(cx:0.5, cy:0.5, radius:0.8, fx:0.5, fy:0.5, stop:0 #27ae60, stop:1 #1e8449); "
        "   border: 4px solid #144d26; "
        "   border-radius: 15px; "
        "   font-weight: bold; color: #ffffff; "
        "}");

    tableLayout = new QGridLayout(tableContainer);
    tableLayout->setAlignment(Qt::AlignCenter);
    tableContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Add table to center column and make it stretch
    middleGrid->addWidget(tableContainer, 0, 2, 1, 1);
    middleGrid->setColumnStretch(1, 4); // Table gets 5x the width of side columns

    gameLayout->addLayout(middleGrid, 5);
    middleGrid->setRowStretch(0, 10);

    // 6. Bottom Zone (Your hand)
    p2StatsContainer = new QWidget(this);

    QGroupBox *handContainer = new QGroupBox(this);
    handContainer->setFlat(true);

    handContainer->setFixedWidth(800);

    handContainer->setStyleSheet(
        "QGroupBox { "
        "   border: 1px solid #34495e; " // Bordo discreto, non elettrico
        "   border-radius: 12px; "       // Arrotondamento più morbido
        "}");
    QHBoxLayout *handZoneLayout = new QHBoxLayout(handContainer);
    handContainer->setFixedHeight(230);
    // A. Zona Statistiche
    PGAvatar = new QLabel(this);
    PGAvatar->setFixedSize(100, 130);
    p2StatsContainer->setFixedWidth(100);
    p2StatsContainer->setStyleSheet(playerStyle);
    QVBoxLayout *statsLayout = new QVBoxLayout(p2StatsContainer);

    mazzoPreseIcon = new QLabel();
    mazzoText = new QLabel("Scope: 0\nPrese: 0", this);
    mazzoScopeIcon = new QLabel();

    statsLayout->addWidget(mazzoScopeIcon);
    statsLayout->addWidget(mazzoPreseIcon);
    statsLayout->addWidget(mazzoText);

    // B. Zona Carte (Il trucco qui)
    QWidget *cardsWrapper = new QWidget(); // Questo serve solo per lo stile
    cardsWrapper->setStyleSheet(playerStyle);

    // cardsWrapper->setFixedHeight(200);
    // Creiamo l'handLayout dentro il wrapper, così le funzioni lo trovano
    handLayout = new QHBoxLayout(cardsWrapper);
    handLayout->setAlignment(Qt::AlignCenter);

    // C. Assemblaggio finale
    handZoneLayout->addWidget(PGAvatar);
    handZoneLayout->addWidget(p2StatsContainer);
    handZoneLayout->addWidget(cardsWrapper); // Aggiungiamo il wrapper, non handLayout!

    gameLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setAlignment(Qt::AlignCenter);
    QHBoxLayout *handWrapper = new QHBoxLayout();
    handWrapper->addStretch();             // Spinge a destra
    handWrapper->addWidget(handContainer); // Il widget sta al centro
    handWrapper->addStretch();
    gameLayout->addLayout(handWrapper, 2);

    stackedWidget->addWidget(gameArea);

    scoreArea = new QWidget();

    // Il padre ha un layout verticale di base

    mainLayout->addWidget(stackedWidget);

    // Collegamento con array di puntatori

    for (int i = 0; i < 4; ++i)
    {
        avatarArr[i] = nullptr; // Fondamentale!
    }

    mazzoPreseIconArr[0] = mazzoPreseP0Icon;
    mazzoScopeIconArr[0] = mazzoScopeP0Icon;
    mazzoTextArr[0] = mazzoP0Text;
    avatarArr[0] = P0Avatar;

    mazzoPreseIconArr[1] = mazzoPreseP1Icon;
    mazzoScopeIconArr[1] = mazzoScopeP1Icon;
    mazzoTextArr[1] = mazzoP1Text;
    avatarArr[1] = P1Avatar;

    mazzoPreseIconArr[2] = mazzoPreseIcon;
    mazzoScopeIconArr[2] = mazzoScopeIcon;
    mazzoTextArr[2] = mazzoText;
    avatarArr[2] = PGAvatar;

    mazzoPreseIconArr[3] = mazzoPreseP3Icon;
    mazzoScopeIconArr[3] = mazzoScopeP3Icon;
    mazzoTextArr[3] = mazzoP3Text;
    avatarArr[3] = P3Avatar;

    for (int i = 0; i < 4; ++i)
    {
        mazzoScopeIconArr[i]->setStyleSheet("QLabel {border: none}");
        mazzoPreseIconArr[i]->setStyleSheet("QLabel {border: none}");
    };

    // Score Area Setup - AREA PUNTEGGIO

    QGridLayout *scoreGrid = new QGridLayout();

    for (int i = 0; i < 4; i++)
    {
        playerArea[i] = new QWidget();
        QString bgColor = (i == 0 || i == 2) ? "#001a2e" : "#2e0e00";
        QString borderColor = (i == 0 || i == 2) ? "#3db0d3" : "#d3603d";
        playerArea[i]->setStyleSheet(QString("QWidget { background-color: %1; border: 1px solid %2; border-radius: 5px}")
                                         .arg(bgColor)
                                         .arg(borderColor));
        playerArea[i]->setMinimumSize(300, 150);
        playerScoreLayout[i] = new QHBoxLayout(playerArea[i]);
        scoreAvatar[i] = new QLabel();
        scoreAvatar[i]->setStyleSheet("QLabel {border: none}");
        scoreAvatar[i]->setScaledContents(true);
        scoreAvatar[i]->setFixedSize(100, 130);

        QWidget *leftContainer = new QWidget();
        QVBoxLayout *leftLayout = new QVBoxLayout(leftContainer);

        QWidget *rightContainer = new QWidget();
        QGridLayout *dataLayout = new QGridLayout(rightContainer);

        nameLabel[i] = new QLabel();
        nameLabel[i]->setStyleSheet("color: white;");

        lblScope[i] = new QLabel("Scope: 0");
        lblCarte[i] = new QLabel("Carte: 0");
        lblDenari[i] = new QLabel("Denari: 0");
        lblSettebello[i] = new QLabel("Settebello: No");
        lblPiccola[i] = new QLabel("Piccola: -");
        lblGrande[i] = new QLabel("Grande: -");
        primieraContainer[i] = new QWidget();
        lblTotale[i] = new QLabel("Totale: 0");

        dataLayout->addWidget(lblScope[i], 0, 0);
        dataLayout->addWidget(lblCarte[i], 1, 0);
        dataLayout->addWidget(lblDenari[i], 1, 1);
        dataLayout->addWidget(lblSettebello[i], 2, 0);

        dataLayout->addWidget(primieraContainer[i], 2, 1);

        dataLayout->addWidget(lblPiccola[i], 3, 0);
        dataLayout->addWidget(lblGrande[i], 3, 1);
        dataLayout->addWidget(lblTotale[i], 4, 0, 1, 2);

        labelPunti[i] = new QLabel("Punti: 0");
        labelPunti[i]->setStyleSheet("color: yellow; font-weight: bold;");

        leftLayout->addWidget(scoreAvatar[i]);
        leftLayout->addWidget(nameLabel[i]);
        leftLayout->addWidget(labelPunti[i]);
        leftLayout->addStretch(); // Spinge il testo in alto

        playerScoreLayout[i]->addWidget(leftContainer);
        playerScoreLayout[i]->addWidget(rightContainer);
        playerScoreLayout[i]->addStretch();
    }

    for (int i = 0; i < 4; i++)
    {
        int riga = (i == 0 || i == 2) ? 0 : 1;
        int colonna = (i == 0 || i == 1) ? 0 : 1;
        scoreGrid->addWidget(playerArea[i], riga, colonna);
    }

    scoreArea->setLayout(scoreGrid);
    stackedWidget->addWidget(scoreArea);

    // 7. Initialization
    setupGame();

    QTimer::singleShot(0, this, &Cirulla::startGame);
}

void Cirulla::startGame()
{

    // 1. First shuffle and show the dealer extraction cards
    state.deck = generateShuffledDeck();
    state.dealerIndex = selectDealer();

    // 2. New shuffle to start the real match
    if (isTestMode)
    {
        outputArea->append("TEST TEST TEST TEST TEST");
        state.deck = generateTestDeck();
    }
    else
    {
        state.deck = generateShuffledDeck();
    }
    state.currentTurnIndex = (state.dealerIndex + 1) % state.seats.size();

    // 3. Deal match cards and draw hands
    initialDeal();
    showHands();
    showTable();
    dealersChance();
}

void Cirulla::setupGame()
{
    config.mode = GameMode::Offline;
    config.humanSeatIndex = 2;
    config.playerNames = {"Baciccia", "Ugo", "Alberto", "Mussadiferro"};
    state.seats.clear();
    for (int i = 0; i < 4; ++i)
    {
        PlayerState p;
        p.id = i;
        p.totaleScope = 0;
        QString avatarPath = "avatars/" + config.playerNames[i] + ".png";
        QPixmap cardImage(avatarPath);

        if (avatarArr[i])
        {

            avatarArr[i]->setPixmap(cardImage.scaled(120, 140, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            applyRoundedCorners(avatarArr[i]);
        }

        if (i == config.humanSeatIndex && botGame == false)
        {
            p.name = config.playerNames[i] + "(tu)";
            p.type = SeatType::Human;
            p.carteScoperte = false;
        }
        else
        {
            p.name = config.playerNames[i];
            p.type = SeatType::Bot;
        }
        state.seats.append(p);
        aggiornaStats(p.id);
    }
}

void Cirulla::initialDeal()
{

    // 1. Deal 3 cards to each player's structural hand vector
    for (auto &player : state.seats)
    {
        player.hand.clear();
        player.scope.clear();
        player.prese.clear();
        player.totaleScope = 0;
        // Ensure hand is clean at the start of a round

        for (int i = 0; i < 3; ++i)
        {
            if (state.deckIndex < state.deck.size())
            {
                player.hand.append(state.deck[state.deckIndex++]);
            }
        }
    }

    // 3. Table Cards
    state.tableCards.clear();
    for (int i = 0; i < 4; ++i)
    {
        if (state.deckIndex < state.deck.size())
        {
            state.tableCards.append(state.deck[state.deckIndex++]);
        }
    }
}

void Cirulla::showTable()
{
    while (QLayoutItem *item = tableLayout->takeAt(0))
    {
        if (QWidget *oldWidget = item->widget())
        {
            oldWidget->deleteLater(); // Safely schedules the old QLabel for deletion
        }
        delete item; // Frees the layout position slot
    }
    int cardsPerRow = 4;

    for (int i = 0; i < state.tableCards.size(); ++i)
    {
        QLabel *cardLabel = new QLabel(nullptr);
        cardLabel->setProperty("type", "table");
        cardLabel->setProperty("tableCardIndex", i);
        cardLabel->installEventFilter(this);
        cardLabel->setProperty("isClickable", true);

        // evidencing logic
        if (selectedTableIndices.contains(i))
        {
            cardLabel->setStyleSheet("border: 3px solid red");
        }
        else
        {
            // normal style
            cardLabel->setStyleSheet("");
        }

        QString qPathStr = QString("cards/%1.png").arg(state.tableCards[i].id + 1);

        // Load and scale the image
        QPixmap cardImage(qPathStr);
        cardLabel->setPixmap(cardImage.scaled(100, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        // Calculate grid coordinates
        int row = i / cardsPerRow;
        int column = i % cardsPerRow;

        // 3. Add to grid
        tableLayout->addWidget(cardLabel, row, column);
    }
}

int Cirulla::cards_sum(const QVector<Carta> &carte)
{
    int total = 0;
    for (const auto &card : carte)
    {
        total += cardValue(card);
    }
    return total;
}
void Cirulla::dealersChance()
{
    bool mattaPresente = contieneMatta(state.tableCards);
    int total = cards_sum(state.tableCards);
    if (mattaPresente)
    {
        if (total > 11 && total < 22)
            total = 15;

        else if (total > 26 && total < 37)
            total = 30;
    }

    QString totalString = QString("Totale sul tavolo: %1").arg(total);
    outputArea->append(totalString);

    if (total == 15 || total == 30)
    {
        // Mostriamo il messaggio immediatamente
        QString message = (total == 15) ? "Ci sono 15 punti: scopa per il mazziere!"
                                        : "Ci sono 30 punti: due scope per il mazziere!";
        outputArea->append(message);

        // Usiamo un timer per eseguire la presa dopo 2 secondi (2000 millisecondi)
        QTimer::singleShot(2000, this, [this, total]()
                           {
            if (total == 15) {
                state.seats[state.dealerIndex].scope.append(state.tableCards[0]);
                for (int i = 1; i < state.tableCards.size(); ++i) {
                    state.seats[state.dealerIndex].prese.append(state.tableCards[i]);
                }
                    state.seats[state.dealerIndex].totaleScope = 1;
                  aggiornaStats(state.dealerIndex);
                
            } else {
                state.seats[state.dealerIndex].scope.append(state.tableCards[0]);
                state.seats[state.dealerIndex].scope.append(state.tableCards[1]);
                for (int i = 2; i < state.tableCards.size(); ++i) {
                    state.seats[state.dealerIndex].prese.append(state.tableCards[i]);
                }
                    
                    state.seats[state.dealerIndex].totaleScope = 2;
                     aggiornaStats(state.dealerIndex);
            
            }
            
            state.tableCards.clear();
            showTable(); // Aggiorna il tavolo solo dopo la pausa
            outputArea->append("Il tavolo è stato ripulito."); });
    }

    processTurn();
}

void Cirulla::renderHandToLayout(const PlayerState &p, QLayout *targetLayout)
{
    // 1. Clear the specific layout passed in
    while (QLayoutItem *item = targetLayout->takeAt(0))
    {
        if (QWidget *oldWidget = item->widget())
        {
            oldWidget->deleteLater();
        }
        delete item;
    }

    // 2. Add cards to the target layout
    for (int i = 0; i < p.hand.size(); ++i)
    {
        const Carta &card = p.hand[i];
        QLabel *cardLabel = new QLabel(); // No parent needed here, layout takes ownership
        cardLabel->setStyleSheet("border: none");
        cardLabel->installEventFilter(this);
        bool isMe = (p.id == config.humanSeatIndex);
        bool isRevealed = (p.id == state.currentTurnIndex && i == revealedCardIndex);

        cardLabel->setProperty("cardIndex", i);
        cardLabel->setProperty("type", "hand");
        cardLabel->setProperty("isClickable", isMe);

        QString path;
        if (isMe || isRevealed)
        {
            // Show the actual card
            path = QString("cards/%1.png").arg(card.id + 1);
            if (i == selectedHandCardIndex)
            {
                cardLabel->setStyleSheet("border: 3px solid gold");
            }
            else
            {
                cardLabel->setStyleSheet("border: none");
            }
        }
        else
        {
            // Show the flipside image unless dictated by "buona" rule
            if (!p.carteScoperte)
                path = "cards/back-teal.png";
            else
                path = QString("cards/%1.png").arg(card.id + 1);
        }

        QPixmap cardImage(path);
        cardLabel->setPixmap(cardImage.scaled(80, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        targetLayout->addWidget(cardLabel);
    }
}

QVector<Carta> Cirulla::generateShuffledDeck()
{
    QVector<Carta> deck;
    deck.reserve(40);

    for (int i = 0; i < 40; i++)
    {
        Carta card;
        card.id = i;
        card.faceValue = (i % 10) + 1;
        card.seme = static_cast<Seme>(i / 10); // 0 = Picche, etc...
        deck.append(card);
    }

    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(deck.begin(), deck.end(), g);

    state.deckIndex = 0;
    return deck;
}

QVector<Carta> Cirulla::generateTestDeck()
{
    QVector<Carta> deck;
    deck.reserve(40);
    // Vettore 0-39 (Indici 0-39)
    // Posizioni 16, 17, 18 (Indici 15, 16, 17): 1, 2, 3
    QVector<int> fisse = {
        // 0-14
        35, 12, 7, 22, 39, 4, 18, 29, 0, 33, 11, 25, 8, 36, 14,
        // 15, 16, 17 (I tuoi numeri: 1, 2, 3)
        1, 2, 3,
        // 18-39
        30, 19, 5, 26, 9, 37, 15, 21, 6, 32, 10, 27, 13, 38, 20, 28, 24, 34, 17, 31, 23, 16};

    // Lista delle carte che vogliamo "blindare" in cima

    // 1. Aggiungi le carte fisse
    for (int id : fisse)
    {
        deck.append(generaCarta(id));
    }

    // 2. Aggiungi le restanti carte escludendo le fisse
    for (int i = 0; i < 40; ++i)
    {
        // Controlla se 'i' è presente nel vettore 'fisse'
        if (!fisse.contains(i))
        {
            deck.append(generaCarta(i));
        }
    }

    // 3. Mescola solo dal punto in cui finiscono le fisse
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(deck.begin() + fisse.size(), deck.end(), g);

    state.deckIndex = 0;
    return deck;
}

Carta Cirulla::generaCarta(int i)
{
    Carta card;
    card.id = i;
    card.faceValue = (i % 10) + 1;
    card.seme = static_cast<Seme>(i / 10);
    return card;
}

int Cirulla::selectDealer()
{

    QVector<int> active_players;
    for (int k = 0; k < state.seats.size(); ++k)
    {
        active_players.append(k);
    }

    int round = 1;

    outputArea->append("--- Estrazione del Mazziere ---");

    while (active_players.size() > 1)
    {
        QVector<int> tied_players;
        int max_value = -1;

        QString roundMsg = QString("Turno di spareggio %1:").arg(round);
        outputArea->append(roundMsg);

        for (int player : active_players)
        {
            Carta current_card = state.deck[state.deckIndex];
            int value = cardValue(current_card);

            state.tableCards.append(current_card);

            QString drawMsg = QString("%1 ha pescato il valore: %2")
                                  .arg(state.seats[player].name)
                                  .arg(value);
            outputArea->append(drawMsg);

            if (value > max_value)
            {
                max_value = value;
                tied_players.clear();
                tied_players.append(player);
            }
            else if (value == max_value)
            {
                tied_players.append(player);
            }

            state.deckIndex++;
        }
        if (tied_players.size() > 1)
        {
            QString tieMsg = "Pareggio tra i giocatori: ";
            for (int p : tied_players)
                tieMsg += QString("%1 ").arg(p);
            outputArea->append(tieMsg + "\n");
        }

        active_players = tied_players;
        round++;
    }

    showTable();

    QString winnerMsg = QString("%1 vince il mazzo ed è il Mazziere!").arg(state.seats[active_players[0]].name);
    outputArea->append(winnerMsg);
    qApp->processEvents();

    state.tableCards.clear();
    QMessageBox::information(
        this,
        "Estrazione Mazziere",
        winnerMsg + "\n\nClicca OK per iniziare la smazzata.");

    return active_players[0];
}

int Cirulla::cardValue(const Carta &carta)
{
    return carta.faceValue;
}

int Cirulla::getLocalSeat(int player_id, int human_seat_index, int total_players)
{
    // Now you have all the variables you need passed in directly
    return (player_id - human_seat_index + total_players) % total_players;
}

void Cirulla::showHands()
{
    for (const auto &player : state.seats)
    {
        int seat = getLocalSeat(player.id, config.humanSeatIndex, state.seats.size());

        switch (seat)
        {
        case 0: // Bottom (You)
            renderHandToLayout(state.seats[config.humanSeatIndex], handLayout);
            break;
        case 1: // Right (Player 3)
            renderHandToLayout(player, player1Container->layout());
            break;
        case 2: // Top (Player 0)
            renderHandToLayout(player, player0Container->layout());
            break;
        case 3: // Left (Player 1)
            renderHandToLayout(player, player3Container->layout());
            break;
        }
    }
}

bool Cirulla::all_cards_same(const QVector<Carta> &carte)
{
    if (carte.size() < 2)
        return true;

    int firstValue = carte[0].faceValue;
    for (int i = 1; i < carte.size(); ++i)
    {
        if (carte[i].faceValue != firstValue)
            return false;
    }
    return true;
}

void Cirulla::processTurn()
{
    // 1. Identifichiamo il giocatore corrente
    int currentPlayerIndex = state.currentTurnIndex;
    if (checkBuonaDaDieci(currentPlayerIndex))
    {
        applyBuonaDaDieci(currentPlayerIndex);
    }
    else if (checkBuonaDaTre(currentPlayerIndex))
    {
        applyBuonaDaTre(currentPlayerIndex);
    }

    // 2. Se è il giocatore umano (supponiamo sia sempre al seat 2)
    if (currentPlayerIndex == config.humanSeatIndex)
    {
        // Abilitiamo il clic sulle carte nella mano del giocatore
        enableHandInteraction(true);
        outputArea->append("Tocca a te! Clicca su una carta.");

        // analisi trasparente delle mosse
        const auto &miaMano = state.seats[config.humanSeatIndex].hand;
        bool totaleMosseValide = false;

        for (int i = 0; i < miaMano.size(); ++i)
        {
            QVector<Mossa> mosseCarta = possibiliPrese(i);

            if (!mosseCarta.isEmpty())
            {
                totaleMosseValide = true;
                // outputArea->append(QString("Carta %1 (valore %2) può prendere:")
                //                        .arg(i)
                //                        .arg(miaMano[i].faceValue));

                // for (const auto &m : mosseCarta)
                // {
                //     QString info;
                //     for (int idx : m.tableIndices)
                //     {
                //         info += QString::number(state.tableCards[idx].faceValue) + " ";
                //     }
                //     outputArea->append("  > con: " + info);
                // }
            }
        }

        if (!totaleMosseValide)
        {
            outputArea->append("Nessuna presa possibile. Puoi calare una carta qualsiasi.");
        }
    }
    else
    {
        // Se è un bot, disabilitiamo l'input e avviamo la sua logica
        enableHandInteraction(false);

        QTimer::singleShot(waitTime, this, &Cirulla::botPlay); // Delay per realismo
    }
}

void Cirulla::enableHandInteraction(bool enabled)
{
    // Supponendo che 'handLayout' contenga i widget delle carte
    for (int i = 0; i < handLayout->count(); ++i)
    {
        QWidget *widget = handLayout->itemAt(i)->widget();
        if (widget)
        {

            widget->setEnabled(enabled);

            // Opzionale: cambia l'aspetto per far capire che è il turno del giocatore
            widget->setCursor(enabled ? Qt::PointingHandCursor : Qt::ArrowCursor);
        }
    }
}

bool Cirulla::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        // Verifica che il turno sia dell'umano
        if (state.currentTurnIndex != config.humanSeatIndex)
            return false;

        QWidget *w = qobject_cast<QWidget *>(obj);
        if (w && w->property("isClickable").toBool())
        {
            QString type = w->property("type").toString();

            // CASO 1: Carta in mano
            if (type == "hand")
            {
                int newIndex = w->property("cardIndex").toInt();

                // Se clicco una carta diversa, aggiorno la selezione
                if (selectedHandCardIndex != newIndex)
                {
                    selectedHandCardIndex = newIndex; // <--- Mancava questa riga!
                    selectedTableIndices.clear();
                    isSelectingTableCards = true;

                    renderHandToLayout(state.seats[state.currentTurnIndex], handLayout);
                    showTable();
                }
                return true;
            }

            // CASO 2: Carta sul tavolo (ORA È FUORI DALL'IF DELLA MANO)
            if (type == "table" && isSelectingTableCards)
            {
                int tableIndex = w->property("tableCardIndex").toInt();

                // Toggle selezione
                if (selectedTableIndices.contains(tableIndex))
                    selectedTableIndices.removeOne(tableIndex);
                else
                    selectedTableIndices.append(tableIndex);

                showTable();
                return true;
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

void Cirulla::playCard(int handIndex, QList<int> &tableIndices)
{
    int indexGiocatoreCheMuove = state.currentTurnIndex;

    // 1. Esegui la mossa (questo usa state.currentTurnIndex, quindi è già corretto)
    makeMove(handIndex, tableIndices);

    // 2. Disabilita interazione durante l'animazione
    enableHandInteraction(false);
    selectedHandCardIndex = -1;
    selectedTableIndices.clear();
    updatePlayerUI(indexGiocatoreCheMuove);

    bool tuttiHannoFinito = true;
    for (const auto &player : state.seats)
    {
        if (!player.hand.isEmpty())
        {
            tuttiHannoFinito = false;
            break;
        }
    }

    if (tuttiHannoFinito)
    {
        if (state.deckIndex < state.deck.size())
        {
            for (auto &player : state.seats)
            {
                player.carteScoperte = false;
            }
            dealNextRound();
            state.currentTurnIndex = (state.dealerIndex + 1) % state.seats.size();
            for (int i = 0; i < state.seats.size(); ++i)
            {
                updatePlayerUI(i);
            }
        }
        else
        {
            handleEndOfGame();
        }
    }
    else
    {
        state.currentTurnIndex = (state.currentTurnIndex + 1) % state.seats.size();
        // --- NUOVA LOGICA: Controllo Buone PRIMA della mossa ---
        // Verifichiamo solo se il giocatore ha 3 carte (inizio mano)
        // e se non ha ancora dichiarato nulla in questa smazzata.
        if (state.seats[state.currentTurnIndex].hand.size() == 3 &&
            !state.seats[state.currentTurnIndex].carteScoperte)
        {
            if (checkBuonaDaDieci(state.currentTurnIndex))
            {
                applyBuonaDaDieci(state.currentTurnIndex);
                // Qui richiami la tua notifica soft (non bloccante)
            }
            else if (checkBuonaDaTre(state.currentTurnIndex))
            {
                applyBuonaDaTre(state.currentTurnIndex);
                // Qui richiami la tua notifica soft (non bloccante)
            }
        }
    }

    //  Prossima mossa
    if (isCurrentPlayerBot())
    {
        enableHandInteraction(false);
        QTimer::singleShot(waitTime, this, &Cirulla::botPlay);
    }
    else
    {
        enableHandInteraction(true);
    }
}

void Cirulla::makeMove(int handIndex, QList<int> &tableIndices)
{
    PlayerState &giocatore = state.seats[state.currentTurnIndex];
    Carta giocata = giocatore.hand[handIndex];

    if (tableIndices.isEmpty())
    {
        // CASO SCARTO: La carta finisce sul tavolo
        state.tableCards.append(giocata);
    }
    else
    {
        // CASO PRESA O SCOPA
        lastPlayerToScore = state.currentTurnIndex;
        std::sort(tableIndices.begin(), tableIndices.end(), std::greater<int>());
        bool isScopa = (state.tableCards.size() == tableIndices.size());

        if (isScopa)
        {
            outputArea->append("SCOPA!");
            ++giocatore.totaleScope;
            giocatore.scope.append(giocata); // La carta che ha fatto scopa va nelle scope

            aggiornaMazzoScope(giocatore, giocata);
        }
        else
        {
            if (giocatore.totaleScope > giocatore.scope.size())
            {
                giocatore.scope.append(giocata); // trasferisco la carta giocata nel mazzetto delle scope
                aggiornaMazzoScope(giocatore, giocata);
            }
            else
            {

                giocatore.prese.append(giocata); // La carta che ha preso va nelle prese
                aggiornaMazzoPrese(giocatore);
            }
        }

        // Aggiungi le carte del tavolo alle prese

        for (int idx : tableIndices)
        {
            Carta c = state.tableCards[idx];

            // Logica unificata: se ho scope da girare, vanno in scope, altrimenti in prese
            if (giocatore.totaleScope > giocatore.scope.size())
            {
                giocatore.scope.append(c);

                aggiornaMazzoScope(giocatore, c);
            }
            else
            {
                giocatore.prese.append(c);

                aggiornaMazzoPrese(giocatore);
            }
        } // fine ciclo aggiunta

        // Rimuovi le carte dal tavolo
        for (int i : tableIndices)
        {
            state.tableCards.removeAt(i);
        }
    }

    // Rimuovi la carta dalla mano del giocatore
    giocatore.hand.removeAt(handIndex);
}

void Cirulla::botPlay()
{
    PlayerState &bot = state.seats[state.currentTurnIndex];
    QVector<Mossa> mosseValide;

    // 1. Cerchiamo prese
    for (int i = 0; i < bot.hand.size(); ++i)
    {
        QVector<Mossa> prese = trovaTutteLePrese(i);
        if (prese.isEmpty())
        {
            // Se non ci sono prese, aggiungiamo una mossa di "scarto"
            Mossa scarto;
            scarto.handIndex = i;
            scarto.tableIndices = {};
            mosseValide.append(scarto);
        }
        else
        {
            mosseValide.append(prese);
        }
    }

    // 2. Valutiamo le mosse
    for (auto &m : mosseValide)
    {
        m.ranking = calcolaValoreTattico(m);

        // --- LOGICA ANTI-AUTOLESIONISMO ---
        // Se è una mossa di scarto (tableIndices vuoto) e gioco un Asso...
        Carta c = bot.hand[m.handIndex];
        if (m.tableIndices.isEmpty() && c.faceValue == 1)
        {
            // ...e ho altre carte in mano...
            if (bot.hand.size() > 1)
            {
                // ...abbasso drasticamente il ranking per evitare che venga scelta
                m.ranking = -999;
            }
        }
    }

    // 3. Esecuzione
    if (!mosseValide.isEmpty())
    {
        auto bestMossa = std::max_element(mosseValide.begin(), mosseValide.end(),
                                          [](const Mossa &a, const Mossa &b)
                                          { return a.ranking < b.ranking; });
        executeBotMove(*bestMossa);
    }
}

void Cirulla::resetSelection()
{
    selectedHandCardIndex = -1;
    selectedTableIndices.clear();
    isSelectingTableCards = false;
    showTable();
}

void Cirulla::validateAndPlay()
{
    // A. Controlli preliminari
    if (selectedHandCardIndex == -1)
    {
        outputArea->append("Seleziona una carta dalla mano!");
        return;
    }

    // B. Calcoliamo le mosse possibili SOLO per la carta selezionata
    QVector<Mossa> mossePossibili = possibiliPrese(selectedHandCardIndex);
    bool sceltaDelleCarteTavolo = !selectedTableIndices.isEmpty();

    // C. Logica di Validazione
    bool mossaValida = false;

    if (sceltaDelleCarteTavolo)
    {
        // CASO 1: L'utente vuole fare una presa
        QSet<int> selezione(selectedTableIndices.begin(), selectedTableIndices.end());
        for (const auto &mossa : mossePossibili)
        {
            QSet<int> mossaSet(mossa.tableIndices.begin(), mossa.tableIndices.end());
            if (mossaSet == selezione)
            {
                mossaValida = true;
                break;
            }
        }
        if (!mossaValida)
            outputArea->append("Presa non valida!");
    }
    else
    {
        // CASO 2: L'utente vuole calare (non ha selezionato carte sul tavolo)
        if (mossePossibili.isEmpty())
        {
            mossaValida = true; // Calo permesso
        }
        else
        {
            outputArea->append("Non puoi calare: hai delle prese possibili!");
        }
    }

    // D. Esecuzione
    if (mossaValida)
    {
        playCard(selectedHandCardIndex, selectedTableIndices);
        // Reset UI
        selectedHandCardIndex = -1;
        selectedTableIndices.clear();
        showTable();
    }
    else
    {
        selectedTableIndices.clear();
        showTable();
    }
}

void Cirulla::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        if (selectedHandCardIndex != -1)
        {
            // Caso 1: Hai selezionato una presa
            if (!selectedTableIndices.isEmpty())
            {
                validateAndPlay();
            }
            // Caso 2: Vuoi scartare
            else
            {
                // Qui dobbiamo verificare se lo scarto è LEGALE
                // (ovvero se davvero non puoi prendere nulla)
                if (trovaTutteLePrese(selectedHandCardIndex).isEmpty())
                {
                    // Nessuna presa possibile per questa carta, lo scarto è legale!
                    playCard(selectedHandCardIndex, selectedTableIndices);
                }
                else
                {
                    outputArea->append("Hai delle prese disponibili, non puoi scartare!");
                }
            }
        }
        else
        {
            outputArea->append("Seleziona prima una carta in mano!");
        }
    }
    else
    {
        QWidget::keyPressEvent(event);
    }
}

QVector<Mossa> Cirulla::possibiliPrese(int handIndex)
{
    QVector<Mossa> candidati = {};
    PlayerState &giocatore = state.seats[state.currentTurnIndex];
    if (giocatore.hand[handIndex].faceValue == 1 && numeroAssiInTavola() == 0)
    {
        QList<int> tuttiGliIndici;
        for (int i = 0; i < state.tableCards.size(); ++i)
        {
            tuttiGliIndici.append(i);
        }
        Mossa scopaConAsso = {handIndex, tuttiGliIndici};
        candidati.append(scopaConAsso);
        return candidati;
    }

    int valoreCartaInMano = giocatore.hand[handIndex].faceValue;

    candidati.append(trovaTutteLePrese(handIndex));

    return candidati;
}
QVector<Mossa> Cirulla::trovaTutteLePrese(int handIndex)
{
    PlayerState &giocatore = state.seats[state.currentTurnIndex];
    Carta c = giocatore.hand[handIndex];
    bool isMatta = c.isMatta();
    int valore = c.faceValue;

    QVector<Mossa> prese;
    int n = state.tableCards.size();

    // AGGIUNTA: Se il tavolo è vuoto, nessuna carta può fare presa!
    if (n == 0)
    {
        return QVector<Mossa>();
    }

    if (valore == 1 && numeroAssiInTavola() == 0)
    {

        QList<int> tuttoIlTavolo;
        for (int j = 0; j < n; ++j)
        {
            tuttoIlTavolo.append(j);
        }
        // Aggiungiamo la mossa "Presa Totale"
        prese.append({handIndex, tuttoIlTavolo});
        return prese;
    }

    // Iteriamo su tutte le combinazioni possibili (Power Set)
    for (int i = 1; i < (1 << n); ++i)
    {
        int sommaTavolo = 0;
        QList<int> combo;
        for (int j = 0; j < n; ++j)
        {
            if ((i >> j) & 1)
            {
                sommaTavolo += state.tableCards[j].faceValue;
                combo.append(j);
            }
        }

        // LOGICA UNIFICATA
        if (isMatta)
        {
            // La matta prende se sommaTavolo è un valore che lei può "coprire"
            // Caso 1: Presa normale (sommaTavolo tra 1 e 10)
            // Caso 2: Presa 15 (sommaTavolo tra 5 e 14, perché 15 - [1..10] = [14..5])
            if (sommaTavolo >= 1 && sommaTavolo <= 14)
            {
                prese.append({handIndex, combo});
            }
        }
        else
        {
            // Logica per carte normali
            if (sommaTavolo == valore || (sommaTavolo + valore == 15))
            {
                prese.append({handIndex, combo});
            }
        }
    }
    return prese;
}

int Cirulla::numeroAssiInTavola() const
{
    return (std::count_if(state.tableCards.begin(), state.tableCards.end(),
                          [](const Carta &c)
                          { return c.faceValue == 1; }));
}

int Cirulla::calcolaValoreTattico(const Mossa &m)
{
    int rank = 0;

    // 1. PRIORITÀ MASSIMA: SCOPA
    if (m.tableIndices.size() == state.tableCards.size())
    {
        rank += 1000;
    }

    // 2. PRIORITÀ ALTA: STRATEGIA DENARI (Grande, Piccola, Settebello)
    for (int index : m.tableIndices)
    {
        const Carta &c = state.tableCards[index];
        if (c.seme == Seme::Denari)
        {
            // La Grande (K, Q, J)
            if (c.faceValue >= 11 && c.faceValue <= 13)
                rank += 500;
            // La Piccola (A, 2, 3)
            else if (c.faceValue >= 1 && c.faceValue <= 3)
                rank += 300;
            // Settebello
            else if (c.faceValue == 7)
                rank += 100;
        }

        // 3. PRIORITÀ MEDIA: VALORE PRIMIERA
        // I 7 valgono 21, i 6 valgono 18, gli Assi 16
        if (c.faceValue == 7)
            rank += 40;
        else if (c.faceValue == 6)
            rank += 30;
        else if (c.faceValue == 1)
            rank += 25;
    }

    return rank;
}

bool Cirulla::isCurrentPlayerBot() const
{
    // Controlliamo il tipo del giocatore di turno
    return state.seats[state.currentTurnIndex].type == SeatType::Bot;
}

void Cirulla::executeBotMove(const Mossa &m)
{
    revealedCardIndex = m.handIndex;
    showHands();

    QTimer::singleShot(waitTime, this, [=]()
                       {
        // Ora esegui la mossa vera e propria
        this->playCard(m.handIndex, const_cast<QList<int>&>(m.tableIndices)); 
    revealedCardIndex = -1; });
}

void Cirulla::updatePlayerUI(int playerIndex)
{
    // 1. Aggiorniamo sempre il tavolo (visibile a tutti)
    showTable();

    revealedCardIndex = -1;

    // 2. Recuperiamo il giocatore che ha appena mosso
    const PlayerState &p = state.seats[playerIndex];

    // 3. Identifichiamo quale layout dobbiamo aggiornare
    QLayout *targetLayout = nullptr;

    // Usiamo una logica simile a showHands per capire dove si trova il giocatore
    int seat = getLocalSeat(p.id, config.humanSeatIndex, state.seats.size());
    switch (seat)
    {
    case 0:
        targetLayout = handLayout;
        break;
    case 1:
        targetLayout = player1Container->layout();
        break;
    case 2:
        targetLayout = player0Container->layout();
        break;
    case 3:
        targetLayout = player3Container->layout();
        break;
    }

    if (targetLayout)
    {
        // Se è l'umano, aggiorniamo normalmente
        if (p.type == SeatType::Human)
        {
            renderHandToLayout(p, targetLayout);
        }
        else
        {
            // SE È UN BOT: aggiorniamo il layout MA manteniamo il retro della carta
            // Basta richiamare renderHandToLayout: è già scritta per gestire il caso "isMe = false"
            // e mostrare "cards/back-teal.png".
            renderHandToLayout(p, targetLayout);
        }
    }
}

void Cirulla::dealNextRound()
{
    outputArea->append(QString("indice delle carte: %1").arg(state.deckIndex));
    // Distribuisci 3 carte a ciascuno
    for (auto &player : state.seats)
    {
        for (int i = 0; i < 3; ++i)
        {
            if (state.deckIndex < state.deck.size())
            {
                player.hand.append(state.deck[state.deckIndex++]);
            }
        }
    }
    // Nota: NON puliamo le prese e le scope qui!

    // Aggiorniamo l'interfaccia per mostrare le nuove carte
    for (int i = 0; i < state.seats.size(); ++i)
    {
        updatePlayerUI(i);
    }
}

void Cirulla::handleEndOfGame()
{

    PlayerState &ultimoAPrendere = state.seats[lastPlayerToScore];
    outputArea->append("Le carte rimaste sul tavolo vanno all'ultimo giocatore che ha preso: " + config.playerNames[ultimoAPrendere.id]);

    int lastOneId = ultimoAPrendere.id;

    for (auto &card : state.tableCards)
    {
        // outputArea->append("Carta aggiunta: " + card.faceValue);
        ultimoAPrendere.prese.append(card);
    }

    QTimer::singleShot(4000, this, [=]()
                       {
    PlayerState &player = state.seats[lastOneId];
    aggiornaMazzoPrese(player);
    state.tableCards.clear();
    showTable(); });
    for (int i = 0; i < 4; i++)
    {
        nameLabel[i]->setText(config.playerNames[i]);
        QString avatarPath = "avatars/" + config.playerNames[i] + ".png";
        QPixmap cardImage(avatarPath);

        scoreAvatar[i]->setPixmap(cardImage.scaled(120, 140, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        applyRoundedCorners(scoreAvatar[i]);
    }

    // visualizza schermo punteggi
    QTimer::singleShot(4000, this, [=]()
                       { stackedWidget->setCurrentIndex(1); });
    outputArea->clear();

    for (auto &player : state.seats)
    {
        QVector<Carta> totaleCarte = player.prese + player.scope;
        player.punteggi.append(calcolaPunteggio(totaleCarte, player.totaleScope));
    }

    // Carte, denari e primiera

    for (int i = 0; i < 3; i++)
    {
        calcolaPunti(i);
    }

    // VISUALIZZAZIONE

    outputArea->append("Primiera:");

    for (int i = 0; i < 4; ++i)
    {
        lblScope[i]->setStyleSheet("color: #2ecc71; font-weight: bold;");
        lblScope[i]->setText(QString("Scope: %1").arg(state.seats[i].punteggi[state.hand].scope));

        QString carteResult = QString("N. Carte: %1").arg(state.seats[i].punteggi[state.hand].carte);
        if (state.seats[i].punteggi[state.hand].cartePunto == 1)
        {
            // Il giocatore ha la maggioranza: mettiamo il testo in verde e aggiungiamo un segno distintivo
            lblCarte[i]->setText(carteResult + " [Punto!]");
            lblCarte[i]->setStyleSheet("color: #2ecc71; font-weight: bold;"); // Verde brillante
        }
        else
        {
            // Nessun punto o pareggio
            lblCarte[i]->setText(carteResult);
        }

        // DENARI
        QString denariResult = QString("N. Denari: %1").arg(state.seats[i].punteggi[state.hand].denari);

        if (state.seats[i].punteggi[state.hand].denariPunto == 1)
        {
            // Il giocatore ha la maggioranza: mettiamo il testo in verde e aggiungiamo un segno distintivo
            lblDenari[i]->setText(denariResult + " [Punto!]");
            lblDenari[i]->setStyleSheet("color: #2ecc71; font-weight: bold;"); // Verde brillante
        }
        else
        {
            // Nessun punto o pareggio
            lblDenari[i]->setText(denariResult);
        }

        // SETTEBELLO
        if (state.seats[i].punteggi[state.hand].settebello == 1)
        {
            // Il giocatore ha il settebello
            lblSettebello[i]->setText("Settebello: 1 [Punto!]");
            lblSettebello[i]->setStyleSheet("color: #2ecc71; font-weight: bold;"); // Verde brillante
        }
        else
        {
            // Nessun punto
            lblSettebello[i]->setText("Settebello: -");
        }

        // PRIMIERA

        QHBoxLayout *primieraLayout = new QHBoxLayout(primieraContainer[i]);
        primieraText[i] = new QLabel();
        // primieraText[i]->setStyleSheet("border: none");
        primieraLayout->setContentsMargins(0, 0, 0, 0);
        primieraLayout->setSpacing(2);

        for (int j = 0; j < 4; ++j)
        {
            primieraThumbnails[i][j] = new QLabel();
            primieraThumbnails[i][j]->setFixedSize(25, 35); // Dimensione miniatura
            primieraThumbnails[i][j]->setScaledContents(true);
            primieraLayout->addWidget(primieraThumbnails[i][j]);

            QString cardPath =
                QString("cards/%1.png").arg(state.seats[i].punteggi[state.hand].primieraCarte[j].id + 1);
            QString info = QString("%1 di %2, ")
                               .arg(state.seats[i].punteggi[state.hand].primieraCarte[j].faceValue)
                               .arg(cartaSemeToString(state.seats[i].punteggi[state.hand].primieraCarte[j].seme)); // Usa una funzione helper

            outputArea->insertPlainText(info);

            primieraThumbnails[i][j]->setPixmap(cardPath);
            primieraThumbnails[i][j]->show();
        }

        outputArea->append("");

        primieraLayout->addWidget(primieraText[i]);

        if (state.seats[i].punteggi[state.hand].primieraPunto == 1)
        {
            // Il giocatore ha la primiera
            primieraText[i]->setText("Primiera: 1 [Punto!]");
            primieraText[i]->setStyleSheet("color: #2ecc71; font-weight: bold;"); // Verde brillante
        }
        else
        {
            // Nessun punto
            primieraText[i]->setText("Primiera: -");
        }

        // PICCOLA

        if (state.seats[i].punteggi[state.hand].piccola > 0)
        {
            // Il giocatore ha la piccola
            lblPiccola[i]->setText(QString("Piccola: %1 punti!)").arg(state.seats[i].punteggi[state.hand].piccola));
            lblPiccola[i]->setStyleSheet("color: #2ecc71; font-weight: bold;"); // Verde brillante
        }
        else
        {
            // Nessun punto
            lblPiccola[i]->setText("Piccola: -");
        }

        // GRANDE

        if (state.seats[i].punteggi[state.hand].grande > 0)
        {
            // Il giocatore ha la grande
            lblGrande[i]->setText(QString("Grande: %1 punti!)").arg(state.seats[i].punteggi[state.hand].grande));
            lblGrande[i]->setStyleSheet("color: #2ecc71; font-weight: bold;"); // Verde brillante
        }
        else
        {
            // Nessun punto
            lblGrande[i]->setText("Grande: -");
        }

        int totale = state.seats[i].punteggi[state.hand].calcolaTotale();
        lblTotale[i]->setText(QString("Totale: %1").arg(totale));
        lblTotale[i]->setStyleSheet("color: #dc046d; font-weight: bold;");
    }

    ++state.hand;
    /* * PSEUDOCODICE PER IL RESET E NUOVA PARTITA:
     * * 1. RESET_GIOCATORI:
     * Per ogni giocatore in state.seats:
     * - Svuota il vettore 'prese'.
     * - Svuota il vettore 'scope'.
     * - Azzera 'totaleScope' (o aggiornalo se è un contatore cumulativo).
     * - Azzera il numero di carte in mano (o prepara il nuovo mazzo).
     * * 2. RESET_TAVOLO:
     * - Svuota state.tableCards.
     * - Resetta il deck di gioco (mescola e prepara).
     * * 3. UI_CLEANUP:
     * - Rimuovi fisicamente le icone carte dagli oggetti grafici.
     * - Aggiorna i nomi/avatar (che devono restare visibili).
     * - Richiama showTable() per visualizzare il tavolo vuoto/nuovo.
     * * 4. AVVIO:
     * - startGame() // Inizia la distribuzione delle nuove carte.
     */
}

void Cirulla::aggiornaMazzoPrese(PlayerState &p)
{
    QPixmap retro("cards/back-teal.png");
    mazzoPreseIconArr[p.id]->setPixmap(retro.scaled(50, 70, Qt::KeepAspectRatio));
    aggiornaStats(p.id);
}

void Cirulla::aggiornaMazzoScope(PlayerState &p, Carta &c)
{

    QString cartaIconPath = QString("cards/%1.png").arg(c.id + 1);

    QPixmap cartaIcon(cartaIconPath);
    mazzoScopeIconArr[p.id]->setPixmap(cartaIcon.scaled(50, 70, Qt::KeepAspectRatio));
    aggiornaStats(p.id);
}

bool Cirulla::contieneMatta(const QVector<Carta> &tableCards)
{
    for (const auto &c : tableCards)
    {
        if (c.id == 16)
            return true; // 7 di Cuori
    }
    return false;
}

bool Cirulla::checkBuonaDaDieci(int playerIndex)
{

    QVector<Carta> carteInMano = state.seats[playerIndex].hand;

    // Safety check: non eseguire se non siamo nelle condizioni giuste
    if (state.seats[playerIndex].carteScoperte)
    {
        return false;
    }
    if (carteInMano.size() != 3)
        return false;

    if (contieneMatta(carteInMano))
    {
        int start = 1;
        Carta test = carteInMano[0];
        if (test.id == 16)
        {
            start = 2;
            test = carteInMano[1];
        }
        for (int i = start; i < 3; i++)
        {
            if (carteInMano[i].faceValue == test.faceValue)
            {
                return true;
            }
        }
    }
    else if (all_cards_same(carteInMano))
    {
        return true;
    }
    return false;
}

bool Cirulla::checkBuonaDaTre(int playerIndex)
{

    QVector<Carta> carteInMano = state.seats[playerIndex].hand;
    // Safety check: non eseguire se non siamo nelle condizioni giuste
    if (state.seats[playerIndex].carteScoperte)
    {
        return false;
    }
    bool mattaPresente = contieneMatta(carteInMano);
    int total = cards_sum(carteInMano);
    if (mattaPresente && total <= 15)
    {

        return true;
    }

    else if (total <= 9)
        return true;

    return false;
}

void Cirulla::applyBuonaDaDieci(int playerIndex)
{
    {
        state.seats[playerIndex].carteScoperte = true;
        state.seats[playerIndex].totaleScope += 10;
        aggiornaStats(playerIndex);
        showHands();
        QString nomeGiocatore = (playerIndex == config.humanSeatIndex) ? "Tu" : "Il Giocatore " + config.playerNames[playerIndex];
        QString verbo = (playerIndex == config.humanSeatIndex) ? " hai " : " ha ";
        QString messaggio = verbo + "bussato e " + verbo + "fatto 10 scope!";
        QMessageBox msgBox;
        msgBox.setWindowTitle("Buona da 10!");
        msgBox.setText(nomeGiocatore + messaggio);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec(); // Questo blocca l'esecuzione finche' non si preme OK
    }
}

void Cirulla::applyBuonaDaTre(int playerIndex)
{
    {
        state.seats[playerIndex].carteScoperte = true;
        state.seats[playerIndex].totaleScope += 3;
        aggiornaStats(playerIndex);
        showHands();

        QString nomeGiocatore = (playerIndex == config.humanSeatIndex) ? "Tu" : "Il Giocatore " + config.playerNames[playerIndex];
        QString verbo = (playerIndex == config.humanSeatIndex) ? " hai " : " ha ";
        QString messaggio = verbo + "bussato e " + verbo + "fatto 3 scope!";
        QMessageBox msgBox;
        msgBox.setWindowTitle("Barsega!");
        msgBox.setText(nomeGiocatore + messaggio);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec(); // Questo blocca l'esecuzione finche' non si preme OK
    }
}

void Cirulla::aggiornaStats(int playerIndex)
{
    mazzoTextArr[playerIndex]->setText(config.playerNames[playerIndex] +
                                       "\nPrese: " + QString::number(state.seats[playerIndex].prese.size()) +
                                       "\nScope: " + QString::number(state.seats[playerIndex].totaleScope));
}

void Cirulla::applyRoundedCorners(QLabel *label)
{
    // Ottieni l'immagine originale dal widget
    QPixmap original = label->pixmap();
    if (original.isNull())
        return;

    // Crea una nuova pixmap trasparente della stessa dimensione
    QPixmap target(label->size());
    target.fill(Qt::transparent);

    QPainter painter(&target);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    // Definisci il rettangolo arrotondato
    QPainterPath path;
    path.addRoundedRect(0, 0, label->width(), label->height(), 20, 20);

    // "Taglia" l'area col path e disegna l'immagine dentro
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, original.scaled(label->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));

    painter.end();

    // Applica l'immagine "arrotondata" alla label
    label->setPixmap(target);
}

Score Cirulla::calcolaPunteggio(const QVector<Carta> &totaleCarte, int totaleScope)
{
    Score score;
    score.scope = totaleScope;
    score.carte = totaleCarte.length();
    QVector<Carta> picche;
    QVector<Carta> cuori;
    QVector<Carta> denari;
    QVector<Carta> fiori;
    QVector<Carta> piccola;
    QVector<Carta> grande;
    for (auto &carta : totaleCarte)
    {
        switch (carta.seme)
        {
        case Seme::Picche:
            picche.append(carta);
            break;
        case Seme::Cuori:
            cuori.append(carta);
            break;
        case Seme::Denari:

            denari.append(carta);
            if (carta.faceValue == 7)
                score.settebello = 1;
            break;
        case Seme::Fiori:
            fiori.append(carta);
            break;
        }
    }

    score.denari = denari.length();

    std::sort(denari.begin(), denari.end(), [](const Carta &a, const Carta &b)
              { return a.faceValue < b.faceValue; });

    // find the Piccola
    // 1. Assicurati che ci siano denari prima di procedere
    if (!denari.isEmpty())
    {
        int expectedValue = 1; // La scala deve partire dall'Asso (1)

        for (const auto &carta : denari)
        {
            if (carta.faceValue == expectedValue)
            {
                piccola.append(carta);
                expectedValue++; // Ti aspetti il valore successivo
            }
            else
            {
                break; // La sequenza si è interrotta
            }
        }
    }

    outputArea->append("PICCOLA:");

    for (Carta carta : piccola)
    {
        QString cardPath =
            QString("cards/%1.png").arg(carta.id + 1);
        QString info = QString("%1 di %2, ")
                           .arg(carta.faceValue)
                           .arg(cartaSemeToString(carta.seme)); // Usa una funzione helper

        outputArea->insertPlainText(info);
    }

    int piccolaSize = piccola.length();
    if (piccolaSize >= 3)
    {
        score.piccola = piccolaSize;
    }

    // find the Grande
    if (!denari.isEmpty())
    {
        int expectedValue = 10;

        // Scorriamo il vettore denari al contrario per trovare la sequenza decrescente
        for (auto it = denari.rbegin(); it != denari.rend(); ++it)
        {
            if (it->faceValue == expectedValue)
            {
                grande.append(*it);
                expectedValue--;
            }
            else
            {
                break;
            }
        }
    }

    if (grande.length() >= 3)
    {
        score.grande = 5;
    }
    else
    {
        score.grande = 0;
    }

    // Funzione temporanea per gestire l'assegnazione sicura
    auto assegnaCarta = [&](int index, const QVector<Carta> &mazzoSeme)
    {
        Carta migliore = trovaCartaMigliorePerSeme(mazzoSeme);

        // Se l'ID è valido (es. maggiore di 0), assegniamo la carta.
        // Altrimenti, lasciamo l'array pulito o settiamo un flag di "vuoto".
        if (migliore.id > 0)
        {
            score.primieraCarte[index] = migliore;
        }
        else
        {
            score.primieraCarte[index].id = 0; // Segnaliamo che qui non c'è carta
            score.primieraCarte[index].faceValue = 0;
        }
    };

    assegnaCarta(0, picche);
    assegnaCarta(1, cuori);
    assegnaCarta(2, denari);
    assegnaCarta(3, fiori);

    score.primieraValore = 0;
    for (int i = 0; i < 4; i++)
    {
        score.primieraValore += valorePrimieraCarta(score.primieraCarte[i]);
    }

    return score;
}

int Cirulla::valorePrimieraCarta(const Carta &carta)
{
    if (carta.id == ID_CARTA_NULLA)
    {
        return -1;
    }
    switch (carta.faceValue)
    {
    case 7:
        return 21;
    case 6:
        return 18;
    case 1:
        return 16;
    case 5:
        return 15;
    case 4:
        return 14;
    case 3:
        return 13;
    case 2:
        return 12;
    case 8:
    case 9:
    case 10:
        return 10;
    default:
        return 0;
    }
}

Carta Cirulla::trovaCartaMigliorePerSeme(const QVector<Carta> &carteSeme)
{
    Carta migliore;
    migliore.id = ID_CARTA_NULLA;
    migliore.faceValue = 0;
    int maxVal = -1;

    for (const auto &c : carteSeme)
    {
        int v = valorePrimieraCarta(c);
        if (v > maxVal)
        {
            maxVal = v;
            migliore = c;
        }
    }
    return migliore;
}

void Cirulla::calcolaPunti(int tipoStat)
{ // tipoStat: 0=carte, 1=denari, 2=primiera
    int valoreMax = -1;
    int conteggioMassimi = 0;

    // 1. Trova il massimo e conta quanti lo hanno
    for (const auto &giocatore : state.seats)
    {
        int valore = (tipoStat == 0) ? giocatore.punteggi[state.hand].carte : (tipoStat == 1) ? giocatore.punteggi[state.hand].denari
                                                                                              : giocatore.punteggi[state.hand].primieraValore;

        if (valore > valoreMax)
        {
            valoreMax = valore;
            conteggioMassimi = 1;
        }
        else if (valore == valoreMax && valoreMax > 0)
        {
            conteggioMassimi++;
        }
    }

    // 2. Assegna il punto solo se c'è un vincitore unico
    for (auto &giocatore : state.seats)
    {
        int valore = (tipoStat == 0) ? giocatore.punteggi[state.hand].carte : (tipoStat == 1) ? giocatore.punteggi[state.hand].denari
                                                                                              : giocatore.punteggi[state.hand].primieraValore;

        bool vince = (valore == valoreMax && conteggioMassimi == 1 && valoreMax > 0);

        if (tipoStat == 0)
            giocatore.punteggi[state.hand].cartePunto = vince ? 1 : 0;
        else if (tipoStat == 1)
            giocatore.punteggi[state.hand].denariPunto = vince ? 1 : 0;
        else
            giocatore.punteggi[state.hand].primieraPunto = vince ? 1 : 0;
    }
}

QString Cirulla::cartaSemeToString(Seme seme)
{
    switch (seme)
    {
    case Seme::Denari:
        return "Denari";
    case Seme::Picche:
        return "Picche";
    case Seme::Cuori:
        return "Cuori";
    case Seme::Fiori:
        return "Fiori";
    default:
        return "Sconosciuto";
    }
}

Cirulla::~Cirulla()
{
}
