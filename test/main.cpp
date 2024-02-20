#define MAIN

#include "../glutilities.h"
#include "../vecutils.h"

#include <iostream>
#include <pqxx/pqxx>
#include <cstdlib>
#include <ctime>

void uploadMessage(const std::string& username, const std::string& message, float x, float y, float z) {
    try {
        pqxx::connection C("dbname=emmesen user=postgres password= hostaddr=127.0.0.1 port=5432");
        if (C.is_open()) {
            std::cout << "Opened database successfully: " << C.dbname() << std::endl;
        } else {
            std::cout << "Can't open database" << std::endl;
            return;
        }

        pqxx::work W(C);
        
        std::string sql = "INSERT INTO messages (username, message, x_coord, y_coord, z_coord) VALUES (" +
                          W.quote(username) + ", " +
                          W.quote(message) + ", " +
                          W.quote(x) + ", " +
                          W.quote(y) + ", " +
                          W.quote(z) + ");";

        W.exec(sql);
        W.commit();
        std::cout << "Records created successfully" << std::endl;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

Vector3 *msgPosArr;
void fetchAllMessages() {
    // TODO: fetch amount of messages and store in variable (in this case the 6)
	msgPosArr = (Vector3 *)malloc(sizeof(GLfloat) * 3 * 6);
    try {
        pqxx::connection C("dbname=emmesen user=postgres password= hostaddr=127.0.0.1 port=5432");
        if (C.is_open()) {
            std::cout << "Opened database successfully: " << C.dbname() << std::endl;
        } else {
            std::cout << "Can't open database" << std::endl;
            return;
        }

        pqxx::nontransaction N(C);
        
        std::string sql = "SELECT id, username, message, x_coord, y_coord, z_coord, timestamp FROM messages;";

        pqxx::result R( N.exec(sql) );

        std::cout << "Fetching messages..." << std::endl;
        
        int i = 0;
        for (auto c : R) {
            std::cout << "ID = " << c[0].as<int>() << std::endl;
            std::cout << "Username = " << c[1].as<std::string>() << std::endl;
            std::cout << "Message = " << c[2].as<std::string>() << std::endl;
            std::cout << "X Coordinate = " << c[3].as<float>() << std::endl;
            std::cout << "Y Coordinate = " << c[4].as<float>() << std::endl;
            std::cout << "Z Coordinate = " << c[5].as<float>() << std::endl;

            msgPosArr[i].x = c[3].as<float>();
            msgPosArr[i].y = c[4].as<float>();
            msgPosArr[i].z = c[5].as<float>();

            std::cout << "Timestamp = " << c[6].as<std::string>() << std::endl << std::endl;
            i++;
        }

        std::cout << "Records fetched successfully" << std::endl;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

/*

DEMO PROGRAM

*/

GLuint program;

Matrix4 projectionMatrix;

GLuint tex1, tex2;

Model *tm, *msg;
TextureData ttex;

Vector3 *vArray;
Model* GenerateTerrain(TextureData *tex) {
    int vCount = tex->w * tex->h;
	int tCount = (tex->w - 1) * (tex->h - 1) * 2;

	vArray = (Vector3 *)malloc(sizeof(GLfloat) * 3 * vCount);
	
    Vector3 *nArray = (Vector3 *)malloc(sizeof(GLfloat) * 3 * vCount);
	Vector2 *tcArray = (Vector2 *)malloc(sizeof(GLfloat) * 2 * vCount);
	
    GLuint *iArray = (GLuint *)malloc(sizeof(GLuint) * tCount * 3);

    // Vertex array
    unsigned int x, z;
	printf("bpp %d\n", tex->bpp);
	for (x = 0; x < tex->w; x++) {
		for (z = 0; z < tex->h; z++) {
            vArray[(x + z * tex->w)].x = x / 1.0;
			vArray[(x + z * tex->w)].y = tex->imageData[(x + z * tex->w) * (tex->bpp / 8)] / 10.0; // THIS IS THE PROBLEM
			vArray[(x + z * tex->w)].z = z / 1.0;
        }
    }
    
    // Normal vectors
	for (x = 0; x < tex->w - 1; x++) {
		for (z = 0; z < tex->h - 1; z++) {
			Vector3 dir = Vector3(0, 1, 0);

			int i = (x + z * tex->w);
			Vector3 vertex = Vector3(vArray[i].x, vArray[i].y, vArray[i].z);

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

					Vector3 other = {vArray[neighbouring].x, vArray[neighbouring].y, vArray[neighbouring].z};
                    
                    Vector3 otherDir = Normalize(SubV3(other, vertex));
                    dir = AddV3(Cross(otherDir, dir), otherDir);
                }
            }

			dir = Normalize(dir);
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

	projectionMatrix = Frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 500.0);
	
    program = glUtilitiesLoadShaders("test/main.vert", "test/main.frag");
	glUseProgram(program);

	glUtilitiesReportError("SHADER INIT");

	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniform1i(glGetUniformLocation(program, "tex"), 0);
	glUtilitiesLoadTGATextureSimple("test/res/tex.tga", &tex1);
	glUtilitiesLoadTGATextureSimple("test/res/tex3.tga", &tex2);
    
    glUtilitiesLoadTGATextureData("test/res/terrain.tga", &ttex);
	tm = GenerateTerrain(&ttex);

	glUtilitiesReportError("TERRAIN INIT");

	msg = glUtilitiesLoadModel("test/res/star.obj");
}

Vector2 camRot = Vector2(0, 0);
Vector3 velocity = Vector3(0, 0, 0);
Vector3 FORWARD;

GLfloat lastFrameTime = 0.0;
GLfloat deltaTime = 0.0;

/*
float randomFloat(float min, float max) {
    return (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (max - min) + min;
}
*/

float SPEED = 35.0f;
void keysf(unsigned char k, int x, int y) {
    Vector3 RIGHT = Normalize(Cross(FORWARD, {0, 1, 0}));
	Vector3 UP = Normalize(Cross(RIGHT, FORWARD));

    // TODO: KeyIsDown does not seem to work
    // TODO: Turn into switch statement
    float ds = deltaTime * SPEED * 4;
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
    
    ds = deltaTime * SPEED;
    if(k == 115) { // s
        Vector3 tmp = Vector3(FORWARD.x * ds, FORWARD.y * ds, FORWARD.z * ds);
		velocity = Vector3(velocity.x - tmp.x, velocity.y - tmp.y, velocity.z - tmp.z);
    }

    if(k == 119) { // w
		Vector3 tmp = Vector3(FORWARD.x * ds, FORWARD.y * ds, FORWARD.z * ds);
		velocity = Vector3(velocity.x + tmp.x, velocity.y + tmp.y, velocity.z + tmp.z);
    }

    if(k == 101) { // e
		Vector3 tmp = Vector3(UP.x * ds, UP.y * ds, UP.z * ds);
		velocity = Vector3(velocity.x + tmp.x, velocity.y + tmp.y, velocity.z + tmp.z);
    }

    if(k == 113) { // q
        Vector3 tmp = Vector3(UP.x * ds, UP.y * ds, UP.z * ds);
		velocity = Vector3(velocity.x - tmp.x, velocity.y - tmp.y, velocity.z - tmp.z);
    }

    if(k == 97) { // a
        Vector3 tmp = Vector3(RIGHT.x * ds, RIGHT.y * ds, RIGHT.z * ds);
		velocity = Vector3(velocity.x - tmp.x, velocity.y - tmp.y, velocity.z - tmp.z);
    }

    if(k == 100) { // d
        Vector3 tmp = Vector3(RIGHT.x * ds, RIGHT.y * ds, RIGHT.z * ds);
		velocity = Vector3(velocity.x + tmp.x, velocity.y + tmp.y, velocity.z + tmp.z);
    }

    if(k == 32) { // space
        uploadMessage("user", "empty", velocity.x, velocity.y, velocity.z);
    }
    //printf("%d\n", k);
}

float lerp(float t, float a, float b) {
	return (1 - t) * a + t * b;
}

float curr = 0;
float getY(float x, float z, TextureData *tex) {
	Vector2 last = Vector2((int)floor(x), (int)floor(z)); // last visited point

	int other = (((x - last.x) + (z - last.y)) >= 1); // upper right triangle

	float h1 = vArray[(int)((last.x + other) + (last.y + other) * tex->w)].y; // bottom left or top right
	float h2 = vArray[(int)((last.x + 1) + last.y * tex->w)].y;
	float h3 = vArray[(int)(last.x + (last.y + 1) * tex->w)].y;

	float height = (lerp((x - round(x + 0.5f)) + 1, h1, h2) + lerp((z - round(z + 0.5f)) + 1, h1, h3)) / 2;

	curr = lerp(((x - last.x) + (z - last.y)) / 2, curr, height);
	return curr;
}

float PLAYER_HEIGHT = 5.0f;
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
	Vector2 camRotRad = Vector2(camRot.x * M_PI / 180, camRot.y * M_PI / 180); // convert to radians

	Vector3 target;
	target.x = cos(camRotRad.x) * cos(camRotRad.y);
	target.y = sin(camRotRad.y);
	target.z = sin(camRotRad.x) * cos(camRotRad.y);
	Vector3 direction = Normalize(target);
	FORWARD = direction;

    // TODO: Implement rest
    velocity.y = getY(velocity.x, velocity.z, &ttex) + PLAYER_HEIGHT;
    Matrix4 worldToView = LookAtVector(velocity, AddV3(velocity, direction), {0, 1, 0});
 	glUniformMatrix4fv(glGetUniformLocation(program, "worldToView"), 1, GL_TRUE, worldToView.m);
	
    // Identity Matrix
    Matrix4 modelView = IdentityMatrix();
	
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, modelView.m);
	glBindTexture(GL_TEXTURE_2D, tex1);
	glUtilitiesDrawModel(tm, program, "inPosition", "inNormal", "inTexCoord");
	
	glBindTexture(GL_TEXTURE_2D, tex2);
    for (int i = 0; i < 6; i++) {
        Matrix4 modelView2 = Transform(msgPosArr[i].x, msgPosArr[i].y, msgPosArr[i].z);
        modelView2 = MultM4(modelView2, RotateX(M_PI/2));
        glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, modelView2.m);
	    glUtilitiesDrawModel(msg, program, "inPosition", "inNormal", "inTexCoord");
    }

	glUtilitiesReportError("DISPLAY");

    glUtilitiesSwapBuffers();
}

void mousef(int b, int s, int x, int y) {

}

int main(int argc, char *argv[]) {
    glUtilitiesInit(&argc, argv);
    glUtilitiesDisplayMode(DOUBLE | DEPTH);
    glUtilitiesContextVersion(3, 2);
    
    glUtilitiesWindowSize(600, 600);
    glUtilitiesCreateWindow("Test");
    
    glUtilitiesDisplayFunc(displayf);
    glUtilitiesKeyEventFunc(keysf);
    glUtilitiesMouseFunc(mousef);

    initf();

    fetchAllMessages();

    glUtilitiesRepeatingTimerFunc(20);
    glUtilitiesMain();

	exit(0);
}