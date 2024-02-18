#include "../glutilities.h"

/*

DEMO PROGRAM

*/

GLuint program;

mat4 projectionMatrix;

void initf(void) {
	glClearColor(0.2, 0.2, 0.5, 0);
	
    glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glUtilitiesReportError("GL INIT");

	projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 500.0);
	
    program = glUtilitiesLoadShaders("test/main.vert", "test/main.frag");
	glUseProgram(program);

	glUtilitiesReportError("SHADER INIT");

	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
}

void displayf(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUtilitiesReportError("PRE DISPLAY");
	glUseProgram(program);

    // TODO: Implement camera

	glUtilitiesReportError("DISPLAY");

    // glUtilitiesSwapBuffers(); causes error
}

void keysf(unsigned char k, int x, int y) {

}

void mousef(int b, int s, int x, int y) {

}

int main(int argc, char *argv[]) {
    glUtilitiesInit(&argc, argv);
    glUtilitiesContextVersion(3, 2);
    glUtilitiesDisplayMode(RGB | DOUBLE | DEPTH);
    
    glUtilitiesWindowSize(600, 600);
    glUtilitiesCreateWindow("Test");
    
    glUtilitiesDisplayFunc(displayf);
    glUtilitiesKeyEventFunc(keysf);
    glUtilitiesMouseFunc(mousef);

    initf();

    glUtilitiesRepeatingTimerFunc(20);
    glUtilitiesMain();

	exit(0);
}