#ifndef HALLOFFAMEDIALOG_H
#define HALLOFFAMEDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QLabel>

class HallOfFameDialog : public QDialog
{
    Q_OBJECT

private:
    QLabel *titleLabel;

public:
    explicit HallOfFameDialog(QWidget *parent = nullptr);
};

#endif