#include "homeScreen.h"
#include "CharacterManager.h"
#include "hallOfFameDialog.h"
#include <QLabel>
#include <QFont>
#include <QFile>
#include <QFileDialog>

HomeScreen::HomeScreen(QWidget *parent) : QWidget(parent)
{
    this->setObjectName("HomeWindow");

    // Invece di QWidget, usa il nome dell'oggetto con il # e forza la proprietà
    QString css = "QWidget#HomeWindow { "
                  "  background-image: url('pictures/cirulla-game-image.png'); "
                  "  background-position: center; "
                  "  background-repeat: no-repeat; "
                  "  background-color: transparent; " // Forza la trasparenza rispetto al padre
                  "}";

    this->setStyleSheet(css);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Titolo
    QLabel *titleLabel = new QLabel("C++RULLA", this);
    QLabel *subtitleLabel = new QLabel("Il gioco di carte genovese", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-family: 'Montserrat', 'Roboto', sans-serif; "
                              "font-size: 100px; "
                              "font-weight: 800; "
                              "color: #d7c73e;");
    subtitleLabel->setStyleSheet("font-family: 'Montserrat', 'Roboto', sans-serif; "
                                 "font-size: 50px; "
                                 "font-weight: 800; "
                                 "color: #6a6960;");

    QVBoxLayout *inputLayout = new QVBoxLayout();
    nameEdit = new QLineEdit(this);
    nameEdit->setFixedWidth(300);
    nameEdit->setPlaceholderText("Es: Baciccia");

    inputLayout->addWidget(titleLabel, 0, Qt::AlignCenter);
    inputLayout->addWidget(subtitleLabel, 0, Qt::AlignCenter);
    inputLayout->addWidget(new QLabel("Inserisci nome giocatore:"), 0, Qt::AlignCenter);
    inputLayout->addWidget(nameEdit, 0, Qt::AlignCenter);
    inputLayout->setAlignment(Qt::AlignCenter);

    // --- Bottone ---
    btnStart = new QPushButton("Inizia Partita", this);
    btnStart->setFixedWidth(200);
    QPushButton *btnHallOfFame = new QPushButton("Classifica", this);

    mainLayout->addStretch();
    mainLayout->addLayout(inputLayout);
    mainLayout->addWidget(btnStart, 0, Qt::AlignCenter);
    mainLayout->addStretch();
    mainLayout->addWidget(btnHallOfFame, 0, Qt::AlignCenter);

    connect(btnStart, &QPushButton::clicked, this, [this]()
            {

    QString nome = nameEdit->text().trimmed();
    handleStartGame(); });
    connect(btnHallOfFame, &QPushButton::clicked, this, [this]()
            {
                ::HallOfFameDialog dlg(this);
                dlg.exec(); });
}

void HomeScreen::handleStartGame()
{
    QString nome = nameEdit->text().trimmed();
    if (nome.isEmpty())
        return; // Non fare nulla se il nome è vuoto

    // 1. Logica di controllo (chiamando il Manager)
    if (!CharacterManager::playerExists(nome))
    {

        // 2. Logica di UI (apertura dialogo)
        QString avatarPath = QFileDialog::getOpenFileName(this, "Seleziona Avatar");

        // 3. Se l'utente annulla, usciamo dalla funzione e NON emettiamo il segnale
        if (avatarPath.isEmpty())
        {
            return;
        }

        // 4. Se arriviamo qui, l'utente ha scelto: registriamo il nuovo giocatore
        CharacterManager::createNewPlayer(nome, avatarPath);
    }

    // 5. Solo adesso, sicuri che il giocatore esiste, lanciamo il segnale per avviare il gioco
    emit startRequested(nome);
}
