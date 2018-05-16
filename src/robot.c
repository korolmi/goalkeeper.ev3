
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

static int      isInit;		// признак инициализации робота

#define NM_LEFT_SENSOR_MODE	0
#define NM_RIGHT_SENSOR_MODE    1
#define NM_LEFT_SENSOR 		2
#define NM_RIGHT_SENSOR        	3
#define NM_LEFT_MOTOR_SET_POS	4
#define NM_LEFT_MOTOR_SET_TIMER 5
#define NM_LEFT_MOTOR_SET_SPEED 6
#define NM_LEFT_MOTOR_COMMAND 	7
#define NM_RIGHT_MOTOR_SET_POS 	8
#define NM_RIGHT_MOTOR_SET_TIMER 9
#define NM_RIGHT_MOTOR_SET_SPEED 10
#define NM_RIGHT_MOTOR_COMMAND 	11
#define NM_LEFT_MOTOR_SPEED 	12
#define NM_RIGHT_MOTOR_SPEED 	13
#define NM_LEFT_LED_GREEN 	14
#define NM_LEFT_LED_RED 	15
#define NM_RIGHT_LED_GREEN 	16
#define NM_RIGHT_LED_RED 	17

static char*	MODE_PING = "US-SI-CM"; // режим однократного измерения
static char*	MODE_CONT = "US-DIST-CM"; // режим постоянного измерения

// имена файлов (начинаются с признака - читаем или пишем в файл
char * devFNames[] = {
  "w/sys/class/lego-sensor/sensor0/mode",
  "w/sys/class/lego-sensor/sensor1/mode",
  "r/sys/class/lego-sensor/sensor0/value0",
  "r/sys/class/lego-sensor/sensor1/value0",
  "r/sys/class/tacho-motor/motor0/position_sp",
  "w/sys/class/tacho-motor/motor0/time_sp",
  "w/sys/class/tacho-motor/motor0/speed_sp",
  "w/sys/class/tacho-motor/motor0/command",
  "w/sys/class/tacho-motor/motor1/position_sp",
  "w/sys/class/tacho-motor/motor1/time_sp",
  "w/sys/class/tacho-motor/motor1/speed_sp",
  "w/sys/class/tacho-motor/motor1/command",
  "r/sys/class/tacho-motor/motor0/speed",
  "r/sys/class/tacho-motor/motor1/speed",
  "w/sys/class/leds/ev3:left:green:ev3dev/brightness",
  "w/sys/class/leds/ev3:left:red:ev3dev/brightness",
  "w/sys/class/leds/ev3:right:green:ev3dev/brightness",
  "w/sys/class/leds/ev3:right:red:ev3dev/brightness"
};
#define NDEVS 18	// пока не используем LEDs
int devFDs[NDEVS];
#define DEV_ERR(n)	(-1 * ( 1000 + n ))

extern void SetLed(int nled, int green, int red);
  
// инициализация робота - главным образом 
// открытие всех необходимых файлов
int InitRobot(void){

  int i;
  
  if (isInit) { // значит робот проинициализирован
    return 0;
  }
  isInit = 1;

  // открываем файлы
  for(i=0;i<NDEVS;i++){
    if ( devFNames[i][0]=='r' )
      devFDs[i] = open ( devFNames[i]+1, O_RDONLY );
    else
      devFDs[i] = open ( devFNames[i]+1, O_WRONLY );
    if ( devFDs[i]<0 )
      return DEV_ERR(i);
  }

  SetLed( 0, 0, 0);
  SetLed( 1, 0, 0);
  
  return 0;
}

// остановка робота - закрытие файлов
void ShutRobot(void){

  int i;

  if (isInit) {
    for(i=0;i<NDEVS;i++){
      close(devFDs[i]);
    }
    isInit = 0;
  }
  
}

// получить расстояние (в нативных единицах)
// при этом надо не забыть переключить режим датчиков (иначе может быть считано только последнее значение)
// и закрыть файлы...
int GetDistance(int n_sensor){

  char buf[10];
  int fdSensor, res;

  fdSensor = open(devFNames[n_sensor ? NM_RIGHT_SENSOR : NM_LEFT_SENSOR], O_RDONLY);
  if (fdSensor < 0 || read(fdSensor, buf, 10) < 0) {
    res = DEV_ERR((n_sensor ? NM_RIGHT_SENSOR : NM_LEFT_SENSOR));
  }
  else{
    res = atoi(buf);
  }
  close(fdSensor);
  
  return res;

}

// получить расстояние (в нативных единицах) с пары сенсоров
// при этом автоматом происходит переключение в режим пинга
// возвращает левую дистанцию * 10000 + правую дистанцию
// в заголовочном файле есть макросы:
//#define LEFT_DIST(d) (d/10000)
//#define RIGHT_DIST(d) (d%10000)
int GetBiDistance( int sleep ){

  char buf[10];
  int res, biDist;

  // переключаем датчики в режим пинга
  res = 0;
  if ( write(devFDs[NM_LEFT_SENSOR_MODE], MODE_PING, 10) < 0) {
    res = DEV_ERR(NM_LEFT_SENSOR_MODE);
  }
  if ( res ) return res;

  usleep(sleep);
  
  if ( write(devFDs[NM_RIGHT_SENSOR_MODE], MODE_PING, 10) < 0) {
    res = DEV_ERR(NM_RIGHT_SENSOR_MODE);
  }
  if ( res ) return res;

  //usleep(10000);
  
  // считываем значения
  lseek(devFDs[NM_LEFT_SENSOR],0,SEEK_SET);
  if ( read(devFDs[NM_LEFT_SENSOR], buf, 10) < 0) {
    res = DEV_ERR(NM_LEFT_SENSOR);
  }
  else{
    res = atoi(buf);
  }
  if ( res < 0 ) return res;
  biDist = res * 10000;

  lseek(devFDs[NM_RIGHT_SENSOR],0,SEEK_SET);
  if ( read(devFDs[NM_RIGHT_SENSOR], buf, 10) < 0) {
    res = DEV_ERR(NM_RIGHT_SENSOR);
  }
  else{
    res = atoi(buf);
  }
  if ( res < 0 ) return res;

  return biDist + res;

}

// получить скорость мотора (в нативных единицах)
int GetSpeed(int n_motor){

    char buf[10];

    if ( read(devFDs[NM_LEFT_MOTOR_SPEED+n_motor], buf, 10) < 0) {
      return DEV_ERR(NM_LEFT_MOTOR_SPEED+n_motor);
    }
    return atoi(buf);

}

// повернуть робота на заданный угол
void RotateRobot(int angle, int speed){

    char speedStr[10], angleStr[10];
    int dst;

    // пересчитаем расстояние
    dst = 505 * angle / 90;

    // устанавливаем скорость
    sprintf(speedStr, "%d", speed);
    write(devFDs[NM_LEFT_MOTOR_SET_SPEED], speedStr, strlen(speedStr));
    write(devFDs[NM_RIGHT_MOTOR_SET_SPEED], speedStr, strlen(speedStr));

    // устанавливаем счетчик
    sprintf(angleStr, "%d", dst);
    write(devFDs[NM_LEFT_MOTOR_SET_POS], angleStr, strlen(angleStr));
    sprintf(angleStr, "%d", -1 * dst);
    write(devFDs[NM_RIGHT_MOTOR_SET_POS], angleStr, strlen(angleStr));

    // запускаем моторы
    write(devFDs[NM_LEFT_MOTOR_COMMAND], "run-to-rel-pos", 14);
    write(devFDs[NM_RIGHT_MOTOR_COMMAND], "run-to-rel-pos", 14);

    while (1) {
      if (GetSpeed(0) != 0 && GetSpeed(1) != 0)
	break;
    }

}

// двигаться миллисекунд с нужной скоростью
void RunRobotTimed(int msecs, int speed){

    char speedStr[10], timeStr[10];
    int dst;

    // устанавливаем скорость
    sprintf(speedStr, "%d", speed);
    write(devFDs[NM_LEFT_MOTOR_SET_SPEED], speedStr, strlen(speedStr));
    write(devFDs[NM_RIGHT_MOTOR_SET_SPEED], speedStr, strlen(speedStr));

    // устанавливаем время
    sprintf(timeStr, "%d", msecs);
    write(devFDs[NM_LEFT_MOTOR_SET_TIMER], timeStr, strlen(timeStr));
    write(devFDs[NM_RIGHT_MOTOR_SET_TIMER], timeStr, strlen(timeStr));

    // запускаем моторы
    write(devFDs[NM_LEFT_MOTOR_COMMAND], "run-timed", 9);
    write(devFDs[NM_RIGHT_MOTOR_COMMAND], "run-timed", 9);

    /*
    while (1) {
	if (GetSpeed(0) != 0 && GetSpeed(1) != 0)
	    break;
    }
    */

}

// работа со светодиодами
void SetLed(int nled, int green, int red){

  char gbrt[4], rbrt[4];

  sprintf(gbrt,"%d",green);
  sprintf(rbrt,"%d",red);
  
  if ( nled ){ // правый
    write(devFDs[NM_RIGHT_LED_GREEN],gbrt,strlen(gbrt));
    write(devFDs[NM_RIGHT_LED_RED],rbrt,strlen(rbrt));
  }
  if ( nled==0 ){ // левый
    write(devFDs[NM_LEFT_LED_GREEN],gbrt,strlen(gbrt));
    write(devFDs[NM_LEFT_LED_RED],rbrt,strlen(rbrt));
  }

}

void communicate(char *phrase){

    char            buf[1000];
    buf[0] = 0;

    strcat(buf, "./speak.sh ");
    strcat(buf, phrase);
    strcat(buf, " 2>/dev/null");
    system(buf);
    printf("ROBOT: %s\n", phrase);

}

#ifdef WITH_MAIN		// для тестирования
				// функций

int
main()
{

    if (InitRobot()) {
	printf("Cannot init");
	return 1;
    }

    communicate("scanning earth");

    RunRobotTimed(2000, 100);

    /*
     * RotateRobot ( 90, 100 ); //sleep(1); printf("SPEED: %d\n",
     * GetSpeed(0)); sleep(10); RotateRobot ( -90, 100 ); printf("SPEED:
     * %d\n", GetSpeed(0)); printf("Distance = %d\n", GetDistance()); 
     */

}

#endif
