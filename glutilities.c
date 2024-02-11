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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <GL/gl.h>
#include <math.h>

#include <X11/keysym.h>
#include <X11/Xlib.h>
#include <sys/time.h>
#include <GL/glext.h>
#include <GL/glx.h>
#include <unistd.h>

#include "glutilities.h"

#ifndef M_PI
#define M_PI 3.14159265
#endif

static unsigned int WINDOW_HEIGHT = 512;
static unsigned int WINDOW_WIDTH = 512;

static unsigned int WINDOW_POS_X = 48;
static unsigned int WINDOW_POS_Y = 48;

static int CONTEXT_VERSION_MAJOR = 0;
static int CONTEXT_VERSION_MINOR = 0;

static GLXContext CONTEXT;
static Display *DISPLAY;
static Window WINDOW;

static char KEYMAP[256];
static char RUNNING = 1;
static char ANIMATE = 1;

struct timeval TIME;

int DISPLAY_MODE;

void glUtilitiesInit(int *argc, char *argv[]) {
    gettimeofofday(&TIME, NULL);
    memset(KEYMAP, 0, sizeof(KEYMAP));
}

void glUtilitiesDisplayMode(unsigned int m) {
    DISPLAY_MODE = m;
}

void glUtilitiesWindowSize(int w, int h) {
    WINDOW_WIDTH = w;
    WINDOW_HEIGHT = h;
}

void glUtilitiesWindowPos(int x, int y) {
    WINDOW_POS_X = x;
    WINDOW_POS_Y = y;
}

static void create_window(Display *d, const char *n, int x, int y, int w, int h, Window *win, GLXContext *ctx) {
    XSetWindowAttributes attributes;
    XVisualInfo *info;

    GLXContext context;
    Window window;

    unsigned long mask;
    int screen = DefaultScreen(d);
    Window root = RootWindow(d, screen);

    if(CONTEXT_VERSION_MAJOR > 2) {
		typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
		glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
        
        if(strstr(glXQueryExtensionsString(d, screen), "GLX_ARB_create_context") != 0) {
		    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddress((const GLubyte *)"glXCreateContextAttribsARB");
        }

        if(!glXCreateContextAttribsARB) {
            printf("CREATE_WINDOW WARNING: Could not create GL context!\n");
        }

        int elements;
        GLXFBConfig *config;

        int attr[] = {
            GLX_RENDER_TYPE,
            GLX_RGBA_BIT,
            GLX_DRAWABLE_TYPE,
            GLX_WINDOW_BIT,
            GLX_RED_SIZE, 1,
            GLX_GREEN_SIZE, 1,
            GLX_BLUE_SIZE, 1,
            GLX_ALPHA_SIZE, 1,
            None, None, None, None, 
            None, None, None, None, 
            None, None, None, None,
            None, None, None, None
        };

        int i = 12;
        if(DISPLAY_MODE & DOUBLE) {
            attr[i++] = GLX_DOUBLEBUFFER;
            attr[i++] = 1;
        }

        if(DISPLAY_MODE & DEPTH) {
            attr[i++] = GLX_DEPTH_SIZE;
            attr[i++] = 1;
        }

        if(DISPLAY_MODE & STENCIL) {
			attr[i++] = GLX_STENCIL_SIZE;
            attr[i++] = 8;
        }

        if(DISPLAY_MODE & MULTISAMPLE) {
			attr[i++] = GLX_SAMPLE_BUFFERS;
			attr[i++] = 1;
			attr[i++] = GLX_SAMPLES;
			attr[i++] = 4;
        }

        config = glXChooseFBConfig(d, screen, attr, &elements);
        if(!config) {
            config = glXChooseFBConfig(d, screen, NULL, &elements);
            if(!config) {
                printf("CREATE_WINDOW WARNING: Could not get FB configurations!\n");
            }
        }

        int gl3attr[] = {
            GLX_CONTEXT_MAJOR_VERSION_ARB, CONTEXT_VERSION_MAJOR,
            GLX_CONTEXT_MINOR_VERSION_ARB, CONTEXT_VERSION_MINOR,
            GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_DEBUG_BIT_ARB,
            GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
            None
        };

        context = glXCreateContextAttribsARB(d, config[0], NULL, 1, gl3attr);
        if(!context) {
            printf("CREATE_WINDOW WARNING: No context!\n");
        }

        info = glXGetVisualFromFBConfig(d, config[0]);
        if(!info) {
            printf("CREATE_WINDOW ERROR: Could not create OpenGL window with given pixel format!\n");
        }
    }
    else {
        int attr[] = {
            GLX_RGBA,
            GLX_RED_SIZE, 1,
            GLX_GREEN_SIZE, 1,
            GLX_BLUE_SIZE, 1,
            None, None, None, None, 
            None, None, None, None, 
            None, None, None, None,
            None, None, None, None
        };

        int i = 7;
        if(DISPLAY_MODE & DOUBLE) {
            attr[i++] = GLX_DOUBLEBUFFER;
        }

        if(DISPLAY_MODE & DEPTH) {
            attr[i++] = GLX_DEPTH_SIZE;
            attr[i++] = 1;
        }

        if(DISPLAY_MODE & STENCIL) {
            attr[i++] = GLX_STENCIL_SIZE;
            attr[i++] = 8;
        }

        info = glXChooseVisual(d, screen, attr);
        if(!info) {
            printf("CREATE_WINDOW ERROR: Could not get visual\n");
            exit(1);
        }

        context = glXCreateContext(d, info, 0, True);
        if(!context) {
            printf("CREATE_WINDOW WARNING: No context!\n");
        }
    }

    attributes.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPress | ButtonReleaseMask | Button1MotionMask | PointerMotionMask;
    attributes.override_redirect = 0;
    attributes.background_pixel = 0;
    attributes.border_pixel = 0;
    attributes.colormap = XCreateColormap(d, root, info->visual, AllocNone);

    mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect;
    window = XCreateWindow(d, root, x, y, w, h, 0, info->depth, InputOutput, info->visual, mask, &attributes);

    wmDeleteMessage = XInternAtom(d, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(d, window, &wmDeleteMessage, 1);

    XSizeHints hints;
    hints.x = x;
    hints.y = y;
    hints.width = w;
    hints.height = h;
    hints.flags = USSize | USPosition;

    XSetNormalHints(d, window, n, n, None, (char **)NULL, 0, &hints);

    if(!context) {
        printf("CREATE_WINDOW ERROR: Could not create context\n");
        exit(1);
    }

    *win = window;
    *ctx = context;
}

void glUtilitiesCreateWindow(const char *t) {
    DISPLAY = XOpenDisplay(NULL);
    if(!DISPLAY) {
        printf("CREATE_WINDOW ERROR: Could not open display %s\n", t);
    }

    create_window(
        DISPLAY, 
        t,
        WINDOW_POS_X, 
        WINDOW_POS_Y,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        &WINDOW,
        &CONTEXT
    );

    XMapWindow(DISPLAY, WINDOW);
    glXMakeCurrent(DISPLAY, WINDOW, CONTEXT);
}

void (*reshape)(int w, int h);
void (*display)(void);
void (*idle)(void);

void (*modkeyup)(unsigned char k, int x, int y);
void (*modkey)(unsigned char k, int x, int y);
void (*keyup)(unsigned char k, int x, int y);
void (*key)(unsigned char k, int x, int y);

void (*mouse)(int b, int s, int x, int y);
void (*mousedragged)(int x, int y);
void (*mousemoved)(int x, int y);

int LAST_MOUSE_POS_X, LAST_MOUSE_POS_Y;

void glUtilitiesReshapeFunc(void (*func)(int w, int h)) {
    reshape = func;
}

void glUtilitiesDisplayFunc(void (*func)(void)) {
    display = func;
}

void glUtilitiesIdleFunc(void (*func)(void)) {
    idle = func;
}

void glUtilitiesKeyUpEventFunc(void (*func)(unsigned char k, int x, int y)) {
    keyup = func;
}

void glUtilitiesKeyEventFunc(void (*func)(unsigned char k, int x, int y)) {
    key = func;
}

void glUtilitiesModUpEventFunc(void (*func)(unsigned char k, int x, int y)) {
    modkeyup = func;
}

void glUtilitiesModEventFunc(void (*func)(unsigned char k, int x, int y)) {
    modkey = func;
}

void glUtilitiesMouseFunc(void (*func)(int b, int b, int x, int y)) {
    mouse = func;
}

void glUtilitiesPassiveMouseMoveFunc(void (*func)(int x, int y)) {
    mousemoved = func;
}

void glUtilitiesMouseMoveFunc(void (*func)(int x, int y)) {
    mousedragged = func;
}

char BUTTON_PRESSED[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void handle_key_event(XEvent e, void (*proc)(unsigned char k, int x, int y), void (*modProc)(unsigned char k, int x, int y), int mapVal) {
    char buffer[10];
    int code = ((XKeyEvent *)&e)->keycode;

    XLookupString(&e.xkey, buffer, sizeof(buffer), NULL, NULL);

    char raw = buffer[0];
    switch(code) {
        // TODO: Implement
    }

    if(raw == 0) {
        if(modProc) {
            modProc(buffer[0], 0, 0);
        }
        else {
            if(proc) {
                proc(buffer[0], 0, 0);
            }
        }
    }
    else {
        if(proc) {
            proc(buffer[0], 0, 0);
        }
    }
    KEYMAP[(int)buffer[0]] = mapVal;
}

void timer(int x) {
    glUtilitiesRedisplay();
}

void glUtilitiesMain() {
    char pressed = 0;
    int i;

    XAllowEvents(DISPLAY, AsyncBoth, CurrentTime);

    glUtilitiesTimerFunc(100, timer, 0);

    while(RUNNING) {
        // TODO: Implement
    }
}

void glUtilitiesRedisplay() {
    ANIMATE = 1;
}

int glUtilitiesGet(int t) {
    struct timeval tv;

    switch(t) {
        case ELAPSED_TIME:
            gettimeofofday(&tv, NULL);
            return (tv.tv_usec - timeStart.tv_usec) / 1000 + (tv.tv_sec - timeStart.tv_sec) * 1000;
        case WIN_WIDTH:
            return WINDOW_WIDTH;
        case WIN_HEIGHT:
            return WINDOW_HEIGHT;
        case MOUSE_POS_X:
            return LAST_MOUSE_POS_X;
        case MOUSE_POS_Y:
            return LAST_MOUSE_POS_Y;
    }

    return 0;
}

// TODO: Implement timers

void glUtilitiesContextVersion(int major, int minor) {
    CONTEXT_VERSION_MAJOR = major;
    CONTEXT_VERSION_MINOR = minor;
}

void glUtilitiesWarpPointer(int x, int y) {
    if(!DISPLAY) {
        fprintf(stderr, "WARP_POINTER FAILED: Glutilities not initialized!\n");
        return;
    }

    if(x == LAST_MOUSE_POS_X && y == LAST_MOUSE_POS_Y) {
        return;    
    }

    XWarpPointer(
        DISPLAY,
        None,
        WINDOW,
        0, 0, 0, 0,
        x, y
    );

    XFlush(DISPLAY);
}

void glUtilitiesShowCursor() {
    XUndefineCursor(DISPLAY, WINDOW);
}

void glUtilitiesHideCursor() {
    if(!DISPLAY) {
	    printf("HIDE_CURSOR ERROR: Glutilities not initialized!\n");
        return;
    }

    static char Z[] = {0, 0, 0};

    Pixmap pixmap = XCreateBitmapFromData(DISPLAY, WINDOW, Z, 1, 1);
    Cursor cursor = XCreatePixmapCursor(DISPLAY, pixmap, pixmap, (XColor *)Z, (XColor *)Z, 0, 0);

    XDefineCursor(DISPLAY, WINDOW, cursor);
    XFreeCursor(DISPLAY, cursor);
    XFreePixmap(DISPLAY, pixmap);
}

char glUtilitiesKeyIsDown(unsigned char c) {
    return BUTTON_PRESSED[(unsigned int)c];
}

void glUtilitiesReshapeWindow(int w, int h) {
    XResizeWindow(DISPLAY, WINDOW, w, h);
}

void glUtilitiesSetWindowPos(int x, int y) {
    XMoveWindow(DISPLAY, WINDOW, x, y);
}

void glUtilitiesSetTindowTitle(char *t) {
    XStoreName(DISPLAY, WINDOW, t);
}

char FULLSCREEN = 0;

unsigned int SAVED_HEIGHT;
unsigned int SAVED_WIDTH;

int SAVED_X;
int SAVED_Y;

void glUtilitiesExitFullscreen() {
    FULLSCREEN = 0;
    XMoveResizeWindow(DISPLAY, WINDOW, SAVED_X, SAVED_Y, SAVED_WIDTH, SAVED_HEIGHT);
}

void glUtilitiesFullscreen() {
    FULLSCREEN = 1;

    Drawable drawable;
    unsigned int a, b;

    XGetGeometry(DISPLAY, WINDOW, &drawable, &SAVED_X, &SAVED_Y, &SAVED_WIDTH, &SAVED_HEIGHT, &a, &b);

    int screen = DefaultScreen(DISPLAY);

    int w = DisplayWidth(DISPLAY, screen);
    int h = DisplayHeight(DISPLAY, screen);

    XMoveResizeWindow(DISPLAY, WINDOW, 0, 0, w, h);
}

void glUtilitiesToggleFullscreen() {
    if(FULLSCREEN) {
        glUtilitiesExitFullscreen();
    }
    else  {
        glUtilitiesFullscreen();
    }
}

void glUtilitiesExit() {
    RUNNING = 0;
}