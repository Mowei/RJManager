#ifndef RJMANAGER_H
#define RJMANAGER_H

#include <QMainWindow>

namespace Ui {
class RJManager;
}

class RJManager : public QMainWindow
{
    Q_OBJECT

public:
    explicit RJManager(QWidget *parent = 0);
    ~RJManager();

private:
    Ui::RJManager *ui;
};

#endif // RJMANAGER_H
