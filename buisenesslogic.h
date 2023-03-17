#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#define SIZE 1024
#include <string.h>

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
    double solution_min;                                  // результат вычислений (функция calculateData())
    double solution_max;
    size_t len;                                       // число строк в таблице
    size_t fields_num;                                // число столбцов в таблице
} FuncReturningValue;

typedef struct
{
    char* filename;                             // имя файла (функция getData())
    char ***data;                                     // данные (функция calculateData())
    char **headers;                                   // заголовки таблицы (функция getData())
    char **solution;                                  // результат вычислений (для освобождения памяти)
    char *region;                               //регион по которому вычисляются метрики
    int column;                                    //колонка по которой вычисляются метрики
    size_t len;                                       // число строк в таблице
    size_t fields_num;                                // число столбцов в таблице
} FuncArgument;


FuncReturningValue* entryPoint(FuncType ft, FuncArgument* fa);

