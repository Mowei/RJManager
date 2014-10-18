#include "rjmanager.h"
#include "ui_rjmanager.h"
#include <iostream>
#include <curl/curl.h>
#include <QRegExp>
#include <QDebug>
#include <QImageReader>
#include <QBuffer>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
size_t callbackfunction(void *ptr, size_t size, size_t nmemb, void* userdata)
{
    int len= size*nmemb;
    QByteArray *tempfile =(QByteArray*) userdata;
    tempfile->append(QByteArray((char*)ptr,nmemb));

    return len;
}

RJManager::RJManager(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RJManager)
{
    ui->setupUi(this);

    clipboard = QApplication::clipboard();

    //DB
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("data.db");
    db.open()?qDebug()<<"LOGIN!":qDebug()<<"LOGIN FAIL!";

    modeltemp = new QSqlQueryModel;
    model = new QSqlQueryModel;
    //model->setQuery("SELECT * FROM RJ");

    ui->tableView->setModel(modeltemp);
    ui->tableView_2->setModel(model);

}

RJManager::~RJManager()
{
    delete ui;
}
QByteArray RJManager::CurlDownload(QString srcUrl){
    qDebug()<<srcUrl;

    QByteArray tempfile =NULL;
    CURL* curlCtx = curl_easy_init();
    CURLcode res;

    if(curlCtx) {
        curl_easy_setopt(curlCtx, CURLOPT_URL, srcUrl.toLocal8Bit().constData());
        curl_easy_setopt(curlCtx, CURLOPT_WRITEDATA,&tempfile);
        curl_easy_setopt(curlCtx, CURLOPT_WRITEFUNCTION, callbackfunction);
        curl_easy_setopt(curlCtx, CURLOPT_FOLLOWLOCATION, 1);

        res = curl_easy_perform(curlCtx);

        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        curl_easy_cleanup(curlCtx);
    }
    return tempfile;
}
//----------------------------Tab1----------------------------//
QString RJManager::findRJ(QRegExp rx,QString src)
{
    int pos = 0;
    if((pos = rx.indexIn(src, pos)) != -1) {
        return rx.cap(1);
    }
    return NULL;
}
void RJManager::on_pushButton_clicked()
{
    QString name;
    name =ui->RJNameLineEdit->text().toUpper();
    QString RJNumber;
    QRegExp rjname("(RJ\\d\\d\\d\\d\\d\\d)");
    if((RJNumber=findRJ(rjname,name))!=NULL){
        srcUrl ="http://www.dlsite.com/maniax/work/=/product_id/"+RJNumber+".html";
        DlsitePageAnalysis(srcUrl);
    }
    ShowTab1();

}
void RJManager::DlsitePageAnalysis(QString srcUrl){

    int pos;
    groupName="";
    RJNumber="";
    saleDate="";
    gameName="";
    GJImg="";

    QString str(CurlDownload(srcUrl));

    QRegExp rjname("(RJ\\d\\d\\d\\d\\d\\d)");
    QRegExp group("<span itemprop=\"brand\">(.*)</span></a>");
    QRegExp date("\\d\\d(\\d\\d)年(\\d\\d)月(\\d\\d)日");
    QRegExp name("id=\"work_name\">(.*)</h1>");
    QRegExp mainImg("<div id=\"work_visual\" style=\"background-image: url\\((.*)\\); width: ");

    RJNumber=findRJ(rjname,str);
    groupName =findRJ(group,str);
    gameName=findRJ(name,str);
    GJImg="http:"+findRJ(mainImg,str);

    pos = 0;
    if((pos = date.indexIn(str, pos)) != -1) {
        saleDate=date.cap(1)+date.cap(2)+date.cap(3);
    }

}
void RJManager::ShowTab1()
{
    QByteArray tempfile=CurlDownload(GJImg);
    QBuffer buffer(&tempfile);
    buffer.open(QIODevice::ReadOnly);
    QImageReader reader(&buffer);
    QImage newImage = reader.read();//Copy
    newImage.save("dsfsdfsf.jpg");
    buffer.close();

    ui->label->setText(gameName);
    ui->PicLabel->setPixmap(QPixmap::fromImage(newImage));
    ui->UrlLineEdit->setText(srcUrl);
    ui->FileNameLineEdit->setText("["+groupName+"]["+saleDate+"]["+RJNumber+"]"+gameName);
}

void RJManager::on_copyFileNameButton_clicked()
{
    clipboard->setText(ui->FileNameLineEdit->text());
}

void RJManager::on_copyUrlButton_clicked()
{
    clipboard->setText(ui->UrlLineEdit->text());
}
//----------------------------Tab2----------------------------//

//http://openhome.cc/Gossip/Qt4Gossip/QTreeWidgetQTreeWidgetItem.html
void RJManager::listFile(QTreeWidgetItem *parentWidgetItem, QFileInfo &parent) {

    bool played =false;
    if(parent.fileName()=="!"){
        qDebug() <<"PLAYED";
        played =true;
    }

    QDir dir;
    dir.setPath(parent.filePath());
    dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoSymLinks);
    dir.setSorting(QDir::DirsFirst | QDir::Name);

    const QFileInfoList fileList = dir.entryInfoList();

    for (int i = 0; i < fileList.size(); i++) {
        QFileInfo fileInfo = fileList.at(i);
        QStringList fileColumn;
        fileColumn.append(fileInfo.fileName());
        fileColumn.append(fileInfo.path());
        fileColumn.append(QString::number(played));
        if (fileInfo.fileName() == "." || fileInfo.fileName() == ".." ); // nothing
        else if(fileInfo.isDir()) {
            QTreeWidgetItem *child = new QTreeWidgetItem(fileColumn);
            parentWidgetItem->addChild(child);
            // 查詢子目錄
            listFile(child, fileInfo);
        }
        else {
            QTreeWidgetItem *child = new QTreeWidgetItem(fileColumn);

            QRegExp rjname("\\[(.*)\\]\\[(.*)\\]\\[(RJ.*)\\](.*)");
            if((findRJ(rjname,fileInfo.fileName()))!=NULL){
                child->setCheckState(0, Qt::Checked);
            }else{
                child->setCheckState(0, Qt::Unchecked);
            }

            parentWidgetItem->addChild(child);
        }
    }
}
void RJManager::on_pushButton_3_clicked()
{
    ui->treeWidget->clear();
    QString directory = QFileDialog::getExistingDirectory(this,
                                                          tr("Find Files"), QDir::currentPath());

    if(directory.length() == 0) {
        QMessageBox::information(NULL, tr("Path"), tr("You didn't select any path."));
        return;
    }

    ui->dirPathLineEdit->setText(directory);
    // 設定欄位名稱
    QStringList columnTitle;
    columnTitle.append("Name");
    columnTitle.append("Path");
    columnTitle.append("Played");
    ui->treeWidget->setHeaderLabels(columnTitle);

    // 查詢的目錄
    QFileInfo fileInfo(directory);
    QStringList fileColumn;
    fileColumn.append(fileInfo.fileName());

    QTreeWidgetItem *dir = new QTreeWidgetItem(fileColumn);
    //dir->setCheckState(0, Qt::Unchecked); // 設定可核取的方塊
    ui->treeWidget->addTopLevelItem(dir);

    // 查詢目錄
    listFile(dir, fileInfo);
    //ui->treeWidget->expandAll();

}

void RJManager::on_treeWidget_expanded(const QModelIndex &index)
{
    for(int i=0;i<ui->treeWidget->columnCount();i++)
        ui->treeWidget->resizeColumnToContents(i);
}
void RJManager::childitem(QTreeWidgetItem &pitem)
{
    for( int i = 0; i < pitem.childCount(); ++i )
    {
        QTreeWidgetItem *item = pitem.child(i);
        if(item->childCount()>0)
            childitem(*item);
        if(item->checkState(0)==Qt::Checked){

            QRegExp rjname("\\[(.*)\\]\\[(.*)\\]\\[(RJ.*)\\](.*)");
            int pos = 0;
            if((pos = rjname.indexIn(item->text(0), pos)) != -1) {
                QString tmpsql ="INSERT INTO TEMP VALUES('"+rjname.cap(3)+"','"+rjname.cap(1)+"','"+rjname.cap(4)+"','"+rjname.cap(2)+"','"+item->text(1)+"',"+item->text(2)+",'')";
                modeltemp->setQuery(tmpsql);
                qDebug()<<tmpsql;
                ui->statusBar->showMessage(tmpsql);
            }
            else{
                QString RJNumber;
                QRegExp rjname("(RJ\\d\\d\\d\\d\\d\\d)");
                if((RJNumber=findRJ(rjname,item->text(0)))!=NULL){
                    srcUrl ="http://www.dlsite.com/maniax/work/=/product_id/"+RJNumber+".html";
                    DlsitePageAnalysis(srcUrl);
                    QString tmpsql ="INSERT INTO TEMP VALUES('"+this->RJNumber+"','"+groupName+"','"+saleDate+"','"+gameName+"','"+item->text(1)+"',"+item->text(2)+",'')";
                    modeltemp->setQuery(tmpsql);
                }
            }

            qDebug()<<item->text(0);
        }
    }
}

void RJManager::on_pushButton_4_clicked()
{
    modeltemp->setQuery("DROP TABLE TEMP");
    modeltemp->setQuery("CREATE TABLE [TEMP] (\
                        number TEXT PRIMARY KEY NOT NULL,\
                        groupname VARCHAR,\
                        name VARCHAR,\
                        date TEXT,\
                        filepath  VARCHAR,\
                        played    BOOLEAN,\
                        comment   VARCHAR \
                        );");

    for( int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i )
    {
        QTreeWidgetItem *item = ui->treeWidget->topLevelItem( i );
        childitem(*item);
    }
    modeltemp->setQuery("SELECT * FROM TEMP");

}
