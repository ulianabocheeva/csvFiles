#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow){
    ui->setupUi(this);
    this->setWindowTitle("Csv files");
}

MainWindow::~MainWindow()
{
    clean3DArray(tbl.data,tbl.len,tbl.fields_num);
    delete ui;
}

void MainWindow::clean2DArray(char **arr, size_t size)
{
    for (size_t i = 0;i < size; i++)
        if (strstr((*arr+i),"")==NULL)
            free(*(arr+i));
    free(arr);
}

void MainWindow::clean3DArray(char ***arr, size_t sizeX, size_t sizeY)
{
    for (size_t i = 0; i < sizeX; i++)
        clean2DArray(*(arr+i), sizeY);
    free(arr);
}

char* QstringToCharArray(QString qstr)
{
    char *str = (char*)malloc(sizeof(char)*(qstr.size() + 1));
    size_t i;
    for (i = 0; i < qstr.size(); i++)
    {
        *(str+i) = qstr.at(i).unicode();
    }
    str[i] = 0;
    return str;
}

QStringList ConvertRowToQTFormat(char **row, size_t size)
{
    QStringList qsl = {};
    for (size_t i = 0; i < size; i++)
    {
        qsl.append(QString::fromUtf8(*(row+i)));
    }
    return qsl;
}

void MainWindow::on_btn_choseFileName_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Choose file", "", "*.csv");
    if (filename.isEmpty())
        QMessageBox::information(this,"Error","You should chose file");
    else
    {
        tbl={};
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
    }
}

void MainWindow::on_btn_Load_data_clicked()
{
    QString filename=ui->lbl_filename->text();
    if (!filename.isEmpty()){
        FuncArgument fa = {
            .filename = QstringToCharArray(filename)
        };
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
            entryPoint(cleanData, &fa2);
            free(frv);
        }
        ui->btn_Load_data->setEnabled(false);
        ui->btn_calc_metrics->setEnabled(true);
    }
}

void MainWindow::showData(FuncReturningValue* frv)
{
    ui->tb_widget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tb_widget-> setColumnCount(frv->fields_num);
    QStringList header = ConvertRowToQTFormat(frv->headers, frv->fields_num);
    ui->tb_widget->setHorizontalHeaderLabels(header);
    if (frv->data != NULL)
    {
        ui->tb_widget->setRowCount(0);
        for (size_t i = 0; i < frv->len; i++)
        {
            QStringList currentRow = ConvertRowToQTFormat(frv->data[i], frv->fields_num);
                ui->tb_widget->setRowCount(i + 1);
                for (int j = 0; j < currentRow.count(); j++)
                {
                    QTableWidgetItem *item = new QTableWidgetItem();
                    item->setText(currentRow.at(j));
                    ui->tb_widget->setItem(i, j, item);
                }
        }
        if (tbl.key!=1)
            tbl={.data = getDataFromTable(),.headers=header,.len=frv->len,.fields_num=frv->fields_num,.key=1};
        if (ui->box_column->count()==0){
            ui->box_region->addItems(getRegions());
            ui->box_column->addItems(getColumns());
        }
    }
}

void MainWindow::showDataForCalcMetrics()
{
    ui->tb_widget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tb_widget-> setColumnCount(tbl.fields_num);
    ui->tb_widget->setHorizontalHeaderLabels(tbl.headers);
    if (tbl.data != NULL)
    {
        ui->tb_widget->setRowCount(0);
        for (size_t i = 0; i < tbl.len; i++)
        {
            QStringList currentRow = ConvertRowToQTFormat(tbl.data[i], tbl.fields_num);
                ui->tb_widget->setRowCount(i + 1);
                for (int j = 0; j < currentRow.count(); j++)
                {
                    QTableWidgetItem *item = new QTableWidgetItem();
                    item->setText(currentRow.at(j));
                    ui->tb_widget->setItem(i, j, item);
                }
        }
    }
}

char*** MainWindow::getDataFromTable()
{
    char ***data = (char ***)malloc(sizeof(char**) * ui->tb_widget->rowCount());
    for (size_t i = 0; i < (size_t)ui->tb_widget->rowCount(); i++)
    {
        *(data+i) = (char **)malloc(sizeof(char*) * ui->tb_widget->columnCount());
        for (size_t j = 0; j < (size_t)ui->tb_widget->columnCount(); j++)
        {
            //Получаем значение в i-ой строке и j-ом столбце
            QTableWidgetItem *item = ui->tb_widget->item(i,j);
            //Приводим значение ячейки к стандартному типу строки
            char* str = QstringToCharArray(item->text());
            *(*(data+i)+j) = str;
        }
    }
    return data;
}

void MainWindow::on_btn_calc_metrics_clicked()
{
    showDataForCalcMetrics();
    size_t index_of_column=(size_t)ui->box_column->currentIndex();
    FuncArgument fa = {
            .filename=QstringToCharArray(ui->lbl_filename->text()),
            .data = getDataFromTable(),
            .region=QstringToCharArray(ui->box_region->currentText()),
            .column=index_of_column+calculateColumns(index_of_column),
            .len = (size_t)ui->tb_widget->rowCount(),
            .fields_num = (size_t)ui->tb_widget->columnCount(),
            .region_number=(size_t)ui->box_region->currentIndex()
    };
    FuncReturningValue* frv = entryPoint(calculateData, &fa);
    if (frv->error!=CALCULATE_ERROR){
        ui->lbl_min->setText("Min value: " + QString::number(frv->solution_min));
        ui->lbl_max->setText("Max value: " + QString::number(frv->solution_max));
        ui->lbl_median->setText("Median value: " + QString::number(frv->solution_median));
        showData(frv);
        //draw();
        entryPoint(cleanData, &fa);
    }
    else
        QMessageBox::critical(this,"Error","It's impossible to calculate metrics for this column");
    free(frv);
}

QStringList MainWindow::getColumns(){
    QStringList columns={};
    for (size_t i=0;i<(size_t)ui->tb_widget->columnCount();i++)
    {
        if ((strcmp((tbl.data[0][i]),"")!=0)&&(!isalpha(*(tbl.data[0][i]))))
            columns.append(tbl.headers.at(i));
    }
    return columns;
}

QStringList MainWindow::getRegions()
{
    QStringList regions={};
    for (size_t i=0;i<(size_t)ui->tb_widget->rowCount();i++)
    {
        QTableWidgetItem *item = ui->tb_widget->item(i,1);
        regions.append(item->text());
    }
    regions.removeDuplicates();
    return regions;
}

size_t MainWindow::calculateColumns(size_t index_of_column){
    QStringList box_content=getColumns();
    size_t count=0;
    for (size_t i=0;i<=index_of_column;i++)
    {
        if ((strcmp(QstringToCharArray(tbl.headers.at(i+count)),QstringToCharArray(box_content.at(i)))!=0))
            count++;
    }
    return count;
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
