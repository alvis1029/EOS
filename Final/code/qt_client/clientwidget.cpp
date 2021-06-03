#include "clientwidget.h"
#include "ui_clientwidget.h"
#include <QHostAddress>
#include <QElapsedTimer>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <QFont>
#include <QFontDialog>
#include <QColor>
#include <QColorDialog>



char s[38]="/home/hou/Downloads/Qt_Demo-master/00";
char buffer [2];
char d_1[18]="Remaining deck:22";
char d_2[18]="Remaining deck:22";
int i_1=22;
int i_2=22;
bool first_time=true;
clientwidget::clientwidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::clientwidget)
{
    QColor color = "red";
    ui->setupUi(this);
    QImage *image = new QImage("/home/hou/Downloads/Qt_Demo-master/00");
    ui->label_card_1->setPixmap(QPixmap::fromImage(*image));
    ui->label_card_2->setPixmap(QPixmap::fromImage(*image));
    ui->label_card_3->setPixmap(QPixmap::fromImage(*image));
    ui->label_card_4->setPixmap(QPixmap::fromImage(*image));
    ui->label_card_5->setPixmap(QPixmap::fromImage(*image));
    ui->label_card_6->setPixmap(QPixmap::fromImage(*image));
    ui->label_card_7->setPixmap(QPixmap::fromImage(*image));
    ui->label_card_8->setPixmap(QPixmap::fromImage(*image));
    ui->label_card_9->setPixmap(QPixmap::fromImage(*image));
    ui->label_card_10->setPixmap(QPixmap::fromImage(*image));
    ui->deck_1->setPixmap(QPixmap::fromImage(*image));
    ui->deck_2->setPixmap(QPixmap::fromImage(*image));
    ui->textEdit_recv_2->setText(d_1);
    ui->textEdit_recv_3->setText(d_2);
    setWindowTitle("客户端");
    //分配空间，指定父对象
    tcpsocket = new QTcpSocket(this);
    //建立连接
    connect(tcpsocket, &QTcpSocket::connected,
        [=]() {
            ui->textEdit_recv->setText("成功和服务器建立了连接");
            tcpsocket->write("display");
        }
    );
    //接收数据
    connect(tcpsocket, &QTcpSocket::readyRead,
        [=](){
            //获取对方发送的内容
            QByteArray array = tcpsocket->readAll();
            //追加到编辑区中
            ui->textEdit_recv->append(array);


            s[35]=array[0];
            s[36]=array[1];
            image->load(s);
            ui->label_card_1->setPixmap(QPixmap::fromImage(*image));
            s[35]=array[2];
            s[36]=array[3];
            image->load(s);
            ui->label_card_2->setPixmap(QPixmap::fromImage(*image));
            s[35]=array[4];
            s[36]=array[5];
            image->load(s);
            ui->label_card_3->setPixmap(QPixmap::fromImage(*image));
            s[35]=array[6];
            s[36]=array[7];
            image->load(s);
            ui->label_card_4->setPixmap(QPixmap::fromImage(*image));
            s[35]=array[8];
            s[36]=array[9];
            image->load(s);
            ui->label_card_5->setPixmap(QPixmap::fromImage(*image));
            s[35]=array[10];
            s[36]=array[11];
            image->load(s);
            ui->label_card_6->setPixmap(QPixmap::fromImage(*image));
            s[35]=array[12];
            s[36]=array[13];
            image->load(s);
            ui->label_card_7->setPixmap(QPixmap::fromImage(*image));
            s[35]=array[14];
            s[36]=array[15];
            image->load(s);
            ui->label_card_8->setPixmap(QPixmap::fromImage(*image));
            s[35]=array[16];
            s[36]=array[17];
            image->load(s);
            ui->label_card_9->setPixmap(QPixmap::fromImage(*image));
            s[35]=array[18];
            s[36]=array[19];
            image->load(s);
            ui->label_card_10->setPixmap(QPixmap::fromImage(*image));
            if(array[23]=='1')
            {
                if(array[21]=='1')
                {
                    if(i_1>0)
                        i_1-=1;
                    snprintf(buffer, 3, "%d", i_1);
                    if(i_1>9)
                    {
                        d_1[15]=buffer[0];
                        d_1[16]=buffer[1];
                    }
                    else
                    {
                        d_1[15]='0';
                        d_1[16]=buffer[0];
                    }

                    ui->textEdit_recv_3->setText(d_1);
                    ui->textEdit_recv_4->setTextColor(color);
                    ui->textEdit_recv_4->setText("lower player change success");
                }
                else if(array[21]=='2')
                {
                    if(i_2>0)
                        i_2-=1;
                    snprintf(buffer, 3, "%d", i_2);
                    if(i_2>9)
                    {
                        d_2[15]=buffer[0];
                        d_2[16]=buffer[1];
                    }
                    else
                    {
                        d_2[15]='0';
                        d_2[16]=buffer[0];
                    }
                    ui->textEdit_recv_2->setText(d_2);
                    ui->textEdit_recv_4->setTextColor(color);
                    ui->textEdit_recv_4->setText("upper player change success");
                }
                else if(array[21]=='4')
                {
                    char fuck[2];
                    fuck[0]=array[25];
                    int minus=atoi(fuck)-1;
                    for(int i=0;i<minus;i++)
                    {
                        if(i_1>0)
                            i_1-=1;
                        if(i_2>0)
                            i_2-=1;
                    }
                    if(i_1>0)
                        i_1-=1;
                    snprintf(buffer, 3, "%d", i_1);
                    if(i_1>9)
                    {
                        d_1[15]=buffer[0];
                        d_1[16]=buffer[1];
                    }
                    else
                    {
                        d_1[15]='0';
                        d_1[16]=buffer[0];
                    }
                    ui->textEdit_recv_3->setText(d_1);
                    if(i_2>0)
                        i_2-=1;
                    if(i_2>0)
                        i_2-=1;
                    snprintf(buffer, 3, "%d", i_2);
                    if(i_2>9)
                    {
                        d_2[15]=buffer[0];
                        d_2[16]=buffer[1];
                    }
                    else
                    {
                        d_2[15]='0';
                        d_2[16]=buffer[0];
                    }
                    ui->textEdit_recv_2->setText(d_2);
                    ui->textEdit_recv_4->setTextColor(color);
                    ui->textEdit_recv_4->setText("change both decks");
                }
                else if(array[21]=='3')
                {
                    char fuck[2];
                    fuck[0]=array[25];
                    int minus=atoi(fuck)-1;
                    for(int i=0;i<minus;i++)
                    {
                        if(i_1>0)
                            i_1-=1;
                        if(i_2>0)
                            i_2-=1;
                    }
                    if(i_1>0)
                        i_1-=1;
                    if(i_1>0)
                        i_1-=1;
                    snprintf(buffer, 3, "%d", i_1);
                    if(i_1>9)
                    {
                        d_1[15]=buffer[0];
                        d_1[16]=buffer[1];
                    }
                    else
                    {
                        d_1[15]='0';
                        d_1[16]=buffer[0];
                    }
                    ui->textEdit_recv_3->setText(d_1);
                    if(i_2>0)
                        i_2-=1;
                    snprintf(buffer, 3, "%d", i_2);
                    if(i_2>9)
                    {
                        d_2[15]=buffer[0];
                        d_2[16]=buffer[1];
                    }
                    else
                    {
                        d_2[15]='0';
                        d_2[16]=buffer[0];
                    }
                    ui->textEdit_recv_2->setText(d_2);
                    ui->textEdit_recv_4->setTextColor(color);
                    ui->textEdit_recv_4->setText("change both decks");
                }
            }
            else if(array[23]=='0')
            {
                if(array[21]=='2')
                {
                    ui->textEdit_recv_4->setTextColor(color);
                    ui->textEdit_recv_4->setText("upper player change failed");
                }
                else if(array[21]=='1')
                {
                    ui->textEdit_recv_4->setTextColor(color);
                    ui->textEdit_recv_4->setText("lower player change failed");
                }
            }

            if(array[27]=='1')
            {
                ui->textEdit_recv_4->setTextColor(color);
                ui->textEdit_recv_4->setText("lower player win!!!");
            }
            else if(array[27]=='2')
            {
                ui->textEdit_recv_4->setTextColor(color);
                ui->textEdit_recv_4->setText("upper player win!!!");
            }
            else if(array[27]=='3')
            {
                ui->textEdit_recv_4->setTextColor(color);
                ui->textEdit_recv_4->setText("even!!!");
            }

        }
    );
    //断开连接
    connect(tcpsocket, &QTcpSocket::disconnected,
        [=](){
            ui->textEdit_recv->append("和服务器断开了连接");
        }
    );
}

clientwidget::~clientwidget()
{
    delete ui;
}

void clientwidget::on_pushButton_send_clicked()
{
    //获取编辑框内容
    QString str = ui->textEdit_send->toPlainText();
    //发送数据
    tcpsocket->write(str.toUtf8().data());
}

void clientwidget::on_pushButton_close_clicked()
{
    //主动和对方断开连接
    tcpsocket->disconnectFromHost();
    tcpsocket->close();
}

void clientwidget::on_pushButton_connect_clicked()
{
    //获取服务器ip和端口
    QString ip = ui->lineEdit_ip->text();
    qint16 port = ui->lineEdit_port->text().toInt();
    //主动和服务器建立连接
    tcpsocket->connectToHost(QHostAddress(ip), port);
}
