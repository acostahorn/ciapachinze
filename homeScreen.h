#ifndef HOMESCREEN_H
#define HOMESCREEN_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include "GameTypes.h"

class HomeScreen : public QWidget
{
    Q_OBJECT
public:
    explicit HomeScreen(QWidget *parent = nullptr);

    QString getPlayerName() const;

signals:
    void startRequested(QString nomeGiocatoreUmano);

private:
    QLineEdit *nameEdit;
    QPushButton *btnStart;

    void handleStartGame();
};

#endif // HOMESCREEN_H