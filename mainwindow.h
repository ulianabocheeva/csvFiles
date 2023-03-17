#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QMessageBox"
#include "buisenesslogic.h"
#include <QFileDialog>
#include <QRegExp>
#include "QStandardItemModel"
#include "QStandardItem"
#include <QTableWidget>
#include <QFile>
#include <QtAlgorithms>
#include <QVector>
#include <QDebug>
#include <string>
#include <functional>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_btn_choseFileName_clicked();

    void on_btn_Load_data_clicked();

    void on_btn_calc_metrics_clicked();


private:
    Ui::MainWindow *ui;
    void showData(FuncReturningValue*);
    char*** getDataFromTable();
    QStringList calculateColumns(FuncReturningValue*);
    QStringList calculateRegions();
};

#endif // MAINWINDOW_H
