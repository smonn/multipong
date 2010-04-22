/*
 *		  File: vector.h
 *		Author: dan
 * Description: ...
 *	   Created: April 21st 2009, 19:26
 *	   Updated: April 22nd 2009, 21:35 by simon
 */
#ifndef VECTOR_H_
#define VECTOR_H_

/*
 * A simple structure type containing two values. used mainly for positions,
 * distances and lines
 */
typedef struct {
	float x, y;
} t_vector;

/*
 * A line in 2d space represented by a Point and a direction,
 * normal is calculated and set by CreateLine()
 */
typedef struct {
	t_vector point, direction, normal;
} t_line;
/*
 * Adds two vectors and
 * Returns the sum as a t_vector
 */
t_vector Add(t_vector v1, t_vector v2);

/*
 * Subtracts vector v2 from vector v1 and
 * returns the difference as a t_vector
 */
t_vector Subtract(t_vector v1, t_vector v2);


/*
 * takes a vector and a factor and returns the product as a t_vector
 */
t_vector Multiply(t_vector vector, float factor);

/*
 * takes a t_vector as a parameter and
 * returns the length(magnitude) as a float
 *
 * uses sqrt() and is therefore more resource-demanding than
 * LengthSquared(t_vector v), which should be used when possible
 */
float Length(t_vector v);

/*
 * takes a t_vector as a parameter and
 * returns the length(magnitude) squared, as a float
 *
 * almost the same purpose as Length(t_vector v), but this
 * is to be preferred when comparing values, as it is
 * less resource-demanding to square the value to compare it with,
 * than calculating the square root, as Length(t_vector v) does internally
 */
float LengthSquared(t_vector v);

/*
 * calculates and returns the dot (scalar) product of two vectors as a float
 */
float DotProduct(t_vector v1, t_vector v2);

/*
 * Returns the Normalized vector(vector w/ magnitude of 1) of the vector
 * taken as a parameter, as a t_vector, uses sqrt() and shouldn't be used
 * frequently
 */
t_vector Normalized(t_vector v);

/*
 * Returns a t_line
 * takes: point1, point2, any two different points on the line
 * normalDir is a point which the normal will set as positive direction
 * (use the middle of the playing area, important for collision
 * detection and possibly for Bounce as well)
 */
t_line CreateLine(t_vector point1, t_vector point2, t_vector normalDir);

#endif /* VECTOR_H_ */
