#include "../header/vector.h"

#include <math.h>

t_vector Add(t_vector v1, t_vector v2) {
	t_vector temp;
	temp.x = v1.x + v2.x;
	temp.y = v1.y + v2.y;
	return temp;
}

t_vector Subtract(t_vector v1, t_vector v2) {
	t_vector temp;
	temp.x = v1.x - v2.x;
	temp.y = v1.y - v2.y;
	return temp;
}

t_vector Multiply(t_vector vector, float factor){
	t_vector temp;
	temp.x = vector.x * factor;
	temp.y = vector.y * factor ;
	return temp;
}

float Length(t_vector v) {
	return sqrt(v.x*v.x + v.y*v.y);
}

float LengthSquared(t_vector v) {
	return (v.x*v.x + v.y*v.y);
}

float DotProduct(t_vector v1, t_vector v2) {
	return (v1.x * v2.x + v1.y * v2.y);
}

t_vector Normalized(t_vector v){
	t_vector temp;
	float length = Length(v);
	temp.x = v.x/length;
	temp.y = v.y/length;
	return temp;
}

t_line CreateLine(t_vector point1, t_vector point2, t_vector normalDir){
	t_line temp;
	t_vector tempNorm;
	temp.point = point1;
	temp.direction = Normalized(Subtract(point2, point1));
	tempNorm.y = temp.direction.x;
	tempNorm.x = -temp.direction.y;
	tempNorm = Normalized(tempNorm);
	temp.normal = (DotProduct(Subtract(normalDir, temp.point), tempNorm)>0)? tempNorm : Multiply(tempNorm, -1);
	return temp;
}
