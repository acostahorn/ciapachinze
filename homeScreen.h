#ifndef HOMESCREEN_H
#define HOMESCREEN_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>

class HomeScreen : public QWidget
{
    Q_OBJECT
public:
    explicit HomeScreen(QWidget *parent = nullptr) : QWidget(parent)
    {

        QVBoxLayout *layout = new QVBoxLayout(this);
        QPushButton *btnStart = new QPushButton("Inizia Partita", this);
        layout->addWidget(btnStart);

        // Connessione di prova per testare il cambio schermata
        connect(btnStart, &QPushButton::clicked, this, &HomeScreen::startRequested);
    }

signals:
    void startRequested();
};

#endif // HOMESCREEN_H