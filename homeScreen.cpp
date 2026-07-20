#include "homeScreen.h"
#include "CharacterManager.h"
#include "hallOfFameDialog.h"
#include "RegoleDialog.h"
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
                  "}"
                  "QPushButton { "
                  " font-size: 20px; "
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

    // --- Bottoni ---
    btnStart = new QPushButton("Inizia Partita", this);
    btnStart->setFixedWidth(200);

    QHBoxLayout *navLayout = new QHBoxLayout();
    QPushButton *btnHallOfFame = new QPushButton("Classifica", this);
    QPushButton *btnRules = new QPushButton("Regole", this);

    navLayout->addWidget(btnHallOfFame);
    navLayout->addWidget(btnRules);

    // --- Configurazione finale del mainLayout ---
    mainLayout->addStretch(); // Spinge tutto giù

    mainLayout->addLayout(inputLayout);

    // Aggiungiamo un piccolo spazio tra input e bottone
    mainLayout->addSpacing(20);

    mainLayout->addWidget(btnStart, 0, Qt::AlignCenter);

    // Aggiungiamo un altro spazio prima della barra di navigazione
    mainLayout->addSpacing(20);

    mainLayout->addStretch(); // Spinge tutto su (il doppio stretch bilancia lo spazio)

    mainLayout->addLayout(navLayout);

    connect(btnStart, &QPushButton::clicked, this, [this]()
            {

    QString nome = nameEdit->text().trimmed();
    handleStartGame(); });
    connect(btnHallOfFame, &QPushButton::clicked, this, [this]()
            {
                ::HallOfFameDialog dlg(this);
                dlg.exec(); });
    connect(btnRules, &QPushButton::clicked, this, [this]()
            {
                ::RegoleDialog dlg(this);
                dlg.exec(); });
}

void HomeScreen::handleStartGame()
{
    QString nome = nameEdit->text().trimmed();
    if (nome.isEmpty())
        return;

    // 1. Logica di controllo (chiamando il Manager)
    if (!CharacterManager::playerExists(nome))
    {

        QString avatarPath = QFileDialog::getOpenFileName(this, "Seleziona Avatar");

        if (avatarPath.isEmpty())
        {
            return;
        }

        CharacterManager::createNewPlayer(nome, avatarPath);
    }

    emit startRequested(nome);
}
