// Наши функции управления роботом

int InitRobot(void); 	// инициализация робота, обязательно проверить результат
void ShutRobot(void);	// завершение работы с роботом
int GetDistance(void);	// измерить расстояние 
int GetSpeed(int n_motor);	// измерить скорость вращения мотора (0=левый, 1=правый) 
void RotateRobot( int angle, int speed);	// повернуть робота на угол с заданной скоростью

