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
#include "vecutils.h"

#include <stdlib.h>
#include <GL/gl.h>

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

typedef struct Material {
	char newmtl[255];
	
	Vector3 Ka, Kd, Ks, Ke;

	GLfloat Ns, Tr, d;

	int illumination;
	int texidKa, texidKd, texidKs, texidKe, texidNs, texid_d, texid_bump;

	char map_Ka[255], map_Kd[255], map_Ks[255], map_Ke[255], map_Ns[255], map_d[255], map_bump[255];
} Material, *MaterialPtr, **MaterialHandle;

typedef struct {
  Vector2* texCoordArray;

  Vector3* vertexArray;
  Vector3* normalArray;
  Vector3* colorArray;
  
  GLuint* indexArray;
  
  int numVertices;
  int numIndices;

  char data;
  
  GLuint vao;
  GLuint vb, ib, nb, tb; // VBOs
  
  Material *material;
} Model;

Model** glUtilitiesLoadModelSet(const char* n); // Multi-part Object
Model* glUtilitiesLoadModel(const char* n); // Single Object

void glUtilitiesDrawWireframe(Model *m, GLuint program, const char* vertexVar, const char* normalVar, const char* textureVar);
void glUtilitiesDrawModel(Model *m, GLuint program, const char* vertexVar, const char* normalVar, const char* textureVar);

Model* glUtilitiesLoadModelData(Vector3 *vertices, Vector3 *normals, Vector2 *texCoords, Vector3 *colors, GLuint *indices, int numVertices, int numIndices);

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

/*

GUI UTILITIES

*/

typedef void (*NoArgProcPtr)();

int glUtilitiesSlider(int x, int y, int w, float *var, float min, float max);
int glUtilitiesColorBox(int x, int y, float *r, float *g, float *b);
int glUtilitiesColorDrop(int x, int y, float *r, float *g, float *b);

void glUtilitiesSliderColorGroup(int x, int y, int w, float *r, float *g, float *b);

int glUtilitiesCheckBox(int x, int y, const char *s, int *var);
int glUtilitiesStaticString(int x, int y, const char *s);
int glUtilitiesDynamicString(int x, int y, char *s);
int glUtilitiesDisplayInt(int x, int y, const char *s, int *variable);
int glUtilitiesDisplayFloat(int x, int y, const char *s, float *variable);
int glUtilitiesRadio(int x, int y, const char *s, int *variable, int index);
int glUtilitiesButton(int x, int y, const char *s, NoArgProcPtr callback);

void glUtilitiesSliderV3Group(int x, int y, int w, float *v, float min, float max);

int glUtilitiesMenu(int x, int y, const char *s);
int glUtilitiesMenuItem(const char *s, NoArgProcPtr callback);

int glUtilitiesColorClicker(int x, int y, float r, float g, float b, float *dr, float *dg, float *db);
int glUtilitiesSmallColorClicker(int x, int y, float r, float g, float b, float *dr, float *dg, float *db);

void glUtilitiesColorPalette(int x, int y, float *dr, float *dg, float *db);
void glUtilitiesSmallColorPalette(int x, int y, float *dr, float *dg, float *db);

int glUtilitiesLeftStepper(int x, int y, int *var);
int glUtilitiesRightStepper(int x, int y, int *var);

void glUtilitiesSteppers(int x, int y, int *var);

static int glUtilitiesCreateItem(int x, int y);
void glUtilitiesRemoveItem(int i);

void glUtilitiesDrawGUI();

int glUtilitiesMouseGUI(int state, int x, int y);
int glUtilitiesMouseDragGUI(int x, int y);

void glUtilitiesLoadFont(unsigned char *data, float charW, float charH, int imgW, int imgH, int space);

void glUtilitiesSetFrameColor(float r, float g, float b);
void glUtilitiesSetFillColor(float r, float g, float b);
void glUtilitiesSetSliderFillColor(int ID, float r, float g, float b);
void glUtilitiesSetSliderDefaultFillColor(float r, float g, float b);
void glUtilitiesSetBackgroundColor(float r, float g, float b, float a);
void glUtilitiesSetBackgroundBorder(int b);
void glUtilitiesSetTextColor(float r, float g, float b);
void glUtilitiesSetPosition(int x, int y);
void glUtilitiesSetSpacing(int s);
void glUtilitiesSetScale(int s);

#ifdef __cplusplus
}
#endif

#endif