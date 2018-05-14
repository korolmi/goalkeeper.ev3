#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "robot.h"
#include "mathfunc.h"
#include "params.h"

void decision(int result, int distance){
              printf("decision\n");
              if (abs(result)<700)
              result+=result;
              if(distance < 500){
                result > 0 ? printf("MOVE RIGHT (average res is:  %d) \n",result) : printf("MOVE LEFT (average res is:  %d) \n",result);
              } 
  // result > 0 ? RunRobotTimed(10, 500) : RunRobotTimed(10, -500);
}

int main(int argc, char **argv){

    
    // инициализируем робота
    int deg;

  init_params();
  read_params();
  deg = get_param("сектор");
  
  if (InitRobot()) {
    printf("Cannot init");
    return 1;
  }
  int counter=0;

  const int n=4;
  int i,mas[n];
  for (i=0; i<n; ++i)
    mas[i]=0;
  int result;

  while(1){
    deg = GetBiDistance();
    if ( deg / 10000 < 0 || deg % 10000 <0 )
        printf("BIDIST: %d %d\n", deg / 10000, deg % 10000);
    if (deg / 10000 < 2500 && deg / 10000 > 0 && deg % 10000 < 2500 && deg % 10000 > 0){
        mas[counter++] = deg / 10000;
        mas[counter++] = deg % 10000;

        if (counter==4) {
            if (mas[0]==mas[2] || mas[1]==mas[3])
                counter=2;
            else
            {
              result=calcIntersect(200, mas[0], mas[1], mas[2], mas[3]);
              printf("RESULT: %d \n", result);
              decision(result, mas[0]);
              mas[0]=mas[2]; mas[1]=mas[3];
              counter = 2;
            } 
        }
    }
    fflush( stdout );
    usleep(50000);
  }
  //printf("result: %d \n", result);
    
    ShutRobot();

}
