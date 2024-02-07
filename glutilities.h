/*

GLUtilities: An OpenGL Utility Library
--------------------------------------

Author: Isak Horvath
Year:   2024

OVERVIEW:
GLUtilities is a comprehensive utility library designed for OpenGL applications. 
It leverages the functionalities provided by the GLUT/FreeGLUT library, 
offering an extended set of tools for developers working with OpenGL.

INSPIRATION:
This project draws inspiration from the work of Ingemar Ragnemalm,
author of MicroGLUT and examiner for university computer graphics course TSKB07. 

*/

#ifndef _GLUTILITIES_
#define _GLUTILITIES_

#ifdef __cplusplus
extern "C" {
#endif

#include "constants.h"

void glUtilitiesContextVersion(int major, int minor);

void glUtilitiesReshapeWindow(int w, int h);
void glUtilitiesCreateWindow(const char *t);

void glUtilitiesSetWindowPos(int x, int y);
void glUtilitiesSetWindowTitle(char *t);

void glUtilitiesWindowSize(int w, int h);
void glUtilitiesWindowPos(int x, int y);

void glUtilitiesInit(int *argcp, char **argv);
void glUtilitiesMain();

void glUtilitiesCheck();

void glUtilitiesReshapeFunc(void (*func)(int w, int h));
void glUtilitiesDisplayMode(unsigned int m);
void glUtilitiesDisplayFunc(void (*func)(void));
void glUtilitiesSwapBuffers();
void glUtilitiesRedisplay();

void glUtilitiesKeyUpEventFunc(void (*func)(unsigned char k, int x, int y));
void glUtilitiesKeyEventFunc(void (*func)(unsigned char k, int x, int y));
char glUtilitiesKeyIsDown(unsigned char c);

void glUtilitiesModUpEventFunc(void (*func)(unsigned char k, int x, int y));
void glUtilitiesModEventFunc(void (*func)(unsigned char k, int x, int y));

void glUtilitiesPassiveMouseMoveFunc(void (*func)(int x, int y));
void glUtilitiesMouseMoveFunc(void (*func)(int x, int y));
void glUtilitiesMouseFunc(void (*func)(int b, int s, int x, int y));
char glUtilitiesMouseIsDown(unsigned char c);

void glUtilitiesShowCursor();
void glUtilitiesHideCursor();

int  glUtilitiesGet(int t);

void glUtilitiesIdleFunc(void (*func)(void));

void glUtilitiesTimerFunc(int ms, void (*func)(int arg), int arg);
void glUtilitiesRepeatingTimerFunc(int ms);

void glUtilitiesWarpPointer(int x, int y);

void glUtilitiesToggleFullscreen();
void glUtilitiesExitFullscreen();
void glUtilitiesFullscreen();
void glUtilitiesExit();

#ifdef __cplusplus
}
#endif

#endif