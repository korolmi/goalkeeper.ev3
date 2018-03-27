#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "robot.h"
#include "params.h"
int             maxf = 400;	// максимальное
				// расстояние при
				// сканировании земли
int             minf;		// здесь храним
				// минимальное
				// расстояние, которое
				// находит следующая
				// функция
int             indf;		// здесь храним угол, на
				// который нужно
				// повернуться, чтобы
				// нацелиться на
				// ближайшую точку
				// (также побочный
				// результат следующей
				// функции)
void
min_search(int deg)
{
    int             c[5000];
    int             i = 0,
	y,
	ii;

    RotateRobot(deg, 100);
    while (1) {
	c[i++] = GetDistance();
	if (i == 5000)
	    break;
	if (GetSpeed(0) == 0)
	    break;
    }
    for (y = 0; y < i; y++) {
	if (c[y] > maxf)
	    maxf = c[y];
	if (c[y] <= minf) {
	    minf = c[y];
	    ii = y;
	}
    }
    indf = (deg * ii) / i;
}

int
main(int argc, char **argv)
{

    int             m,
                    w_ball;
    int             min1;
    int             deg;
    char            wbuf[1000];

    // инициализируем робота
    if (InitRobot()) {
	printf("Cannot init");
	return 1;
    }
    RunRobotTimed(2000, 1000);
    exit(0);
    
    // читаем параметры из файла params.dat
    init_params();
    read_params();
    w_ball = get_param("ждем мячика");
    m = get_param("доворот");
    minf = get_param("далеко");
    deg = get_param("сектор");

    // ###################### НАХОДИМ МЯЧИК
    // ###########################
    // сканируем местность, определяем
    // расстояние до ближайшего предмета
    communicate("scanning earth");
    int             earthEnd,
                    ballDist,
                    ballAngle;
    e_scan(deg);
    earthEnd = minDist();
    sprintf(wbuf, "distance till earth end is %d millimeters", earthEnd);
    communicate(wbuf);

    // чуть подождем и еще раз просканируем 
    // 
    // местность - поищем "мячик"
    sprintf(wbuf, "will search ball in %d seconds", w_ball);
    communicate(wbuf);
    sleep(w_ball);
    communicate("scanning for ball");
    e_scan(deg);
    ballDist = minDist();
    int             r = get_param("зазор");	// то
    // минимальное 
    // расстояние, 
    // позволяющее 
    // нам
    // понять,
    // что
    // мячик
    // появился
    if (r <= earthEnd - ballDist) {	// мячик есть,
	// поворачиваем
	// робота на него
	sprintf(wbuf, "ball is %d milimeters away", ballDist);
	communicate(wbuf);
	ballAngle = minAngle(deg, r);
    printf("ball angle is %d", ballAngle);
	sprintf(wbuf, "turning on ball you can hit ball in %d seconds",
		w_ball);
	communicate(wbuf);
	RotateRobot(ballAngle, 100);
	sleep(w_ball);
    } else {			// мячика нет...
	communicate("no ball found");
	//communicate("turning back");
	//RotateRobot(-1 * deg, 100);
	exit(1);
    }

    //exit(0);

    // ###################### УБЕГАЕМ ОТ МЯЧИКА
    // ###########################

    int             dist,	// текущее расстояние
                    old_dist,	// предыдущее
	// расстояние
                    min_dist,	// отсюда уже надо
	// убегать
                    sp_intvl;	// предельное изменение 
				// 
    // скорости

    sp_intvl = get_param("мимо");
    old_dist = get_param("далеко");;
    min_dist = get_param("близко");
    int             sl_time = get_param("пауза");	// спим
    // столько
    // микросекунд 
    // между
    // замерами 
    // расстояний

    communicate("you can hit the ball now");
    int             counter = 0;
    int             counter1 = 0;	// счетчик
    // увеличивания
    // расстояния до
    // мяча

    // в цикле смотрим за скоростью
    // приближения "мячика" к нам
    while (1) {
	dist = GetDistance();
	usleep(sl_time);
	printf("расстояние=%d\n", dist);
	if (dist <= min_dist) {	// мячик слишком
	    // приблизился, пора
	    // убегать
	    // communicate("go");
        printf("go\n");
	    counter += 1;
	    break;
	   }
    
	if (dist - old_dist >= sp_intvl)	// скорость
	    // слишком
	    // изменилась,
	    // значит летит 
	    // мимо...
	    counter1 += 1;
	else
	    counter1 = 0;
	if (counter1 >= 10) {
	    communicate("you missed me");
	    break;
	}

    }
	if (counter > 0) {
	    RotateRobot(90, 500);
	    while (1)
		if (GetSpeed(0) == 0)
		    break;
	    RunRobotTimed(2000, 500);
	    while (1)
		if (GetSpeed(0) == 0)
		    break;
	    RotateRobot(-90, 500);
	    counter = 0;
	}

    
    ShutRobot();

}
