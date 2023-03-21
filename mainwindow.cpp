#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow){
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
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
    char str[]="\n";
    for (size_t i = 0; i < size; i++)
    {
        qsl.append(QString::fromUtf8(*(row+i)));
        if (i!=size-1 && strstr(*(row+i),str)!=NULL)
        {
            qsl.clear();
            for (size_t i = 0; i < size; qsl.append("error"),i++);
            break;
        }
    }
    if (strstr(QstringToCharArray(qsl.at(size-1)),str)!=NULL){
        QString tmp=qsl.at(size-1);
        qsl.removeLast();
        qsl.append(tmp.split(str));
        qsl.removeLast();
    }
    return qsl;
}

void MainWindow::on_btn_choseFileName_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Choose file", "", "*.csv");
    ui->lbl_filename->setText(filename);
    ui->btn_Load_data->setEnabled(true);
    FuncArgument fa={};
    FuncReturningValue frv={};
    ui->box_column->clear();
    ui->box_region->clear();
    ui->tb_widget->setColumnCount(0);
    ui->tb_widget->setRowCount(0);
    ui->tb_widget->clearContents();
}

void MainWindow::on_btn_Load_data_clicked()
{
    QString filename=ui->lbl_filename->text();
    if (!filename.isEmpty()){
        FuncArgument fa = {
            .filename = QstringToCharArray(filename)
        };
        FuncReturningValue* frv = entryPoint(getData, &fa);
        if (frv==NULL)
            QMessageBox::information(this,"Error","There are problems with opening the file");
        else
        {
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
    QStringList QColumns = ConvertRowToQTFormat(frv->headers, frv->fields_num);
    ui->tb_widget->setHorizontalHeaderLabels(QColumns);
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
        if (ui->box_column->count()==0){
            QStringList regions=calculateRegions();
            ui->box_region->addItems(regions);
            ui->box_column->addItems(QColumns);
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
    on_btn_Load_data_clicked();
    FuncArgument fa = {
            .filename=QstringToCharArray(ui->lbl_filename->text()),
            .data = getDataFromTable(),
            .region=QstringToCharArray(ui->box_region->currentText()),
            .column=(size_t)ui->box_column->currentIndex(),
            .len = (size_t)ui->tb_widget->rowCount(),
            .fields_num = (size_t)ui->tb_widget->columnCount(),
            .region_number=(size_t)ui->box_region->currentIndex()
    };
    FuncReturningValue* frv = entryPoint(calculateData, &fa);
    if (frv==NULL)
        QMessageBox::information(this,"Error","You should choose another column, in this column the values are unsuitable for calculations");
    else{
        ui->lbl_min->setText("Min value: " + QString::number(frv->solution_min));
        ui->lbl_max->setText("Max value: " + QString::number(frv->solution_max));
        ui->lbl_median->setText("Median value: " + QString::number(frv->solution_median));
        showData(frv);
        //draw();
    }
    entryPoint(cleanData, &fa);
    free(frv);
}

QStringList MainWindow::calculateRegions()
{
    QStringList regions={};
    for (size_t i=0;i<(size_t)ui->tb_widget->rowCount();i++)
    {
        QTableWidgetItem *item = ui->tb_widget->item(i,1);
        regions.append(item->text());
    }
    regions.removeDuplicates();
    regions.removeAll("error");
    return regions;
}

/*void MainWindow::draw()
{
    QGraphicsScene *scene = new QGraphicsScene(ui->graphicsView);
    //Это как раз создана сцена. Сцена - это класс для работы с 2D графикой.
    //Теперь, раз это график, то построим координатные оси X и Y.
    QPen pen(Qt::green);//Просто выбираем цвет для карандашика
    scene->addLine(0,90,180,90,pen);//x
    scene->addLine(90,0,90,180,pen);//y
    ui->graphicsView->setScene(scene);
}*/
