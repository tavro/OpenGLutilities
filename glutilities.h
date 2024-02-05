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

#include "constants.h"

void glutilities_context_version(int major, int minor);

void glutilities_reshape_window(int w, int h);
void glutilities_create_window(const char *t);

void glutilities_set_window_pos(int x, int y);
void glutilities_set_window_title(char *t);

void glutilities_window_size(int w, int h);
void glutilities_window_pos(int x, int y);

void glutilities_init(int *argcp, char **argv);
void glutilities_main();

void glutilities_check();

void glutilities_reshape_func(void (*func)(int w, int h));
void glutilities_display_mode(unsigned int m);
void glutilities_display_func(void (*func)(void));
void glutilities_swap_buffers();
void glutilities_redisplay();

void glutilities_key_up_event_func(void (*func)(unsigned char k, int x, int y));
void glutilities_key_event_func(void (*func)(unsigned char k, int x, int y));
char glutilities_key_is_down(unsigned char c);

void glutilities_mod_up_event_func(void (*func)(unsigned char k, int x, int y));
void glutilities_mod_event_func(void (*func)(unsigned char k, int x, int y));

void glutilities_passive_mouse_move_func(void (*func)(int x, int y));
void glutilities_mouse_move_func(void (*func)(int x, int y));
void glutilities_mouse_func(void (*func)(int b, int s, int x, int y));
char glutilities_mouse_is_down(unsigned char c);

void glutilities_show_cursor();
void glutilities_hide_cursor();

int  glutilities_get(int t);

void glutilities_idle_func(void (*func)(void));

void glutilities_timer_func(int ms, void (*func)(int arg), int arg);
void glutilities_repeating_timer_func(int ms);

void glutilities_warp_pointer(int x, int y);

void glutilities_toggle_fullscreen();
void glutilities_exit_fullscreen();
void glutilities_fullscreen();
void glutilities_exit();

#endif