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

#include <stdlib.h>
#include <GL/gl.h>

/*

TEMPORARY

*/

#include <stdio.h>
#include <GL/gl.h>
#include <math.h>

typedef struct mat4 {
	GLfloat m[16];
#ifdef __cplusplus
    mat4() {}
	mat4(GLfloat x2) {
		m[0] = x2; m[1] = 0; m[2] = 0; m[3] = 0;
		m[4] = 0; m[5] = x2; m[6] = 0; m[7] = 0;
		m[8] = 0; m[9] = 0; m[10] = x2; m[11] = 0;
		m[12] = 0; m[13] = 0; m[14] = 0; m[15] = x2;
	}
	mat4(GLfloat p0, GLfloat p1, GLfloat p2, GLfloat p3,
		GLfloat p4, GLfloat p5, GLfloat p6, GLfloat p7,
		GLfloat p8, GLfloat p9, GLfloat p10, GLfloat p11, 
		GLfloat p12, GLfloat p13, GLfloat p14, GLfloat p15) {
	    m[0] = p0; m[1] = p1; m[2] = p2; m[3] = p3;
		m[4] = p4; m[5] = p5; m[6] = p6; m[7] = p7;
		m[8] = p8; m[9] = p9; m[10] = p10; m[11] = p11;
		m[12] = p12; m[13] = p13; m[14] = p14; m[15] = p15;
	}
#endif
} mat4;

typedef struct vec3 {
		union {GLfloat x; GLfloat r;};
		union {GLfloat y; GLfloat g;};
		union {GLfloat z; GLfloat b;};
		
        #ifdef __cplusplus
            vec3() {}
			vec3(GLfloat x2, GLfloat y2, GLfloat z2) : x(x2), y(y2), z(z2) {}
			vec3(GLfloat x2) : x(x2), y(x2), z(x2) {}
		#endif
} vec3, *vec3Ptr;

typedef struct vec2 {
		union {GLfloat x; GLfloat s;};
		union {GLfloat y; GLfloat t;};
		
		#ifdef __cplusplus
            vec2() {}
			vec2(GLfloat x2, GLfloat y2) : x(x2), y(y2) {}
		#endif
} vec2, *vec2Ptr;

mat4 frustum(float left, float right, float bottom, float top, float znear, float zfar);

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

/*

SHADER UTILITIES

*/

void glUtilitiesReportError(const char *n);
void glUtilitiesDump(void);

GLuint glUtilitiesLoadShaders(const char *vp, const char *fp);
GLuint glUtilitiesLoadGeoShaders(const char *vp, const char *fp, const char *gp);
GLuint glUtilitiesLoadGeoTexShaders(const char *vp, const char *fp, const char *gp, const char *tp1, const char *tp2);

/*

FBO UTILITIES

*/

typedef struct {
    GLuint texID;
    GLuint fb, rb;
    GLuint depth;
    int w, h;
} FBOData;

FBOData *glUtilitiesInitFBO(int w, int h, int ifunc);
FBOData *glUtilitiesInitFBODepth(int w, int h, int ifunc, int depth);

void glUtilitiesUseFBO(FBOData *out, FBOData *in1, FBOData *in2);
void glUtilitiesUpdateScreenFBO(int w, int h);

/*

MODEL UTILITIES

*/

// TODO: Materials

typedef struct {
  vec2* texCoordArray;

  vec3* vertexArray;
  vec3* normalArray;
  vec3* colorArray;
  
  GLuint* indexArray;
  
  int numVertices;
  int numIndices;

  char data;
  
  GLuint vao;
  GLuint vb, ib, nb, tb; // VBOs
  
  // TODO: Material *material;
} Model;

Model** glUtilitiesLoadModelSet(const char* n); // Multi-part Object
Model* glUtilitiesLoadModel(const char* n); // Single Object

void glUtilitiesDrawWireframe(Model *m, GLuint program, const char* vertexVar, const char* normalVar, const char* textureVar);
void glUtilitiesDrawModel(Model *m, GLuint program, const char* vertexVar, const char* normalVar, const char* textureVar);

Model* glUtilitiesLoadModelData(vec3 *vertices, vec3 *normals, vec2 *texCoords, vec3 *colors, GLuint *indices, int numVertices, int numIndices);

void glUtilitiesReloadModelData(Model *m);

void glUtilitiesScaleModel(Model *m, float sx, float sy, float sz);
void glUtilitiesDisposeModel(Model *m);
void glUtilitiesCenterModel(Model *m);

/*

TGA UTILITIES

*/

#include <string.h>
#include <stdio.h>

typedef struct TextureData {
	GLubyte	*imageData;
    GLuint	bpp;
	GLuint	w;
	GLuint	h;
	GLuint	texID;
	GLfloat	texWidth, texHeight;
} TextureData, *TextureDataPtr;

void glUtilitiesLoadTGATextureSimple(const char *n, GLuint *tex);
void glUtilitiesLoadTGASetMipmapping(bool active);

bool glUtilitiesLoadTGATextureData(const char *n, TextureData *tex);
bool glUtilitiesLoadTGATexture(const char *n, TextureData *tex);

int glUtilitiesSaveTGAData(char	*n, short int w, short int h, unsigned char pixelDepth, unsigned char *imageData);

void glUtilitiesSaveTGA(TextureData *tex, char *n);
void glUtilitiesSaveTGAFramebuffer(char *n, GLint x, GLint y, GLint w, GLint h);

#ifdef __cplusplus
}
#endif

#endif