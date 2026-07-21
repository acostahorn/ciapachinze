#ifndef HOMESCREEN_H
#define HOMESCREEN_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include "GameTypes.h"

class HomeScreen : public QWidget
{
    Q_OBJECT
public:
    explicit HomeScreen(QWidget *parent = nullptr);

    bool isCoupleMode() const;

signals:
    void startRequested(QString nomeGiocatoreUmano);

private:
    QLineEdit *nameEdit;
    QPushButton *btnStart;
    QPushButton *btnHallOfFame;
    QComboBox *gameModeSelector;
    QDialog *HallOfFameDialog;

    void handleStartGame();
};

#endif // HOMESCREEN_H