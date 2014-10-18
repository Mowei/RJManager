#ifndef RJMANAGER_H
#define RJMANAGER_H

#include <QMainWindow>
#include <QByteArray>
#include <QString>
#include <QClipboard>
#include <QTreeWidget>
#include <QFileInfo>
#include <QSqlDatabase>


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
    //Tab2
    void listFile(QTreeWidgetItem *parentWidgetItem, QFileInfo &parent);


private slots:
    void on_pushButton_clicked();

    void on_copyFileNameButton_clicked();

    void on_copyUrlButton_clicked();

    void on_pushButton_3_clicked();

    void on_treeWidget_expanded(const QModelIndex &index);

    void on_pushButton_4_clicked();

private:
    Ui::RJManager *ui;

private:
    //QByteArray tempfile;
    QString groupName;
    QString RJNumber;
    QString saleDate;
    QString gameName;

    QClipboard *clipboard;

    QSqlDatabase db;
};

#endif // RJMANAGER_H
