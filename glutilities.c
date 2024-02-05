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

void glutilities_init(int *argc, char *argv[]) {
    gettimeofofday(&TIME, NULL);
    memset(KEYMAP, 0, sizeof(KEYMAP));
}

void glutilities_display_mode(unsigned int m) {
    DISPLAY_MODE = m;
}

void glutilities_window_size(int w, int h) {
    WINDOW_WIDTH = w;
    WINDOW_HEIGHT = h;
}

void glutilities_window_pos(int x, int y) {
    WINDOW_POS_X = x;
    WINDOW_POS_Y = y;
}

static void create_window(Display *d, const char *n, int x, int y, int w, int h, Window *win, GLXContext *ctx) {
    // TODO: Implement
}

void glutilities_create_window(const char *t) {
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

void glutilities_reshape_func(void (*func)(int w, int h)) {
    reshape = func;
}

void glutilities_display_func(void (*func)(void)) {
    display = func;
}

void glutilities_idle_func(void (*func)(void)) {
    idle = func;
}

void glutilities_key_up_event_func(void (*func)(unsigned char k, int x, int y)) {
    keyup = func;
}

void glutilities_key_event_func(void (*func)(unsigned char k, int x, int y)) {
    key = func;
}

void glutilities_mod_up_event_func(void (*func)(unsigned char k, int x, int y)) {
    modkeyup = func;
}

void glutilities_mod_event_func(void (*func)(unsigned char k, int x, int y)) {
    modkey = func;
}

void glutilities_mouse_func(void (*func)(int b, int b, int x, int y)) {
    mouse = func;
}

void glutilities_passive_mouse_move_func(void (*func)(int x, int y)) {
    mousemoved = func;
}

void glutilities_mouse_move_func(void (*func)(int x, int y)) {
    mousedragged = func;
}

char BUTTON_PRESSED[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void handle_key_event() {
    // TODO: Implement
}

void timer(int x) {
    glutilities_redisplay();
}

void glutilities_main() {
    char pressed = 0;
    int i;

    XAllowEvents(DISPLAY, AsyncBoth, CurrentTime);

    glutilities_timer_func(100, timer, 0);

    while(RUNNING) {
        // TODO: Implement
    }
}

void glutilities_redisplay() {
    ANIMATE = 1;
}

int glutilities_get(int t) {
    struct timeval tv;

    switch(t) {
        // TODO: Implement
    }

    return 0;
}

// TODO: Implement timers

void glutilities_context_version(int major, int minor) {
    CONTEXT_VERSION_MAJOR = major;
    CONTEXT_VERSION_MINOR = minor;
}

void glutilities_warp_pointer(int x, int y) {
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

void glutilities_show_cursor() {
    XUndefineCursor(DISPLAY, WINDOW);
}

void glutilities_hide_cursor() {
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

char glutilities_key_is_down(unsigned char c) {
    return BUTTON_PRESSED[(unsigned int)c];
}

void glutilities_reshape_window(int w, int h) {
    XResizeWindow(DISPLAY, WINDOW, w, h);
}

void glutilities_set_window_pos(int x, int y) {
    XMoveWindow(DISPLAY, WINDOW, x, y);
}

void glutilities_set_window_title(char *t) {
    XStoreName(DISPLAY, WINDOW, t);
}

char FULLSCREEN = 0;

unsigned int SAVED_HEIGHT;
unsigned int SAVED_WIDTH;

int SAVED_X;
int SAVED_Y;

void glutilities_exit_fullscreen() {
    FULLSCREEN = 0;
    XMoveResizeWindow(DISPLAY, WINDOW, SAVED_X, SAVED_Y, SAVED_WIDTH, SAVED_HEIGHT);
}

void glutilities_fullscreen() {
    FULLSCREEN = 1;

    Drawable drawable;
    unsigned int a, b;

    XGetGeometry(DISPLAY, WINDOW, &drawable, &SAVED_X, &SAVED_Y, &SAVED_WIDTH, &SAVED_HEIGHT, &a, &b);

    int screen = DefaultScreen(DISPLAY);

    int w = DisplayWidth(DISPLAY, screen);
    int h = DisplayHeight(DISPLAY, screen);

    XMoveResizeWindow(DISPLAY, WINDOW, 0, 0, w, h);
}

void glutilities_toggle_fullscreen() {
    if(FULLSCREEN) {
        glutilities_exit_fullscreen();
    }
    else  {
        glutilities_fullscreen();
    }
}

void glutilities_exit() {
    RUNNING = 0;
}