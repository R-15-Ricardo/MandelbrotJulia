//compile command: gcc -O3 main.c -lm -o main -lcsfml-graphics -lcsfml-window -lcsfml-system -lcsfml-audio
#include "isosurface.h"

//Tamaño de coordstr
#define BUFF 255

void drawMandelbrot (sfUint8*,sfVector2i,double,int,sfTexture*,sfSprite*,double,double,double,double);
void drawJulia (sfUint8*,sfVector2i,double,sfTexture*,sfSprite*,double,double,double,double);
void getjuliacoord (char*,sfVector2i,double,double,double,double,double);

int main ()
{
    //Crea Ventana
    sfVideoMode wndmode={WITDH,HEIGHT,BPP};
    sfRenderWindow* wnd;
    wnd=sfRenderWindow_create(wndmode,"Mandelbrot & Julia",sfClose,NULL);
    if (!wnd) exit(EXIT_FAILURE);
    sfRenderWindow_setFramerateLimit(wnd,30);

    //Evento cerra Ventana
    sfEvent close;

    //Crea Texto con fuente arial 12
    char coordstr[BUFF];
    sfText* coord;
    sfFont* arial;
    sfVector2f textpos={WITDH/2,HEIGHT-20};
    arial=sfFont_createFromFile("arial.ttf");
    if (!arial) exit(EXIT_FAILURE);
    coord=sfText_create();
    sfText_setFont(coord,arial);
    sfText_setCharacterSize(coord,12);
    sfText_setColor(coord,sfWhite);
    sfText_setPosition(coord,textpos);

    //Vectores coordenadas mouse
    sfVector2i mouse_pos;

    //Recursos Mandelbrot
    sfSprite* finishmandelbrot;
    finishmandelbrot=sfSprite_create();
    sfTexture* brotText;
    brotText=sfTexture_create(WITDH,HEIGHT);
    sfUint8* brotpixels;
    brotpixels=(sfUint8*)malloc(WITDH*HEIGHT*4);

    //Recursos Julia
    sfSprite* finishjulia;
    finishjulia=sfSprite_create();
    sfTexture* julText;
    julText=sfTexture_create(WITDH,HEIGHT);
    sfUint8* julpixels;
    julpixels=(sfUint8*)malloc(WITDH*HEIGHT*4);

    //Información de escala y modo
    float offxinf=-2.5,offxsup=1.5,offyinf=-1.5,offysup=1.5;
    float zFact=1;
    int mode=0;

    //Loop de la ventana
    while (sfRenderWindow_isOpen(wnd))
    {

        //Espera evento de cerrado
        while (sfRenderWindow_pollEvent(wnd,&close))
        {
            if (close.type == sfEvtClosed) sfRenderWindow_close(wnd);
        }

        //Controles. WASD Moverse. QE Zoom- Zoom+. RT Modo HD Modo Normal
        if (sfKeyboard_isKeyPressed(sfKeyW))
        {
            offyinf-=0.05/zFact;
            offysup-=0.05/zFact;
        }
        if (sfKeyboard_isKeyPressed(sfKeyA))
        {
            offxinf-=0.05/zFact;
            offxsup-=0.05/zFact;
        }
        if (sfKeyboard_isKeyPressed(sfKeyS))
        {
            offyinf+=0.05/zFact;
            offysup+=0.05/zFact;
        }
        if (sfKeyboard_isKeyPressed(sfKeyD))
        {
            offxinf+=0.05/zFact;
            offxsup+=0.05/zFact;
        }
        if (sfKeyboard_isKeyPressed(sfKeyR)) mode=1;
        if (sfKeyboard_isKeyPressed(sfKeyT)) mode=0;
        if (sfKeyboard_isKeyPressed(sfKeyQ)) zFact/=1.2;
        if (sfKeyboard_isKeyPressed(sfKeyE)) zFact*=1.2;

        //Obtine pocision del mouse y adapta coordenadas a escala
        mouse_pos=sfMouse_getPositionRenderWindow(wnd);
        getjuliacoord(coordstr,mouse_pos,zFact,offxinf,offxsup,offyinf,offysup);
        sfText_setString(coord,coordstr);

        //Inicializa pixeles para este ciclo
        drawMandelbrot(brotpixels,mouse_pos,zFact,mode,brotText,finishmandelbrot,offxinf,offxsup,offyinf,offysup);
        drawJulia(julpixels,mouse_pos,zFact,julText,finishjulia,offxinf,offxsup,offyinf,offysup);

        //Limpia ventana
        sfRenderWindow_clear(wnd, sfBlack);

        //Dibuja en ventana los fractales y coordenadas de Julia
        sfRenderWindow_drawSprite(wnd,finishjulia, NULL);
        sfRenderWindow_drawSprite(wnd,finishmandelbrot, NULL);
        sfRenderWindow_drawText(wnd,coord,NULL);

        //Muestra ventana
        sfRenderWindow_display(wnd);

    }
    //Libera Recursos
    sfSprite_destroy(finishmandelbrot);
    sfSprite_destroy(finishjulia);
    sfTexture_destroy(brotText);
    sfTexture_destroy(julText);
    sfText_destroy(coord);
    sfFont_destroy(arial);
    sfRenderWindow_destroy(wnd);
    free(brotpixels);
    free(julpixels);
    return 0;
}

//Recibe arreglo de pixeles. Factor escalar. Modo (HD/Normal). Textura de sprite. Sprite. [x,x']. [y,y'].
void drawMandelbrot (sfUint8 *pixels,sfVector2i pos,double zf,int mode, sfTexture *imgtex, sfSprite *sprite, double lft, double rght, double top, double bot)
{
    int i,j;
    //Convierte el rango xy actual a la nueva escala
    float ox=map(WITDH/4,0,WITDH/2,lft,rght);
    float oy=map(HEIGHT/2,0,HEIGHT,top,bot);
    lft=(lft-ox)/zf+ox;
    rght=(rght-ox)/zf+ox;
    top=(top-oy)/zf+oy;
    bot=(bot-oy)/zf+oy;
    //Asigna color para cada pixel en 640*480
    for (i=0;i<HEIGHT;i++)
    {
        for (j=0;j<WITDH/2;j++)
        {
            MandelClasifier(pixels,i,j,lft,rght,top,bot,mode);
        }
    }
    //Actualiza textura de imagen final con el arreglo de pixeles
    sfTexture_updateFromPixels(imgtex,pixels,WITDH,HEIGHT,0,0);
    if (!imgtex) exit(EXIT_FAILURE);
    sfSprite_setTexture(sprite,imgtex,sfTrue);
}

//Recibe arreglo de pixeles. Factor escalar. Textura de sprite. Sprite. [x,x']. [y,y'].
void drawJulia (sfUint8* pixels,sfVector2i pos, double zf, sfTexture *imgtex, sfSprite *sprite, double lft, double rght, double top, double bot)
{
    int i,j;
    //Convierte el rango xy actual a la nueva escala
    float ox=map(WITDH/4,0,WITDH/2,lft,rght);
    float oy=map(HEIGHT/2,0,HEIGHT,top,bot);
    lft=(lft-ox)/zf+ox;
    rght=(rght-ox)/zf+ox;
    top=(top-oy)/zf+oy;
    bot=(bot-oy)/zf+oy;
    //Asigna color para cada pixel en 640*480
    for (i=0;i<HEIGHT;i++)
    {
        for (j=WITDH/2;j<WITDH;j++)
        {
            JuliaClasifier(pixels,pos,i,j,lft,rght,top,bot);
        }
    }
    //Actualiza textura de imagen final con el arreglo de pixeles
    sfTexture_updateFromPixels(imgtex,pixels,WITDH,HEIGHT,0,0);
    if (!imgtex) exit(EXIT_FAILURE);
    sfSprite_setTexture(sprite,imgtex,sfTrue);
}

//Recibe apuntador a arreglo char. Factor escalar. [x,x']. [y,y'].
void getjuliacoord (char* s,sfVector2i pos,double zf,double lft,double rght,double top,double bot)
{
    //Convierte el rango xy actual a la nueva escala
    float ox=map(WITDH/4,0,WITDH/2,lft,rght);
    float oy=map(HEIGHT/2,0,HEIGHT,top,bot);
    lft=(lft-ox)/zf+ox;
    rght=(rght-ox)/zf+ox;
    top=(top-oy)/zf+oy;
    bot=(bot-oy)/zf+oy;
    //Toma xy de la nueva escala y lo convierta a formato a+bi
    sprintf(s,"Julia of: %f+%fi",map(pos.x,0,WITDH/2,lft,rght),map(pos.y,0,HEIGHT,top,bot));
}
