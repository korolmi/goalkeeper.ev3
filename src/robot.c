
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

static int isInit;		// признак инициализации робота
// СЕНСОР - у него один файл (для чтения значения)
#define NM_SENSOR 		"/sys/class/lego-sensor/sensor0/value0"
#define ERR_SENSOR 				-1001
static int fdSensor;	// файл чтения значения сенсора

// МОТОРЫ - тут много файлов..
#define ERR_LEFT_MOTOR  		-1002
#define NM_LEFT_MOTOR_SET_POS 	"/sys/class/tacho-motor/motor0/position_sp"
static int fdLeftMotorSetPos;
#define NM_LEFT_MOTOR_SET_SPEED "/sys/class/tacho-motor/motor0/speed_sp"
static int fdLeftMotorSetSpeed;
#define NM_LEFT_MOTOR_COMMAND 	"/sys/class/tacho-motor/motor0/command"
static int fdLeftMotorCommand;
#define ERR_RIGHT_MOTOR  		-1003
#define NM_RIGHT_MOTOR_SET_POS 	"/sys/class/tacho-motor/motor1/position_sp"
static int fdRightMotorSetPos;
#define NM_RIGHT_MOTOR_SET_SPEED "/sys/class/tacho-motor/motor1/speed_sp"
static int fdRightMotorSetSpeed;
#define NM_RIGHT_MOTOR_COMMAND 	"/sys/class/tacho-motor/motor1/command"
static int fdRightMotorCommand;
#define NM_LEFT_MOTOR_SPEED 	"/sys/class/tacho-motor/motor0/speed"
static int fdLeftMotorSpeed;
#define NM_RIGHT_MOTOR_SPEED 	"/sys/class/tacho-motor/motor1/speed"
static int fdRightMotorSpeed;
#define ERR_SPEED		  		-1004


// инициализация робота - главным образом открытие всех необходимых файлов
int InitRobot(void){

	if ( isInit ){ // файл уже открыт, значит робот был проинициализирован
		return 0;
	}
	isInit = 1;

	// открываем файлы
	fdLeftMotorSetPos = open(NM_LEFT_MOTOR_SET_POS,O_WRONLY);
	fdRightMotorSetPos = open(NM_RIGHT_MOTOR_SET_POS,O_WRONLY);
	fdLeftMotorSetSpeed = open(NM_LEFT_MOTOR_SET_SPEED,O_WRONLY);
	fdRightMotorSetSpeed = open(NM_RIGHT_MOTOR_SET_SPEED,O_WRONLY);
	fdLeftMotorCommand = open(NM_LEFT_MOTOR_COMMAND,O_WRONLY);
	fdRightMotorCommand = open(NM_RIGHT_MOTOR_COMMAND,O_WRONLY);
	if ( fdLeftMotorCommand<0 || fdLeftMotorSetSpeed<0 || fdLeftMotorSetPos<0 ){
		return ERR_LEFT_MOTOR;
	}
	if ( fdRightMotorCommand<0 || fdRightMotorSetSpeed<0 || fdRightMotorSetPos<0 ){
		return ERR_RIGHT_MOTOR;
	}

	return 0;
}

// остановка робота - закрытие файлов
void ShutRobot(void){

	if ( isInit ){
		close(fdLeftMotorCommand);
		close(fdLeftMotorSetSpeed);
		close(fdLeftMotorSetPos);
		close(fdRightMotorCommand);
		close(fdRightMotorSetSpeed);
		close(fdRightMotorSetPos);
		isInit = 0;
	}

}

// получить расстояние (в нативных единицах)
int GetDistance(void){

	char buf[10];

	fdSensor = open(NM_SENSOR,O_RDONLY);
	if ( fdSensor<0 || read(fdSensor,buf,10)<0 ){
		return ERR_SENSOR;
	}
	close(fdSensor);

	return atoi(buf);
	
}

// получить скорость мотора (в нативных единицах)
int GetSpeed(int n_motor){

	char buf[10];
	int fd;

	fd = (n_motor==0) ? open(NM_LEFT_MOTOR_SPEED,O_RDONLY) : open(NM_RIGHT_MOTOR_SPEED,O_RDONLY);
	if ( fd<0 || read(fd,buf,10)<0 ){
		return ERR_SPEED;
	}
	close(fd);

	return atoi(buf);
	
}

// повернуть робота на заданный угол
void RotateRobot( int angle, int speed){

	char speedStr[10], angleStr[10];
	int dst;

	// пересчитаем расстояние
	dst = 505 * angle / 90;

	// устанавливаем скорость
	sprintf(speedStr, "%d", speed);
	write(fdLeftMotorSetSpeed,speedStr,strlen(speedStr));
	write(fdRightMotorSetSpeed,speedStr,strlen(speedStr));

	// устанавливаем счетчик
	sprintf(angleStr,"%d",dst);
	write(fdLeftMotorSetPos,angleStr,strlen(angleStr));
	sprintf(angleStr,"%d",-1*dst);
	write(fdRightMotorSetPos,angleStr,strlen(angleStr));

	// запускаем моторы
	write(fdLeftMotorCommand,"run-to-rel-pos",14);
	write(fdRightMotorCommand,"run-to-rel-pos",14);

	//sleep(1);
	while (1){
		if ( GetSpeed(0)!=0 && GetSpeed(1)!=0 ) break;
	}
	
}

#ifdef TEST // для тестирования функций

int main()
{
	
	if ( InitRobot() ){
		printf("Cannot init");
		return 1;
	}

	RotateRobot ( 90, 100 );
	//sleep(1);
	printf("SPEED: %d\n", GetSpeed(0));
	sleep(10);
	RotateRobot ( -90, 100 );
	printf("SPEED: %d\n", GetSpeed(0));
	printf("Distance = %d\n", GetDistance());
	
    return 0;
}

#endif
