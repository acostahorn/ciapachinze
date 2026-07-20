#include "hallOfFameDialog.h"
#include "CharacterManager.h"
#include <QHeaderView>

HallOfFameDialog::HallOfFameDialog(QWidget *parent) : QDialog(parent)
{
    this->setObjectName("HallWindow");

    // Invece di QWidget, usa il nome dell'oggetto con il # e forza la proprietà
    QString css = "QWidget#HallWindow { "
                  "  background-image: url('pictures/cirulla-game-image.png'); "
                  "  background-position: center; "
                  "  background-repeat: no-repeat; "
                  "  background-color: transparent; "
                  "}";
    this->setStyleSheet(css);

    setWindowTitle("Hall of Fame");
    resize(800, 600);

    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel("HALL OF FAME", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 70px; font-family: 'Montserrat', 'Roboto', sans-serif; font-weight: 700; color: #fde3ef; margin: 10px;");
    layout->addWidget(titleLabel);

    QTableWidget *table = new QTableWidget(0, 4, this);

    table->setStyleSheet("background-color: rgba(0, 0, 0, 150); " // Sfondo semi-trasparente
                         "color: white; font-size: 16px;");

    table->verticalHeader()->setVisible(false);
    table->verticalHeader()->setDefaultSectionSize(60);
    table->setIconSize(QSize(50, 50));                                     //
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // Si adatta da solo

    layout->addWidget(table);

    table->setHorizontalHeaderLabels({"Posizione", "Nome", "Partite", "Win Rate"});

    auto players = CharacterManager::getAllPlayers();

    QVector<ProfileData> sortedPlayers = players;

    // int maxIndex;
    // ProfileData swap;

    // for (int i = 0; i < sortedPlayers.size(); ++i)
    // {
    //     maxIndex = i;
    //     for (int j = i + 1; j < sortedPlayers.size(); ++j)
    //     {
    //         if (sortedPlayers[j].getWinRate() > sortedPlayers[maxIndex].getWinRate())
    //         {
    //             maxIndex = j;
    //         }
    //     }
    //     if (maxIndex > i)
    //     {

    //         swap = sortedPlayers[i];
    //         sortedPlayers[i] = sortedPlayers[maxIndex];
    //         sortedPlayers[maxIndex] = swap;
    //     }
    // }

    std::sort(sortedPlayers.begin(), sortedPlayers.end(), [](const ProfileData &a, const ProfileData &b)
              { return a.getWinRate() > b.getWinRate(); });

    table->setRowCount(sortedPlayers.size());

    int rank = 1;
    for (int i = 0; i < sortedPlayers.size(); ++i)
    {
        if (i > 0 && sortedPlayers[i].getWinRate() < sortedPlayers[i - 1].getWinRate())
            ++rank;

        QTableWidgetItem *nameItem = new QTableWidgetItem(sortedPlayers[i].name);
        QTableWidgetItem *rankItem = new QTableWidgetItem(QString::number(rank));
        QTableWidgetItem *matchesItem = new QTableWidgetItem(QString::number(sortedPlayers[i].playedMatches));
        QTableWidgetItem *winRateItem = new QTableWidgetItem(QString::number(sortedPlayers[i].getWinRate(), 'f', 1) + "%");

        // 2. Carica l'immagine (usa il path che hai salvato in ProfileData)
        // Assicurati che avatarPath sia il percorso corretto rispetto all'eseguibile
        QPixmap avatar(sortedPlayers[i].avatarPath);

        // 3. Imposta l'icona (che viene usata come thumbnail)
        // Scaliamo l'immagine a 32x32 per farla stare bene nella riga
        nameItem->setIcon(QIcon(avatar.scaled(40, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation)));

        if (rank == 1)
        {
            QColor goldColor("#FFD700"); // Oro
            rankItem->setForeground(goldColor);
            nameItem->setForeground(goldColor);
            matchesItem->setForeground(goldColor);
            winRateItem->setForeground(goldColor);

            // Opzionale: rendiamolo anche in grassetto
            QFont font = nameItem->font();
            font.setBold(true);
            nameItem->setFont(font);
        }

        table->setItem(i, 1, nameItem);

        table->setItem(i, 0, new QTableWidgetItem(QString::number(rank)));
        // table->setItem(i, 1, new QTableWidgetItem(sortedPlayers[i].name));
        table->setItem(i, 2, new QTableWidgetItem(QString::number(sortedPlayers[i].playedMatches)));
        table->setItem(i, 3, new QTableWidgetItem(QString::number(sortedPlayers[i].getWinRate(), 'f', 1) + "%"));
    }
}

// for (int i = 0; i < sortedPlayers.size(); ++i)
// {
//     maxIndex = i;
//     for (int j = i + 1; j < sortedPlayers.size(); ++j)
//     {
//         if (sortedPlayers[j].getWinRate() > sortedPlayers[maxIndex].getWinRate())
//         {
//             maxIndex = j;
//         }
//     }
//     if (maxIndex > i)
//     {

//         swap = sortedPlayers[i];
//         sortedPlayers[i] = sortedPlayers[maxIndex];
//         sortedPlayers[maxIndex] = swap;
//     }
// }