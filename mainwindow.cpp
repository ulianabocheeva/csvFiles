#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QMessageBox"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow){
    ui->setupUi(this);
    this->setWindowTitle("Csv files");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btn_choseFileName_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Choose file", "", "*.csv");
    key=0;
    ui->lbl_filename->setText(filename);
    ui->btn_Load_data->setEnabled(true);
    ui->btn_calc_metrics->setEnabled(false);
    ui->box_column->clear();
    ui->box_region->clear();
    ui->tb_widget->setColumnCount(0);
    ui->tb_widget->setRowCount(0);
    ui->tb_widget->clearContents();
    ui->lbl_min->setText("Min value: ");
    ui->lbl_max->setText("Max value: ");
    ui->lbl_median->setText("Median value: ");
    if (filename.isEmpty())
        QMessageBox::critical(this,"Error","You should chose file");
}

void MainWindow::on_btn_Load_data_clicked()
{
    QString filename=ui->lbl_filename->text();
    if (!filename.isEmpty()){
        FuncArgument fa = {.filename = QstringToCharArray(filename)};
        FuncReturningValue* frv = entryPoint(getData, &fa);
        if (frv->error==FILE_OPEN_ERROR)
            QMessageBox::information(this,"Error","There are problems with opening the file");
        else{
            showData(frv);
            FuncArgument fa2 = {
                .filename = fa.filename,
                .data = frv->data,
                .headers = frv->headers,
                .len = frv->len,
                .fields_num = frv->fields_num
            };
            key=1;
            entryPoint(cleanData, &fa2);
            free(frv);
        }
        ui->btn_Load_data->setEnabled(false);
        ui->btn_calc_metrics->setEnabled(true);
    }
}

void MainWindow::on_btn_calc_metrics_clicked()
{
    on_btn_Load_data_clicked();
    size_t index_of_column=(size_t)ui->box_column->currentIndex();
    FuncArgument fa = {
            .filename=QstringToCharArray(ui->lbl_filename->text()),
            .data = getDataFromTable(),
            .region=QstringToCharArray(ui->box_region->currentText()),
            .column=index_of_column+calculateColumns(index_of_column),
            .len = (size_t)ui->tb_widget->rowCount(),
            .fields_num = (size_t)ui->tb_widget->columnCount(),
            .region_number=(size_t)ui->box_region->currentIndex(),
            .region_index_at_header=(size_t)headers.indexOf("region")
    };
    FuncReturningValue* frv = entryPoint(calculateData, &fa);
    showData(frv);
    if (frv->error!=CALCULATE_ERROR){
        ui->lbl_min->setText("Min value: " + QString::number(frv->solution_min));
        ui->lbl_max->setText("Max value: " + QString::number(frv->solution_max));
        ui->lbl_median->setText("Median value: " + QString::number(frv->solution_median));
        //draw();
        entryPoint(cleanData, &fa);
    }
    else{
        QMessageBox::critical(this,"Error","It's impossible to calculate metrics for this column");
        ui->lbl_min->setText("Min value: ");
        ui->lbl_max->setText("Max value: ");
        ui->lbl_median->setText("Median value: ");
    }
    free(frv);
}

void MainWindow::on_box_region_currentTextChanged()
{
    if (key==1)
    {
        ui->tb_widget->setColumnCount(0);
        ui->tb_widget->setRowCount(0);
        ui->tb_widget->clearContents();
        ui->lbl_min->setText("Min value: ");
        ui->lbl_max->setText("Max value: ");
        ui->lbl_median->setText("Median value: ");
    }
}


void MainWindow::on_box_column_currentTextChanged()
{
    if (key==1)
    {
        ui->lbl_min->setText("Min value: ");
        ui->lbl_max->setText("Max value: ");
        ui->lbl_median->setText("Median value: ");
    }
}

void MainWindow::draw()
{
    /*QGraphicsScene *scene = new QGraphicsScene(ui->view_for_draw);
    //Это как раз создана сцена. Сцена - это класс для работы с 2D графикой.
    //Теперь, раз это график, то построим координатные оси X и Y.
    QPen pen(Qt::black);//Просто выбираем цвет для карандашика
    scene->addLine(0,90,180,90,pen);//x
    scene->addLine(90,0,90,180,pen);//y
    ui->view_for_draw->setScene(scene);*/
}
