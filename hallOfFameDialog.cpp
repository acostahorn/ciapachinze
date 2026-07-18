#include "hallOfFameDialog.h"
#include "CharacterManager.h"

HallOfFameDialog::HallOfFameDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Hall of Fame");
    resize(400, 300);

    QVBoxLayout *layout = new QVBoxLayout(this);
    QTableWidget *table = new QTableWidget(0, 3, this);

    table->setHorizontalHeaderLabels({"Nome", "Partite", "Win Rate"});

    // Popolamento
    auto players = CharacterManager::getAllPlayers();
    table->setRowCount(players.size());

    for (int i = 0; i < players.size(); ++i)
    {
        table->setItem(i, 0, new QTableWidgetItem(players[i].name));
        table->setItem(i, 1, new QTableWidgetItem(QString::number(players[i].playedMatches)));
        table->setItem(i, 2, new QTableWidgetItem(QString::number(players[i].getWinRate(), 'f', 1) + "%"));
    }

    layout->addWidget(table);
}