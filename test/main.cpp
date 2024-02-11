#include "../glutilities.h"

void initf(void) {

}

void displayf(void) {

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

    glUtilitiesMain();

    return 0;
}