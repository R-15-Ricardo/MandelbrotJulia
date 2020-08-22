#include "mybits.h"
#include <SFML/Graphics.h>
#include <SFML/Config.h>

//Tamaño de la pantalla
#define WITDH 640*2
#define HEIGHT 480
//32 Bits por pixel
#define BPP 32

double map (double,double,double,double,double);
int* HSVtoRGB (float,float,float);
void MandelClasifier (sfUint8*,int,int,double,double,double,double,int);
void JuliaClasifier (sfUint8*,sfVector2i,int,int,double,double,double,double);
void setpixelcolor (sfUint8*,int,int,int,int,int,int);

//toma x en un intervalo y lo mapea a un nuevo intervalo
double map (double var, double ins, double ine, double outs, double oute)
{
    double d1,d2,dor;
    d1=ine-ins;
    d2=oute-outs;
    dor=outs-ins*(d2/d1);
    return var*(d2/d1)+dor;
}

//Convierte color tipo Hue Saturation Brightness a Red Green Blue
//Formula usada: https://www.rapidtables.com/convert/color/hsv-to-rgb.html
int* HSVtoRGB (float h,float s,float v)
{
    int *rgb;
    rgb=(int*)malloc(3*sizeof(int));
    float rp,gp,bp;
    float c=v*s;
    float x=c*(1-abs((int)(h/60)%2-1));
    float m=v-c;
    if (0<=h && h<60)
    {
        rp=c;
        gp=x;
        bp=0;
    }
    else if (60<=h && h<120)
    {
        rp=x;
        gp=c;
        bp=0;
    }
    else if (120<=h && h<180)
    {
        rp=0;
        gp=c;
        bp=x;
    }
    else if (180<=h && h<240)
    {
        rp=0;
        gp=x;
        bp=c;
    }
    else if (240<=h && h<300)
    {
        rp=x;
        gp=0;
        bp=c;
    }
    else if (300<=h && h<360)
    {
        rp=c;
        gp=0;
        bp=x;
    }
    rgb[0]=floor((rp+m)*255);
    rgb[1]=floor((gp+m)*255);
    rgb[2]=floor((bp+m)*255);
    return rgb;
}

//Cambia el color de un pixel en un arreglo de pixeles. Formato RGB
void setpixelcolor (sfUint8 *pix,int x,int y,int r,int g,int b,int a)
{
    int pos=4*(y*WITDH+x);
    pix[pos]=r;
    pix[pos+1]=g;
    pix[pos+2]=b;
    pix[pos+3]=a;
}

//Asigna color a un pixel dependiendo del tiempo de escape al iterar z²+c desde z=0;
//Recibe arrglo de pixeles. xy del pixel. [x,x']. [y,y']. Modo (HD/Normal)
void MandelClasifier (sfUint8 *pix, int y,int x, double lft, double rght, double top, double bot,int mode)
{
    //Comlpejo c asignado a coordenadas del mouse
    //Complejo z=0+0i
    cplx c;
    cplx z={0,0};
    c.r=map(x,0,WITDH/2,lft,rght);
    c.i=map(y,0,HEIGHT,top,bot);
    int i;
    int maxiterations;
    //Cambia numero de iteraciones dependiendo del modo (HD/Normal)
    if (mode) maxiterations=500;
    else maxiterations=50;
    for (i=0;i<maxiterations;i++)
    {
        z=sum(tim(z,z),c);
        if (z.r*z.r+z.i*z.i>=4)
        {
            float shade;
            int *rgbval;
            //Asigna brillo dependiendo del tiempo de escape. 0=Negro. maxiterations=azul.
            shade=3.5*map(i,0,175,0,1);
            rgbval=HSVtoRGB(217,0.78,shade);
            setpixelcolor(pix,x,y,rgbval[0],rgbval[1],rgbval[2],255);
            free(rgbval);
            break;
        }
        //Asigna negro a los pixeles que esten en el conjunto
        setpixelcolor(pix,x,y,0,0,0,255);
    }
}

//Asigna color a un pixel dependiendo del tiempo de escape al iterar z²+c con c dependiente de la pocision del mouse
//Recibe arreglo de pixeles. xy del mouse. xy del pixel. [x,x']. [y,y'].
void JuliaClasifier (sfUint8 *pix, sfVector2i pos, int y,int x, double lft, double rght, double top, double bot)
{
    cplx c;
    cplx z;
    //Complejo c asignado a la pocision del mouse
    //Comlpejo z asignado a la coordenada del pixel que se está procesando
    c.r=map(pos.x,0,WITDH/2,lft,rght);
    c.i=map(pos.y,0,HEIGHT,top,bot);
    z.r=map(x,WITDH/2,WITDH,-2,2);
    z.i=map(y,0,HEIGHT,-1.5,1.5);
    int i;
    //Julia siempre se itera 50 veces. No es necesario cambiar esto porque no se hace zoom
    for (i=0;i<50;i++)
    {
        z=sum(tim(z,z),c);
        if (z.r*z.r+z.i*z.i>=4)
        {
            float shade;
            int *rgbval;
            //Asigna brillo dependiendo del tiempo de escape. 0=Negro. maxiterations=naranja.
            shade=3.5*map(i,0,175,0,1);
            rgbval=HSVtoRGB(24,0.79,shade);
            setpixelcolor(pix,x,y,rgbval[0],rgbval[1],rgbval[2],255);
            free(rgbval);
            break;
        }
        //Asigna negro a los pixeles que esten en el conjunto
        setpixelcolor(pix,x,y,0,0,0,255);
    }
}
