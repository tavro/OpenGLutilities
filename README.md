# GLUtilities: An OpenGL Utility Library

## OVERVIEW
GLUtilities is a comprehensive utility library designed for OpenGL applications. 
It leverages the functionalities provided by the GLUT/FreeGLUT library, 
offering an extended set of tools for developers working with OpenGL.

## INSPIRATION
This project draws inspiration from the work of Ingemar Ragnemalm,
author of MicroGLUT and examiner for university computer graphics course TSKB07.

## COMPILING INSTRUCTIONS

gcc -Wall -o main test/main.cpp glutilities.c -DGL_GLEXT_PROTOTYPES -lXt -lX11 -lGL -lm