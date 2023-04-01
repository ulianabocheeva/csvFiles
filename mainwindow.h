#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "buisenesslogic.h"
#include "QMessageBox"
#include <QFileDialog>
//#include <QPainter>
//#include <QGraphicsScene>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    typedef struct
    {
        char ***data;                                     // данные (функция getData())
        QStringList headers;                            // заголовки таблицы (функция getData())
        size_t len;                                       // число строк в таблице
        size_t fields_num;                                // число столбцов в таблице
        size_t key;
    } table;
    table tbl;

private slots:

    void on_btn_choseFileName_clicked();

    void on_btn_Load_data_clicked();

    void on_btn_calc_metrics_clicked();


private:
    Ui::MainWindow *ui;
    //QStringList headers;
    //size_t len;
    //size_t fields_num;
    void showData(FuncReturningValue*);
    void showDataForCalcMetrics();
    void draw();
    char*** getDataFromTable();
    QStringList calculateColumns();
    QStringList calculateRegions();
    char***memory_alloc_tb(size_t rows,size_t cols);
};

#endif // MAINWINDOW_H
