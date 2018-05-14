#include <math.h>
#include <stdio.h>

// возвращает расстояние до пересечения высотой основания треугольника
double calcX ( int b, int l, int r ){

	double deg;

	deg = acos((double)(b*b + l*l - r*r)/(double)(2*b*l));

	return l * cos(deg);
}

// возвращает катет прямоугольного треугольника по его катету и гипотенузе 
double calcY ( double x, int l){

	return sqrt(l*l-x*x);
}

// возвращает X пересечения прямой оси X
double calcLineIsec ( double x1, double y1, double x2, double y2 ){

	return ((x2*y1 - x1*y2)/(y1 - y2));
}

// возвращает точку пересечения мячиком оси Х (линии ворот)
int calcIntersect( int base, int l1, int r1, int l2, int r2 ){
	printf(" %d %d %d %d \n", l1, r1, l2, r2 );

	double x1, y1, x2, y2, z;

	x1 = calcX( base, l1, r1);
	y1 = calcY( x1, l1);


	x2 = calcX( base, l2, r2);
	y2 = calcY( x2, l2);

	//printf("%f %f %f %f\n", x1,y1,x2,y2);
	z = calcLineIsec ( x1, y1, x2, y2);

	return round(z);

}

