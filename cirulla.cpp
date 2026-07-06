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

Cirulla::Cirulla(QWidget *parent) : QWidget(parent)
{

    this->setMinimumSize(1000, 900);
    this->resize(1000, 900);

    // 1. INITIALIZE ALL CONTAINERS
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

    // Per il Giocatore 3
    player3Container = new QWidget(this);
    QVBoxLayout *layout3 = new QVBoxLayout(player3Container); // Questo è NECESSARIO per evitare crash
    layout3->setContentsMargins(5, 5, 5, 5);
    player3Container->setMinimumSize(100, 400);

    // Apply size policies for all
    auto setContainerStyle = [](QWidget *container)
    {
        container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    };
    setContainerStyle(player0Container);
    setContainerStyle(player1Container);
    setContainerStyle(player3Container);

    // 2. Main Layout (Vertical)
    mainLayout = new QVBoxLayout(this);

    // 3. Output Area (Separate, full-width row)
    outputArea = new QTextEdit(this);
    outputArea->setReadOnly(true);
    outputArea->setMaximumHeight(90);
    mainLayout->addWidget(outputArea);

    QWidget *gameArea = new QWidget(this);
    QVBoxLayout *gameLayout = new QVBoxLayout(gameArea);
    gameLayout->setContentsMargins(0, 0, 0, 0); // Nessun margine extra

    // 4. Top Row (Player 0 centered)
    // 1. Il contenitore delle statistiche
    p0StatsContainer = new QWidget(this);

    QVBoxLayout *statsLayoutP0 = new QVBoxLayout(p0StatsContainer); // Assegnato qui!

    mazzoPreseP0Label = new QLabel("Prese: 0", this);
    mazzoScopeP0Label = new QLabel("Scope: 0", this);
    // 2. Aggiungi le label al layout del contenitore
    statsLayoutP0->addWidget(mazzoPreseP0Label);
    statsLayoutP0->addWidget(mazzoScopeP0Label);

    // 3. Ora p0StatsContainer è "pieno" di statistiche.
    // Possiamo metterlo nel Grid insieme al contenitore delle carte (player0Container)
    QGridLayout *middleTopGrid = new QGridLayout();
    QWidget *p0Wrapper = new QWidget(this);
    p0Wrapper->setStyleSheet("QWidget { border: 2px solid #16a9ac; border-radius: 8px; }");
    QHBoxLayout *wrapper0Layout = new QHBoxLayout(p0Wrapper);
    wrapper0Layout->setContentsMargins(5, 5, 5, 5);
    p0StatsContainer->setFixedWidth(100);
    // player0Container->setFixedHeight(200);

    wrapper0Layout->addWidget(p0StatsContainer);
    wrapper0Layout->addWidget(player0Container);
    middleTopGrid->addWidget(p0Wrapper, 0, 0);

    gameLayout->addLayout(middleTopGrid, 1);

    // 5. Middle Zone (Using GridLayout for better table control)
    QGridLayout *middleGrid = new QGridLayout();

    mazzoPreseP3Label = new QLabel("Prese: 0", this);
    mazzoScopeP3Label = new QLabel("Scope: 0", this);
    layout3->addWidget(mazzoPreseP3Label);
    layout3->addWidget(mazzoScopeP3Label);

    mazzoPreseP1Label = new QLabel("Prese: 0", this);
    mazzoScopeP1Label = new QLabel("Scope: 0", this);

    middleGrid->setColumnStretch(0, 1);
    middleGrid->setColumnStretch(1, 1);
    middleGrid->setColumnStretch(2, 10); // Il tavolo domina
    middleGrid->setColumnStretch(3, 1);
    middleGrid->setColumnStretch(4, 1);

    middleGrid->addWidget(mazzoPreseP1Label, 0, 0);
    middleGrid->addWidget(mazzoScopeP1Label, 1, 0);
    middleGrid->addWidget(mazzoPreseP3Label, 0, 4);
    middleGrid->addWidget(mazzoScopeP3Label, 1, 4);

    player1Container->setFixedWidth(100); // Scegli una misura che ti piace
    player3Container->setFixedWidth(100);

    // Add side players to the sides
    middleGrid->addWidget(player3Container, 0, 1); // Left
    middleGrid->addWidget(player1Container, 0, 3); // Right

    // Table
    QGroupBox *tableContainer = new QGroupBox();
    tableContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // Forza il widget a ignorare le dimensioni minime calcolate dai figli
    tableContainer->setMinimumSize(0, 0);
    tableContainer->setStyleSheet(
        "QGroupBox { background-color: #1e6b36; border: 2px solid #144d26; border-radius: 8px; margin-top: 12px; font-weight: bold; color: #ffffff; }"
        "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; padding: 0 5px; color: #2c3e50; }");
    tableLayout = new QGridLayout(tableContainer);
    tableLayout->setAlignment(Qt::AlignCenter);
    tableContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Add table to center column and make it stretch
    middleGrid->addWidget(tableContainer, 0, 2, 1, 1);
    middleGrid->setColumnStretch(1, 5); // Table gets 5x the width of side columns

    gameLayout->addLayout(middleGrid, 5);
    middleGrid->setRowStretch(0, 10);

    // 6. Bottom Zone (Your hand)
    p2StatsContainer = new QWidget(this);

    QGroupBox *handContainer = new QGroupBox("LA TUA MANO", this);
    handContainer->setStyleSheet(
        "QGroupBox { "
        "   border: 2px solid #16a9ac; " // Stesso bordo degli altri
        "   border-radius: 8px; "
        "   margin-top: 10px; " // Spazio per il titolo
        "   font-weight: bold; "
        "   color: #27ae60; "
        "} "
        "QGroupBox::title { "
        "   subcontrol-origin: margin; "
        "   subcontrol-position: top center; " // Titolo centrato
        "   padding: 0 5px; "
        "}");

    handContainer->setStyleSheet("QGroupBox { font-weight: bold; color: #27ae60; }");
    QHBoxLayout *handZoneLayout = new QHBoxLayout(handContainer);

    // A. Zona Statistiche
    // Se p0StatsContainer è il nome del widget che le tue funzioni si aspettano, usa quello
    p2StatsContainer->setStyleSheet("QWidget { border: 2px solid #16a9ac; border-radius: 8px; }");
    p2StatsContainer->setFixedWidth(100);
    QVBoxLayout *statsLayout = new QVBoxLayout(p2StatsContainer);

    mazzoPreseIcon = new QLabel();
    mazzoPreseText = new QLabel("Prese: 0", this);
    mazzoScopeIcon = new QLabel();
    mazzoScopeText = new QLabel("Scope: 0", this);

    statsLayout->addWidget(mazzoScopeIcon);
    statsLayout->addWidget(mazzoScopeText);
    statsLayout->addWidget(mazzoPreseIcon);
    statsLayout->addWidget(mazzoPreseText);

    // B. Zona Carte (Il trucco qui)
    QWidget *cardsWrapper = new QWidget(); // Questo serve solo per lo stile
    cardsWrapper->setStyleSheet("QWidget { border: 2px solid #16a9ac; border-radius: 8px; }");

    // cardsWrapper->setFixedHeight(200);
    // Creiamo l'handLayout dentro il wrapper, così le funzioni lo trovano
    handLayout = new QHBoxLayout(cardsWrapper);
    handLayout->setAlignment(Qt::AlignCenter);

    // C. Assemblaggio finale
    handZoneLayout->addWidget(p2StatsContainer);
    handZoneLayout->addWidget(cardsWrapper); // Aggiungiamo il wrapper, non handLayout!

    gameLayout->addWidget(handContainer, 2);
    mainLayout->addWidget(gameArea, 1);

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
    state.deck = generateShuffledDeck();
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
    config.playerNames = {"Pippo", "Ugo", "Bacci", "Pina"};
    state.seats.clear();
    for (int i = 0; i < 4; ++i)
    {
        PlayerState p;
        p.id = i;
        if (i == config.humanSeatIndex)
        {
            p.name = config.playerNames[i] + "(tu)";
            p.type = SeatType::Human;
        }
        else
        {
            p.name = config.playerNames[i];
            p.type = SeatType::Bot;
        }
        state.seats.append(p);
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
        cardLabel->setPixmap(cardImage.scaled(100, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));

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
    int total = cards_sum(state.tableCards);
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
            } else {
                state.seats[state.dealerIndex].scope.append(state.tableCards[0]);
                state.seats[state.dealerIndex].scope.append(state.tableCards[1]);
                for (int i = 2; i < state.tableCards.size(); ++i) {
                    state.seats[state.dealerIndex].prese.append(state.tableCards[i]);
                }
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
                cardLabel->setStyleSheet("border: 3px solid blue");
            }
            else
            {
                cardLabel->setStyleSheet("border: none");
            }
        }
        else
        {
            // Show the flipside image
            path = "cards/back-teal.png";
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
                outputArea->append(QString("Carta %1 (valore %2) può prendere:")
                                       .arg(i)
                                       .arg(miaMano[i].faceValue));

                for (const auto &m : mosseCarta)
                {
                    QString info;
                    for (int idx : m.tableIndices)
                    {
                        info += QString::number(state.tableCards[idx].faceValue) + " ";
                    }
                    outputArea->append("  > con: " + info);
                }
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
        QTimer::singleShot(1000, this, &Cirulla::botPlay); // Delay per realismo
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
            return;
        }
    }
    else
    {
        state.currentTurnIndex = (state.currentTurnIndex + 1) % state.seats.size();
    }
    //  Prossima mossa
    if (isCurrentPlayerBot())
    {
        enableHandInteraction(false);
        QTimer::singleShot(1000, this, &Cirulla::botPlay);
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
            giocatore.scope.append(giocata); // La carta che ha fatto scopa va nelle scope
            if (state.currentTurnIndex == 2)
            {
                aggiornaMazzoScope(giocatore, giocata);
            }
        }
        else
        {
            giocatore.prese.append(giocata); // La carta che ha preso va nelle prese
            if (state.currentTurnIndex == 2)
            {
                aggiornaMazzoPrese(giocatore);
            }
        }

        // Aggiungi le carte del tavolo alle prese
        for (int idx : tableIndices)
        {
            giocatore.prese.append(state.tableCards[idx]);
            if (state.currentTurnIndex == 2)
            {
                aggiornaMazzoPrese(giocatore);
            }
        }

        // Rimuovi le carte dal tavolo
        for (int i : tableIndices)
        {
            state.tableCards.removeAt(i);
        }
    }

    if (state.currentTurnIndex == 0)
    {
        mazzoPreseP0Label->setText(QString("Prese: %1").arg(giocatore.prese.size()));
        mazzoScopeP0Label->setText(QString("Scope: %1").arg(giocatore.scope.size()));
    }
    if (state.currentTurnIndex == 1)
    {
        mazzoPreseP1Label->setText(QString("Prese: %1").arg(giocatore.prese.size()));
        mazzoScopeP1Label->setText(QString("Scope: %1").arg(giocatore.scope.size()));
    }
    if (state.currentTurnIndex == 3)
    {
        mazzoPreseP3Label->setText(QString("Prese: %1").arg(giocatore.prese.size()));
        mazzoScopeP3Label->setText(QString("Scope: %1").arg(giocatore.scope.size()));
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

    QTimer::singleShot(1000, this, [=]()
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
            // mazzoPreseLabel->setText("Prese: " + QString::number(p.prese.size()));
            // mazzoScopeLabel->setText("Scope: " + QString::number(p.scope.size()));
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
    outputArea->append("Le carte rimaste sul tavolo vanno all'ultimo giocatore che ha preso");

    PlayerState &ultimoAPrendere = state.seats[lastPlayerToScore];

    for (auto &card : state.tableCards)
    {
        ultimoAPrendere.prese.append(card);
    }

    state.tableCards.clear();

    showTable();
}

void Cirulla::aggiornaMazzoPrese(PlayerState &p)
{
    outputArea->append("aggiunta carta a mazzo prese");

    QPixmap retro("cards/back-teal.png");
    mazzoPreseIcon->setPixmap(retro.scaled(50, 75, Qt::KeepAspectRatio));
    mazzoPreseText->setText("Prese: " + QString::number(p.prese.size()));
}

void Cirulla::aggiornaMazzoScope(PlayerState &p, Carta &c)
{
    outputArea->append("aggiunta carta a mazzo scope");

    QString cartaIconPath = QString("cards/%1.png").arg(c.id + 1);

    QPixmap cartaIcon(cartaIconPath);
    mazzoScopeIcon->setPixmap(cartaIcon.scaled(50, 75, Qt::KeepAspectRatio));
    mazzoScopeText->setText("Scope: " + QString::number(p.scope.size()));
}

Cirulla::~Cirulla()
{
}
