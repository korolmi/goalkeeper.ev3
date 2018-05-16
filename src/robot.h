// Наши функции управления роботом

int InitRobot(void); 	// инициализация робота, обязательно проверить результат
void ShutRobot(void);	// завершение работы с роботом
int GetDistance(int n_sensor);	// измерить расстояние 
int GetBiDistance(int sleep);	// измерить расстояние двумя датчиками 
int GetSpeed(int n_motor);	// измерить скорость вращения мотора (0=левый, 1=правый) 
void RotateRobot( int angle, int speed);	// повернуть робота на угол с заданной скоростью
void RunRobotTimed( int msecs, int speed);	// проехать прямо заданное время с заданной скоростью
extern void SetLed(int nled, int green, int red); // зажечь LED зеленым и красным цветами (0 = погасить, макс = 255)
void communicate(char* phrase);	// сказать фразу и вывести ее на терминал

// полезные макросы
#define LEFT_DIST(d) (d/10000)
#define RIGHT_DIST(d) (d%10000)

