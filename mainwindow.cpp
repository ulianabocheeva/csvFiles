#include "mainwindow.h"
#include "ui_mainwindow.h"
#define SIZEOF(a) sizeof(a)/sizeof(*a)

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow){
    ui->setupUi(this);
    //connect(ui->btn_choseFileName,SIGNAL(clicked()),this,SLOT());
}

MainWindow::~MainWindow()
{
    delete ui;
}

QStringList ConvertRowToQTFormat(char **row, size_t size)
{
    QStringList qsl = {};
    char str[]="\n";
    for(size_t i = 0; i < size; i++){
        qsl.append(QString::fromUtf8(*(row+i)));
        if (i!=size-1 && strstr(*(row+i),str)!=NULL){
            qsl.clear();
            for(size_t i = 0; i < size; qsl.append("error"),i++);
            break;
        }
    }
    return qsl;
}

char* QstringToCharArray(QString qstr)
{
    char *str = (char*)malloc(sizeof(char)*(qstr.size() + 1));
    size_t i;
    for (i = 0; i < qstr.size(); i++)
    {
        str[i] = qstr.at(i).unicode();
    }
    str[i] = 0;
    return str;
}

void MainWindow::on_btn_choseFileName_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Choose file", "", "*.csv");
    ui->lbl_filename->setText(filename);
    ui->btn_Load_data->setEnabled(true);
}

void MainWindow::on_btn_Load_data_clicked()
{
    QString filename=ui->lbl_filename->text();
    if (!filename.isEmpty()){
        FuncArgument fa = {
            .filename = QstringToCharArray(filename)//Приводим строку с путем к файлу от типа QString к стандартному типу строки
        };
        FuncReturningValue* frv = entryPoint(getData, &fa);
        //Выводим на экран данные
        showData(frv);

        FuncArgument fa2 = {
            .filename = fa.filename,
            .data = frv->data,
            .headers = frv->headers,
            .len = frv->len,
            .fields_num = frv->fields_num
        };
        entryPoint(cleanData, &fa2);
        free(frv);
        ui->btn_Load_data->setEnabled(false);
        ui->btn_calc_metrics->setEnabled(true);
    }
}

void MainWindow::showData(FuncReturningValue* frv)
{
    ui->tb_widget-> setColumnCount(frv->fields_num);
    QStringList QColumns = ConvertRowToQTFormat(frv->headers, frv->fields_num);
    ui->tb_widget->setHorizontalHeaderLabels(QColumns);
    if (frv->data != NULL)
    {
        ui->tb_widget->setRowCount(0);
        for (size_t i = 0; i < frv->len; i++)
        {
            QStringList currentRow = ConvertRowToQTFormat(frv->data[i], frv->fields_num);
            if (!currentRow.empty()){
                ui->tb_widget->setRowCount(i + 1);
                for (int j = 0; j < currentRow.count(); j++)
                {
                    QTableWidgetItem *item = new QTableWidgetItem();
                    item->setData(Qt::EditRole, currentRow.at(j).toDouble());
                    item->setText(currentRow.at(j));
                    ui->tb_widget->setItem(i, j, item);
                }
            }
        }
        QStringList regions=calculateRegions(),columns=calculateColumns(frv);
        ui->box_region->addItems(regions);
        ui->box_column->addItems(columns);
    }
}
char*** MainWindow::getDataFromTable()
{
    char ***data = (char ***)malloc(sizeof(char**) * ui->tb_widget->rowCount());
    for (size_t i = 0; i < (size_t)ui->tb_widget->rowCount(); i++)
    {
        data[i] = (char **)malloc(sizeof(char*) * ui->tb_widget->columnCount());
        for (size_t j = 0; j < (size_t)ui->tb_widget->columnCount(); j++)
        {
            //Получаем значение в i-ой строке и j-ом столбце
            QTableWidgetItem *item = ui->tb_widget->item(i,j);
            //Приводим значение ячейки к стандартному типу строки
            char* str = QstringToCharArray(item->text());
            data[i][j] = str;
        }
    }
    return data;
}

void MainWindow::on_btn_calc_metrics_clicked()
{

    FuncArgument fa = {
            .data = getDataFromTable(),
            .region=QstringToCharArray(ui->box_region->currentText()),
            .column=(ui->box_column->currentText()).toInt(),
            .len = (size_t)ui->tb_widget->rowCount(),
            .fields_num = (size_t)ui->tb_widget->columnCount()
        };
        FuncReturningValue* frv = entryPoint(calculateData, &fa);
        ui->lbl_min->setText("Min: " + QString::number(frv->solution_min));
        ui->lbl_max->setText("Max: " + QString::number(frv->solution_max));

        entryPoint(cleanData, &fa);
        free(frv);
        ui->btn_calc_metrics->setEnabled(false);
}

QStringList MainWindow::calculateColumns(FuncReturningValue* frv){
    QStringList columns;
    for (size_t i=2;i<frv->fields_num;i++){
        columns.append(QString::number(i+1));
    }
    return columns;
}

QStringList MainWindow::calculateRegions(){
    QStringList regions;
    for (size_t i=0;i<(size_t)ui->tb_widget->rowCount();i++){
        QTableWidgetItem *item = ui->tb_widget->item(i,1);
        regions.append(item->text());
    }
    regions.removeDuplicates();
    regions.removeAll("error");
    return regions;
}
