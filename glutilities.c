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

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>
#include <stdio.h>
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

static Atom wmDeleteMessage;

int DISPLAY_MODE;

void glUtilitiesInit(int *argc, char *argv[]) {
    gettimeofday(&TIME, NULL);
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

    XSetNormalHints(d, window, &hints);
    XSetStandardProperties(d, window, n, n, None, (char **)NULL, 0, &hints);

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

void glUtilitiesSwapBuffers() {
	glFlush();
	glXSwapBuffers(DISPLAY, WINDOW);
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

void glUtilitiesMouseFunc(void (*func)(int b, int s, int x, int y)) {
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
        case 80:
        case 111:
            buffer[0] = KEY_UP;
            break;
        case 85:
        case 114:
            buffer[0] = KEY_RIGHT;
            break;
        case 88:
        case 116:
            buffer[0] = KEY_DOWN;
            break;
        case 83:
        case 113:
            buffer[0] = KEY_LEFT;
            break;
        case 67:
            buffer[0] = KEY_F1;
            break;
        case 68:
            buffer[0] = KEY_F2;
            break;
        case 69:
            buffer[0] = KEY_F3;
            break;
        case 70:
            buffer[0] = KEY_F4;
            break;
        case 71:
            buffer[0] = KEY_F5;
            break;
        case 72:
            buffer[0] = KEY_F6;
            break;
        case 73:
            buffer[0] = KEY_F7;
            break;
        case 81:
        case 112:
            buffer[0] = KEY_PAGE_UP;
            break;
        case 89:
        case 117:
            buffer[0] = KEY_PAGE_DOWN;
            break;
        case 79:
        case 110:
            buffer[0] = KEY_HOME;
            break;
        case 87:
        case 115:
            buffer[0] = KEY_END;
            break;
        case 90:
        case 118:
            buffer[0] = KEY_INSERT;
            break;
        case 50:
            buffer[0] = MOD_KEY_LEFT_SHIFT;
            break;
        case 62:
            buffer[0] = MOD_KEY_RIGHT_SHIFT;
            break;
        case 37:
        case 105:
            buffer[0] = MOD_KEY_CONTROL;
            break;
        case 64:
        case 108:
            buffer[0] = MOD_KEY_ALT;
            break;
        case 82:
            buffer[0] = 127;
            break;
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

static void check_timers();
void glUtilitiesMain() {
    char pressed = 0;
    int i;

    XAllowEvents(DISPLAY, AsyncBoth, CurrentTime);

    glUtilitiesTimerFunc(100, timer, 0);

    while(RUNNING) {
        while(XPending(DISPLAY) > 0) {
            XEvent e;
            XNextEvent(DISPLAY, &e);

            switch(e.type) {
                case ClientMessage:
                    if(e.xclient.data.l[0] == wmDeleteMessage) {
                        RUNNING = 0;
                    }
                    break;
                case Expose:
                    break;
                case ConfigureNotify:
                    if(reshape) {
                        reshape(e.xconfigure.width, e.xconfigure.height);
                    }
                    else {
                        glViewport(0, 0, e.xconfigure.width, e.xconfigure.height);
                    }
                    ANIMATE = 1;
                    WINDOW_WIDTH = e.xconfigure.width;
                    WINDOW_HEIGHT = e.xconfigure.height;
                    break;
                case KeyPress:
                    handle_key_event(e, key, modkey, 1);
                    break;
                case KeyRelease:
                    handle_key_event(e, keyup, modkeyup, 0);
                    break;
                case ButtonPress:
                    BUTTON_PRESSED[e.xbutton.button] = 1;
                    if(mouse) {
                        switch(e.xbutton.button) {
                            case Button1:
                                mouse(MOUSE_LEFT, MOUSE_DOWN, e.xbutton.x, e.xbutton.y);
                                break;
                            case Button2:
                                mouse(MOUSE_MIDDLE, MOUSE_DOWN, e.xbutton.x, e.xbutton.y);
                                break;
                            case Button3:
                                mouse(MOUSE_RIGHT, MOUSE_DOWN, e.xbutton.x, e.xbutton.y);
                                break;
                        }
                    }
                    break;
                case ButtonRelease:
                    BUTTON_PRESSED[e.xbutton.button] = 0;
                    if(mouse) {
                        switch(e.xbutton.button) {
                            case Button1:
                                mouse(MOUSE_LEFT, MOUSE_UP, e.xbutton.x, e.xbutton.y);
                                break;
                            case Button2:
                                mouse(MOUSE_MIDDLE, MOUSE_UP, e.xbutton.x, e.xbutton.y);
                                break;
                            case Button3:
                                mouse(MOUSE_RIGHT, MOUSE_UP, e.xbutton.x, e.xbutton.y);
                                break;
                        }
                    }
                    break;
                case MotionNotify:
                    pressed = 0;
                    for(i = 0; i < 5; i++) {
                        if(BUTTON_PRESSED[i]) {
                            pressed = 1;
                        }
                    }

                    LAST_MOUSE_POS_X = e.xbutton.x;
                    LAST_MOUSE_POS_Y = e.xbutton.y;
                    
                    if(pressed && mousedragged) {
                        mousedragged(e.xbutton.x, e.xbutton.y);
                    }
                    else if(mousemoved) {
                        mousemoved(e.xbutton.x, e.xbutton.y);
                    }
                    break;
                default:
                    break;
            }
        }

        if(ANIMATE) {
            ANIMATE = 0;
            if(display) {
                display();
            }
            else {
                printf("MAIN WARNING: No display function!\n");
            }
        }
        else if(idle) {
            idle();
        }

        check_timers();
    }

    glXMakeCurrent(DISPLAY, None, NULL);
    glXDestroyContext(DISPLAY, CONTEXT);
    XDestroyWindow(DISPLAY, WINDOW);
    XCloseDisplay(DISPLAY);
}

void glUtilitiesRedisplay() {
    ANIMATE = 1;
}

int glUtilitiesGet(int t) {
    struct timeval tv;

    switch(t) {
        case ELAPSED_TIME:
            gettimeofday(&tv, NULL);
            return (tv.tv_usec - TIME.tv_usec) / 1000 + (tv.tv_sec - TIME.tv_sec) * 1000;
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

typedef struct Timer {
    int arg;
    int time;
    int repeatTime;
    void (*func)(int arg);
    char repeating;
    struct Timer *next;
    struct Timer *prev;
} Timer;
Timer *timers = NULL;

void glUtilitiesTimerFunc(int ms, void (*func)(int arg), int arg) {
    Timer *t = (Timer*)malloc(sizeof(Timer));
    t->arg = arg;
	t->time = ms + glUtilitiesGet(ELAPSED_TIME);
	t->repeatTime = 0;
	t->repeating = 0;
	t->func = func;
	t->next = timers;
	t->prev = NULL;
    
    if(timers) {
        timers->prev = t;
    }
    timers = t;
}

void glUtilitiesRepeatingTimerFunc(int ms) {
    Timer *t = (Timer*)malloc(sizeof(Timer));
    t->arg = 0;
	t->time = ms + glUtilitiesGet(ELAPSED_TIME);
	t->repeatTime = ms;
	t->repeating = 1;
	t->func = NULL;
	t->next = timers;
	t->prev = NULL;

    if(timers) {
        timers->prev = t;
    }
    timers = t;
}

static void check_timers() {
    if(timers) {
        Timer *t, *tt = NULL;

        int now = glUtilitiesGet(ELAPSED_TIME);
        int next = now + 1000; // 1 second

        t = timers;
        for(t = timers; t != NULL; t = t->next) {
            if(t->time < next) {
                next = t->time;
            }

            if(t->time < now) {
                tt = t;
            }
        }

        if(tt) {
            if(tt->func) {
                tt->func(tt->arg);
            }
            else {
                glUtilitiesRedisplay();
            }

            if(tt->repeating) {
                tt->time = now + tt->repeatTime;
            }
            else {
                if(tt->prev) {
                    tt->prev->next = tt->next;
                }
                else {
                    timers = tt->next;
                }

                if(tt->next) {
                    tt->next->prev = tt->prev;
                }
                free(tt);
            }
        }

        if(!ANIMATE) {
            if(next > now) {
                usleep((next - now) * 1000);
            }
        }        
    }
    else {
        if(!ANIMATE) {
            usleep(10);
        }
    }
}

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

/*

SHADER UTILITIES

*/

char* read_file(char *file) {
    if(!file) {
        return NULL;
    }

    FILE *fptr = fopen(file, "rb");
    char *buf;
    long len;

    if(!fptr) {
        return NULL;
    }

    fseek(fptr, 0, SEEK_END);
    len = ftell(fptr);
    buf = (char*)malloc(len + 1);

    memset(buf, 0, sizeof(char)*(len + 1));
    fseek(fptr, 0, SEEK_SET);
    fread(buf, len, 1, fptr);
    fclose(fptr);

    buf[len] = 0;
    return buf;
}

void print_shader_log(GLuint obj, const char *fn) {
    GLint written = 0;
    GLint logLen = 0;

    glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &logLen);

    char *log;
    if(logLen > 2) {
        fprintf(stderr, "[from %s:]\n", fn);
        
        log = (char *)malloc(logLen);
        glGetShaderInfoLog(obj, logLen, &written, log);
        
        fprintf(stderr, "[from %s:]\n", log);
        free(log);
    }
}

void print_program_log(GLuint obj, const char *vfn, const char *ffn, const char *gfn, const char *tcfn, const char *tefn) {
    GLint written = 0;
    GLint logLen = 0;
	
    glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &logLen);

    char *log;
    if(logLen > 2) {
        if(ffn == NULL) {
			fprintf(stderr, "[from %s:]\n", vfn);
        }
        else if(gfn == NULL) {
			fprintf(stderr, "[from %s+%s:]\n", vfn, ffn);
        }
        else if(tcfn == NULL || tefn == NULL) {
			fprintf(stderr, "[from %s+%s+%s:]\n", vfn, ffn, gfn);
        }
        else {
			fprintf(stderr, "[from %s+%s+%s+%s+%s:]\n", vfn, ffn, gfn, tcfn, tefn);
        }

        log = (char *)malloc(logLen);
        glGetShaderInfoLog(obj, logLen, &written, log);

        fprintf(stderr, "%s\n", log);
        free(log);
    }
}

GLuint compile_shaders(const char *vs, const char *fs, const char *gs, const char *tcs, const char *tes, const char *vp, const char *fp, const char *gp, const char *tp1, const char *tp2) {
	GLuint v = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(v, 1, &vs, NULL);
	glCompileShader(v);
    
    GLuint f, g, tc, te;
    if(fs) {
		f = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(f, 1, &fs, NULL);
		glCompileShader(f);
    }

    if(gs) {
        g = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(g, 1, &gs, NULL);
		glCompileShader(g);
    }

#ifdef GL_TESS_CONTROL_SHADER
    if(tcs) {
        tc = glCreateShader(GL_TESS_CONTROL_SHADER);
		glShaderSource(tc, 1, &tp1, NULL);
		glCompileShader(tc);
    }

    if(tes) {
        te = glCreateShader(GL_TESS_EVALUATION_SHADER);
		glShaderSource(te, 1, &tp2, NULL);
		glCompileShader(te);
    }
#endif

	GLuint p = glCreateProgram();
    if(vs) {
        glAttachShader(p, v);
    }

    if(fs) {
        glAttachShader(p, f);
    }

    if(gs) {
        glAttachShader(p, g);
    }

    if(tcs) {
        glAttachShader(p, tc);
    }

    if(tes) {
        glAttachShader(p, te);
    }
    glLinkProgram(p);
	glUseProgram(p);

    if(vs) {
        print_shader_log(v, vp);
    }

    if(fs) {
        print_shader_log(f, fp);
    }

    if(gs) {
        print_shader_log(g, gp);
    }

    if(tcs) {
        print_shader_log(tc, tp1);
    }

    if(tes) {
        print_shader_log(te, tp2);
    }

    print_program_log(p, vp, fp, gp, tp1, tp2);
    return p;
}

GLuint glUtilitiesLoadShaders(const char *vp, const char *fp) {
    return glUtilitiesLoadGeoTexShaders(vp, fp, NULL, NULL, NULL);
}

GLuint glUtilitiesLoadGeoShaders(const char *vp, const char *fp, const char *gp) {
	return glUtilitiesLoadGeoTexShaders(vp, fp, gp, NULL, NULL);
}

GLuint glUtilitiesLoadGeoTexShaders(const char *vp, const char *fp, const char *gp, const char *tp1, const char *tp2) {
    char *vs = read_file((char *)vp);
    if(!vs && vp) {
		fprintf(stderr, "Failed to read %s from disk.\n", vp);
    }

    char *fs = read_file((char *)fp);
    if(!fs && fp) {
		fprintf(stderr, "Failed to read %s from disk.\n", fp);
    }

    char *gs = read_file((char *)gp);
    if(!gs && gp) {
		fprintf(stderr, "Failed to read %s from disk.\n", gp);
    }

    char *tcs = read_file((char *)tp1);
    if(tcs && tp1) {
		fprintf(stderr, "Failed to read %s from disk.\n", tp1);
    }

    char *tes = read_file((char *)tp2);
    if(tes && tp2) {
		fprintf(stderr, "Failed to read %s from disk.\n", tp2);
    }

	GLuint p = 0;
    if(vs && fs) {
        p = compile_shaders(vs, fs, gs, tcs, tes, vp, fp, gp, tp1, tp2);
    }

    if(vs) {
        free(vs);
    }

    if(fs) {
        free(fs);
    }

    if(gs) {
        free(gs);
    }

    if(tcs) {
        free(tcs);
    }

    if(tes) {
        free(tes);
    }

    return p;
}

void glUtilitiesDump(void) {
    printf("vendor: %s\n", glGetString(GL_VENDOR));
    printf("renderer: %s\n", glGetString(GL_RENDERER));
    printf("version: %s\n", glGetString(GL_VERSION));
    printf("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    
    glUtilitiesReportError("dump");
}

static GLenum LAST_ERR = 0;
static char LAST_ERR_FUNC[1024] = "";
void glUtilitiesReportError(const char *n) {
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        if(LAST_ERR != err || strcmp(n, LAST_ERR_FUNC)) {
            fprintf(stderr, "GL error 0x%X detected in %s\n", err, n);
	        strcpy(LAST_ERR_FUNC, n);
            LAST_ERR = err;
        }
    }
}

/*

FBO UTILITIES

*/

void buffer_status() {
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "Framebuffer not complete!\n");
    }
}

FBOData *glUtilitiesInitFBO(int w, int h, int ifunc) {
    FBOData *fbo = (FBOData *)malloc(sizeof(FBOData));
    fbo->w = w;
    fbo->h = h;

    glGenFramebuffers(1, &fbo->fb);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->fb);
	glGenTextures(1, &fbo->texID);
	fprintf(stderr, "%i\n",fbo->texID);
	glBindTexture(GL_TEXTURE_2D, fbo->texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if(ifunc == 0) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo->texID, 0);
    
    glGenRenderbuffers(1, &fbo->rb);
    glBindRenderbuffer(GL_RENDERBUFFER, fbo->rb);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, fbo->w, fbo->h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fbo->rb);
    buffer_status();

	fprintf(stderr, "FBO %d\n", fbo->fb);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return fbo;
}

FBOData *glUtilitiesInitFBODepth(int w, int h, int ifunc, int depth) {
    FBOData *fbo = (FBOData *)malloc(sizeof(FBOData));
    fbo->w = w;
    fbo->h = h;

    glGenRenderbuffers(1, &fbo->rb);
    glGenFramebuffers(1, &fbo->fb);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->fb);
    glGenTextures(1, &fbo->texID);
    fprintf(stderr, "%i\n",fbo->texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    if(ifunc) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo->texID, 0);
    if (depth != 0) {
        glGenTextures(1, &fbo->depth);
        glBindTexture(GL_TEXTURE_2D, fbo->depth);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, w, h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0L);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fbo->depth, 0);
        fprintf(stderr, "Depth Tex: %i\n",fbo->depth);
    }

    glBindRenderbuffer(GL_RENDERBUFFER, fbo->rb);
    buffer_status();

    fprintf(stderr, "FBO %d\n", fbo->fb);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return fbo;
}

static int LAST_W = 0;
static int LAST_H = 0;
void glUtilitiesUpdateScreenFBO(int w, int h) {
    LAST_W = w;
    LAST_H = h;
}

void glUtilitiesUseFBO(FBOData *out, FBOData *in1, FBOData *in2) {
    GLint curfbo;

    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &curfbo);
    if(curfbo == 0) {
		GLint viewport[4] = {0, 0, 0, 0};
		glGetIntegerv(GL_VIEWPORT, viewport);

		GLint w, h;
        w = viewport[2] - viewport[0];
		h = viewport[3] - viewport[1];

        static unsigned int FBW = 65536;
        if(w > 0 && h > 0 && w < FBW && h < FBW) {
            glUtilitiesUpdateScreenFBO(w, h);
        }
    }

    if(out != 0L) {
		glViewport(0, 0, out->w, out->h);
    }
    else {
		glViewport(0, 0, LAST_W, LAST_H);
    }

    if(out != 0L) {
		glBindFramebuffer(GL_FRAMEBUFFER, out->fb);
		glViewport(0, 0, out->w, out->h);
    }
    else {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

	glActiveTexture(GL_TEXTURE0);
    if(in1 != 0L) {
		glBindTexture(GL_TEXTURE_2D, in1->texID);
    }
    else {
		glBindTexture(GL_TEXTURE_2D, 0);
    }

	glActiveTexture(GL_TEXTURE1);
    if(in2 != 0L) {
		glBindTexture(GL_TEXTURE_2D, in2->texID);
    }
    else {
		glBindTexture(GL_TEXTURE_2D, 0);
    }
}

/*

MODEL UTILITIES

*/

static char LINE_END = 0;
static char FILE_END = 0;

static char parse_string(char *line, int *pos, char *s) {
    char c = line[(*pos)++];
    while(c == 32 || c == 9) {
        c = line[(*pos)++]; // skip trailing spaces
    }

    int i = 0;
    while(c != 0 && c != 32 && c != 9  && c != EOF && c != '/' && i < 254) {
		s[i++] = c;
		c = line[(*pos)++];
    }

    LINE_END = (c == 0);
	s[i] = 0;

    return c;
}

static float parse_float(char *line, int *pos) {
	char s[255];
	parse_string(line, pos, s);
	
    float val = 0.0;
	sscanf(s, "%f", &val);
	if (isnan(val) || isinf(val))  {
		val = 0.0;
    }
    
	return val;
}

static int parse_int(char *line, int *pos, char *endc) {
	char s[255];
    char c = parse_string(line, pos, s);
	if (endc) {
        *endc = c;
    }

	int val = 0;
	if (strlen(s) == 0) {
		val = -1;
    }
    else {
		sscanf(s, "%d", &val);
    }

	return val;
}

static Vector3 parse_vec3(char *line, int *pos) {
	Vector3 v;
	v.x = parse_float(line, pos);
	v.y = parse_float(line, pos);
	v.z = parse_float(line, pos);
	return v;
}

void parse_line(FILE *fp, char *line) {
	int i, j;
	char c = '_';
	for (i = 0, j = 0; i < 2048; i++) {
		c = getc(fp);
		if (c != 10 && c != 13) {
			line[j++] = c;
        }

		if (c == EOF || ((c == 10 || c == 13) && j > 0)) {
			if (c == EOF) {
                FILE_END = 1;
            }
			line[j] = 0;
			return;
        }
    }
	line[j] = 0;
}

static void dispose_material_list(Material **materials) {
    if(materials) {
        for(int i = 0; materials[i] != NULL; i++) {
		    free(materials[i]);
        }
    }
	free(materials);
}

static Material **parse_material(char *n) {
    FILE *fp = fopen(n, "rb");
	if (!fp) {
		return NULL;
    }

    LINE_END = 0;
    FILE_END = 0;

    int pos;
    char s[255];
    char line[2048];
    int materialCount = 0;
    while(!FILE_END) {
        parse_line(fp, line);
        pos = 0;
        parse_string(line, &pos, s);

        if(strcmp(s, "newmtl") == 0) {
            materialCount++;
        }
    }
    rewind(fp);

    Material **materials = (Material **)calloc(sizeof(MaterialPtr) * (materialCount + 1), 1);

    materialCount = 0;
	LINE_END = 0;
	FILE_END = 0;

    Material *m;
    while(!FILE_END) {
        parse_line(fp, line);
        pos = 0;
        parse_string(line, &pos, s);

        if(strcmp(s, "newmtl") == 0) {
            materialCount++;
            materials[materialCount - 1] = (Material *)calloc(sizeof(Material), 1);
			m = materials[materialCount - 1];
			materials[materialCount] = NULL;

            parse_string(line, &pos, m->newmtl);
        }

        if(m) {
            if(strcmp(s, "Ka") == 0) {
				m->Ka = parse_vec3(line, &pos);
            }

            if(strcmp(s, "Kd") == 0) {
				m->Kd = parse_vec3(line, &pos);
            }

            if(strcmp(s, "Ks") == 0) {
				m->Ks = parse_vec3(line, &pos);
            }

            if(strcmp(s, "Ke") == 0) {
				m->Ke = parse_vec3(line, &pos);
            }

            if(strcmp(s, "Tr") == 0) {
				m->Tr = parse_float(line, &pos);
				m->d = 1 - m->Tr;
            }

            if(strcmp(s, "d") == 0) {
				m->d = parse_float(line, &pos);
				m->Tr = 1 - m->d;
            }

            if(strcmp(s, "illum") == 0) {
				m->illumination = parse_int(line, &pos, NULL);
            }

            if(strcmp(s, "map_Ka") == 0) {
				parse_string(line, &pos, m->map_Ka);
            }

            if(strcmp(s, "map_Kd") == 0) {
				parse_string(line, &pos, m->map_Kd);
            }

            if(strcmp(s, "map_Ks") == 0) {
				parse_string(line, &pos, m->map_Ks);
            }

            if(strcmp(s, "map_Ke") == 0) {
				parse_string(line, &pos, m->map_Ke);
            }

            if(strcmp(s, "map_d") == 0) {
				parse_string(line, &pos, m->map_d);
            }

            if(strcmp(s, "map_bump") == 0) {
				parse_string(line, &pos, m->map_bump);
            }

            if(strcmp(s, "bump") == 0) {
				parse_string(line, &pos, m->map_bump);
            }
        }
    }
    fclose(fp);

    return materials;
}

typedef struct Mesh {
	Vector3	*vertexNormals;
	Vector2	*textureCoords;
	Vector3	*vertices;
	
	int		normalsCount;
    int		vertexCount;
	int		coordCount;
	int		groupCount;
	int		texCount;
	
	int		*coordIndex;
	int		*normalsIndex;
	int		*textureIndex;

	int		*coordStarts;
	
	char	*materialName;
} Mesh, *MeshPtr;

static int NORMAL_COUNT;
static int COORD_COUNT;
static int VERT_COUNT;
static int TEX_COUNT;

static int ZERO_FIX;

static char HAS_POS_IDXS;
static char HAS_TEX_IDXS;
static char HAS_NORMAL_IDXS;

Material **MATERIALS;

char **MATERIAL_NAME_LIST;
char *MATERIAL_LIB_NAME = NULL;

static char parse_obj(const char *n, MeshPtr mp) {
    int lastCoordCount = -1;
    
    FILE *fp = fopen(n, "rb");
    if(!fp) {
		fprintf(stderr, "File \"%s\" could not be opened\n", n);
        return -1;
    }

    NORMAL_COUNT = 0;
    COORD_COUNT = 0;
    VERT_COUNT = 0;
    TEX_COUNT = 0;

    LINE_END = 0;
    FILE_END = 0;

    while(!FILE_END) {
		char line[2048];
		parse_line(fp, line);

        int pos = 0;
        char s[256];
		parse_string(line, &pos, s);
		if (strcmp(s, "v") == 0) {
			Vector3 v = parse_vec3(line, &pos);
            if(mp->vertices) {
                mp->vertices[VERT_COUNT++] = v;
            }
            else {
                VERT_COUNT += 1;
            }
        }

		if (strcmp(s, "vn") == 0) {
			Vector3 v = parse_vec3(line, &pos);
            if(mp->vertexNormals) {
                mp->vertexNormals[NORMAL_COUNT++] = v;
            }
            else {
                NORMAL_COUNT += 1;
            }
        }

		if (strcmp(s, "vt") == 0) {
			Vector3 v = parse_vec3(line, &pos);
            if(mp->textureCoords) {
                mp->textureCoords[TEX_COUNT].x = v.x;
                mp->textureCoords[TEX_COUNT++].y = v.y;
            }
            else {
                TEX_COUNT += 1;
            }
        }

		if (strcmp(s, "f") == 0) {
			char done = 0;
			char *ns;

			int i = -1;
            int p = -1;
			int p1;

            for(i = pos; !done; pos++) {
				if (line[pos] == 0) {
                    done = 1;
                }

                if(p >= 0) {
					char c = line[pos];
					if (c == '/' || c == ' ' || c == 0) {
                        ns = &line[p1];
                        line[pos] = 0;
                        if(p1 != pos) {
							i = atoi(ns);
							if(i == 0) {
                                ZERO_FIX = 1;
                            }

                            if(i < 0) {
								switch (p) {
                                    case 0:
                                        i = VERT_COUNT + i + 1;
                                        break;
                                    case 1:
                                        i = TEX_COUNT + i + 1;
                                        break;
                                    case 2:
                                        i = NORMAL_COUNT + i + 1;
                                        break;
                                }
                            }
                            i = i + ZERO_FIX;
                        }
                        else {
                            i = 0;
                        }

                        if(i > 0) {
						    switch (p) {
							    case 0:
                                    if(mp->coordIndex) {
                                        mp->coordIndex[COORD_COUNT - 1] = i - 1;
                                    }
                                    break;
							    case 1:
                                    if(mp->textureIndex) {
                                        if(i >= 0) {
                                            mp->textureIndex[COORD_COUNT - 1] = i - 1;
                                        }
                                    }
                                    HAS_TEX_IDXS = 1;
                                    break;
							    case 2:
                                    if(mp->normalsIndex) {
                                        if(i >= 0) {
                                            mp->normalsIndex[COORD_COUNT - 1] = i - 1;
                                        }
                                    }
                                    HAS_NORMAL_IDXS = 1;
                                    break;
                            }
                            p1 = pos + 1;
                            p++;
                        }
                    }

                    if(c == ' ') {
                        p = -1;
                    }
                }
                else if(line[pos] != ' ') {
                    if(!done) {
                        COORD_COUNT++;
                    }
                    p1 = pos;
                    p = 0;
                }
            }

            if(mp->coordIndex) {
                mp->coordIndex[COORD_COUNT] = -1;
            }
            
            if(mp->textureIndex) {
                mp->textureIndex[COORD_COUNT] = -1;
            }

            if(mp->normalsIndex) {
                mp->normalsIndex[COORD_COUNT] = -1;
            }
            COORD_COUNT++;
        }

        if(strcmp(s, "mtllib") == 0) {
			char libname[256];
        	char tmp[255];
			int index = 0;
			for(unsigned int i = 0; i < strlen(n); i++) {
				if(n[i] == '/') {
					index = i;
                }
            }

            if(index != 0) {
				for(int i = 0; i < index + 1; i++) {
					char ch = n[i];
					strncat(tmp, &ch, 1);
                }
            }

            parse_string(line, &pos, libname);
			MATERIAL_LIB_NAME = (char *)malloc(strlen(libname) + 1);
			strcpy(MATERIAL_LIB_NAME, libname);
        }

		if (strcmp(s, "usemtl") == 0) {
            if (COORD_COUNT > 0) {
				if (lastCoordCount != COORD_COUNT) {
					mp->groupCount += 1;
					if (mp->coordStarts)  {
						mp->coordStarts[mp->groupCount] = COORD_COUNT;
                    }
					lastCoordCount = COORD_COUNT;
				}
				else {
					printf("Ignored part!\n"); // TODO: Update this
                }
			}

            if(MATERIAL_NAME_LIST) {
                char matname[255];
                parse_string(line, &pos, matname);
                MATERIAL_NAME_LIST[mp->groupCount] = (char *)malloc(strlen(matname) + 1);
                strcpy(MATERIAL_NAME_LIST[mp->groupCount], matname);
            }
        }
    }

    if(COORD_COUNT > lastCoordCount) {
        mp->groupCount += 1;
        if(mp->coordStarts) {
            mp->coordStarts[mp->groupCount] = COORD_COUNT;
        }
    }

    fclose(fp);
    
    if(MATERIAL_LIB_NAME) {
		MATERIALS = parse_material(MATERIAL_LIB_NAME);
    }

    if(!MATERIALS) {
	    if (strlen(n) > 4) {
            char tryname[255];
            strcpy(tryname, n);
            tryname[strlen(tryname) - 4] = '_';
            strcat(tryname, ".mtl");
            MATERIALS = parse_material(tryname);
        }
    }

    if(!MATERIALS) {
	    if (strlen(n) > 4) {
            char tmpname[255];
            strcpy(tmpname, n);
            tmpname[strlen(tmpname) - 4] = 0;
            strcat(tmpname, ".mtl");
            MATERIALS = parse_material(tmpname);
        }
    }

    return 0;
}

static struct Mesh *load_obj(const char *n) {
	Mesh *mp = (Mesh *)calloc(sizeof(Mesh), 1);

    HAS_NORMAL_IDXS = 0;
    HAS_TEX_IDXS = 0;
    HAS_POS_IDXS = 1;

    mp->coordStarts = NULL;
    mp->groupCount = 0;

    MATERIAL_NAME_LIST = NULL;
	MATERIAL_LIB_NAME = NULL;
	MATERIALS = NULL;

    NORMAL_COUNT = 0;
    COORD_COUNT = 0;
    VERT_COUNT = 0;
    TEX_COUNT = 0;
    ZERO_FIX = 0;

    parse_obj(n, mp);

    if(VERT_COUNT > 0) {
        mp->vertices = (Vector3 *)malloc(sizeof(Vector3) * VERT_COUNT);
    }

    if(TEX_COUNT > 0) {
        mp->textureCoords = (Vector2 *)malloc(sizeof(Vector2) * TEX_COUNT);
    }

    if(NORMAL_COUNT > 0) {
        mp->vertexNormals = (Vector3 *)malloc(sizeof(Vector3) * NORMAL_COUNT);
    }

    if(HAS_POS_IDXS) {
        mp->coordIndex = (int *)calloc(COORD_COUNT, sizeof(int));
    }

    if(HAS_NORMAL_IDXS) {
        mp->normalsIndex = (int *)calloc(COORD_COUNT, sizeof(int));
    }

    if(HAS_TEX_IDXS) {
        mp->textureIndex = (int *)calloc(COORD_COUNT, sizeof(int));
    }

	MATERIAL_NAME_LIST = (char **)calloc(sizeof(char *) * (mp->groupCount + 1), 1);
    mp->coordStarts = (int *)calloc(sizeof(int) * (mp->groupCount + 2), 1);
	mp->groupCount = 0;

    NORMAL_COUNT = 0;
    COORD_COUNT = 0;
    VERT_COUNT = 0;
    TEX_COUNT = 0;

    parse_obj(n, mp);

    mp->normalsCount = NORMAL_COUNT;
    mp->vertexCount = VERT_COUNT;
    mp->coordCount = COORD_COUNT;
    mp->texCount = TEX_COUNT;

    return mp;
}

void to_triangles(struct Mesh *mp) {
    int *newTextureIndex;
    int *newNormalsIndex;
    int *newCoords;

    int newIndex = 0;
    int first = 0;
   
    int i;
    int vertexCount = 0;
    int triangleCount = 0;
    for(i = 0; i < mp->coordCount; i++) {
        if(mp->coordIndex[i] == -1) {
            if(vertexCount > 2) {
                triangleCount += vertexCount - 2;
            }
            vertexCount = 0;
        }
        else {
            vertexCount = vertexCount + 1;
        }
    }

    newCoords = (int *)calloc(triangleCount * 3, sizeof(int));
    if(mp->normalsIndex) {
		newNormalsIndex = (int *)calloc(triangleCount * 3, sizeof(int));
    }

    if(mp->textureIndex) {
		newTextureIndex = (int *)calloc(triangleCount * 3, sizeof(int));
    }

    vertexCount = 0;
    for(i = 0; i < mp->coordCount; i++) {
		if (mp->coordIndex[i] == -1) {
			vertexCount = 0;
			first = i + 1;
        }
        else {
			vertexCount = vertexCount + 1;
			if (vertexCount > 2) {
                newCoords[newIndex++] = mp->coordIndex[first];
				newCoords[newIndex++] = mp->coordIndex[i - 1];
				newCoords[newIndex++] = mp->coordIndex[i];
                if(mp->normalsIndex) {
					newNormalsIndex[newIndex - 3] = mp->normalsIndex[first];
					newNormalsIndex[newIndex - 2] = mp->normalsIndex[i - 1];
					newNormalsIndex[newIndex - 1] = mp->normalsIndex[i];
                }

                if(mp->textureIndex) {
					newTextureIndex[newIndex - 3] = mp->textureIndex[first];
					newTextureIndex[newIndex - 2] = mp->textureIndex[i - 1];
					newTextureIndex[newIndex - 1] = mp->textureIndex[i];
                }
            }
        }
    }

    free(mp->coordIndex);
    mp->coordIndex = newCoords;
	mp->coordCount = triangleCount * 3;

    if(mp->normalsIndex) {
        free(mp->normalsIndex);
		mp->normalsIndex = newNormalsIndex;
    }

    if(mp->textureIndex) {
        free(mp->textureIndex);
		mp->textureIndex = newTextureIndex;
    }
}

static void generate_normals(Mesh* m) {
    if(m->vertices && !m->vertexNormals) {
        m->vertexNormals = (Vector3 *)calloc(sizeof(Vector3) * m->vertexCount, 1);
        m->normalsCount = m->vertexCount;
        m->normalsIndex = (int *)calloc(m->coordCount, sizeof(GLuint));
		memcpy(m->normalsIndex, m->coordIndex, sizeof(GLuint) * m->coordCount);
    
		int face;
		int normalIndex;
        for(face = 0; face * 3 < m->coordCount; face++) {
            int i0 = m->coordIndex[face * 3 + 0];
            int i1 = m->coordIndex[face * 3 + 1];
            int i2 = m->coordIndex[face * 3 + 2];

            Vector3 v0 = SubV3(m->vertices[i1], m->vertices[i0]);
            Vector3 v1 = SubV3(m->vertices[i2], m->vertices[i0]);
            Vector3 v2 = SubV3(m->vertices[i2], m->vertices[i1]);

            float sqrLen0 = Dot(v0, v0);
			float sqrLen1 = Dot(v1, v1);
			float sqrLen2 = Dot(v2, v2);

            float len0 = 1e-3f;
            if(sqrLen0 >= 1e-6f) {
                len0 = sqrt(sqrLen0);
            }

            float len1 = 1e-3f;
            if(sqrLen1 >= 1e-6f) {
                len1 = sqrt(sqrLen1);
            }

            float len2 = 1e-3f;
            if(sqrLen2 >= 1e-6f) {
                len2 = sqrt(sqrLen2);
            }

            float influence0 =  Dot(v0, v1) / (len0 * len1);
			float influence1 = -Dot(v0, v2) / (len0 * len2);
			float influence2 =  Dot(v1, v2) / (len1 * len2);

            float angle0 = acos(influence0);
            if(influence0 >= 1.f) {
                angle0 = 0;
            }
            else if(influence0 <= -1.f) {
                angle0 = M_PI;
            }

            float angle1 = acos(influence1);
            if(influence1 >= 1.f) {
                angle1 = 0;
            }
            else if(influence1 <= -1.f) {
                angle1 = M_PI;
            }

            float angle2 = acos(influence2);
            if(influence2 >= 1.f) {
                angle2 = 0;
            }
            else if(influence2 <= -1.f) {
                angle2 = M_PI;
            }

            Vector3 normal = Cross(v0, v1);
			m->vertexNormals[i0] = AddV3(Scalar(normal, angle0), m->vertexNormals[i0]);
			m->vertexNormals[i1] = AddV3(Scalar(normal, angle1), m->vertexNormals[i1]);
			m->vertexNormals[i2] = AddV3(Scalar(normal, angle2), m->vertexNormals[i2]);
        }

        for(normalIndex = 0; normalIndex < m->normalsCount; normalIndex++) {
			float len = Norm(m->vertexNormals[normalIndex]);
            if(len > 0.01f) {
				m->vertexNormals[normalIndex] = Scalar(m->vertexNormals[normalIndex], 1 / len);
            }
        }
    }
}

static Model* generate_model(Mesh* m) {
    typedef struct {
        int posIndex;
        int texIndex;
        int normalIndex;
        int newIndex;
    } IndexTriplet;

    int hashGap = 6;
    int indexHMSize = (m->vertexCount * hashGap + m->coordCount);
    
    IndexTriplet *indexHashMap = (IndexTriplet *)malloc(sizeof(IndexTriplet) * indexHMSize);
    
    int numNewVertices = 0;

    Model* model = (Model *)malloc(sizeof(Model));
	memset(model, 0, sizeof(Model));

    model->indexArray = (GLuint *)malloc(sizeof(GLuint) * m->coordCount);
	model->numIndices = m->coordCount;

	memset(indexHashMap, 0xff, sizeof(IndexTriplet) * indexHMSize);

    int i;
    int max;
    for(i = 0; i < m->coordCount; i++) {
        IndexTriplet currVert = {-1, -1, -1, -1};
		int insertPos = 0;

        if(m->coordIndex) {
			currVert.posIndex = m->coordIndex[i];
        }

        if(m->normalsIndex) {
			currVert.normalIndex = m->normalsIndex[i];
        }

        if(m->textureIndex) {
			currVert.texIndex = m->textureIndex[i];
        }

        if(max < currVert.texIndex) {
			max = currVert.texIndex;
        }

		if (currVert.posIndex >= 0) {
			insertPos = currVert.posIndex * hashGap;
        }

        while(true) {
			if (indexHashMap[insertPos].newIndex == -1) {
				currVert.newIndex = numNewVertices++;
				indexHashMap[insertPos] = currVert;
				break;
            }
            else if(indexHashMap[insertPos].posIndex == currVert.posIndex && indexHashMap[insertPos].normalIndex == currVert.normalIndex && indexHashMap[insertPos].texIndex == currVert.texIndex) {
				currVert.newIndex = indexHashMap[insertPos].newIndex;
                break;
            }
            else {
				insertPos++;
            }
        }

        model->indexArray[i] = currVert.newIndex;
    }

    for(i = 0; i < indexHMSize; i++) {
		if (indexHashMap[i].newIndex != -1) {
			if (m->vertices) {
				model->vertexArray[indexHashMap[i].newIndex] = m->vertices[indexHashMap[i].posIndex];
            }

            if (m->vertexNormals) {
				model->normalArray[indexHashMap[i].newIndex] = m->vertexNormals[indexHashMap[i].normalIndex];
            }

            if (m->textureCoords) {
				model->texCoordArray[indexHashMap[i].newIndex] = m->textureCoords[indexHashMap[i].texIndex];
            }
        }
    }

    free(indexHashMap);

	if (MATERIALS) {
	    if (m->materialName) {
		    for (int ii = 0; MATERIALS[ii] != NULL; ii++) {
                Material *mtl = MATERIALS[ii];
                if(strcmp(m->materialName, mtl->newmtl)) {
				    model->material = (Material *)malloc(sizeof(Material));
				    memcpy(model->material, mtl, sizeof(Material));

                    strcpy(model->material->map_Ka, mtl->map_Ka);
                    strcpy(model->material->map_Kd, mtl->map_Kd);
                    strcpy(model->material->map_Ks, mtl->map_Ks);
                    strcpy(model->material->map_Ke, mtl->map_Ke);
                    strcpy(model->material->map_Ns, mtl->map_Ns);
                    strcpy(model->material->map_d, mtl->map_d);
                    strcpy(model->material->map_bump, mtl->map_bump);
                }
            }
        }
    }

    return model;
}

Mesh **split_to_meshes(Mesh *m) {
    int *mapc = (int *)malloc(m->vertexCount * sizeof(int));
	int *mapt = (int *)malloc(m->texCount * sizeof(int));
	int *mapn = (int *)malloc(m->normalsCount * sizeof(int));

	if (m == NULL || m ->vertices == NULL || m->vertexCount == 0) {
		printf("Invalid mesh!\n");
		return NULL;
    }

	Mesh **mm = (Mesh **)calloc(sizeof(Mesh *), m->groupCount + 2);
	for (int mi = 0; mi < m->groupCount; mi++) {
		int from = m->coordStarts[mi];
		int to = m->coordStarts[mi + 1];

		mm[mi] = (Mesh *)calloc(sizeof(Mesh), 1);

		for (int i = 0; i < m->vertexCount; i++) {
			mapc[i] = -1;
        }

		for (int i = 0; i < m->texCount; i++) {
			mapt[i] = -1;
        }

		for (int i = 0; i < m->normalsCount; i++) {
			mapn[i] = -1;
        }

		int intNormalsCount = 0;
        int intVertexCount = 0;
		int intTexCount = 0;

        for (int j = from; j < to; j++) {
			int ix = m->coordIndex[j];
			if (ix > -1) {
				if (mapc[ix] == -1) {
					mapc[ix] = ix;
					intVertexCount++;
                }
            }
			if (m->textureIndex) {
				ix = m->textureIndex[j];
				if (ix > -1) {
					if (mapt[ix] == -1) {
						mapt[ix] = ix;
						intTexCount++;
                    }
                }
            }
			if (m->normalsIndex) {
				ix = m->normalsIndex[j];
				if (ix > -1) {
					if (mapn[ix] == -1) {
						mapn[ix] = ix;
						intNormalsCount++;
                    }
                }
            }
        }

        mm[mi]->coordIndex = (int *)malloc((to - from) * sizeof(int));
		mm[mi]->textureIndex = (int *)malloc((to - from) * sizeof(int));
		mm[mi]->normalsIndex = (int *)malloc((to - from) * sizeof(int));

		if (intVertexCount > 0) {
			mm[mi]->vertices = (Vector3 *)malloc(intVertexCount * sizeof(Vector3));
        }

		if (intTexCount > 0) {
			mm[mi]->textureCoords = (Vector2 *)malloc(intTexCount * sizeof(Vector2));
        }

		if (intNormalsCount > 0) {
			mm[mi]->vertexNormals = (Vector3 *)malloc(intNormalsCount * sizeof(Vector3));
        }

        mm[mi]->vertexCount = intVertexCount;
		mm[mi]->texCount = intTexCount;
		mm[mi]->normalsCount = intNormalsCount;

		for (int i = 0; i < m->vertexCount; i++) {
			mapc[i] = -1;
        }
        
		for (int i = 0; i < m->texCount; i++) {
			mapt[i] = -1;
        }

		for (int i = 0; i < m->normalsCount; i++) {
			mapn[i] = -1;
        }

        int mapcCount = 0;
		int maptCount = 0;
		int mapnCount = 0;
        for (int j = from; j < to; j++) {
			int ix = m->coordIndex[j];
			if (ix > -1) {
                if (mapc[ix] == -1) {
					mapc[ix] = mapcCount++;
					mm[mi]->vertices[mapc[ix]] = m->vertices[ix];
                }
				mm[mi]->coordIndex[j - from] = mapc[ix];
            }
            else {
				mm[mi]->coordIndex[j - from] = -1;
            }

			if (m->textureIndex) {
			    if (mm[mi]->textureIndex) {
				    ix = m->textureIndex[j];
				    if (ix > -1) {
					    if (mapt[ix] == -1) {
						    mapt[ix] = maptCount++;
						    mm[mi]->textureCoords[mapt[ix]] = m->textureCoords[ix];
                        }
					    mm[mi]->textureIndex[j - from] = mapt[ix];
                    }
                }
                else {
					mm[mi]->textureIndex[j - from] = -1;
                }
            }

            if(m->normalsIndex) {
			    if (mm[mi]->normalsIndex) {
				    ix = m->normalsIndex[j];
				    if (ix > -1) {
                        if (mapn[ix] == -1) {
						    mapn[ix] = mapnCount++;
						    mm[mi]->vertexNormals[mapn[ix]] = m->vertexNormals[ix];
                        }
					    mm[mi]->normalsIndex[j - from] = mapn[ix];
                    }
                    else {
					    mm[mi]->normalsIndex[j - from] = -1;
                    }
                }
            }
        }

		mm[mi]->coordCount = to - from;
        
        if (MATERIAL_NAME_LIST) {
			mm[mi]->materialName = MATERIAL_NAME_LIST[mi];
			MATERIAL_NAME_LIST[mi] = NULL;
		}
    }

    return mm;
}

static void dispose_mesh(Mesh *m) {
    if(m) {
        if(m->vertices) {
            free(m->vertices);
        }

        if(m->vertexNormals) {
            free(m->vertexNormals);
        }

        if(m->textureCoords) {
            free(m->textureCoords);
        }

        if(m->coordIndex) {
            free(m->coordIndex);
        }

        if(m->normalsIndex) {
            free(m->normalsIndex);
        }

        if(m->textureIndex) {
            free(m->textureIndex);
        }

        if (m->materialName) {
			free(m->materialName);
        }

		if (MATERIAL_NAME_LIST) {
			for (int i = 0; i < m->groupCount; i++) {
				if (MATERIAL_NAME_LIST[i]) {
					free(MATERIAL_NAME_LIST[i]);
                }
            }
			free(MATERIAL_NAME_LIST);
			MATERIAL_NAME_LIST = NULL;
		}

        free(m);
    }
}

void glUtilitiesCenterModel(Model *m) {
    float maxx = -1e10;
    float maxy = -1e10;
    float maxz = -1e10;
    
    float minx = 1e10;
    float miny = 1e10;
    float minz = 1e10;

    int i;
	for (i = 0; i < m->numVertices; i++) {
        if(m->vertexArray[i].x < minx) {
            minx = m->vertexArray[i].x;
        }

        if(m->vertexArray[i].x > maxx) {
            maxx = m->vertexArray[i].x;
        }

        if(m->vertexArray[i].y < miny) {
            miny = m->vertexArray[i].y;
        }

        if(m->vertexArray[i].y > maxy) {
            maxy = m->vertexArray[i].y;
        }

        if(m->vertexArray[i].z < minz) {
            minz = m->vertexArray[i].z;
        }

        if(m->vertexArray[i].z > maxz) {
            maxz = m->vertexArray[i].z;
        }
    }

	for (i = 0; i < m->numVertices; i++) {
        m->vertexArray[i].x -= (maxx + minx) / 2.0f;
		m->vertexArray[i].y -= (maxy + miny) / 2.0f;
		m->vertexArray[i].z -= (maxz + minz) / 2.0f;
    }
}

void glUtilitiesScaleModel(Model *m, float sx, float sy, float sz) {
	for (long i = 0; i < m->numVertices; i++) {
		m->vertexArray[i].x *= sx;
		m->vertexArray[i].y *= sy;
		m->vertexArray[i].z *= sz;
	}
}

static void report_loader_error(const char *caller, const char *n) {
	static unsigned int err_count = 0;
    if(err_count < MAX_ERRORS) {
		fprintf(stderr, "%s warning: '%s' not found in shader!\n", caller, n);
        err_count++;
    }
    else if(err_count == MAX_ERRORS) {
	    fprintf(stderr, "%s: Nr of errors bigger than %i. No more will be printed.\n", caller, MAX_ERRORS);
        err_count++;
    }
}

// TODO: Write helper function for glUtilitiesDrawModel & glUtilitiesDrawWireframe to avoid so much code repetition
void glUtilitiesDrawModel(Model *m, GLuint program, const char* vertexVar, const char* normalVar, const char* textureVar) {
    if(m) {
        glBindVertexArray(m->vao);
        glUseProgram(program);
        
		glBindBuffer(GL_ARRAY_BUFFER, m->vb);
		GLint loc = glGetAttribLocation(program, vertexVar);
		if(loc >= 0) {
			glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0); 
			glEnableVertexAttribArray(loc);
        }
        else {
            report_loader_error("glUtilitiesDrawModel", vertexVar);
        }

		if(normalVar) {
			loc = glGetAttribLocation(program, normalVar);
		    if(loc >= 0) {
				glBindBuffer(GL_ARRAY_BUFFER, m->nb);
				glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(loc);
            }
            else {
                report_loader_error("glUtilitiesDrawModel", normalVar);
            }
        }

		if(m->texCoordArray && textureVar) {
			loc = glGetAttribLocation(program, textureVar);
			if(loc >= 0) {
				glBindBuffer(GL_ARRAY_BUFFER, m->tb);
				glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(loc);
            }
            else {
                report_loader_error("glUtilitiesDrawModel", textureVar);
            }
        }

		glDrawElements(GL_TRIANGLES, m->numIndices, GL_UNSIGNED_INT, 0L);
    }
}

void glUtilitiesDrawWireframe(Model *m, GLuint program, const char* vertexVar, const char* normalVar, const char* textureVar) {
	if(m) {
		glBindVertexArray(m->vao);
        glUseProgram(program);

		glBindBuffer(GL_ARRAY_BUFFER, m->vb);
		GLint loc = glGetAttribLocation(program, vertexVar);
		if(loc >= 0) {
			glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0); 
			glEnableVertexAttribArray(loc);
        }
        else {
            report_loader_error("glUtilitiesDrawWireframe", vertexVar);
        }

		if (normalVar) {
			loc = glGetAttribLocation(program, normalVar);
			if(loc >= 0) {
				glBindBuffer(GL_ARRAY_BUFFER, m->nb);
				glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(loc);
            }
            else {
                report_loader_error("glUtilitiesDrawWireframe", normalVar);
            }
        }

		if(m->texCoordArray && textureVar) {
			loc = glGetAttribLocation(program, textureVar);
			if(loc >= 0) {
				glBindBuffer(GL_ARRAY_BUFFER, m->tb);
				glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(loc);
            }
            else {
                report_loader_error("glUtilitiesDrawWireframe", textureVar);
            }
        }

		glDrawElements(GL_LINE_STRIP, m->numIndices, GL_UNSIGNED_INT, 0L);
    }
}

void glUtilitiesReloadModelData(Model *m) {
	glBindVertexArray(m->vao);
	
    glBindBuffer(GL_ARRAY_BUFFER, m->vb);
	glBufferData(GL_ARRAY_BUFFER, m->numVertices * 3 * sizeof(GLfloat), m->vertexArray, GL_STATIC_DRAW);
	
    glBindBuffer(GL_ARRAY_BUFFER, m->nb);
	glBufferData(GL_ARRAY_BUFFER, m->numVertices * 3 * sizeof(GLfloat), m->normalArray, GL_STATIC_DRAW);
    
	if (m->texCoordArray) {
		glBindBuffer(GL_ARRAY_BUFFER, m->tb);
		glBufferData(GL_ARRAY_BUFFER, m->numVertices * 2 * sizeof(GLfloat), m->texCoordArray, GL_STATIC_DRAW);
    }
    
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m->numIndices * sizeof(GLuint), m->indexArray, GL_STATIC_DRAW);
}

static void generate_model_buffers(Model *m) {
    glGenVertexArrays(1, &m->vao);
	glGenBuffers(1, &m->vb);
	glGenBuffers(1, &m->ib);
	glGenBuffers(1, &m->nb);
    
	if (m->texCoordArray) {
		glGenBuffers(1, &m->tb);
    }
    
    glUtilitiesReloadModelData(m);
}

Model* glUtilitiesLoadModel(const char* n) {
	Mesh *mesh = load_obj(n);
    to_triangles(mesh);
    generate_normals(mesh);
    
    Model *model = generate_model(mesh);
    dispose_mesh(mesh);

    generate_model_buffers(model);
    model->data = 0;

    return model;
}

Model** glUtilitiesLoadModelSet(const char* n) {
	Mesh *mesh = load_obj(n);
	Mesh **mm = split_to_meshes(mesh);

    int i;
	for (i = 0; mm[i] != NULL; i++) {} // for populating i
	
    Model **md = (Model **)calloc(sizeof(Model *), i + 1);
    for(i = 0; mm[i] != NULL; i++) {
        to_triangles(mm[i]);
        generate_normals(mm[i]);
		md[i] = generate_model(mm[i]);
        dispose_mesh(mm[i]);
    }

    free(mm);
    dispose_material_list(MATERIALS);
    dispose_mesh(mesh);
    
    MATERIAL_NAME_LIST = NULL;
	MATERIALS = NULL;

    for(i = 0; md[i] != NULL; i++) {
        generate_model_buffers(md[i]);
		md[i]->data = 0;
    }

    return md;
}

Model* glUtilitiesLoadModelData(Vector3 *vertices, Vector3 *normals, Vector2 *texCoords, Vector3 *colors, GLuint *indices, int numVertices, int numIndices) {
	Model* m = (Model *)malloc(sizeof(Model));
	memset(m, 0, sizeof(Model));
    
    m->vertexArray = vertices;
	m->texCoordArray = texCoords;
	m->normalArray = normals;
	m->indexArray = indices;
	m->numVertices = numVertices;
	m->numIndices = numIndices;
	m->data = 1;

    generate_model_buffers(m);

    return m;
}

void glUtilitiesDisposeModel(Model *m) {
    if(m) {
        if(m->data == 0) {
		    if(m->vertexArray) {
				free(m->vertexArray);
            }

            if(m->normalArray) {
				free(m->normalArray);
            }

            if(m->texCoordArray) {
				free(m->texCoordArray);
            }

            if(m->colorArray) {
				free(m->colorArray);
            }

            if(m->indexArray) {
				free(m->indexArray);
            }
        }

        glDeleteBuffers(1, &m->vb);
		glDeleteBuffers(1, &m->ib);
		glDeleteBuffers(1, &m->nb);
		glDeleteBuffers(1, &m->tb);
		glDeleteVertexArrays(1, &m->vao);

        if (m->material) {
			free(m->material);
        }
    }
    
    free(m);
}

/*

TGA UTILITIES

*/

static bool MIPMAP = true;

void glUtilitiesLoadTGASetMipmapping(bool active) {
    MIPMAP = active;
}

bool glUtilitiesLoadTGATextureData(const char *filename, TextureData *texture) {
	GLuint i;
	GLubyte uncompressedheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	GLubyte compressedheader[12] = {0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	GLubyte uncompressedbwheader[12] = {0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	GLubyte compressedbwheader[12] = {0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	GLubyte actualHeader[12];
    GLubyte header[6];

	GLuint bytesPerPixel;
	GLuint imageSize;
	GLuint temp;

    GLubyte *rowP;
	
    long rowSize, stepSize, bytesRead;
	long row, rowLimit;
	
    int err;
    int b;
	
    GLubyte rle;
	GLubyte pixelData[4];	
	
	char flipped;
	long step;
	
    FILE *file = fopen(filename, "rb");
	err = 0;

	if (file == NULL) {
        err = 1;
    }
	else if (fread(actualHeader, 1, sizeof(actualHeader), file) != sizeof(actualHeader)) {
        err = 2;
    }
	else if (
				(memcmp(uncompressedheader, actualHeader, sizeof(uncompressedheader)-4) != 0) &&
				(memcmp(compressedheader, actualHeader, sizeof(compressedheader)-4) != 0) &&
				(memcmp(uncompressedbwheader, actualHeader, sizeof(uncompressedheader)-4) != 0) &&
				(memcmp(compressedbwheader, actualHeader, sizeof(compressedheader)-4) != 0)
			) {
		err = 3; // Does The Header Match What We Want?
		for (i = 0; i < 12; i++) {
			printf("%d ", actualHeader[i]);
        }
		printf("\n");
	}
	else if (fread(header, 1, sizeof(header), file) != sizeof(header)) {
        err = 4;
    }
	
	if (err != 0) {
		switch (err) {
			case 1: printf("could not open file %s\n", filename); break;
			case 2: printf("could not read header of %s\n", filename); break;
			case 3: printf("unsupported format in %s\n", filename); break;
			case 4: printf("could not read file %s\n", filename); break;
		}
		
		if(file == NULL) {
			return false;

        } 
        else {
			fclose(file);
			return false;
		}
	}

	texture->w  = header[1] * 256 + header[0];
    texture->h = header[3] * 256 + header[2];
	if (texture->w <= 0 || texture->h <= 0 || (header[4] != 24 && header[4] != 32 && header[4] != 8)) {
		fclose(file);		// If Anything Failed, Close The File
		return false;
	}
	flipped = (header[5] & 32) != 0;

	texture->bpp = header[4];
	bytesPerPixel = texture->bpp/8;
	imageSize = texture->w * texture->h * bytesPerPixel;
	rowSize	= texture->w * bytesPerPixel;
	stepSize = texture->w * bytesPerPixel;
	texture->imageData = (GLubyte *)calloc(1, imageSize);
	if (texture->imageData == NULL) {
		fclose(file);
		return false;
	}

	if (flipped) {
		step = -stepSize;
		rowP = &texture->imageData[imageSize - stepSize];
		row = 0 + (texture->h -1) * stepSize;
	}
	else {
		step = stepSize;
		rowP = &texture->imageData[0];
		row = 0;
	}

	if (actualHeader[2] == 2 || actualHeader[2] == 3) {
		for (i = 0; i < texture->h; i++) {
			bytesRead = fread(rowP, 1, rowSize, file);
			rowP += step;
			if (bytesRead != rowSize) {
				free(texture->imageData);
				fclose(file);
				return false;
			}
		}
	}
	else {
		i = row;
		rowLimit = row + rowSize;
		do {
			bytesRead = fread(&rle, 1, 1, file);
			if (rle < 128) {
				bytesRead = fread(&texture->imageData[i], 1, (rle+1)*bytesPerPixel, file);
				i += bytesRead;
				if (bytesRead == 0) {
					i = imageSize;
                }
			}
			else {
				bytesRead = fread(&pixelData, 1, bytesPerPixel, file);
				do {
					for (b = 0; b < bytesPerPixel; b++) {
						texture->imageData[i+b] = pixelData[b];
                    }
					i += bytesPerPixel;
					rle = rle - 1;
				} while (rle > 127);
			}
			if (i >= rowLimit) {
				row = row + step;
				rowLimit = row + rowSize;
				i = row;
			}
		} while (i < imageSize);
	}

	if (bytesPerPixel >= 3) {
        for (i = 0; i < (int)(imageSize); i += bytesPerPixel) {
            temp = texture->imageData[i];
            texture->imageData[i] = texture->imageData[i + 2];
            texture->imageData[i + 2] = temp;
        }
    }	
	fclose (file);

	return true;
}

bool glUtilitiesLoadTGATexture(const char *n, TextureData *tex) {
    char ok;
    GLuint type = GL_RGBA;

    ok = glUtilitiesLoadTGATextureData(n, tex);
    if(!ok) {
        return false;
    }

    glGenTextures(1, &tex->texID);
    glBindTexture(GL_TEXTURE_2D, tex->texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    if(tex->bpp == 8) {
        type = GL_RED;
    }

    if(tex->bpp == 24) {
        type = GL_RGB;
    }
	glTexImage2D(GL_TEXTURE_2D, 0, type, tex->w, tex->h, 0, type, GL_UNSIGNED_BYTE, tex[0].imageData);
    
	if(MIPMAP) {
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }
    
    return true;
}

void glUtilitiesLoadTGATextureSimple(const char *n, GLuint *tex) {
	TextureData texture;
    memset(&texture, 0, sizeof(texture));

	if (glUtilitiesLoadTGATexture(n, &texture)) {
		if(texture.imageData) {
			free(texture.imageData);
        }
        *tex = texture.texID;
    }
    else {
        *tex = 0;
    }
}

int glUtilitiesSaveTGAData(char	*n, short int w, short int h, unsigned char pixelDepth, unsigned char *imageData) {
    FILE *file = fopen(n, "w");
    if(!file) {
        return ERR_OPEN;
    }

    unsigned char aux;
    unsigned char garbage = 0;
	unsigned char mode = pixelDepth / 8;

    char uncompressedHeader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    fwrite(&uncompressedHeader, 12, 1, file);
	
    fwrite(&w, sizeof(short int), 1, file);
	fwrite(&h, sizeof(short int), 1, file);
	fwrite(&pixelDepth, sizeof(unsigned char), 1, file);
	fwrite(&garbage, sizeof(unsigned char), 1, file);

    int i;
    if(mode >= 3) {
	    for (i = 0; i < w * h * mode; i += mode) {
		    aux = imageData[i];
		    imageData[i] = imageData[i + 2];
		    imageData[i + 2] = aux;
        }
    }

	int width = 1;
	while (width < w) {
        width = width << 1;
    }

    int ix;
	for (i = 0; i < h; i++) {
        ix = i * w * mode;
		fwrite(&imageData[ix], sizeof(unsigned char), w * mode, file);
    }
	fclose(file);

    return OK;
}

void glUtilitiesSaveTGA(TextureData *tex, char *n) {
    glUtilitiesSaveTGAData(n, tex->w, tex->h, tex->bpp, tex->imageData);
}

void glUtilitiesSaveTGAFramebuffer(char *n, GLint x, GLint y, GLint w, GLint h) {
    int err;
    void *buffer = malloc(h * w * 3);
	glReadPixels(x, y, w, h, GL_RGB, GL_UNSIGNED_BYTE, buffer);
    err = glUtilitiesSaveTGAData(n, w, h, 3 * 8, (unsigned char *)buffer);
    printf("(glUtilities) SaveTGAFramebuffer returned: %d\n", err);
}