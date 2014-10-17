#include "rjmanager.h"
#include "ui_rjmanager.h"
#include <iostream>
#include <curl/curl.h>
#include <QRegExp>
#include <QDebug>
#include<QImageReader>
#include <QBuffer>
#include <QDir>
#include <QFileDialog>
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
void RJManager::on_pushButton_clicked()
{
    QString name,srcUrl;
    name =ui->RJNameLineEdit->text();
    QRegExp rjname("(RJ\\d\\d\\d\\d\\d\\d)");

    int pos = 0;
    if((pos = rjname.indexIn(name, pos)) != -1) {
        srcUrl ="http://www.dlsite.com/maniax/work/=/product_id/"+rjname.cap(1)+".html";
        DlsitePageAnalysis(srcUrl);
    }


}
void RJManager::DlsitePageAnalysis(QString srcUrl){

    int pos;
    groupName="";
    RJNumber="";
    saleDate="";
    gameName="";
    QString GJImg;
    QString str(CurlDownload(srcUrl));

    QRegExp group("<span itemprop=\"brand\">(.*)</span></a>");
    QRegExp date("\\d\\d(\\d\\d)年(\\d\\d)月(\\d\\d)日");
    QRegExp rjname("(RJ\\d\\d\\d\\d\\d\\d)");
    QRegExp name("id=\"work_name\">(.*)</h1>");
    QRegExp mainImg("<div id=\"work_visual\" style=\"background-image: url\\((.*)\\); width: ");

    pos = 0;
    if((pos = group.indexIn(str, pos)) != -1) {
        groupName =group.cap(1);
    }
    pos = 0;
    if((pos = date.indexIn(str, pos)) != -1) {
        saleDate=date.cap(1)+date.cap(2)+date.cap(3);
    }

    pos = 0;
    if((pos = rjname.indexIn(str, pos)) != -1) {
        RJNumber=rjname.cap(1);
    }

    pos = 0;
    if((pos = name.indexIn(str, pos)) != -1) {
        gameName=name.cap(1);
    }
    pos = 0;
    if((pos = mainImg.indexIn(str, pos)) != -1) {
        GJImg="http:"+mainImg.cap(1);
    }

    qDebug()<<gameName;
    ui->label->setText(gameName);

    //GJImg ="http://img.dlsite.jp/modpub/images2/work/doujin/RJ143000/RJ142572_img_main.jpg";
    QByteArray tempfile=CurlDownload(GJImg);
    QBuffer buffer(&tempfile);
    buffer.open(QIODevice::ReadOnly);
    QImageReader reader(&buffer);
    QImage newImage = reader.read();//Copy
    newImage.save("dsfsdfsf.jpg");
    buffer.close();

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
    if(parent.fileName()=="!"){
        qDebug()<<"true";
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
        if (fileInfo.fileName() == "." || fileInfo.fileName() == ".." ); // nothing
        else if(fileInfo.isDir()) {
            QTreeWidgetItem *child = new QTreeWidgetItem(fileColumn);
            parentWidgetItem->addChild(child);
            // 查詢子目錄
            listFile(child, fileInfo);
        }
        else {
            QTreeWidgetItem *child = new QTreeWidgetItem(fileColumn);
            QRegExp rjname("(RJ\\d\\d\\d\\d\\d\\d)");

            int pos = 0;
            if((pos = rjname.indexIn(fileInfo.fileName(), pos)) != -1) {
                child->setCheckState(0, Qt::Checked);
            }
            else{
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

    ui->dirPathLineEdit->setText(directory);
    // 設定欄位名稱
    QStringList columnTitle;
    columnTitle.append("Name");
    columnTitle.append("Path");
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
void childitem(QTreeWidgetItem &pitem)
{
    //qDebug()<<"  1  ";
    for( int i = 0; i < pitem.childCount(); ++i )
    {
        QTreeWidgetItem *item = pitem.child(i);
        if(item->childCount()>0)
            childitem(*item);
        if(item->checkState(0)==Qt::Checked)
            qDebug()<<item->text(0);
    }
}

void RJManager::on_pushButton_4_clicked()
{
    for( int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i )
    {
        QTreeWidgetItem *item = ui->treeWidget->topLevelItem( i );
        childitem(*item);
    }

}
