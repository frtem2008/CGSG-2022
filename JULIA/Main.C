/* AN2 */
#define _CRT_SECURE_NO_WARNINGS //for vs 2022 or 2019
#include <stdlib.h>

#include <windows.h> //for delay between frames
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <time.h>

#include "glut.h"

#include "CMPL.H"

#define W 1200
#define H 1200

#define RENDER_FRAMES 250
#define GET_DWORD_COLOR(R, G, B) ((R) << 16 | (G) << 8 | (B))

typedef unsigned char byte;
typedef unsigned dword;

static int RENDER_START = 1;

static dword Frame[W * H];
static dword JuliaMas[W * H];
static INT rendered = 0;
static DBL x0 = -2.0, y0 = -2.0, x1 = 2.0, y1 = 2.0;
static DBL xStep0, yStep0, xStep1, yStep1;

static char RenderFileName[250];

static DBL finX0 = 0.225000, finY0 = -0.635000, finX1 = 0.295000, finY1 = -0.577500;
//static DBL finX0 = -0.775000, finY0 = -0.105000, finX1 = -0.785000, finY1 = -0.115000;

static DBL Zoom = 1, xPos = 0, yPos = 0;

void PutPixel(int x, int y, dword Col)
{
  if (x >= 0 && x < W && y >= 0 && y < H)
	Frame[y * W + x] = Col;
}

DBL convertX( INT X )
{
  return x0 + X * (x1 - x0) / W;
}

DBL convertY( INT Y )
{
  return y0 + Y * (y1 - y0) / H;
}

DBL CalculateStepX0( void )
{
  return (finX0 - x0) / RENDER_FRAMES;
}

DBL CalculateStepY0( void )
{
  return (finY0 - y0) / RENDER_FRAMES;
}

DBL CalculateStepX1( void )
{
  return (finX1 - x1) / RENDER_FRAMES;
}

DBL CalculateStepY1(void)
{
  return (finY1 - y1) / RENDER_FRAMES;
}

void JuliaGen( void )
{
  INT i, j, n;

  static CMPL Z;
  for (i = 0; i < W; i++)
	for (j = 0; j < H; j++)
	{
	  n = Julia(CmplSet(convertX(i), convertY(j)), CmplSet(convertX(i), convertY(j)));
	  JuliaMas[j * W + i] = GET_DWORD_COLOR(n, n << 4, n << 6);
	 
	}
  x0 += xStep0;
  x1 += xStep1;
  y0 += yStep0;
  y1 += yStep1;
}

static FILE *RenderFile;
static FILE *ViewFile;

static dword ViewBuffer[5][W * H];

void Display( void )
{
  static INT i, j, frames;
  static INT t, renderTime, opened = 0;

  static char *RENDERPATH;

  static CMPL Z;

  glClear(GL_COLOR_BUFFER_BIT);
  glPixelZoom(Zoom, -Zoom);
  glRasterPos2d(-1, 1);

  if (rendered == 0)
  {
	printf("RENDERING %i frames in %i x %i\n", RENDER_FRAMES, W, H);

	xStep0 = CalculateStepX0();
	yStep0 = CalculateStepY0();
	xStep1 = CalculateStepX1();
	yStep1 = CalculateStepY1();

	printf("XSTEP0 is %f, YSTEP0 is %f, XSTEP1 is %f, YSTEP1 is %f\n", xStep0, yStep0, xStep1, yStep1);
	
	renderTime = clock();
	t = clock();
	while (frames++ < RENDER_FRAMES)
	{
	  JuliaGen();
	  fwrite(JuliaMas, sizeof(dword), W * H, RenderFile);
	  if (frames != 0)
	  {
		if (frames % 10 == 0)
		{
		  printf("Frames %i - %i done in %f seconds\n", frames - 10, frames, (clock() - t) / (double)CLOCKS_PER_SEC);
		  t = clock();
		}
		if (frames % 100 == 0)
		{
		  fflush(RenderFile);
		  printf("Frames %i - %i sent to disk\n", frames - 100, frames);
		}
	  }
	}
	rendered = 1;

	printf("RENDERING DONE IN %f seconds\n", (clock() - renderTime) / (double)CLOCKS_PER_SEC);
	fclose(RenderFile);
  }
  else if (rendered == 1)
  {	
	memset(Frame, 0, W* H * sizeof(dword));
	if (RENDER_START)
	{
	  if (!opened)
	  {
		ViewFile = fopen(RenderFileName, "rb");
		if (ViewFile == NULL)
		{
		  printf("Failed to open file %s\n", RenderFileName);
		  getchar();
		  exit(0);
		}

		printf("RENDER FILE OPENED\nPress any key to start animation\n");
		_getch();
		Sleep(500);
		printf("3\n");
		Sleep(500);
		printf("2\n");
		Sleep(500);
		printf("1\n");

		opened = 1;
	  }

	  fread(ViewBuffer[4], sizeof(dword), W * H, ViewFile);

	  for (i = 0; i < 4; i++)
		memmove(ViewBuffer[i], ViewBuffer[i + 1], W * H * sizeof(dword));

	  for (i = 0; i < W; i++)
		for (j = 0; j < H; j++)
		  PutPixel(i + xPos, j + yPos, ViewBuffer[0][j * H + i]);
	}
	else
	{
	  for (i = 0; i < W; i++)
		for (j = 0; j < H; j++)
		  PutPixel(i + xPos, j + yPos, JuliaMas[j * H + i]);
	}

	glDrawPixels(W, H, GL_BGRA_EXT, GL_UNSIGNED_BYTE, Frame);
	Sleep(10);
  }

  glFlush();
  glutSwapBuffers();
  glutPostRedisplay();
}

void Keyboard(byte key, int x, int y)
{
  switch (key)
  {
  case ' ':
    printf("%f, %f\n", convertX(x), convertY(y));
    break;
  case 'q':
  case 'Q':
    exit(0);
	break;
  case '=':
  case '+':
    Zoom += 1;
	break;
  case '-':
  case '_':
    Zoom -= 1;
    break;
  case 'A':
  case 'a':
    xPos += 2;
	break;
  case 'D':
  case 'd':
    xPos -= 2;
    break;
  case 'W':
  case 'w':
    yPos += 2;
    break;
  case 'S':
  case 's':
    yPos -= 2;
    break;
  default:
    break;
  }
}

void main( void )
{
  glutInitDisplayMode(GLUT_RGB);
  glutInitWindowPosition(90, 40);
  glutInitWindowSize(W, H);
  glutCreateWindow("AN2 MANDL");

  glutDisplayFunc(Display);
  glutKeyboardFunc(Keyboard);

  glClearColor(0.30, 0.47, 0.8, 1);


  printf("Choose mode: 1 - render, 2 - play\n");
  switch(getchar())
  {
    case '1':
	  printf("Enter a name for render file...\n");
	  scanf("%s", &RenderFileName);

	  RenderFile = fopen(RenderFileName, "wb");
	  if (RenderFile == NULL)
	  {
		printf("Failed to open render file\n");
		getchar();
		return;
	  }
	  break; 
	case '2':
	  printf("Enter render file name to open...\n");
	  scanf("%s", &RenderFileName);
	  rendered = 1;
	break;
  }

  glutMainLoop();
}

