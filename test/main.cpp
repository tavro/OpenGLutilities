#include "../glutilities.h"

/*

DEMO PROGRAM

*/

GLuint program;

mat4 projectionMatrix;

GLuint tex1;
TextureData ttex;

Model *tm;

vec3 *vArray;
Model* GenerateTerrain(TextureData *tex) {
    int vCount = tex->w * tex->h;
	int tCount = (tex->w - 1) * (tex->h - 1) * 2;

	vArray = (vec3 *)malloc(sizeof(GLfloat) * 3 * vCount);
	
    vec3 *nArray = (vec3 *)malloc(sizeof(GLfloat) * 3 * vCount);
	vec2 *tcArray = (vec2 *)malloc(sizeof(GLfloat) * 2 * vCount);
	
    GLuint *iArray = (GLuint *)malloc(sizeof(GLuint) * tCount * 3);

    // Vertex array
    unsigned int x, z;
	printf("bpp %d\n", tex->bpp);
	for (x = 0; x < tex->w; x++) {
		for (z = 0; z < tex->h; z++) {
            vArray[(x + z * tex->w)].x = x / 1.0;
			vArray[(x + z * tex->w)].y = tex->imageData[(x + z * tex->w) * (tex->bpp / 8)] / 10.0;
			vArray[(x + z * tex->w)].z = z / 1.0;
        }
    }
    
    // Normal vectors
	for (x = 0; x < tex->w - 1; x++) {
		for (z = 0; z < tex->h - 1; z++) {
			vec3 dir = vec3(0, 1, 0);

			int i = (x + z * tex->w);
			vec3 vertex = vec3(vArray[i].x, vArray[i].y, vArray[i].z);

            // 8 neighbour vertices
			for (int k = -1; k <= 1; k++) {
				for (int j = -1; j <= 1; j++) {
                    if(k == 0 && j == 0) { // the vertex itself
						continue;
					}

					int neighbouring = i + j - k * tex->w;
					if (neighbouring >= vCount || neighbouring < 0) {
						continue;
                    }

					vec3 other = vec3(vArray[neighbouring].x, vArray[neighbouring].y, vArray[neighbouring].z);
                    vec3 otherDir = normalize(setv(other.x-vertex.x, other.y-vertex.y, other.z-vertex.z));
					vec3 cod = cross(otherDir, dir);
                    dir = setv(cod.x+otherDir.x, cod.y+otherDir.y, cod.z+otherDir.z);
                }
            }

			dir = normalize(dir);
            nArray[(x + z * tex->w)].x = dir.x;
			nArray[(x + z * tex->w)].y = dir.y;
			nArray[(x + z * tex->w)].z = dir.z;

			// Texture coordinates.
            tcArray[(x + z * tex->w)].x = x;
			tcArray[(x + z * tex->w)].y = z;

            // Triangle 1
			iArray[(x + z * (tex->w - 1)) * 6 + 0] = x + z * tex->w;
			iArray[(x + z * (tex->w - 1)) * 6 + 1] = x + (z + 1) * tex->w;
			iArray[(x + z * (tex->w - 1)) * 6 + 2] = x + 1 + z * tex->w;
			
			// Triangle 2
			iArray[(x + z * (tex->w - 1)) * 6 + 3] = x + 1 + z * tex->w;
			iArray[(x + z * (tex->w - 1)) * 6 + 4] = x + (z + 1) * tex->w;
			iArray[(x + z * (tex->w - 1)) * 6 + 5] = x + 1 + (z + 1) * tex->w;
        }
    }

    Model *model = glUtilitiesLoadModelData(vArray, nArray, tcArray, NULL, iArray, vCount, tCount * 3);
    return model;
}

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
	glUniform1i(glGetUniformLocation(program, "tex"), 0);
	glUtilitiesLoadTGATextureSimple("test/res/tex.tga", &tex1);
    
    glUtilitiesLoadTGATextureData("test/res/terrain.tga", &ttex);
	tm = GenerateTerrain(&ttex);

	glUtilitiesReportError("TERRAIN INIT");
}

vec2 camRot = vec2(0, 0);
vec3 velocity = vec3(0, 0, 0);
vec3 FORWARD;

GLfloat lastFrameTime = 0.0;
GLfloat deltaTime = 0.0;

float SPEED = 25.0f;
void keysf(unsigned char k, int x, int y) {
    vec3 RIGHT = normalize(cross(FORWARD, {0, 1, 0}));
	vec3 UP = normalize(cross(RIGHT, FORWARD));

    // TODO: KeyIsDown does not seem to work
    // TODO: Turn into switch statement
    float ds = deltaTime * SPEED;
    if(k == 22) { // UP
		camRot.y += ds;
    }

    if(k == 23) { // DOWN
		camRot.y -= ds;
    }

    if(k == 25) { // RIGHT
		camRot.x += ds;
    }

    if(k == 24) { // LEFT
		camRot.x -= ds;
    }
    
    if(k == 115) { // s
        vec3 tmp = vec3(FORWARD.x * ds, FORWARD.y * ds, FORWARD.z * ds);
		velocity = vec3(velocity.x - tmp.x, velocity.y - tmp.y, velocity.z - tmp.z);
    }

    if(k == 119) { // w
		vec3 tmp = vec3(FORWARD.x * ds, FORWARD.y * ds, FORWARD.z * ds);
		velocity = vec3(velocity.x + tmp.x, velocity.y + tmp.y, velocity.z + tmp.z);
    }

    if(k == 101) { // e
		vec3 tmp = vec3(UP.x * ds, UP.y * ds, UP.z * ds);
		velocity = vec3(velocity.x + tmp.x, velocity.y + tmp.y, velocity.z + tmp.z);
    }

    if(k == 113) { // q
        vec3 tmp = vec3(UP.x * ds, UP.y * ds, UP.z * ds);
		velocity = vec3(velocity.x - tmp.x, velocity.y - tmp.y, velocity.z - tmp.z);
    }

    if(k == 97) { // a
        vec3 tmp = vec3(RIGHT.x * ds, RIGHT.y * ds, RIGHT.z * ds);
		velocity = vec3(velocity.x - tmp.x, velocity.y - tmp.y, velocity.z - tmp.z);
    }

    if(k == 100) { // d
        vec3 tmp = vec3(RIGHT.x * ds, RIGHT.y * ds, RIGHT.z * ds);
		velocity = vec3(velocity.x + tmp.x, velocity.y + tmp.y, velocity.z + tmp.z);
    }
    //printf("%d\n", k);
}

float lerp(float t, float a, float b) {
	return (1 - t) * a + t * b;
}

float curr = 0;
float getY(float x, float z, TextureData *tex) {
	vec2 last = vec2((int)floor(x), (int)floor(z)); // last visited point

	int other = (((x - last.x) + (z - last.y)) >= 1); // upper right triangle

	float h1 = vArray[(int)((last.x + other) + (last.y + other) * tex->w)].y; // bottom left or top right
	float h2 = vArray[(int)((last.x + 1) + last.y * tex->w)].y;
	float h3 = vArray[(int)(last.x + (last.y + 1) * tex->w)].y;

	float height = (lerp((x - round(x + 0.5f)) + 1, h1, h2) + lerp((z - round(z + 0.5f)) + 1, h1, h3)) / 2;

	curr = lerp(((x - last.x) + (z - last.y)) / 2, curr, height);
	return curr;
}

void displayf(void) {
    GLfloat currentTime = (GLfloat)glUtilitiesGet(ELAPSED_TIME);
    deltaTime = (currentTime - lastFrameTime) / 1000.0f;
    lastFrameTime = currentTime;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUtilitiesReportError("PRE DISPLAY");
	glUseProgram(program);

    if(camRot.y > 89) { // cap rotation
		camRot.y = 89;
	}
	else if(camRot.y < -89) {
		camRot.y = -89;
	}
	vec2 camRotRad = vec2(camRot.x * M_PI / 180, camRot.y * M_PI / 180); // convert to radians

	vec3 target;
	target.x = cos(camRotRad.x) * cos(camRotRad.y);
	target.y = sin(camRotRad.y);
	target.z = sin(camRotRad.x) * cos(camRotRad.y);
	vec3 direction = normalize(target);
	FORWARD = direction;

    // TODO: Implement rest
    mat4 worldToView = lookAtv(velocity, vec3(velocity.x + direction.x, velocity.y + direction.y, velocity.z + direction.z), {0, 1, 0});
 	glUniformMatrix4fv(glGetUniformLocation(program, "worldToView"), 1, GL_TRUE, worldToView.m);
	
    // Identity Matrix
    int idx;
    mat4 modelView;
    for(idx = 0; idx <= 15; idx++) {
		modelView.m[idx] = 0;
    }
    for(idx = 0; idx <= 3; idx++) {
		modelView.m[idx * 5] = 1;
    }

	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, modelView.m);
	glBindTexture(GL_TEXTURE_2D, tex1);
	glUtilitiesDrawModel(tm, program, "inPosition", "inNormal", "inTexCoord");
	
	glUtilitiesReportError("DISPLAY");

    glUtilitiesSwapBuffers();
}

void mousef(int b, int s, int x, int y) {

}

int main(int argc, char *argv[]) {
    glUtilitiesInit(&argc, argv);
    glUtilitiesContextVersion(3, 2);
    glUtilitiesDisplayMode(DOUBLE | DEPTH);
    
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