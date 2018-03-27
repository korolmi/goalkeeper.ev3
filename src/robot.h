// Наши функции управления роботом

int InitRobot(void); 	// инициализация робота, обязательно проверить результат
void ShutRobot(void);	// завершение работы с роботом
int GetDistance(int n_sensor);	// измерить расстояние 
int GetBiDistance(void);	// измерить расстояние двумя датчиками 
int GetSpeed(int n_motor);	// измерить скорость вращения мотора (0=левый, 1=правый) 
void RotateRobot( int angle, int speed);	// повернуть робота на угол с заданной скоростью
void RunRobotTimed( int msecs, int speed);	// проехать прямо заданное время с заданной скоростью
void communicate(char* phrase);	// сказать фразу и вывести ее на терминал


