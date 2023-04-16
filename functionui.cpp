#include "functionui.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

char* MainWindow::QstringToCharArray(QString qstr)
{
    char *str = (char*)malloc(sizeof(char)*(qstr.size() + 1));//+1 потому что в уонце добавляется 0
    size_t i;
    for (i = 0; i < qstr.size(); i++)
        *(str+i) = qstr.at(i).unicode();
    *(str+i) = 0;
    return str;
}

QStringList MainWindow::ConvertRowToQTFormat(char **row, size_t size)
{
    QStringList qsl = {};
    for (size_t i = 0; i < size; i++)
        qsl.append(QString::fromUtf8(*(row+i)));
    return qsl;
}

void MainWindow::showDataForCalcMetrics()
{
    ui->tb_widget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tb_widget-> setColumnCount(tbl.fields_num);
    ui->tb_widget->setHorizontalHeaderLabels(tbl.headers);
    if (tbl.data != NULL){
        ui->tb_widget->setRowCount(0);
        for (size_t i = 0; i < tbl.len; i++){
            QStringList currentRow = ConvertRowToQTFormat(tbl.data[i], tbl.fields_num);
            ui->tb_widget->setRowCount(i + 1);
            for (int j = 0; j < currentRow.count(); j++){
                QTableWidgetItem *item = new QTableWidgetItem();
                item->setText(currentRow.at(j));
                ui->tb_widget->setItem(i, j, item);
            }
        }
    }
}

void MainWindow::showData(FuncReturningValue* frv)
{
    ui->tb_widget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tb_widget-> setColumnCount(frv->fields_num);
    QStringList header = ConvertRowToQTFormat(frv->headers, frv->fields_num);
    ui->tb_widget->setHorizontalHeaderLabels(header);
    if (frv->data != NULL){
        ui->tb_widget->setRowCount(0);
        for (size_t i = 0; i < frv->len; i++){
            QStringList currentRow = ConvertRowToQTFormat(frv->data[i], frv->fields_num);
                ui->tb_widget->setRowCount(i + 1);
                for (int j = 0; j < currentRow.count(); j++){
                    QTableWidgetItem *item = new QTableWidgetItem();
                    item->setText(currentRow.at(j));
                    ui->tb_widget->setItem(i, j, item);
                }
        }
        if (tbl.key!=1)
            tbl={.data = getDataFromTable(),.headers=header,.len=frv->len,.fields_num=frv->fields_num,.key=1};
        if ((ui->box_column->count()==0)&&(!getRegions().empty())){
            ui->box_region->addItems(getRegions());
            ui->box_column->addItems(getColumns());
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

QStringList MainWindow::getColumns(){
    QStringList columns={};
    for (size_t i=0;i<(size_t)ui->tb_widget->columnCount();i++){
        if ((strcmp((tbl.data[0][i]),"")!=0)&&(!isalpha(*(tbl.data[0][i]))))
            columns.append(tbl.headers.at(i));
    }
    return columns;
}

QStringList MainWindow::getRegions()
{
    QStringList regions={};
    int column_num=tbl.headers.indexOf("region");
    if (column_num!=-1){
        for (size_t i=0;i<(size_t)ui->tb_widget->rowCount();i++){
            QTableWidgetItem *item = ui->tb_widget->item(i,column_num);
            regions.append(item->text());
        }
        regions.removeDuplicates();
    }
    return regions;
}

size_t MainWindow::calculateColumns(size_t index_of_column){
    QStringList box_content=getColumns();
    size_t count=0;
    for (size_t i=0;i<=index_of_column;i++){
        if ((strcmp(QstringToCharArray(tbl.headers.at(i+count)),QstringToCharArray(box_content.at(i)))!=0))
            count++;
    }
    return count;
}
