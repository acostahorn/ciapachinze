#include "hallOfFameDialog.h"
#include "RegoleDialog.h"
#include "CharacterManager.h"
#include <QHeaderView>
#include <QFile>

RegoleDialog::RegoleDialog(QWidget *parent) : QDialog(parent)
{
    QFile file("cirulla-rules.html");
    this->setObjectName("RulesWindow");

    // Invece di QWidget, usa il nome dell'oggetto con il # e forza la proprietà
    QString css = "QWidget#RulesWindow { "

                  "  background-position: center; "
                  "  background-repeat: no-repeat; "
                  "  background-color: #032633; "
                  "  font-size: 18px"
                  "}";
    this->setStyleSheet(css);

    setWindowTitle("Regole della Cirulla");
    resize(1000, 800);

    QVBoxLayout *layout = new QVBoxLayout(this);
    QTextEdit *textDisplay = new QTextEdit(this);
    QFont font = textDisplay->font();
    font.setPointSize(16); // Oppure setPixelSize(18);
    textDisplay->setFont(font);

    textDisplay->setReadOnly(true);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        textDisplay->setHtml(file.readAll());
    }
    layout->addWidget(textDisplay);
}
