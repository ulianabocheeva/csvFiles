#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#define SIZE 1024
#define STRLEN 120
using namespace std;

enum FuncType
{
    getData, // функция чтения данных из csv файла
    calculateData, // функция для вычислений
    cleanData // освобождение памяти
};

typedef struct
{
    char ***data;                                     // данные (функция getData())
    char **headers;                                   // заголовки таблицы (функция getData())
    double solution_min;                              // результаты вычислений (функция calculateData())
    double solution_max;
    double solution_median;
    size_t len;                                       // число строк в таблице
    size_t fields_num;                                // число столбцов в таблице
} FuncReturningValue;

typedef struct
{
    char* filename;                                    // имя файла (функция getData())
    char ***data;                                      // данные (функция calculateData())
    char **headers;                                    // заголовки таблицы (функция getData())
    char **solution;                                   // результат вычислений (для освобождения памяти)
    char *region;                                      //регион по которому вычисляются метрики
    size_t column;                                        //колонка по которой вычисляются метрики
    size_t len;                                        // число строк в таблице
    size_t fields_num;                                 // число столбцов в таблице
    size_t region_number;                              //индекс региона в списке
} FuncArgument;


FuncReturningValue* entryPoint(FuncType ft, FuncArgument* fa);

