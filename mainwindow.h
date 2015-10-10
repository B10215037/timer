#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QElapsedTimer>
#include <QVector>
#include <QListWidget>
#include <QFileDialog>
#include <QMediaPlayer>
#include <QUrl>

namespace Ui {
class MainWindow;
}

struct hms{
	int h,m,s;
	double ms;
};

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void on_start_clicked();
	void on_clear_clicked();
	void on_add_clicked();
	void on_del_clicked();

	void timeIsOut();
	void updateLcd();

	void on_spinBox_editingFinished();
	void on_spinBox_2_editingFinished();
	void on_spinBox_3_editingFinished();

	void on_listWidget_itemSelectionChanged();

	void on_actionWav_triggered();

	void on_listWidget_itemPressed(QListWidgetItem *item);

private:
	Ui::MainWindow *ui;
	QTimer *timer,*uiTimer;
	QElapsedTimer elapsedTimer;
	void updateSpinBoxTime();
	void confirmSetting();
};

#endif // MAINWINDOW_H
