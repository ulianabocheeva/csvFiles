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

private slots:

    void on_btn_choseFileName_clicked();

    void on_btn_Load_data_clicked();

    void on_btn_calc_metrics_clicked();


private:
    Ui::MainWindow *ui;
    table tbl;
    void clean2DArray(char **arr, size_t size);
    void clean3DArray(char ***arr, size_t sizeX, size_t sizeY);
    void showData(FuncReturningValue*);
    void showDataForCalcMetrics();
    void draw();
    char*** getDataFromTable();
    size_t calculateColumns(size_t);
    QStringList getColumns();
    QStringList getRegions();
};

#endif // MAINWINDOW_H
