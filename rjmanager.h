#ifndef RJMANAGER_H
#define RJMANAGER_H

#include <QMainWindow>
#include <QByteArray>
#include <QString>
#include <QClipboard>

namespace Ui {
class RJManager;
}

class RJManager : public QMainWindow
{
    Q_OBJECT

public:
    explicit RJManager(QWidget *parent = 0);
    ~RJManager();

    QByteArray CurlDownload(QString srcUrl);
    void DlsitePageAnalysis(QString name);


private slots:
    void on_pushButton_clicked();

    void on_copyFileNameButton_clicked();

    void on_copyUrlButton_clicked();

private:
    Ui::RJManager *ui;

private:
    //QByteArray tempfile;
    QString groupName;
    QString RJNumber;
    QString saleDate;
    QString gameName;

    QClipboard *clipboard;
};

#endif // RJMANAGER_H
