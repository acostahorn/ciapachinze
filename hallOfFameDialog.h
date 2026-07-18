#ifndef HALLOFFAMEDIALOG_H
#define HALLOFFAMEDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QVBoxLayout>

class HallOfFameDialog : public QDialog
{
    Q_OBJECT
public:
    explicit HallOfFameDialog(QWidget *parent = nullptr);
};

#endif