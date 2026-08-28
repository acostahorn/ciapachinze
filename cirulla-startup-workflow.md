# Cirulla startup workflow

This file focuses on the constructor `Cirulla::Cirulla` in `cirulla.cpp` and the first chain of calls that launch a game.

```mermaid
flowchart TD
    A[Application creates Cirulla instance<br/>Cirulla::Cirulla(QWidget *parent)] --> A1[Set window size + base style]
    A1 --> A2[Create stackedWidget + HomeScreen]
    A2 --> A3[Build game/score UI containers<br/>player0Container, player1Container, tableLayout, handLayout, scoreArea]
    A3 --> A4[Initialize overlay button + labels]
    A4 --> A5[connect(button, clicked, onGlobalOverlayClicked)]
    A5 --> A6[connect(homeScreen, startRequested, lambda)]

    A6 --> B1[lambda on startRequested
      - stackedWidget.setCurrentIndex(1)
      - updateOverlay("CIRULLA", "Inizia il Gioco", true)
      - setupGame(getPlayers(p), giocoACoppie)]

    B1 --> C1[setupGame(players, giocoACoppie)]
    C1 --> C2[hardReset()]
    C2 --> C3[Clear state.seats + config.players + state.playedCards]
    C3 --> C4[Set config.mode, config.humanSeatIndex, players, couple flag]
    C4 --> C5[Build PlayerState list for each seat
      - assign type: Human or Bot
      - assign avatar/name
      - aggiornaStats(i)]

    C5 --> D1[User presses overlay button]
    D1 --> D2[onGlobalOverlayClicked()]
    D2 --> D3{statoAttualeBottone}

    D3 -->|FaseAvvio| E1[startGame()]
    E1 --> E2[state.deck = generateShuffledDeck()]
    E2 --> E3[state.dealerIndex = selectDealer()]
    E3 --> E4[currentGamePhase = STATE_READY_TO_START]
    E4 --> E5[updateOverlay("<dealer> è il nuovo Mazziere!", "Inizia la smazzata", true)]

    D3 -->|FaseSmazzata| F1[executeDeal()]
    F1 --> F2[Reset moods + safety check]
    F2 --> F3[if isTestMode -> generateTestDeck() else generateShuffledDeck()]
    F3 --> F4[state.currentTurnIndex = (dealer+1) % 4]
    F4 --> F5[initialDeal()]
    F5 --> F6[Create 3 cards for each player + 4 table cards]
    F6 --> F7[showHandsAfterDeal(onComplete)]
    F7 --> F8[showTable()]
    F8 --> F9[dealersChance()]
    F9 --> F10[processTurn()]

    F10 --> G1{current player is human or bot?}
    G1 -->|human, not botGame| G2[enableHandInteraction(true)]
    G1 -->|bot or botGame| G3[enableHandInteraction(false)]
    G3 --> G4[QTimer::singleShot(waitTime, botPlay)]
    G4 --> H1[botPlay()]
    H1 --> H2[Enumerate valid moves: takes + discards]
    H2 --> H3[Evaluate tactical ranking]
    H3 --> H4[Pick best move]
    H4 --> H5[executeBotMove(bestMossa)]
    H5 --> H6[QTimer::singleShot(waitTime, playCard)]
    H6 --> H7[playCard(handIndex, tableIndices)]
    H7 --> H8[makeMove(...)]
    H8 --> H9[updatePlayerUI()]
    H9 --> H10[Advance turn or end hand]
    H10 --> F10

    G2 --> I1[Human interaction path
      - eventFilter handles clicks on hand/table cards
      - validateAndPlay() checks legality
      - playCard() executes move
      - processTurn() resumes flow]

    D3 -->|FaseDopoBuona| J1[onGlobalOverlayClicked() after good declaration]
    J1 --> J2[isWaitingForBuona = false]
    J2 --> J3[showHands()]
    J3 --> J4[If bot, queue processTurn()]

    D3 -->|FaseFineTurno| K1[continueGame()]
    K1 --> K2[Advance dealer, clear current hand data, show table + hands]
    K2 --> K3[updateOverlay("<new dealer>", "Inizia la smazzata", true)]

    D3 -->|RitornoHomeScreen| L1[mainScreen()]
    L1 --> L2[update stats + clear overlay + return to home screen]
```

## Startup interpretation

The constructor is not the `main()` function; it is the application window object that initializes the game screen, the overlay, and the event connections. The critical startup chain is:

1. `Cirulla::Cirulla(...)` builds the UI.
2. `connect(homeScreen, &HomeScreen::startRequested, ...)` captures the start signal from the home screen.
3. The lambda calls `setupGame(getPlayers(p), giocoACoppie)`.
4. Once the overlay button is pressed, `onGlobalOverlayClicked()` sees `statoAttualeBottone == FaseAvvio` and calls `startGame()`.
5. `startGame()` chooses the dealer and then `executeDeal()` initializes the actual hand.
6. `processTurn()` decides whether the next player is human or bot.
7. Bots call `botPlay()`, humans rely on UI events (
   `eventFilter`, `validateAndPlay`, `playCard`).

This is the entry path you want to automate for a test mode: the human UI interactions must be replaced by calls to the same internal methods that the bot logic already uses.
