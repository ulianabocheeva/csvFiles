#include "buisenesslogic.h"

FuncReturningValue* getDataFromFile(const char *filename);
FuncReturningValue* solve(FuncArgument* fa);
void clean(FuncArgument* args);
char** read_csv(const char* filename, size_t *lines);
void clean2DArray(char**, size_t );
void clean3DArray(char***, size_t, size_t);
char** strSplit(char*, size_t *, const char);
char** strSplit(string line);
size_t calculating_region_lines_number(FuncArgument* fa);
char***memory_alloc_for_3DArray(FuncArgument*fa);
char**memory_alloc_for_2DArray(int n);
char**get_headers(FuncArgument* fa);
double calc_median(std::vector<double> vectorForMedian,int count_lines);
double calc_min(FuncArgument *fa);
double calc_max(FuncArgument *fa);

FuncReturningValue* entryPoint(FuncType ft, FuncArgument* fa)
{
    FuncReturningValue* result;
    switch(ft)
    {
        case getData:
            result = getDataFromFile(fa->filename);
            break;
        case calculateData:
            result = solve(fa);
            break;
        case cleanData:
            clean(fa);
        default:
            result = NULL;
            break;
    }

    return result;
}

FuncReturningValue* getDataFromFile(const char* filename)
{
    size_t lines,fields=7;
    FuncReturningValue *frv = (FuncReturningValue *)malloc(sizeof(FuncReturningValue));
    if (frv!=NULL)
    {
        char **rawData = read_csv(filename, &lines);
            if (rawData!=NULL)
            {
                char ***data = (char***)malloc((lines - 1) * sizeof(char**));
                if (data!=NULL)
                {
                    char **headers = strSplit(*rawData);
                    if (headers!=NULL)
                    {
                        for (size_t i = 0; i < lines - 1; i++)
                            *(data+i) = strSplit(*(rawData+i+1));
                        lines--;
                        clean2DArray(rawData, lines);
                        frv->len = lines;
                        frv->fields_num = fields;
                        frv->headers = headers;
                        frv->data = data;
                    }
                }
            }
        }
    return frv;
}

FuncReturningValue* solve(FuncArgument* fa)
{
    int count_lines=0;
    double min=calc_min(fa),max=calc_max(fa),current;
    std::vector<double> vectorForMedian;
    FuncReturningValue *frv = (FuncReturningValue *)malloc(sizeof(FuncReturningValue));
    if (isalpha(*(fa->data[fa->region_number][fa->column])))
        return NULL;
    if (frv!=NULL)
    {
        char ***data_for_chosen_region =memory_alloc_for_3DArray(fa);
        if (data_for_chosen_region!=NULL){
            for (size_t i=0;i<fa->len;i++)
            {
                if (strcmp(fa->data[i][1],fa->region)==0)
                {
                    *(data_for_chosen_region+count_lines)=fa->data[i];
                    count_lines++;
                    if ((strcmp(fa->data[i][fa->column],"")!=0)&&(!isalpha(*(fa->data[i][fa->column])))){
                        current=atof(fa->data[i][fa->column]);
                        current<min? min=current:current>max? max=current:max;
                        vectorForMedian.push_back(current);
                    }
                }
            }
            frv->headers=get_headers(fa);
            frv->fields_num=fa->fields_num;
            frv->len=count_lines;
            frv->data=data_for_chosen_region;
            frv->solution_min=min;
            frv->solution_max=max;
            frv->solution_median=calc_median(vectorForMedian,count_lines-1);
        }
    }
    return frv;
}

double calc_min(FuncArgument *fa)
{
    double min=0;
    for (size_t i=0;i<fa->len;i++)
    {
        if (strcmp(fa->data[i][1],fa->region)==0){
            if (strcmp(fa->data[i][fa->column],"")!=0){
                min=atof(fa->data[i][fa->column]);
                break;
            }
        }
    }
    return min;
}

double calc_max(FuncArgument *fa)
{
    double max=0;
    for (size_t i=0;i<fa->len;i++)
    {
        if (strcmp(fa->data[i][1],fa->region)==0){
            if (strcmp(fa->data[i][fa->column],"")!=0){
                max=atof(fa->data[i][fa->column]);
                break;
            }
        }
    }
    return max;
}

void clean(FuncArgument* args)
{
    if (args->data != NULL)
        clean3DArray(args->data, args->len, args->fields_num);
    if (args->filename != NULL)
        free(args->filename);
    if (args->headers != NULL)
        clean2DArray(args->headers, args->fields_num);
    if (args->solution != NULL)
        clean2DArray(args->solution, args->fields_num);
    if (args->region != NULL)
        free(args->region);
}

char** read_csv(const char* filename, size_t *lines){
    string line;
    size_t llen,counter = 0,max_size = 1;
    char **data = (char **)calloc(max_size, sizeof(char *));
    if (data == NULL)
        return NULL;
    ifstream fp(filename);
    if(!fp.is_open())
        return NULL;
    while (getline(fp,line))
    {
        if (counter >= max_size-1)
        {
            data = (char **)realloc(data,max_size * 2 * sizeof(char *));
            if (data != NULL)
                max_size *= 2;
            else
                return NULL;
        }
        llen = line.length();
        *(data+counter) = (char *)calloc(sizeof(char), llen+1);
        strcpy(*(data+counter), line.c_str());
        counter++;
    }

    *lines = counter;

    return data;
}

void clean2DArray(char **arr, size_t size)
{
    for (size_t i = 0;i < size; i++)
        if (strstr((*arr+i),"")==NULL)
            free(*(arr+i));
    free(arr);
}

void clean3DArray(char ***arr, size_t sizeX, size_t sizeY)
{
    for (size_t i = 0; i < sizeX; i++)
        clean2DArray(*(arr+i), sizeY);
    free(arr);
}

//Функция разбития строки на массив слов
char** strSplit(char* a_str, size_t *len, const char a_delim)
{
    char** result = 0;//** потому что из строки создается матрица
    size_t count = 0;
    char* tmp = a_str;//перезаписываю строку в tmp потому что потом сравниваю
    char* last_comma = 0;
    char delim[2];
    *delim = a_delim;
    *(delim+1) = 0;

    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }
    *len = count + 1;
    /* Добавьте пробел для завершающего токена. */
    count += last_comma < (a_str + strlen(a_str) - 1);
    /* Добавьте пробел для завершения нулевой строки, так называемой
   знает, где заканчивается список возвращаемых строк. */
    count++;
    result = (char**)malloc(sizeof(char*) * count);
    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);//разбиение строки на части по указанному разделителю.

        while (token)
        {
            *(result + idx++) = strdup(token);//дублирование строки
            token = strtok(0, delim);
        }
        *(result + idx) = 0;//терминальный ноль в конец строки
    }

    return result;
}

size_t calculating_region_lines_number(FuncArgument* fa)
{
    size_t lines=0;
    for (size_t i=0;i<fa->len;i++)
    {
        if (strstr(fa->data[i][1],fa->region)!=NULL)
            lines++;
    }
    return lines;
}

char***memory_alloc_for_3DArray(FuncArgument*fa)
{
    size_t rows=calculating_region_lines_number(fa);
    char ***data = (char ***)malloc(rows*sizeof(char**));
    if (data!=NULL)
    {
        for (size_t i = 0; i < rows; i++)
        {
            *(data+i) = (char **)malloc(sizeof(char*) * fa->fields_num);
            if (*(data+i)!=NULL){
                for (size_t j = 0; j < fa->fields_num; j++)
                {
                    *(*(data+i)+j)=(char *)malloc(sizeof(char) *STRLEN);
                }
            }
        }
    }
    return data;
}

char**memory_alloc_for_2DArray(int n)
{
    char **data = (char **)malloc(n*sizeof(char*));
    if (data!=NULL)
    {
        for (size_t i = 0; i < n; i++)
        {
            *(data+i) = (char *)malloc(sizeof(char) * 30);
        }
    }
    return data;
}
char**get_headers(FuncArgument* fa)
{
    size_t i=0;
    char **headers=memory_alloc_for_2DArray(7);
    string line,str;
    ifstream fp(fa->filename);
    if(fp.is_open()){
        getline(fp,line);
        // ss is an object of stringstream that references the S string.
        stringstream ss(line);
        while (getline(ss, str, ',')){
            strcpy(headers[i],str.c_str());
            i++;
        }
    }
    return headers;
}
char **strSplit(string line){
    size_t i=0;
    string str;
    stringstream ss(line);
    char**array=memory_alloc_for_2DArray(line.length());
    while (getline(ss, str, ',')){
        strcpy(array[i],str.c_str());
        i++;
    }
    return array;
}

double calc_median(std::vector<double> vectorForMedian,int count_lines)
{
    double median;
    sort(vectorForMedian.begin(), vectorForMedian.end());
    if((count_lines % 2) ==0)
        median = vectorForMedian[count_lines/2];
    else
        median = (vectorForMedian[count_lines/2] + vectorForMedian[count_lines/2 + 1]) / 2;
    return median;
}
