/*
 * Модуль работы с параметрами, записанными в файл
 * Файл имеет вид
 *
 *   параметр = значение
 * 
 * предоставляет методы
 * 
 *   read_params() - читает значения параметров из файла (вызывается один раз)
 *   get_param ( name ) -  получения значения параметра по его имени
 *   list_params () - список известных модулю параметров
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Перечень параметров (формат имя_параметра : описание параметра)
#define n_pars 8
char* par_names[n_pars] = {
    "ждем мячика: время, отведенное на установку мячика (секунд)",
    "зазор: разница в расстоянии до ближайшего предмета до и после устновки мячика (сантиметры)",
    "доворот: на сколько нужно довернуть робота при развороте на мячик (градусы)",
    "сектор: сектор сканирования (градусы)",
    "пауза: время между измерениями расстояния в процессе убегания (микросекунды)",
    "близко: расстояние, после которого уже нужно убегать от мячика (сантиметры)",
    "далеко: очень далекое расстояние (бесконечность) (сантиметры)",
    "мимо: интервал, который мы считаем говорит нам о том, что мячик катится мимо (сантиметры)"
};
// Значения по умолчанию (на тот случай, если в файле нет данных для параметра)
int def_vals[n_pars] = { 7, 10, 7, 60, 1000, 500, 10000, 10 };
int par_vals[n_pars]; // сами значения - появятся после чтения параметров из файла

// Файл, в котором лежат параметры
char* par_file = "params.dat";

void init_params(){

  int i;

  for(i=0; i<n_pars; i++)
      par_vals[i] = def_vals[i];

}

void list_params(){

  int i;
  
  printf("\nМодуль работы с параметрами различает следующие параметры:\n\n");
  for(i=0; i<n_pars; i++){
    printf("[%d] %s\n",par_vals[i], par_names[i]);
  }
  printf("\n(имя параметра находится слева от двоеточия)\n");
  
}

// возвращает индекс параметра по его имени (-1 если не найден)
int get_index( char* pname){
  
  char par_name[1024];
  int found = 0, i;

  for(i=0; i<n_pars;i++){
    sscanf ( par_names[i], "%[^:]", par_name );
    if ( strlen(pname)==strlen(par_name) && strstr(par_name,pname)!=NULL ){
      found = 1;
      break;
    }
  }
  if ( found )
    return i;
  else
    return -1;
}

// присваивает значение параметру, если параметр не известен - выдает -1
// предполагаем, что значение - целое
int set_param ( char* pname, char* val){

  int i;

  i = get_index(pname);
  if ( i>=0 ){
    par_vals[i] = atoi(val);
    return 0;
  }
  else
    return -1;
}

void trim( char* from ){

  char *p, *e, to[1024];

  p = from;
  while(isspace(*p)) p++;
  strcpy(to,p);
  e = to + strlen(to) - 1;
  while(isspace(*e)) *e-- = 0;
  strcpy(from,to);
  
}

// читает параметры из файла params.dat
void read_params(){
  
  int i;
  char buf[1024], val[100], name[100], bb[10], tmp[1024], *pp, *p;
  FILE *fp;
  
  fp = fopen ( par_file, "rt" );
  if ( fp!=NULL ){
    while ( fgets(buf, 10000, fp)!=NULL) {
      // printf("%s\n",buf);
      if ( (p = strchr(buf,'#'))!=NULL )
	*p = 0;
      trim(buf);
      if (strlen(buf)<3)
	continue;
      sscanf(buf,"%[^=]%[=]%[^\n]",name,bb,val);
      trim(name);
      trim(val);
      // printf(">%s<>%s<\n",name,val );
      if ( set_param(name, val)<0 )
	printf("Параметр %s не известен\n", name);
    }
    fclose(fp);
  }
}

// возвращает значение параметра по его имени, -1 если параметр с таким именем не найден
int get_param ( char* name ){

  int i;

  i = get_index(name);

  if ( i==-1)
    return -1;
  else
    return par_vals[i];
  
}

#ifdef WITH_MAIN
main(){
  init_params();
  read_params();
  list_params();
  printf("Параметр %s = %d\n", "пауза", get_param("пауза"));
  printf("Параметр %s = %d\n", "чей-то", get_param("чей-то"));
}
#endif
