#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PRINTVEC(A)	printf("<%d, %d, %d>", A.x, A.y, A.z);
#define NEWLINE		putchar('\n');

struct  vec	makevec    (int x, int y, int z);
struct  vec	addvec     (struct vec vecA, struct vec vecB);
struct  vec     crossvec   (struct vec vecA, struct vec vecB);
int		dotvec     (struct vec vecA, struct vec vecB);
double          veclen     (struct vec vecA);
void		exitfunc   (void);

struct vec {
	int	x;
	int	y;
	int	z;
};

int main (int argc, char *argv)
{
	struct vec	vecA, vecB;

	vecA = makevec(4,2,6);
	vecB = makevec(3,1,4);

	printf("Vector A:      ");
	PRINTVEC(vecA);
	printf("\tLength: %.02f", veclen(vecA));
	NEWLINE;

	printf("Vector B:      ");
	PRINTVEC(vecB);
	printf("\tLength: %.02f", veclen(vecB));
	NEWLINE;

	NEWLINE;

	printf("Resultant:     ");
	PRINTVEC(addvec(vecA, vecB));
	NEWLINE;

	printf("Dot Product:   ");
	printf("%d", dotvec(vecA, vecB));
	NEWLINE;

	printf("Cross Product: ");
	PRINTVEC(crossvec(vecA, vecB));
	NEWLINE;
	
	if (atexit(exitfunc) != 0) {
		return 1;
	}
}
/* makevec: make a vector with given x,y,z components */
struct vec makevec (int x, int y, int z)
{
	struct vec	temp = {x, y, z};
	return temp;
}
/* addvec: returns the resultant of two given vectors */
struct vec addvec (struct vec vecA, struct vec vecB)
{
	vecA.x += vecB.x;
	vecA.y += vecB.y;
	vecA.z += vecB.z;
	return vecA;
}
/* crossvec: return cross product of given vectors */
struct vec crossvec (struct vec vecA, struct vec vecB)
{
	struct vec	temp;
	temp.x = (vecA.y * vecB.z) - (vecA.z * vecB.y);
	temp.y = (vecA.z * vecB.x) - (vecA.x * vecB.z);
	temp.z = (vecA.x * vecB.y) - (vecA.y * vecB.x);
	return temp;
}
/* dotvec: returns a vector dotproduct */
int dotvec (struct vec vecA, struct vec vecB)
{
	return (vecA.x * vecB.x) + (vecA.y * vecB.y) + (vecA.z * vecB.z);
}
/* veclen: return length of vector */
double veclen (struct vec vecA)
{
	return sqrt((vecA.x * vecA.x) + (vecA.y * vecA.y) + (vecA.z * vecA.z));
}
/* exitfunc: pause before close */
void exitfunc (void)
{
	printf("\n...EXIT ON RETURN...\n\n");
	getchar();
}
