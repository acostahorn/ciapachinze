#include "cirulla.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QFont>
#include <QMessageBox>
#include <iostream>
#include <QDebug>
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

    this->setMinimumSize(800, 600);
    this->resize(1000, 800);

    // 1. INITIALIZE ALL CONTAINERS
    // Top Player (centered)
    player0Container = new QWidget(this);
    QHBoxLayout *player0Layout = new QHBoxLayout(player0Container);
    player0Layout->setAlignment(Qt::AlignCenter);
    player0Container->setMinimumSize(100, 150);

    // Side Players

    player1Container = new QWidget(this);
    setupPlayerDashboard(player1Container, Qt::Vertical);

    player3Container = new QWidget(this);
    setupPlayerDashboard(player3Container, Qt::Vertical);
    player1Container->setMinimumSize(100, 300);
    player3Container->setMinimumSize(100, 300);

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

    // 4. Top Row (Player 0 centered)
    mainLayout->addWidget(player0Container);

    // 5. Middle Zone (Using GridLayout for better table control)
    QGridLayout *middleGrid = new QGridLayout();

    // Add side players to the sides
    middleGrid->addWidget(player3Container, 0, 0); // Left
    middleGrid->addWidget(player1Container, 0, 2); // Right

    // Table
    QGroupBox *tableContainer = new QGroupBox("CARTE IN TAVOLO", this);
    tableContainer->setStyleSheet(
        "QGroupBox { background-color: #1e6b36; border: 2px solid #144d26; border-radius: 8px; margin-top: 12px; font-weight: bold; color: #ffffff; }"
        "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; padding: 0 5px; color: #2c3e50; }");
    tableLayout = new QGridLayout(tableContainer);
    tableLayout->setAlignment(Qt::AlignCenter);

    // Add table to center column and make it stretch
    middleGrid->addWidget(tableContainer, 0, 1, 1, 1);
    middleGrid->setColumnStretch(1, 5); // Table gets 5x the width of side columns

    mainLayout->addLayout(middleGrid);

    // 6. Bottom Zone (Your hand)
    QGroupBox *handContainer = new QGroupBox("LA TUA MANO", this);
    handContainer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    handContainer->setStyleSheet("QGroupBox { font-weight: bold; color: #27ae60; }");

    // Layout principale dentro il GroupBox
    QHBoxLayout *handZoneLayout = new QHBoxLayout(handContainer);

    // A. Zona Statistiche (a sinistra)
    QVBoxLayout *statsLayout = new QVBoxLayout();
    mazzoPreseLabel = new QLabel("Prese: 0", this);
    mazzoScopeLabel = new QLabel("Scope: 0", this);
    statsLayout->addWidget(mazzoPreseLabel);
    statsLayout->addWidget(mazzoScopeLabel);
    statsLayout->setAlignment(Qt::AlignCenter); // Opzionale, per centrare i testi

    // B. Zona Carte (a destra)
    // NON crearlo con 'handContainer' come genitore, perché lo aggiungeremo al layout
    handLayout = new QHBoxLayout();
    handLayout->setAlignment(Qt::AlignCenter);

    // C. Assemblaggio
    handZoneLayout->addLayout(statsLayout); // Aggiunge le stats
    handZoneLayout->addStretch();
    handZoneLayout->addLayout(handLayout); // Aggiunge le carte
    handZoneLayout->addStretch();

    // Infine, aggiungi il contenitore al layout principale della finestra
    mainLayout->addWidget(handContainer);

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
        cardLabel->installEventFilter(this);
        bool isMe = (p.id == config.humanSeatIndex);

        cardLabel->setProperty("cardIndex", i);
        cardLabel->setProperty("type", "hand");
        cardLabel->setProperty("isClickable", isMe);

        QString path;
        if (isMe)
        {
            // Show the actual card
            path = QString("cards/%1.png").arg(card.id + 1);
            if (i == selectedHandCardIndex)
            {
                cardLabel->setStyleSheet("border: 3px solid blue");
            }
            else
            {
                cardLabel->setStyleSheet("");
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

void Cirulla::setupPlayerDashboard(QWidget *container, Qt::Orientation orientation)
{
    QVBoxLayout *layout = new QVBoxLayout(container);
    layout->setContentsMargins(5, 5, 5, 5);

    // Area "Prese" (es. una Label per il numero e una per l'icona)
    QLabel *preseLabel = new QLabel("Prese: 0", container);
    preseLabel->setAlignment(Qt::AlignCenter);

    // Area "Mano" (un piccolo layout dedicato per le carte coperte)
    QHBoxLayout *handArea = new QHBoxLayout();

    layout->addWidget(preseLabel);
    layout->addLayout(handArea);
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
    makeMove(handIndex, tableIndices);
    enableHandInteraction(false);

    const PlayerState &me = state.seats[config.humanSeatIndex];
    renderHandToLayout(me, handLayout);

    state.currentTurnIndex = (state.currentTurnIndex + 1) % state.seats.size();
    selectedHandCardIndex = -1;
    selectedTableIndices.clear();

    showTable();

    // Aggiorna le statistiche (Prese/Scope)
    // showStats();
}

void Cirulla::makeMove(int handIndex, QList<int> &tableIndices)
{
    PlayerState &giocatore = state.seats[state.currentTurnIndex];
    Carta giocata = giocatore.hand[handIndex];
    std::sort(tableIndices.begin(), tableIndices.end(), std::greater<int>());

    bool isScopa = (state.tableCards.size() == tableIndices.size());
    if (isScopa)
    {
        outputArea->append("SCOPA!");
        giocatore.scope.append(giocata);
        for (int idx : tableIndices)
        {
            giocatore.prese.append(state.tableCards[idx]);
        }
    }
    else
    {
        giocatore.prese.append(giocata);
        for (int idx : tableIndices)
        {
            giocatore.prese.append(state.tableCards[idx]);
        }
    }
    for (int i : tableIndices)
    {
        state.tableCards.removeAt(i);
    }

    giocatore.hand.removeAt(handIndex);
    mazzoPreseLabel->setText("Prese: " + QString::number(giocatore.prese.size()));
    mazzoScopeLabel->setText("Scope: " + QString::number(giocatore.scope.size()));
}

void Cirulla::botPlay()
{
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
        if (selectedHandCardIndex != -1 && !selectedTableIndices.isEmpty())
        {
            validateAndPlay();
        }
        else
        {
            qDebug() << "Seleziona prima una carta in mano e una sul tavolo!";
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

    candidati.append(trovaPresePerSomma(handIndex));
    candidati.append(trovaPresePer15(handIndex));

    return candidati;
}

QVector<Mossa> Cirulla::trovaPresePerSomma(int handIndex)
{
    PlayerState &giocatore = state.seats[state.currentTurnIndex];
    int valoreCartaInMano = giocatore.hand[handIndex].faceValue;
    QVector<Mossa> prese;
    int n = state.tableCards.size();

    for (int i = 1; i < (1 << n); ++i)
    {
        int somma = 0;
        QList<int> combo;

        for (int j = 0; j < n; ++j)
        {
            if ((i >> j) & 1)
            {
                somma += state.tableCards[j].faceValue;
                combo.append(j);
            }
        }

        if (somma == valoreCartaInMano)
        {
            prese.append({handIndex, combo});
        }
    }

    return prese;
}

QVector<Mossa> Cirulla::trovaPresePer15(int handIndex)
{
    PlayerState &giocatore = state.seats[state.currentTurnIndex];
    int valoreCartaInMano = giocatore.hand[handIndex].faceValue;
    QVector<Mossa> prese;

    int n = state.tableCards.size();

    for (int i = 1; i < (1 << n); ++i)
    {
        int somma = 0;
        QList<int> combo;

        for (int j = 0; j < n; ++j)
        {
            if ((i >> j) & 1)
            {
                somma += state.tableCards[j].faceValue;
                combo.append(j);
            }
        }

        if (somma + valoreCartaInMano == 15)
        {
            prese.append({handIndex, combo});
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

Cirulla::~Cirulla()
{
}
