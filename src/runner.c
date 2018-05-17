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
  printf("in decision(med=%d,dist=%d)\n",medRes,distance);
  if (distance < get_param("близко")){
    if ( medRes<(SENSORS_DIST/2) && medRes >= -1*(SENSORS_DIST+10*get_param("мячик")) ){
      printf("MOVE RIGHT (average res is:  %d) \n",medRes);
      return 1;
    }
    if ( medRes>=(SENSORS_DIST/2) && medRes <= (2*SENSORS_DIST+10*get_param("мячик")) ){
      printf("MOVE LEFT (average res is:  %d) \n",medRes);
      return -1;
    }
  }
  return 0;
  
}

void play(){
  
  int dist;
  int counter=0, missCounter=0;
  int ld,rd,result,decis=0;

  const int n=4;
  int i,mas[n];
  for (i=0; i<n; ++i)
    mas[i]=0;

  medRes = SENSORS_DIST/2;	// обнулим расчетную точку прилета мячика

  while(1){
    dist = GetBiDistance(get_param("пауза"));
    // printf("%d\n",dist);

#ifdef SIMU
    if ( dist==0 ) break;
#endif

    if ( dist<0 ){  // ошибка - выдадим диагностику
      SetLed(1, 0, 255);
      printf("BIDIST error: %d\n", dist);
      fflush( stdout );
      usleep(10*get_param("пауза"));
      SetLed(1, 0, 0);
      usleep(10*get_param("пауза"));
      SetLed(1, 0, 255);
      usleep(10*get_param("пауза"));
      continue;
    }
    SetLed(0, 255, 0);	// зеленый - меряем дистанцию
    SetLed(1, 0, 0);	// потушим ошибку
    ld = LEFT_DIST(dist);
    rd = RIGHT_DIST(dist);    
    if ( ld<get_param("далеко") && rd<get_param("далеко") ){ // значения отличные от бесконечноти
      mas[counter++] = ld;
      mas[counter++] = rd;
      
      if (counter==4) {
	  
	if (mas[0]==mas[2] || mas[1]==mas[3]){ // отфильтровываем повторные значения - они не важны для работы
	  counter=2;
	  //if (missCounter>10){ // скорее всего, мячик остановился
	  //  medRes = SENSORS_DIST/2;
	  //  missCounter = 0;
	  //  printf("RESET medRes\n");
	  //  SetLed(0,0,0);
	  //  usleep(10*get_param("пауза"));
	  //}
	  missCounter++;
	}
	else{
	  SetLed(0,255,255);
	  result=calcIntersect(200, mas[0], mas[1], mas[2], mas[3]);
	  printf("RESULT(%d,%d->%d,%d): %d \n", mas[0], mas[1], mas[2], mas[3], result);
	  if ( (decis=decision(result, mas[3])) ){
	    SetLed(0,0,255);
	    break;
	  }
	  mas[0]=mas[2]; mas[1]=mas[3];
	  counter = 2;
	}
      }
    }
    else{
      if (missCounter>100){ // скорее всего, мячик ушел из поля зрения
	medRes = SENSORS_DIST/2;
	missCounter = 0;
	printf("RESET medRes (2)\n");
	SetLed(0,0,0);
	usleep(10*get_param("пауза"));
      }
      missCounter++;
    }
    fflush( stdout );
    usleep(4*get_param("пауза"));
  }
  RunRobotTimed(500, decis*1000);
  usleep(1000000*get_param("интервал"));
  RunRobotTimed(500, -1*decis*1000);
  SetLed(0, 0, 0);	// погасили все - игра закончена
    
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
    printf("ROUND STARTED=====================\n");
#ifdef SIMU
    communicate(NULL);
#else
    communicate("You can hit the ball now");
#endif
    play();
    printf("ROUND ENDED=======================\n");
    usleep(1000000*get_param("интервал"));
  }
  
  ShutRobot();

}
