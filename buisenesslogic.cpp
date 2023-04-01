#include "buisenesslogic.h"

FuncReturningValue* getDataFromFile(const char *filename);
FuncReturningValue* solve(FuncArgument* fa);
void clean(FuncArgument* args);
char** read_csv(const char* filename, size_t *lines);
void clean2DArray(char**, size_t );
void clean3DArray(char***, size_t, size_t);
char** strSplit(string line,size_t*);
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
    size_t lines,fields=0;
    FuncReturningValue *frv = (FuncReturningValue *)malloc(sizeof(FuncReturningValue));
    if (frv!=NULL) {
        char **rawData = read_csv(filename, &lines);
            if (rawData!=NULL) {
                char ***data = (char***)malloc((lines - 1) * sizeof(char**));
                if (data!=NULL) {
                    if (*rawData!=NULL) {
                        string str(*rawData);
                        count(str.begin(), str.end(), ',')==0? frv->error=3:0;
                        char **headers = strSplit(*rawData,&fields);
                        if (headers!=NULL) {
                            for (size_t i = 0; i < lines - 1; i++){
                                *(data+i) = strSplit(*(rawData+i+1),&fields);
                            }
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
    }
    return frv;
}
void copyMas(char** newMas, char** mas, int count){
    for (int i = 0; i < count ; i++){
        strcpy(newMas[i], mas[i]);
    }
}

FuncReturningValue* solve(FuncArgument* fa)
{
    size_t count_lines=0;
    double min=calc_min(fa),max=calc_max(fa),current;
    std::vector<double> vectorForMedian;
    FuncReturningValue *frv = (FuncReturningValue *)malloc(sizeof(FuncReturningValue));
    if (frv!=NULL)
    {
        char ***data_for_chosen_region=memory_alloc_for_3DArray(fa);
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
        if (count_lines!=0){
            frv->headers=get_headers(fa);
            frv->fields_num=fa->fields_num;
            frv->len=count_lines;
            frv->data=data_for_chosen_region;
            frv->solution_min=min;
            frv->solution_max=max;
            frv->solution_median=calc_median(vectorForMedian,vectorForMedian.size());}
        else
            frv->error=4;
        //clean3DArray(data_for_chosen_region,fa->fields_num,count_lines);
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

double calc_median(std::vector<double> vectorForMedian,int count_lines)
{
    double median;
    sort(vectorForMedian.begin(), vectorForMedian.end());
    if((count_lines % 2) ==1)
        median = vectorForMedian[count_lines/2];
    else
        median = (vectorForMedian[count_lines/2-1] + vectorForMedian[count_lines/2]) / 2;
    return median;
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
    ifstream fp(filename);
    if(!fp.is_open())
        return NULL;
    char **data = (char **)calloc(max_size, sizeof(char *));
    if (data == NULL)
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
    fp.close();
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
        for (int i = 0; i < n; i++)
        {
            *(data+i) = (char *)malloc(sizeof(char) * STRLEN);
        }
    }
    return data;
}
char**get_headers(FuncArgument* fa)
{
    size_t counter=0,max_size=1,llen;
    char **headers=(char **)calloc(max_size, sizeof(char *));
    string line,str;
    ifstream fp(fa->filename);
    if(fp.is_open()){
        getline(fp,line);
        // ss is an object of stringstream that references the S string.
        stringstream ss(line);
        while (getline(ss, str, ',')){
            if (counter >= max_size-1)
            {
                headers = (char **)realloc(headers,max_size * 2 * sizeof(char *));
                if (headers != NULL)
                    max_size *= 2;
                else
                    return NULL;
            }
            llen = str.length();
            *(headers+counter) = (char *)calloc(sizeof(char), llen+1);
            strcpy(*(headers+counter), str.c_str());
            counter++;
        }
    }
    return headers;
}

char **strSplit(string line,size_t*counter){
    size_t num=0;
    string str,tmp=line;
    stringstream ss(line);
    //+1 потому что слов на 1 больше, чем разделителей
    char**array=(char **)calloc(count(line.begin(), line.end(), ',')+1, sizeof(char *));
    while (getline(ss, str, ',')){
        *(array+num) = (char *)malloc(sizeof(char) * str.length());
        strcpy(array[num],str.c_str());
        num++;
    }
    *counter=num;
    return array;
}
