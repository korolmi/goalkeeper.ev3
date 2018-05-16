#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "robot.h"
#include "mathfunc.h"
#include "params.h"

static int medRes;	// средняя точка попадания мяча на линию ворот
int decision(int result, int distance){
  
  
  if (abs(result)<get_param("далеко")){	// использовать для усреднения будем только "нормальные" результаты
    medRes = (medRes + result)/2;
  }
  printf("in decision(%d)\n",medRes);
  if (distance < get_param("близко")){
    if ( medRes < 0 ){
      printf("MOVE RIGHT (average res is:  %d) \n",medRes);
      return 1;
    }
    else{
      printf("MOVE LEFT (average res is:  %d) \n",medRes);
      return -1;
    }
  }
  return 0;
  
}

void play(){
  
  int deg;
  int counter=0;

  const int n=4;
  int i,mas[n];
  for (i=0; i<n; ++i)
    mas[i]=0;
  int ld,rd,result,decis;

  while(1){
    deg = GetBiDistance(get_param("пауза"));
    if ( deg<0 ){  // ошибка - выдадим диагностику
      SetLed(0, 0, 255);
      printf("BIDIST error: %d\n", deg);
      fflush( stdout );
      usleep(8*get_param("пауза"));
      continue;
    }
    SetLed(0, 255, 0);	// зеленый - меряем дистанцию
    ld = LEFT_DIST(deg);
    rd = RIGHT_DIST(deg);    
    if ( ld<get_param("далеко") && rd<get_param("далеко") ){ // значения отличные от бесконечноти
      mas[counter++] = ld;
      mas[counter++] = rd;
      
      if (counter==4) {
	if (mas[0]==mas[2] || mas[1]==mas[3]) // отфильтровываем повторные значения - они не важны для работы
	  counter=2;
	else{
	  SetLed(1,255,255);
	  result=calcIntersect(200, mas[0], mas[1], mas[2], mas[3]);
	  printf("RESULT(%d,%d->%d,%d): %d \n", mas[0], mas[1], mas[2], mas[3], result);
	  if ( (decis=decision(result, mas[0])) ){
	    SetLed(1,0,0);
	    break;
	  }
	  mas[0]=mas[2]; mas[1]=mas[3];
	  counter = 2;
	} 
      }
    }
    fflush( stdout );
    usleep(4*get_param("пауза"));
  }
  RunRobotTimed(500, decis*500);
  usleep(1000000*get_param("интервал"));
  RunRobotTimed(500, -1*decis*500);
    
  fflush( stdout );
}

int main(int argc, char **argv){
    
  // инициализируем робота
  init_params();
  read_params();
  
  if (InitRobot()) {
    printf("Cannot init");
    return 1;
  }

  while(1){
    play();
  }
  
  ShutRobot();

}
