#include "mainwindow.h"
#include "ui_mainwindow.h"

QVector<hms> segTime;
hms currentTempTime;
bool isCounting=false,firstStart=true;
int currentSegSec,totalMS,currentSegIndex=-1;
QStringList fnames;
QMediaPlayer player;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	timer=new QTimer();
	uiTimer=new QTimer();
	connect(timer,SIGNAL(timeout()),this,SLOT(timeIsOut()));
	connect(uiTimer,SIGNAL(timeout()),this,SLOT(updateLcd()));

	hms t;
	t.h=t.m=t.s=t.ms=0;
	segTime.push_back(t);
	ui->listWidget->addItem(QString::number(ui->listWidget->count()+1));
	ui->spinBox->setEnabled(false);
	ui->spinBox_2->setEnabled(false);
	ui->spinBox_3->setEnabled(false);
	ui->progressBar->setValue(0);
	ui->progressBar_2->setValue(0);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::confirmSetting()
{
	if(ui->listWidget->currentRow()<0||!firstStart)
		return;
	currentSegSec=totalMS=0;
	hms totalTime;
	totalTime.h=totalTime.m=totalTime.s=totalTime.ms=0;
	if(ui->checkBox->isChecked())
		for(int i=ui->listWidget->currentRow();i<segTime.count();i++){
			totalTime.h+=segTime[i].h;
			totalTime.m+=segTime[i].m;
			totalTime.s+=segTime[i].s;
			totalMS+=segTime[i].h*3600+segTime[i].m*60+segTime[i].s;
			if(i==ui->listWidget->currentRow())
				currentSegSec=segTime[i].h*3600+segTime[i].m*60+segTime[i].s;
		}
	else{
		int i=ui->listWidget->currentRow();
		totalTime.h=segTime[i].h;
		totalTime.m=segTime[i].m;
		totalTime.s=segTime[i].s;
		totalMS=segTime[i].h*3600+segTime[i].m*60+segTime[i].s;
		currentSegSec=totalMS;
	}
	totalMS*=1000;
	ui->lcdNumber->display(ui->spinBox->value());//s
	ui->lcdNumber_2->display(ui->spinBox_2->value());//m
	ui->lcdNumber_3->display(ui->spinBox_3->value());//h

	ui->lcdNumber_5->display(totalTime.s);//s
	ui->lcdNumber_6->display(totalTime.m);//m
	ui->lcdNumber_7->display(totalTime.h);//h
	ui->label_4->setText(QString::number(ui->listWidget->currentRow()+1));
	ui->progressBar->setValue(0);
	ui->progressBar_2->setValue(0);
}

void MainWindow::on_start_clicked()
{
	if(firstStart){
		if(currentSegSec==0)
			return;
		timer->start(currentSegSec*1000);
		elapsedTimer.start();
		uiTimer->start(50);
		firstStart=false;
		isCounting=true;
		ui->start->setText(QString::fromLocal8Bit("暫停"));
		ui->checkBox->setEnabled(false);
		ui->spinBox->setEnabled(false);
		ui->spinBox_2->setEnabled(false);
		ui->spinBox_3->setEnabled(false);
		currentSegIndex=ui->listWidget->currentRow();
		player.pause();
		return;
	}
	if(isCounting){
		timer->stop();
		uiTimer->stop();
		currentTempTime.ms=ui->lcdNumber_4->value();
		currentTempTime.s=ui->lcdNumber->intValue();
		currentTempTime.m=ui->lcdNumber_2->intValue();
		currentTempTime.h=ui->lcdNumber_3->intValue();

		totalMS=ui->lcdNumber_8->value()+
				(ui->lcdNumber_5->value()+ui->lcdNumber_6->value()*60+ui->lcdNumber_7->value()*3600)*1000;
		ui->start->setText(QString::fromLocal8Bit("繼續"));
	}
	else{
		timer->start((currentTempTime.h*3600+currentTempTime.m*60+currentTempTime.s)*1000+currentTempTime.ms);
		elapsedTimer.start();
		uiTimer->start(50);
		ui->start->setText(QString::fromLocal8Bit("暫停"));
	}
	isCounting=!isCounting;
}

void MainWindow::on_clear_clicked()
{
	timer->stop();
	uiTimer->stop();
	ui->lcdNumber_4->display(0);
	ui->lcdNumber->display(0);
	ui->lcdNumber_2->display(0);
	ui->lcdNumber_3->display(0);

	ui->lcdNumber_8->display(0);
	ui->lcdNumber_5->display(0);
	ui->lcdNumber_6->display(0);
	ui->lcdNumber_7->display(0);
	isCounting=false;
	firstStart=true;
	currentSegSec=totalMS=0;
	ui->checkBox->setEnabled(true);
	if(ui->listWidget->currentRow()>=0){
		ui->spinBox->setEnabled(true);
		ui->spinBox_2->setEnabled(true);
		ui->spinBox_3->setEnabled(true);
	}
	ui->start->setText(QString::fromLocal8Bit("開始"));
	ui->progressBar->setValue(0);
	ui->progressBar_2->setValue(0);
	currentSegIndex=-1;
	player.pause();
}

void MainWindow::on_add_clicked()
{
	if(ui->listWidget->count()>=10||!firstStart)
		return;
	ui->listWidget->addItem(QString::number(ui->listWidget->count()+1));
	hms t;
	t.h=t.m=t.s=t.ms=0;
	segTime.push_back(t);
	updateSpinBoxTime();
}

void MainWindow::on_del_clicked()
{
	if(ui->listWidget->count()<=1||ui->listWidget->currentRow()<0||!firstStart)
		return;
	int removeIndex=ui->listWidget->currentRow();
	if(removeIndex==currentSegIndex)
		return;
	for(int i=ui->listWidget->count()-1;i>removeIndex;i--)
		ui->listWidget->item(i)->setText(QString::number(i));
	QListWidgetItem* item=ui->listWidget->takeItem(removeIndex);
	delete item;
	segTime.removeAt(removeIndex);
	updateSpinBoxTime();
	on_listWidget_itemSelectionChanged();
}

void MainWindow::timeIsOut()
{
	if(ui->checkBox->isChecked()&&++currentSegIndex<segTime.count()){
		timer->stop();
		ui->listWidget->setCurrentRow(currentSegIndex);
		on_start_clicked();
		ui->label_4->setText(QString::number(currentSegIndex+1));
	}
	else if(ui->checkBox_2->isChecked()&&++currentSegIndex<segTime.count()){
		timer->stop();
		if(fnames.count()>0)
			player.play();
		ui->start->setText(QString::fromLocal8Bit("開始"));
		ui->listWidget->setCurrentRow(currentSegIndex);
	}
	else{
		timer->stop();
		if(fnames.count()>0)
			player.play();
		if(ui->listWidget->currentRow()>=0){
			ui->spinBox->setEnabled(true);
			ui->spinBox_2->setEnabled(true);
			ui->spinBox_3->setEnabled(true);
		}
		ui->checkBox->setEnabled(true);
		ui->start->setText(QString::fromLocal8Bit("開始"));
		currentSegSec=0;
		currentSegIndex=-1;
	}
}

void MainWindow::updateLcd()
{
	int remainHour,remainMin,remainSec;
	remainHour=(timer->interval()-elapsedTimer.elapsed())/3600000;
	remainMin=((timer->interval()-elapsedTimer.elapsed())/1000-remainHour*3600)/60;
	remainSec=(timer->interval()-elapsedTimer.elapsed())/1000-remainHour*3600-remainMin*60;
	if(remainHour+remainMin+remainSec<=0){
		uiTimer->stop();
		ui->lcdNumber_4->display(0);
		ui->lcdNumber->display(0);
		ui->lcdNumber_2->display(0);
		ui->lcdNumber_3->display(0);

		ui->lcdNumber_8->display(0);
		ui->lcdNumber_5->display(0);
		ui->lcdNumber_6->display(0);
		ui->lcdNumber_7->display(0);
		isCounting=false;
		firstStart=true;
		ui->progressBar->setValue(100);
		timeIsOut();
		return;
	}
	double ms=timer->interval()-elapsedTimer.elapsed()-remainHour*3600000-remainMin*60000-remainSec*1000;
	ui->lcdNumber_4->display(ms);
	ui->lcdNumber->display(remainSec);
	ui->lcdNumber_2->display(remainMin);
	ui->lcdNumber_3->display(remainHour);
	ui->progressBar->setValue(((double)elapsedTimer.elapsed()/(double)timer->interval())*100);

	int remainTotalSec=totalMS-elapsedTimer.elapsed();
	remainHour=remainTotalSec/3600000;
	remainMin=(remainTotalSec/1000-remainHour*3600)/60;
	remainSec=remainTotalSec/1000-remainHour*3600-remainMin*60;
	ms=remainTotalSec-remainHour*3600000-remainMin*60000-remainSec*1000;
	ui->lcdNumber_8->display(ms);//ms
	ui->lcdNumber_5->display(remainSec);//s
	ui->lcdNumber_6->display(remainMin);//m
	ui->lcdNumber_7->display(remainHour);//h
	ui->progressBar_2->setValue(((double)elapsedTimer.elapsed()/(double)totalMS)*100);
}

void MainWindow::on_spinBox_editingFinished()//save s
{
	segTime[ui->listWidget->currentRow()].s=ui->spinBox->value();
	on_listWidget_itemSelectionChanged();
}

void MainWindow::on_spinBox_2_editingFinished()//save m
{
	segTime[ui->listWidget->currentRow()].m=ui->spinBox_2->value();
	on_listWidget_itemSelectionChanged();
}

void MainWindow::on_spinBox_3_editingFinished()//save h
{
	segTime[ui->listWidget->currentRow()].h=ui->spinBox_3->value();
	on_listWidget_itemSelectionChanged();
}

void MainWindow::updateSpinBoxTime()
{
	int i=ui->listWidget->currentRow();
	if(i>=0){
		ui->groupBox->setTitle(QString::number(i+1));
		ui->spinBox->setValue(segTime[i].s);
		ui->spinBox_2->setValue(segTime[i].m);
		ui->spinBox_3->setValue(segTime[i].h);
	}
}

void MainWindow::on_listWidget_itemSelectionChanged()//choose seg time
{
	if(firstStart&&ui->listWidget->currentRow()>=0){
		ui->spinBox->setEnabled(true);
		ui->spinBox_2->setEnabled(true);
		ui->spinBox_3->setEnabled(true);
	}
	int index=ui->listWidget->currentItem()->text().toInt()-1;
	ui->groupBox->setTitle(QString::number(index+1));
	ui->spinBox->setValue(segTime[index].s);
	ui->spinBox_2->setValue(segTime[index].m);
	ui->spinBox_3->setValue(segTime[index].h);

	confirmSetting();
}

void MainWindow::on_actionWav_triggered()//open music file
{
	//Open Dialog and Return files Directory
	fnames=QFileDialog::getOpenFileNames(this,"Open Music File" ,
										 "../" , "Music(*.wav;*.mp3);;All(*)");
	if(fnames.count()>0){
		QFile f(fnames[0]);

		//Open File and test whether it is error
		if(!f.open(QIODevice::ReadOnly)) ui->label_5->setText(fnames[0]+"[Open File Error]");

		player.setMedia(QUrl::fromLocalFile(fnames[0]));
		ui->label_5->setText(fnames[0]);
	}
}

void MainWindow::on_listWidget_itemPressed(QListWidgetItem *item)
{
	on_listWidget_itemSelectionChanged();
}
