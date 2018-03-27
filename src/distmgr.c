
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "robot.h"
#include "params.h"

int dists[5000];	// массив расстояний
int c_dists[5000];	// массив сглаженных расстояний
int forms[100];         // массив форм
int f_dists[100];       // массив минимальных расстояний до форм
int get_forms(int brk){
  // проходит по сглаженным расстояниям и выделяет формы:
  // находит начало и конец выбросов больше, чем на brk
  // пишет результаты в массив forms (пары - начало с плюсом, конец - с минусом)

  int i,iUp,f,dCur,mDist,md;

  dCur = c_dists[0];
  iUp = 0;
  f = 0;
  md = 0;
  mDist = 10000;
  
  for(i=1;i<5000;i++){
    if (c_dists[i] == 0)
      break;

    if ( dCur-c_dists[i]>brk ){ // скачок к датчику
      iUp = i;
      mDist = c_dists[i];
    }
    if ( iUp && c_dists[i]<mDist ) // запоминаем минимум
      mDist = c_dists[i];      
    if ( c_dists[i]-dCur>brk ){ // скачок от датчика
      if ( iUp ){ // закончился предмет, сохраняем в формах
	forms[f++] = iUp;
	forms[f++] = -1 * (i-1);
	f_dists[md++] = mDist;
	iUp = 0;
      }
    }
    
    dCur = c_dists[i];
  }

  return (int)f/2;

}

// сканирование пространства, заполнение массива расстояний dists
void e_scan(int deg){
  
  int i = 0;
  
  RotateRobot(deg, 100);
  while (1) {
    dists[i++] = GetDistance(0);
    usleep(30000);
    if (i == 5000)
      break;
    if (GetSpeed(0) == 0)
      break;
  }
  RotateRobot(-1 * deg, 100);

}

// находит угол поворота на элемент массива dists, который есть "центр" мячика
int minAngle(int deg, int r){

  int s1 = 0,
    s2 = 0,
    t, index;

  for (t = 0; t < 5000; ++t) {
    if (dists[t] == 0)
      break;
    if (dists[0] - dists[t] > r && s1 == 0)
      s1 = t;
    if (dists[0] - dists[t] < r && s1 != 0 && s2 == 0)
      s2 = t;
  }
  index = s1 + (s2 - s1) / 2;

  return index * deg / t;

}

// находит минимальное расстояние в массиве расстояний dists
int minDist(){
  
    int i,
        mDist = 10000;	// заменить на константу
    
    for (i = 0; i < 5000; i++) {
    	if (dists[i] == 0)
	    break;
	if (dists[i] <= mDist)
	    mDist = dists[i];	
    }
    return mDist;
    
}

void ch_dists(int brk){
  // идем по ходу поворота
  // если был выброс (больше brk), то усредняем
  // выброс считается от сглаженной (новой) точки
  // усреднение - по новой и будущей точкам

    int i;

    c_dists[0] = dists[0];
    for (i = 1; i < 5000; i++) {
      if (dists[i] == 0) // как только пошли нули - значит данные кончились
	break;
      if ( abs(dists[i]-c_dists[i-1])>brk )	// выброс - усредняем
	c_dists[i] = (c_dists[i-1]+dists[i+1])/2;
      else
	c_dists[i] = dists[i];
    }
    
}

#ifdef TEST
int main(){

  int r = 0, deg;
  
  // читаем параметры из файла params.dat
  init_params();
  read_params();
  deg = get_param("сектор");
  
  if (InitRobot()) {
    printf("Cannot init");
    return 1;
  }

  for ( r=0; r<1000; r++ ){
    deg = GetBiDistance();
    printf("BIDIST: %d %d\n", deg / 10000, deg % 10000);
    fflush( stdout );
    usleep(50000);
  }
  return(0);
  
  e_scan(deg);
ch_dists(100);
while (1) {
printf("%d %d %d\n", r, dists[r], c_dists[r]);
    if (dists[r] == 0)
      break;
    ++r;
  }
  ShutRobot();
}


#endif
