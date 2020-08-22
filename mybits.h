#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

//Estructura de dato a+bi
typedef struct CPX {
	float r;
	float i;
} cplx;

//Operaciones con complejos
cplx sum (cplx,cplx);
cplx res (cplx,cplx);
cplx tim (cplx,cplx);
cplx special_tim(cplx,cplx);
void coutcplx (cplx,int);

void coutcplx (cplx a,int b)
{
	if (!b) printf("%f+i%f",a.r,a.i);
	else printf("%f+i%f\n",a.r,a.i);
}

//Definición de operaciones con complejos
cplx sum (cplx a, cplx b)
{
	cplx c;
	c.r=a.r+b.r;
	c.i=a.i+b.i;
	return c;
}

cplx res (cplx a, cplx b)
{
	cplx c;
	c.r=a.r-b.r;
	c.i=a.i-b.i;
	return c;
}

cplx tim (cplx a, cplx b)
{
	cplx c;
	c.r=(a.r*b.r)-(a.i*b.i);
	c.i=(a.i*b.r)+(a.r*b.i);
	return c;
}

cplx special_tim (cplx a, cplx b)
{
	cplx c;
	c.r=(abs(a.r)*abs(b.r))-(abs(a.i)*abs(b.i));
	c.i=(a.i*b.r)+(a.r*b.i);
	return c;
}
