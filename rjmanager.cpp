#include "rjmanager.h"
#include "ui_rjmanager.h"
#include <iostream>
#include <curl/curl.h>
#include <QRegExp>
#include <QDebug>
#include<QImageReader>
#include <QBuffer>
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
}

RJManager::~RJManager()
{
    delete ui;
}

void RJManager::on_pushButton_clicked()
{
    QString name,srcUrl;
    name =ui->RJNameLineEdit->text();
    QRegExp rjname("(RJ\\d\\d\\d\\d\\d\\d)");

    int pos = 0;
    if((pos = rjname.indexIn(name, pos)) != -1) {
        srcUrl ="http://www.dlsite.com/maniax/work/=/product_id/"+rjname.cap(1)+".html";
    }

    DlsitePageAnalysis(srcUrl);

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
        //qDebug()<<saleDate;
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
        qDebug()<<GJImg;
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
