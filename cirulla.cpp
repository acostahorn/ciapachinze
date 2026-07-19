#include "cirulla.h"
#include "homeScreen.h"
#include "CharacterManager.h"
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
#include <QInputDialog>
#include <QPushButton>

Cirulla::Cirulla(QWidget *parent) : QWidget(parent)
{

    this->setFixedSize(1400, 1000);
    stackedWidget = new QStackedWidget(this);

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

    // riferimento a homeScreen

    homeScreen = new HomeScreen(this);
    stackedWidget->addWidget(homeScreen);

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
    outputArea->setMaximumHeight(10);
    outputArea->hide();
    mainLayout->addWidget(outputArea);

    infoOverlay = new QFrame(this);
    infoOverlay->setStyleSheet("font-weight: bold; font-size: 18px;");

    overlayLayout = new QHBoxLayout(infoOverlay);
    overlayLayout->setContentsMargins(10, 0, 10, 0);

    infoOverlay->setFixedHeight(70);

    infoLabel = new QLabel("CIRULLA", infoOverlay);
    button = new QPushButton("Inizia il Gioco", infoOverlay);

    overlayLayout->addStretch(1);

    overlayLayout->addWidget(infoLabel);

    overlayLayout->addWidget(button);

    overlayLayout->addStretch(1);

    mainLayout->addWidget(infoOverlay);

    // Game Area

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

    manoArea = new QWidget();
    manoLabel = new QLabel();
    manoLabel->setAlignment(Qt::AlignCenter);
    manoLabel->setStyleSheet("font-weight: bold; font-size: 18px;");
    manoLayout = new QHBoxLayout(manoArea);

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

        lblScope[i] = new QLabel("Scope: -");
        lblCarte[i] = new QLabel("Carte: -");
        lblDenari[i] = new QLabel("Denari: -");
        lblSettebello[i] = new QLabel("Settebello: -");
        lblPiccola[i] = new QLabel("Piccola: -");
        lblGrande[i] = new QLabel("Grande: -");
        primieraContainer[i] = new QWidget();
        lblTotale[i] = new QLabel("Totale: -");

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

    manoLayout->addWidget(manoLabel);
    scoreGrid->addWidget(manoArea, 0, 0, 1, 2);

    for (int i = 0; i < 4; i++)
    {
        int riga = (i == 0 || i == 2) ? 1 : 2;
        int colonna = (i == 0 || i == 1) ? 0 : 1;
        scoreGrid->addWidget(playerArea[i], riga, colonna);
    }

    for (int i = 0; i < 4; ++i)
    {

        primieraLayout[i] = new QHBoxLayout(primieraContainer[i]);
        primieraText[i] = new QLabel();
        primieraLayout[i]->setContentsMargins(0, 0, 0, 0);
        primieraLayout[i]->setSpacing(2);
        for (int j = 0; j < 4; ++j)
        {
            primieraThumbnails[i][j] = new QLabel();
            primieraThumbnails[i][j]->setFixedSize(25, 35); // Dimensione miniatura
            primieraThumbnails[i][j]->setScaledContents(true);
            primieraLayout[i]->addWidget(primieraThumbnails[i][j]);
        }
    }

    scoreArea->setLayout(scoreGrid);
    stackedWidget->addWidget(scoreArea);

    // 7. Initialization

    updateOverlay("", "", false);
    connect(button, &QPushButton::clicked, this, &Cirulla::onGlobalOverlayClicked);

    connect(homeScreen, &HomeScreen::startRequested, this, [this](QString p)
            {
                stackedWidget->setCurrentIndex(1);
                updateOverlay("CIRULLA", "Inizia il Gioco", true);
                statoAttualeBottone = FaseBottone::FaseAvvio; 
                this->setupGame(getPlayers(p)); });
}

void Cirulla::mainScreen()
{

    for (int i = 0; i < state.seats.size(); ++i)
    {
        ++config.players[i].playedMatches;
        CharacterManager::updatePlayerStats(config.players[i]);
    }
    updateOverlay("", "", false);
    clearScopeAndPreseLabels();
    stackedWidget->setCurrentIndex(0);
}

void Cirulla::startGame()
{

    // 1. First shuffle and show the dealer extraction cards
    state.deck = generateShuffledDeck();
    state.dealerIndex = selectDealer();

    currentGamePhase = STATE_READY_TO_START;

    // 2. Prepariamo l'interfaccia per la pausa
    QString dealerName = state.seats[state.dealerIndex].name;

    statoAttualeBottone = FaseBottone::FaseSmazzata;
    updateOverlay(QString("%1 è il nuovo Mazziere!").arg(dealerName), "Inizia la smazzata", true);
}

QVector<ProfileData> Cirulla::getPlayers(QString p)
{
    return CharacterManager::getPlayers(p);
}

void Cirulla::executeDeal()
{
    for (int i = 0; i < state.seats.size(); ++i)
    {
        if (!state.seats[i].hand.isEmpty())
        {
            outputArea->append("ERRORE: Tentata distribuzione con carte ancora in mano!");
            return; // Blocca la distribuzione!
        }
    }

    button->setEnabled(false);

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
    state.tableCards.clear();
    initialDeal();

    for (int i = 0; i < 4; i++)
    {
        QString mano;
        for (auto &c : state.seats[i].hand)
            mano += QString::number(c.faceValue) + " ";
        // outputArea->append("DEBUG: Mano Giocatore " + QString::number(i) + ": " + mano);
    }

    showHands();
    showTable();
    dealersChance();
    processTurn();
}

void Cirulla::setupGame(const QVector<ProfileData> &players)
{
    hardReset();

    updateOverlay("CIRULLA", "Inizia il Gioco", true);
    statoAttualeBottone = FaseBottone::FaseAvvio;

    state.seats.clear();
    config.players.clear();

    state.phase = MatchPhase::Playing;
    config.mode = GameMode::Offline;
    config.humanSeatIndex = 2;
    config.players = players;

    for (int i = 0; i < 4; ++i)
    {
        PlayerState p;
        p.id = i;
        p.totaleScope = 0;
        QString avatarPath = config.players[i].avatarPath;
        QPixmap cardImage(avatarPath);

        if (avatarArr[i])
        {

            avatarArr[i]->setPixmap(cardImage.scaled(120, 140, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            applyRoundedCorners(avatarArr[i]);
        }

        if (i == config.humanSeatIndex && botGame == false)
        {
            p.name = config.players[i].name;
            p.type = SeatType::Human;
            p.carteScoperte = false;
        }
        else
        {
            p.name = config.players[i].name;
            p.type = SeatType::Bot;
        }
        state.seats.append(p);
        aggiornaStats(p.id);
    }
}

void Cirulla::initialDeal()
{
    // RESET DI SICUREZZA: Assicuriamoci che l'indice parta da zero
    state.deckIndex = 0;

    int playerIndex = 0; // Contatore reale per i giocatori

    for (auto &player : state.seats)
    {
        player.hand.clear();
        player.scope.clear();
        player.prese.clear();
        player.totaleScope = 0;

        for (int i = 0; i < 3; ++i)
        {
            if (state.deckIndex < state.deck.size())
            {
                Carta c = state.deck[state.deckIndex];
                player.hand.append(c);

                // LOG CORRETTO: ora vediamo davvero chi riceve cosa
                outputArea->append("Giocatore " + QString::number(playerIndex) +
                                   " riceve carta ID: " + QString::number(c.id) +
                                   " (Valore: " + QString::number(c.faceValue) +
                                   ") | Indice: " + QString::number(state.deckIndex));

                state.deckIndex++;
            }
        }
        playerIndex++; // Incrementa l'indice del giocatore
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

    if (state.tableCards.isEmpty())
    {
        outputArea->append("DEBUG: dealersChance saltato, tavolo vuoto.");
        return;
    }
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
        QString messaggio = (total == 15) ? "Scopa per il mazziere!" : "Due scope per il mazziere!";

        outputArea->append(messaggio);

        if (total == 15)
        {
            state.seats[state.dealerIndex].scope.append(state.tableCards[0]);
            for (int i = 1; i < state.tableCards.size(); ++i)
            {
                state.seats[state.dealerIndex].prese.append(state.tableCards[i]);
            }
            state.seats[state.dealerIndex].totaleScope = 1;
            aggiornaStats(state.dealerIndex);
        }
        else
        {
            state.seats[state.dealerIndex].scope.append(state.tableCards[0]);
            state.seats[state.dealerIndex].scope.append(state.tableCards[1]);
            for (int i = 2; i < state.tableCards.size(); ++i)
            {
                state.seats[state.dealerIndex].prese.append(state.tableCards[i]);
            }

            state.seats[state.dealerIndex].totaleScope = 2;
            aggiornaStats(state.dealerIndex);
        }

        state.tableCards.clear();
        showTable(); // Aggiorna il tavolo solo dopo la pausa
        outputArea->append("Il tavolo è stato ripulito.");
        statoAttualeBottone = FaseBottone::DopoScopaMazziere;
    }
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

    // QString winnerMsg = QString("%1 vince il mazzo ed è il Mazziere!").arg(state.seats[active_players[0]].name);
    // outputArea->append(winnerMsg);
    // qApp->processEvents();

    // state.tableCards.clear();
    // QMessageBox::information(
    //     this,
    //     "Estrazione Mazziere",
    //     winnerMsg + "\n\nClicca OK per iniziare la smazzata.");

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
    if (isWaitingForBuona)
        return;

    // 1. Identifichiamo il giocatore corrente
    int currentPlayerIndex = state.currentTurnIndex;

    if (currentPlayerIndex < 0 || currentPlayerIndex >= state.seats.size())
        return;
    updateOverlay("Turno: " + state.seats[currentPlayerIndex].name, "", false);
    outputArea->append("Turno: " + state.seats[currentPlayerIndex].name);

    bool isStartOfHand = (state.seats[currentPlayerIndex].hand.size() == 3);
    bool notYetDeclared = (!state.seats[currentPlayerIndex].carteScoperte);

    if (isStartOfHand && notYetDeclared)
    {
        if (checkBuonaDaDieci(currentPlayerIndex))
        {
            isWaitingForBuona = true;
            applyBuonaDaDieci(currentPlayerIndex);
            QString nomeGiocatore = (currentPlayerIndex == config.humanSeatIndex) ? "Tu" : "Il Giocatore " + config.players[currentPlayerIndex].name;
            QString verbo = (currentPlayerIndex == config.humanSeatIndex) ? " hai " : " ha ";
            QString messaggio = verbo + "bussato e " + verbo + "fatto 10 scope!";
            statoAttualeBottone = FaseBottone::FaseDopoBuona;

            updateOverlay(nomeGiocatore + messaggio, "OK", true);

            return;
        }
        else if (checkBuonaDaTre(currentPlayerIndex))
        {
            isWaitingForBuona = true;
            applyBuonaDaTre(currentPlayerIndex);
            QString nomeGiocatore = (currentPlayerIndex == config.humanSeatIndex) ? "Tu" : "Il Giocatore " + config.players[currentPlayerIndex].name;
            QString verbo = (currentPlayerIndex == config.humanSeatIndex) ? " hai " : " ha ";
            QString messaggio = verbo + "bussato e " + verbo + "fatto 3 scope!";
            statoAttualeBottone = FaseBottone::FaseDopoBuona;
            updateOverlay(nomeGiocatore + messaggio, "OK", true);
            return;
        }
    }

    // 2. Se è il giocatore umano (supponiamo sia sempre al seat 2)
    if (currentPlayerIndex == config.humanSeatIndex && botGame == false)
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
                    selectedHandCardIndex = newIndex;
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
    // 1. Esegui la mossa
    makeMove(handIndex, tableIndices);
    enableHandInteraction(false);
    updatePlayerUI(state.currentTurnIndex);

    bool tuttiHannoFinito = true;
    for (const auto &player : state.seats)
    {
        if (!player.hand.isEmpty())
        {
            tuttiHannoFinito = false;
            break;
        }
    }

    // 2. Gestione fine mano
    if (tuttiHannoFinito)
    {
        // fprintf(stderr, "Tutti hanno finito!\n");
        // fflush(stderr);
        if (state.deckIndex < state.deck.size())
        {
            for (auto &player : state.seats)
                player.carteScoperte = false;
            dealNextRound();
            state.currentTurnIndex = (state.dealerIndex + 1) % state.seats.size();
            for (int i = 0; i < state.seats.size(); ++i)
                updatePlayerUI(i);
        }
        else
        {
            // l'ultimo svuota il tavolo

            PlayerState &ultimoAPrendere = state.seats[lastPlayerToScore];
            outputArea->append("Le carte rimaste sul tavolo vanno all'ultimo giocatore che ha preso: " + config.players[ultimoAPrendere.id].name);

            for (auto &carta : state.deck)
            {
                if (carta.id == 16)
                {
                    carta.faceValue = 7;
                    break;
                }
            }

            resetValoreMatta();

            int lastOneId = ultimoAPrendere.id;

            for (auto &card : state.tableCards)
            {
                // outputArea->append("Carta aggiunta: " + card.faceValue);
                ultimoAPrendere.prese.append(card);
            }

            QTimer::singleShot(2000, this, [this, lastOneId]()
                               {
                
                if (this->state.phase != MatchPhase::Playing) return;

                PlayerState &player = this->state.seats[lastOneId];
                this->aggiornaMazzoPrese(player);
                this->state.tableCards.clear();
                this->showTable();
                
                // Ora chiamiamo la fine gioco in sequenza sicura
                this->handleEndOfGame(); });
            return; // Partita finita, non chiamare processTurn
        }
    }
    else
    {
        // 3. PASSAGGIO TURNO: Solo questo!
        selectedHandCardIndex = -1;
        selectedTableIndices.clear();
        state.currentTurnIndex = (state.currentTurnIndex + 1) % state.seats.size();
        // outputArea->append("DEBUG: Turno passato a " + state.seats[state.currentTurnIndex].name);
    }

    // 4. L'UNICA COSA DA FARE: chiamare il regista
    processTurn();
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
    if (isWaitingForBuona)
        return;
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
    if (state.phase != MatchPhase::Playing)
    {
        QWidget::keyPressEvent(event);
        return;
    }

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

    // 1. Controllo di sicurezza: indice giocatore valido?
    if (state.currentTurnIndex < 0 || state.currentTurnIndex >= state.seats.size())
    {
        return QVector<Mossa>();
    }

    PlayerState &giocatore = state.seats[state.currentTurnIndex];

    // 2. PROTEZIONE CRITICA: Indice mano valido? Mano non vuota?
    if (handIndex < 0 || handIndex >= giocatore.hand.size())
    {
        outputArea->append(QString("ERRORE: Accesso a indice mano %1 non valido (Size: %2)")
                               .arg(handIndex)
                               .arg(giocatore.hand.size()));
        return QVector<Mossa>();
    }

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
        // if (isMatta)
        // {
        //     // La matta prende se sommaTavolo è un valore che lei può "coprire"
        //     // Caso 1: Presa normale (sommaTavolo tra 1 e 10)
        //     // Caso 2: Presa 15 (sommaTavolo tra 5 e 14, perché 15 - [1..10] = [14..5])
        //     if (sommaTavolo >= 1 && sommaTavolo <= 14)
        //     {
        //         prese.append({handIndex, combo});
        //     }
        // }
        // else
        // {
        // Logica per carte normali
        if (sommaTavolo == valore || (sommaTavolo + valore == 15))
        {
            prese.append({handIndex, combo});
        }
        // }
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

    int handIdx = m.handIndex;
    QList<int> tableIndices = m.tableIndices;

    QTimer::singleShot(waitTime, this, [this, handIdx, tableIndices]()

                       {
                        if (this->state.phase != MatchPhase::Playing) return;
                        QList<int> listaLocale = tableIndices;
        // Ora tableIndices è una copia locale, nessuno può toccarla
        // E non serve più il const_cast perché tableIndices è locale e non const
        this->playCard(handIdx, listaLocale); 
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
    // fprintf(stderr, "Diamo le carte!\n");
    // fflush(stderr);
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

    for (int i = 0; i < state.seats.size(); ++i)
    {
        updatePlayerUI(i);
    }
}

void Cirulla::handleEndOfGame()
{

    for (int i = 0; i < 4; i++)
    {
        nameLabel[i]->setText(config.players[i].name);
        QString avatarPath = config.players[i].avatarPath;
        QPixmap cardImage(avatarPath);

        scoreAvatar[i]->setPixmap(cardImage.scaled(120, 140, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        applyRoundedCorners(scoreAvatar[i]);
    }

    state.phase = MatchPhase::Scoring;

    // visualizza schermo punteggi

    stackedWidget->setCurrentIndex(2);
    outputArea->clear();

    QString manoText = QString("MANO N. %1").arg(state.hand + 1);

    if (config.giocoACoppie)
    {
        manoText += QString(" - <font color='%1'>%2</font> e <font color='%1'>%3</font> vs <font color='%4'>%5</font> e <font color='%4'>%6</font>")
                        .arg(colBlu)
                        .arg(config.players[0].name)
                        .arg(config.players[2].name)
                        .arg(colArancio)
                        .arg(config.players[1].name)
                        .arg(config.players[3].name);
    }

    manoLabel->setText(manoText);

    if (config.giocoACoppie)
    {
        for (int i = 0; i < 2; ++i)
        {
            QVector<Carta> totaleCarte = state.seats[i].prese + state.seats[i + 2].prese + state.seats[i].scope + state.seats[i + 2].scope;
            state.seats[i].punteggi.append(calcolaPunteggio(totaleCarte, state.seats[i].totaleScope + state.seats[i + 2].totaleScope));
        }
    }
    else
    {
        for (auto &player : state.seats)
        {
            QVector<Carta> totaleCarte = player.prese + player.scope;
            player.punteggi.append(calcolaPunteggio(totaleCarte, player.totaleScope));
        }
    }

    // Carte, denari e primiera

    for (int i = 0; i < 3; i++)
    {
        calcolaPunti(i);
    }

    // VISUALIZZAZIONE

    for (int i = 0; i < 4; ++i)
    {
        // Clear the text label
        primieraText[i]->clear();

        // Clear all thumbnails
        for (int j = 0; j < 4; ++j)
        {
            primieraThumbnails[i][j]->clear();
            primieraThumbnails[i][j]->hide(); // Optional: hides the empty slot
        }
    }

    outputArea->append("Primiera:");

    int limite = config.giocoACoppie ? 2 : 4;

    for (int i = 0; i < limite; ++i)
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

        for (int j = 0; j < 4; ++j)
        {

            QString cardPath =
                QString("cards/%1.png").arg(state.seats[i].punteggi[state.hand].primieraCarte[j].id + 1);
            QString info = QString("%1 di %2, ")
                               .arg(state.seats[i].punteggi[state.hand].primieraCarte[j].faceValue)
                               .arg(cartaSemeToString(state.seats[i].punteggi[state.hand].primieraCarte[j].seme)); // Usa una funzione helper

            outputArea->insertPlainText(info);

            if (state.seats[i].punteggi[state.hand].primieraCarte[j].faceValue > 0)
            {

                primieraThumbnails[i][j]->setPixmap(cardPath);
                primieraThumbnails[i][j]->show();
            }
        }

        outputArea->append("");

        primieraLayout[i]->addWidget(primieraText[i]);

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
            lblPiccola[i]->setText(QString("Piccola: %1 punti!").arg(state.seats[i].punteggi[state.hand].piccola));
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
            lblGrande[i]->setText(QString("Grande: %1 punti!").arg(state.seats[i].punteggi[state.hand].grande));
            lblGrande[i]->setStyleSheet("color: #2ecc71; font-weight: bold;"); // Verde brillante
        }
        else
        {
            // Nessun punto
            lblGrande[i]->setText("Grande: -");
        }

        state.seats[i].puntiMano.push_back(state.seats[i].punteggi[state.hand].calcolaTotale());
        if (config.giocoACoppie)
        {
            state.seats[i + 2].puntiMano.push_back(state.seats[i].punteggi[state.hand].calcolaTotale());
        }
        lblTotale[i]->setText(QString("Totale: %1").arg(state.seats[i].puntiMano[state.hand]));
        lblTotale[i]->setStyleSheet("color: #dc046d; font-weight: bold;");
        if (config.giocoACoppie)
        {
            lblTotale[i + 2]->setText(QString("Totale: %1").arg(state.seats[i + 2].puntiMano[state.hand]));
            lblTotale[i + 2]->setStyleSheet("color: #dc046d; font-weight: bold;");
        }
    }

    if (config.giocoACoppie)
    {
        if (state.seats[0].puntiMano[state.hand] > state.seats[1].puntiMano[state.hand])
        {
            ++state.seats[0].puntiPartita;
            ++state.seats[2].puntiPartita;
        }
        else if (state.seats[0].puntiMano[state.hand] < state.seats[1].puntiMano[state.hand])
        {
            ++state.seats[1].puntiPartita;
            ++state.seats[3].puntiPartita;
        }
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            state.seats[i].puntiPartita += state.seats[i].puntiMano[state.hand];
        }
    }

    // visualizza punteggio mano
    for (int i = 0; i < 4; i++)
    {
        labelPunti[i]->setText(QString("Punti: %1")
                                   .arg(state.seats[i].puntiPartita));
    }

    if (config.giocoACoppie)
    {
        // Recupera i totali attuali (che sappiamo essere aggiornati correttamente)
        int puntiSquadraA = state.seats[0].puntiPartita;
        int puntiSquadraB = state.seats[1].puntiPartita;

        // Controllo se qualcuno ha vinto la PARTITA (raggiunto i 2 punti)
        if (puntiSquadraA >= 2 || puntiSquadraB >= 2)
        {
            if (puntiSquadraA >= 2)
            {
                ++config.players[0].wonMatches;
                ++config.players[2].wonMatches;
            }
            else
            {
                ++config.players[1].wonMatches;
                ++config.players[3].wonMatches;
            }
            QString vincitore = (puntiSquadraA >= 2) ? config.players[0].name + " e " + config.players[2].name
                                                     : config.players[1].name + " e " + config.players[3].name;
            QString colore = (puntiSquadraA >= 2) ? colBlu : colArancio;

            QString winnerText = QString("<font size=18>Vincono</font> <font color='%1' size=20>%2 !</font>")
                                     .arg(colore)
                                     .arg(vincitore);

            updateOverlay(winnerText, "Fine Partita", true);

            // fine gioco a coppie - ritorno alla schermata di partenza e trasmetti i nuovi dati giocatori
            statoAttualeBottone = FaseBottone::RitornoHomeScreen;
            emit gameFinished(config.players); // Passi i profili aggiornati alla HomeScreen
        }
        else
        {
            ++state.hand;
            state.phase = MatchPhase::Playing;

            // Connect the button's clicked signal to your continueGame function
            updateOverlay(" CONTINUA IL GIOCO", "CONTINUA", true);
            statoAttualeBottone = FaseBottone::FaseFineTurno;

            outputArea->append("Punteggio parziale: Squadra A " + QString::number(puntiSquadraA) +
                               " - Squadra B " + QString::number(puntiSquadraB));
        }
    }
    else if (state.hand > 1)
    {
        int indiceVincitore = -1;
        int maxPunti = -1;
        int conteggioMassimi = 0;
        for (int i = 0; i < 4; ++i)
        {
            if (state.seats[i].puntiPartita > maxPunti)
            {
                maxPunti = state.seats[i].puntiPartita;
                indiceVincitore = i;
                conteggioMassimi = 1;
            }
            else if (state.seats[i].puntiPartita == maxPunti)
            {
                conteggioMassimi++;
            }
        }
        if (conteggioMassimi == 1)
        {
            ++config.players[indiceVincitore].wonMatches;
            QString vincitore = config.players[indiceVincitore].name;

            QString colore = (indiceVincitore % 2 == 0) ? colBlu : colArancio;

            QString winnerText = QString("<font size=18>Vince</font> <font color='%1' size=20>%2 !</font>")
                                     .arg(colore)
                                     .arg(vincitore);

            updateOverlay(winnerText, "Fine Partita", true);
            // fine gioco individuale - ritorno alla schermata di partenza e trasmetti i nuovi dati giocatori
            statoAttualeBottone = FaseBottone::RitornoHomeScreen;
            emit gameFinished(config.players); // Passi i profili aggiornati alla HomeScreen
        }
        else
        {
            ++state.hand;
            state.phase = MatchPhase::Playing;

            // Connect the button's clicked signal to your continueGame function
            updateOverlay(" PAREGGIO : CONTINUA IL GIOCO", "CONTINUA", true);
            statoAttualeBottone = FaseBottone::FaseFineTurno;
        }
    }
    else
    {
        ++state.hand;
        state.phase = MatchPhase::Playing;

        // Connect the button's clicked signal to your continueGame function
        updateOverlay(" CONTINUA IL GIOCO", "CONTINUA", true);
        statoAttualeBottone = FaseBottone::FaseFineTurno;
    }
}

void Cirulla::continueGame()
{
    state.dealerIndex = (state.dealerIndex + 1) % 4;

    selectedHandCardIndex = -1;
    selectedTableIndices.clear();
    isWaitingForBuona = false;

    for (int i = 0; i < state.seats.size(); ++i)
    {
        state.seats[i].prese.clear();
        state.seats[i].scope.clear();
        state.seats[i].hand.clear();
        state.seats[i].totaleScope = 0;
        aggiornaStats(i);
        mazzoScopeIconArr[i]->clear();
        mazzoPreseIconArr[i]->clear();
        state.seats[i].carteScoperte = false;
    }

    stackedWidget->setCurrentIndex(1);
    showTable();
    showHands();
    statoAttualeBottone = FaseSmazzata;
    updateOverlay(QString("%1 è il nuovo Mazziere!").arg(state.seats[state.dealerIndex].name), "Inizia la smazzata", true);
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
    QVector<Carta> &mano = state.seats[playerIndex].hand; // Usa riferimento per modificare

    // outputArea->append(QString("DEBUG: Confronto mano %1 | Valori: %2, %3, %4")
    //                        .arg(state.seats[playerIndex].name)
    //                        .arg(mano[0].faceValue)
    //                        .arg(mano[1].faceValue)
    //                        .arg(mano[2].faceValue));

    if (state.seats[playerIndex].carteScoperte || mano.size() != 3)
        return false;

    if (contieneMatta(mano))
    {
        int indexMatta = -1;
        int indexCarta1 = -1;
        int indexCarta2 = -1;

        // Identifica gli indici
        for (int i = 0; i < 3; ++i)
        {
            if (mano[i].id == 16)
                indexMatta = i;
            else if (indexCarta1 == -1)
                indexCarta1 = i;
            else
                indexCarta2 = i;
        }

        // Verifica se le due carte rimaste sono uguali
        if (mano[indexCarta1].faceValue == mano[indexCarta2].faceValue)
        {
            // La matta assume il valore delle altre due
            mano[indexMatta].faceValue = mano[indexCarta1].faceValue;
            return true;
        }
    }
    else if (all_cards_same(mano))
    {
        return true;
    }

    return false;
}

bool Cirulla::checkBuonaDaTre(int playerIndex)
{

    QVector<Carta> &carteInMano = state.seats[playerIndex].hand;
    // Safety check: non eseguire se non siamo nelle condizioni giuste
    if (state.seats[playerIndex].carteScoperte)
    {
        return false;
    }
    bool mattaPresente = contieneMatta(carteInMano);
    int total = cards_sum(carteInMano);
    if (mattaPresente && total <= 15)
    {
        int indexMatta = -1;
        for (int i = 0; i < carteInMano.size(); ++i)
            if (carteInMano[i].id == 16)
                indexMatta = i;
        int valoreScelto;

        if (playerIndex == config.humanSeatIndex)
        {
            do
            {
                valoreScelto = QInputDialog::getInt(nullptr, "Buona da tre",
                                                    "Scegli un valore per la matta (1-6) tale che la somma sia <= 9:",
                                                    1, 1, 6, 1);

                carteInMano[indexMatta].faceValue = valoreScelto;

            } while (cards_sum(carteInMano) > 9);
        }
        else
        {

            carteInMano[indexMatta].faceValue = 1;
            return true;
        }
    }

    // caso semplice senza matta
    else if (total <= 9)
        return true;

    return false;
}

void Cirulla::applyBuonaDaDieci(int playerIndex)
{

    state.seats[playerIndex].carteScoperte = true;
    state.seats[playerIndex].totaleScope += 10;
    aggiornaStats(playerIndex);
    showHands();
    isWaitingForBuona = true;
}

void Cirulla::applyBuonaDaTre(int playerIndex)
{
    outputArea->append("--- START BUONA DA TRE ---");
    outputArea->append("Giocatore che accusa:" + state.seats[playerIndex].name);
    outputArea->append("Carte in mano (prima della modifica):");
    for (auto &c : state.seats[playerIndex].hand)
    {
        outputArea->append(QString("ID: %1 Valore: %2").arg(c.id).arg(c.faceValue));
    }

    if (playerIndex < 0 || playerIndex >= state.seats.size())
    {
        outputArea->append("ERRORE CRITICO: Indice giocatore non valido!");
        return;
    }
    {
        state.seats[playerIndex].carteScoperte = true;
        state.seats[playerIndex].totaleScope += 3;
        aggiornaStats(playerIndex);
        showHands();
        isWaitingForBuona = true;
    }
}

void Cirulla::aggiornaStats(int playerIndex)
{
    // fprintf(stderr, "DEBUG: playerIndex: %d\n", playerIndex);
    // fprintf(stderr, "Size config.players: %zu\n", (size_t)config.players.size());
    // fprintf(stderr, "Size state.seats: %zu\n", (size_t)state.seats.size());
    // fflush(stderr);
    mazzoTextArr[playerIndex]->setText(config.players[playerIndex].name +
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
    int limite = config.giocoACoppie ? 2 : 4;

    for (int i = 0; i < limite; i++)
    {
        PlayerState &giocatore = state.seats[i];

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
    for (int i = 0; i < limite; i++)
    {
        PlayerState &giocatore = state.seats[i];
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

void Cirulla::updateOverlay(const QString &text, const QString &buttonText, bool buttonEnabled)
{
    // Aggiorna il testo
    infoLabel->setText(text);

    // Abilita o disabilita il bottone
    button->setEnabled(buttonEnabled);
    button->setText(buttonText);

    // Opzionale: cambia visibilità se necessario
    if (!buttonEnabled)
    {
        button->hide(); // Nasconde se non serve
    }
    else
    {
        button->show(); // Mostra se serve
    }
}

void Cirulla::onGlobalOverlayClicked()
{
    updateOverlay("", "", false);

    switch (statoAttualeBottone)
    {
    case FaseAvvio:
        // Logica per avviare la partita
        startGame();
        break;

    case FaseDopoBuona:
        // outputArea->append("DEBUG: Buona confermata, riprendiamo il turno.");
        isWaitingForBuona = false;

        // 1. Aggiorna il turno

        // 2. Aggiorna la vista
        showHands();

        // 3. NON chiamare processTurn()!
        // Lascia che sia il giocatore (o il timer del bot) a innescare la mossa
        // tramite un click o la pressione di un tasto.
        // Se è il turno del bot, usa un timer per farlo giocare tra un secondo.
        // Sostituisci la vecchia chiamata al timer del bot con questa:
        if (state.seats[state.currentTurnIndex].type == SeatType::Bot)
        {
            QTimer::singleShot(1000, this, [this]()
                               {
        if (state.phase == MatchPhase::Playing) {
            this->processTurn();
        } });
        }
        break;

    case FaseSmazzata:
        executeDeal();
        break;

    case DopoScopaMazziere:
        processTurn();
        break;

    case FaseFineTurno:
        // Logica per passare al prossimo giocatore
        continueGame();
        break;
    case RitornoHomeScreen:
        mainScreen();
        break;
    }
}

void Cirulla::resetValoreMatta()
{
    // Resetta nel mazzo (se necessario)
    for (auto &carta : state.deck)
    {
        if (carta.id == 16)
            carta.faceValue = 7;
    }

    // Resetta negli hands, prese e scope di tutti i giocatori
    for (int i = 0; i < state.seats.size(); ++i)
    {
        for (auto &carta : state.seats[i].hand)
            if (carta.id == 16)
                carta.faceValue = 7;
        for (auto &carta : state.seats[i].prese)
            if (carta.id == 16)
                carta.faceValue = 7;
        for (auto &carta : state.seats[i].scope)
            if (carta.id == 16)
                carta.faceValue = 7;
    }
}

void Cirulla::clearScopeAndPreseLabels()
{
    for (int i = 0; i < 4; i++)
    {
        mazzoPreseIconArr[i]->clear();
        mazzoScopeIconArr[i]->clear();
    }
}

void Cirulla::hardReset()
{
    state = GameState();
    config = GameConfig();
}

Cirulla::~Cirulla()
{
}
