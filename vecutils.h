/*

VecUtils: A Vector Utility Library
--------------------------------------

Author: Isak Horvath
Year:   2024

INSPIRATION:
This project draws inspiration from the work of Ingemar Ragnemalm,
author of MicroGLUT and examiner for university computer graphics course TSKB07. 

*/

#ifndef VECUTILS
#define VECUTILS

#include <math.h>
#include <stdio.h>

#ifndef M_PI
#define M_PI    3.14159265358979323846
#endif

typedef struct Vector4 Vector4;

typedef struct Vector3 {
    union {float x; float r;};
    union {float y; float g;};
    union {float z; float b;};

	#ifdef __cplusplus
    Vector3() {}
    Vector3(float x2, float y2, float z2) : x(x2), y(y2), z(z2) {}
    Vector3(float x2) : x(x2), y(x2), z(x2) {}
	Vector3(Vector4 v);
	#endif
} Vector3, *Vector3Ptr;

typedef struct Vector4 {
    union {float x; float r;};
    union {float y; float g;};
    union {float z; float b;};
    union {float h; float w; float a;};

	#ifdef __cplusplus
    Vector4() {}
    Vector4(float x2, float y2, float z2, float w2) : x(x2), y(y2), z(z2), w(w2) {}
	Vector4(float xyz, float w2) : x(xyz), y(xyz), z(xyz), w(w2) {}
	Vector4(Vector3 v, float w2) : x(v.x), y(v.y), z(v.z), w(w2) {}
	Vector4(Vector3 v) : x(v.x), y(v.y), z(v.z), w(1) {}
	#endif
} Vector4, *Vector4Ptr;

typedef struct Vector2 {
	union {float x; float s; float u;};
	union {float y; float t; float v;};
		
	#ifdef __cplusplus
    Vector2() {}
	Vector2(float x2, float y2) : x(x2), y(y2) {}
	#endif
} Vector2, *Vector2Ptr;

typedef struct Matrix3 Matrix3;

typedef struct Matrix4 {
	float m[16];

	#ifdef __cplusplus
    Matrix4() {}
    Matrix4(float x2) {
	    m[0]  = x2; m[1]  =  0; m[2]  =  0; m[3]  =  0;
		m[4]  =  0; m[5]  = x2; m[6]  =  0; m[7]  =  0;
		m[8]  =  0; m[9]  =  0; m[10] = x2; m[11] =  0;
		m[12] =  0; m[13] =  0; m[14] =  0; m[15] = x2;
	}
    Matrix4(float p0,  float p1,  float p2,  float p3,
			float p4,  float p5,  float p6,  float p7,
			float p8,  float p9,  float p10, float p11, 
			float p12, float p13, float p14, float p15) {
		m[0]  =  p0; m[1]  =  p1; m[2]  =  p2; m[3]  =  p3;
		m[4]  =  p4; m[5]  =  p5; m[6]  =  p6; m[7]  =  p7;
		m[8]  =  p8; m[9]  =  p9; m[10] = p10; m[11] = p11;
		m[12] = p12; m[13] = p13; m[14] = p14; m[15] = p15;
	}
	Matrix4(Matrix3 x);
	#endif
} Matrix4;

typedef struct Matrix3 {
	float m[9];
    
	#ifdef __cplusplus
    Matrix3() {}
	Matrix3(float x2) {
	    m[0] = x2; m[1] =  0; m[2] =  0;
		m[3] =  0; m[4] = x2; m[5] =  0;
		m[6] =  0; m[7] =  0; m[8] = x2;
	}
	Matrix3(float p0, float p1, float p2,
			float p3, float p4, float p5,
			float p6, float p7, float p8) {
		m[0] = p0; m[1] = p1; m[2] = p2;
		m[3] = p3; m[4] = p4; m[5] = p5;
		m[6] = p6; m[7] = p7; m[8] = p8;
	}
	Matrix3(Matrix4 x) {
		m[0] = x.m[0]; m[1] = x.m[1]; m[2] =  x.m[2];
		m[3] = x.m[4]; m[4] = x.m[5]; m[5] =  x.m[6];
		m[6] = x.m[8]; m[7] = x.m[9]; m[8] = x.m[10];
	}
	Matrix3(Vector3 x1, Vector3 x2, Vector3 x3) {
		m[0] = x1.x; m[1] = x1.y; m[2] = x1.z;
		m[3] = x2.x; m[4] = x2.y; m[5] = x2.z;
		m[6] = x3.x; m[7] = x3.y; m[8] = x3.z;
	}
	#endif
} Matrix3;

Vector2 SetV2(float x, float y);
Vector3 SetV3(float x, float y, float z);
Vector4 SetV4(float x, float y, float z, float w);

Matrix3 SetM3(float p0,  float p1,  float p2,  float p3, float p4, float p5, float p6, float p7, float p8);
Matrix4 SetM4(float p0,  float p1,  float p2,  float p3,
			  float p4,  float p5,  float p6,  float p7,
			  float p8,  float p9,  float p10, float p11, 
			  float p12, float p13, float p14, float p15);

Vector3 SubV3(Vector3 a, Vector3 b);
Vector3 AddV3(Vector3 a, Vector3 b);

float Dot(Vector3 a, Vector3 b);
float Norm(Vector3 a);

Vector3 Cross(Vector3 a, Vector3 b);
Vector3 Scalar(Vector3 a, float s);
Vector3 Normalize(Vector3 a);

Vector3 GetNormal(Vector3 a, Vector3 b, Vector3 c);

void Split(Vector3 v, Vector3 n, Vector3 *vn, Vector3 *vp);

Matrix4 GetIdentityMatrix();

Matrix4 RotateX(float a);
Matrix4 RotateY(float a);
Matrix4 RotateZ(float a);

Matrix4 Transform(float tx, float ty, float tz);
Matrix4 Scale(float sx, float sy, float sz);

Matrix4 MultM4(Matrix4 a, Matrix4 b);
Matrix3 MultM3(Matrix3 a, Matrix3 b);

Vector3 MultM3V3(Matrix3 a, Vector3 b);
Vector3 MultM4V3(Matrix4 a, Vector3 b);
Vector4 MultM4V4(Matrix4 a, Vector4 b);

void OrthoNormalize(Matrix4 *R);

Matrix4 TransposeM4(Matrix4 m);
Matrix3 TransposeM3(Matrix3 m);

Matrix4 ArbritraryRotate(Vector3 axis, float fi);

Matrix4 CrossMatrix(Vector3 a);

Matrix4 AddM4(Matrix4 a, Matrix4 b);

void SetTransposed(char t);

Matrix4 LookAtVector(Vector3 p, Vector3 l, Vector3 v);
Matrix4 LookAt( float px, float py, float pz, 
			    float lx, float ly, float lz,
			    float vx, float vy, float vz);

Matrix4 Perspective(float fovyInDeg, float aspectRatio, float znear, float zfar);
Matrix4 Frustum(float l, float r, float b, float t, float zn, float zf);
Matrix4 Ortho(float left, float right, float bottom, float top, float near, float far);

Matrix3 InvertM3(Matrix3 in);
Matrix4 InvertM4(Matrix4 in);
Matrix3 InverseTranspose(Matrix4 in);

Matrix3 M4toM3(Matrix4 m);
Matrix4 M3toM4(Matrix3 m);

Vector3 V4toV3(Vector4 v);
Vector4 V3toV4(Vector3 v);

void PrintM4(Matrix4 in);
void PrintM3(Matrix3 in);
void PrintV3(Vector3 in);

/*
#ifdef __cplusplus

inline Vector3 operator+(const Vector3 &a, const Vector3 &b) {
	return SetV3(a.x + b.x, a.y + b.y, a.z + b.z);
}

inline Vector3 operator-(const Vector3 &a, const Vector3 &b) {
	return SetV3(a.x - b.x, a.y - b.y, a.z - b.z);
}

inline Vector3 operator-(const Vector3 &a) {
	return SetV3(-a.x, -a.y, -a.z);
}

inline float operator*(const Vector3 &a, const Vector3 &b) {
	return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
}

inline Vector3 operator*(const Vector3 &b, double a) {
	return SetV3(a * b.x, a * b.y, a * b.z);
}

inline Vector3 operator*(double a, const Vector3 &b) {
	return SetV3(a * b.x, a * b.y, a * b.z);
}

inline Vector3 operator/(const Vector3 &b, double a) {
	return SetV3(b.x / a, b.y / a, b.z / a);
}

inline void operator+=(Vector3 &a, const Vector3 &b) {
	a = a + b;
}

inline void operator-=(Vector3 &a, const Vector3 &b) {
	a = a - b;
}

inline void operator*=(Vector3 &a, const float &b) {
	a = a * b;
}

inline void operator/=(Vector3 &a, const float &b) {
	a = a / b;
}

inline Matrix4 operator*(const Matrix4 &a, const Matrix4 &b) {
	return MultM4(a, b);
}

inline Matrix3 operator*(const Matrix3 &a, const Matrix3 &b) {
	return MultM3(a, b);
}

inline Vector3 operator*(const Matrix4 &a, const Vector3 &b) {
	return MultM4V3(a, b);
}

inline Vector4 operator*(const Matrix4 &a, const Vector4 &b) {
	return MultM4V4(a, b);
}

inline Vector3 operator*(const Matrix3 &a, const Vector3 &b) {
	return MultM3V3(a, b);
}

#endif
*/
#endif

/*

=== IMPLEMENTATION ===

*/

#ifdef MAIN

#ifndef VECUTILS_MAIN
#define VECUTILS_MAIN

char TRANSPOSED = 0;

Vector3 SetV3(float x, float y, float z) {
	Vector3 v;
	v.x = x;
	v.y = y;
	v.z = z;
	return v;
}

Vector2 SetV2(float x, float y) {
	Vector2 v;
	v.x = x;
	v.y = y;
	return v;
}

Vector4 SetV4(float x, float y, float z, float w) {
	Vector4 v;
	v.x = x;
	v.y = y;
	v.z = z;
	v.w = w;
	return v;
}

Matrix3 SetM3(float p0, float p1, float p2, float p3, float p4, float p5, float p6, float p7, float p8) {
	Matrix3 m;
	m.m[0] = p0;
	m.m[1] = p1;
	m.m[2] = p2;
	m.m[3] = p3;
	m.m[4] = p4;
	m.m[5] = p5;
	m.m[6] = p6;
	m.m[7] = p7;
	m.m[8] = p8;
	return m;
}

Matrix4 SetM4(float p0, float p1, float p2, float p3,
			  float p4, float p5, float p6, float p7,
			  float p8, float p9, float p10, float p11, 
			  float p12, float p13, float p14, float p15) {
	Matrix4 m;
	m.m[0] = p0;
	m.m[1] = p1;
	m.m[2] = p2;
	m.m[3] = p3;
	m.m[4] = p4;
	m.m[5] = p5;
	m.m[6] = p6;
	m.m[7] = p7;
	m.m[8] = p8;
	m.m[9] = p9;
	m.m[10] = p10;
	m.m[11] = p11;
	m.m[12] = p12;
	m.m[13] = p13;
	m.m[14] = p14;
	m.m[15] = p15;
	return m;
}

Vector3 SubV3(Vector3 a, Vector3 b) {
	Vector3 v;
	v.x = a.x - b.x;
	v.y = a.y - b.y;
	v.z = a.z - b.z;
	return v;
}
	
Vector3 AddV3(Vector3 a, Vector3 b) {
	Vector3 v;
	v.x = a.x + b.x;
	v.y = a.y + b.y;
	v.z = a.z + b.z;
	return v;
}

Vector3 Cross(Vector3 a, Vector3 b) {
	Vector3 v;
	v.x = a.y*b.z - a.z*b.y;
	v.y = a.z*b.x - a.x*b.z;
	v.z = a.x*b.y - a.y*b.x;
	return v;
}

float Dot(Vector3 a, Vector3 b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector3 Scalar(Vector3 a, float s) {
	Vector3 v;
	v.x = a.x * s;
	v.y = a.y * s;
	v.z = a.z * s;
	return v;
}

float Norm(Vector3 a) {
	return (float)sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

Vector3 Normalize(Vector3 a) {
	float norm = Norm(a);
	Vector3 v;
	v.x = a.x / norm;
	v.y = a.y / norm;
	v.z = a.z / norm;
	return v;
}

Vector3 GetNormal(Vector3 a, Vector3 b, Vector3 c) {
	Vector3 n;
	n = Cross(SubV3(a, b), SubV3(a, c));
	n = Scalar(n, 1 / Norm(n));
	return n;
}

void Split(Vector3 v, Vector3 n, Vector3 *vn, Vector3 *vp) {
	float nlen = Dot(v, n);
	float nlen2 = (n.x * n.x) + (n.y * n.y) + (n.z * n.z);
	
    if (nlen2 == 0) {
	    *vp = v;
	    *vn = SetV3(0, 0, 0);
	}
	else {
	    *vn = Scalar(n, nlen / nlen2);
	    *vp = SubV3(v, *vn);
	}
}

Matrix4 IdentityMatrix() {
	Matrix4 m;
	int i;

	for (i = 0; i <= 15; i++) {
		m.m[i] = 0;
    }

	for (i = 0; i <= 3; i++) {
		m.m[i * 5] = 1;
    }

	return m;
}

Matrix4 RotateX(float a) {
	Matrix4 m = IdentityMatrix();
    
    m.m[5] = (float)cos(a);
    if(TRANSPOSED) {
		m.m[9] = (float)-sin(a);
    }
	else {
		m.m[9] = (float)sin(a);
    }

	m.m[6] = -m.m[9];
	m.m[10] = m.m[5];
	
    return m;
}

Matrix4 RotateY(float a) {
	Matrix4 m = IdentityMatrix();
	
    m.m[0] = (float)cos(a);
	if(TRANSPOSED) {
		m.m[8] = (float)sin(a);
    }
	else {
		m.m[8] = (float)-sin(a);
    }
	
    m.m[2] = -m.m[8];
	m.m[10] = m.m[0];
	
    return m;
}

Matrix4 RotateZ(float a) {
	Matrix4 m = IdentityMatrix();
	m.m[0] = (float)cos(a);
	
    if (TRANSPOSED) {
		m.m[4] = (float)-sin(a);
    }
	else {
		m.m[4] = (float)sin(a);
    }
	
    m.m[1] = -m.m[4];
	m.m[5] =  m.m[0];
	
    return m;
}

Matrix4 Transform(float tx, float ty, float tz) {
	Matrix4 m = IdentityMatrix();
	if (TRANSPOSED) {
		m.m[12] = tx;
		m.m[13] = ty;
		m.m[14] = tz;
	}
	else {
		m.m[3] = tx;
		m.m[7] = ty;
		m.m[11] = tz;
	}
	
    return m;
}

Matrix4 Scale(float sx, float sy, float sz) {
	Matrix4 m = IdentityMatrix();
	m.m[0] = sx;
	m.m[5] = sy;
	m.m[10] = sz;
	return m;
}

Matrix4 MultM4(Matrix4 a, Matrix4 b) {
	Matrix4 m;

	int x, y;
	for(x = 0; x <= 3; x++) {
		for(y = 0; y <= 3; y++) {
			if(TRANSPOSED) {
				m.m[x * 4 + y] = a.m[y + 4 * 0] * b.m[0 + 4 * x] +
				                 a.m[y + 4 * 1] * b.m[1 + 4 * x] +
							     a.m[y + 4 * 2] * b.m[2 + 4 * x] +
							     a.m[y + 4 * 3] * b.m[3 + 4 * x];
            }
			else {
				m.m[y * 4 + x] = a.m[y * 4 + 0] * b.m[0 * 4 + x] +
							     a.m[y * 4 + 1] * b.m[1 * 4 + x] +
							     a.m[y * 4 + 2] * b.m[2 * 4 + x] +
							     a.m[y * 4 + 3] * b.m[3 * 4 + x];
            }
        }
    }

	return m;
}

Matrix3 MultM3(Matrix3 a, Matrix3 b) {
	Matrix3 m;

	int x, y;
	for (x = 0; x <= 2; x++) {
		for (y = 0; y <= 2; y++) {
			if (TRANSPOSED) {
				m.m[x * 3 + y] = a.m[y + 3 * 0] * b.m[0 + 3 * x] +
							     a.m[y + 3 * 1] * b.m[1 + 3 * x] +
							     a.m[y + 3 * 2] * b.m[2 + 3 * x];
            }
			else {
				m.m[y * 3 + x] = a.m[y * 3 + 0] * b.m[0 * 3 + x] +
							     a.m[y * 3 + 1] * b.m[1 * 3 + x] +
							     a.m[y * 3 + 2] * b.m[2 * 3 + x];
            }
        }
    }

	return m;
}

Vector3 MultM4V3(Matrix4 a, Vector3 b) {
	Vector3 r;

	if(!TRANSPOSED) {
		r.x = a.m[0] * b.x + a.m[1] * b.y + a.m[2] * b.z + a.m[3];
		r.y = a.m[4] * b.x + a.m[5] * b.y + a.m[6] * b.z + a.m[7];
		r.z = a.m[8] * b.x + a.m[9] * b.y + a.m[10]* b.z + a.m[11];
	}
	else {
		r.x = a.m[0] * b.x + a.m[4] * b.y + a.m[8] * b.z + a.m[12];
		r.y = a.m[1] * b.x + a.m[5] * b.y + a.m[9] * b.z + a.m[13];
		r.z = a.m[2] * b.x + a.m[6] * b.y + a.m[10]* b.z + a.m[14];
	}

	return r;
}

Vector3 MultM3V3(Matrix3 a, Vector3 b) {
	Vector3 r;
		
	if(!TRANSPOSED) {
		r.x = a.m[0] * b.x + a.m[1] * b.y + a.m[2] * b.z;
		r.y = a.m[3] * b.x + a.m[4] * b.y + a.m[5] * b.z;
		r.z = a.m[6] * b.x + a.m[7] * b.y + a.m[8] * b.z;
	}
	else {
		r.x = a.m[0] * b.x + a.m[3] * b.y + a.m[6] * b.z;
		r.y = a.m[1] * b.x + a.m[4] * b.y + a.m[7] * b.z;
		r.z = a.m[2] * b.x + a.m[5] * b.y + a.m[8] * b.z;
	}
		
	return r;
}

Vector4 MultM4V4(Matrix4 a, Vector4 b) {
	Vector4 r;

	if (!TRANSPOSED) {
		r.x = a.m[0] * b.x + a.m[1] * b.y + a.m[2] * b.z + a.m[3] * b.w;
		r.y = a.m[4] * b.x + a.m[5] * b.y + a.m[6] * b.z + a.m[7] * b.w;
		r.z = a.m[8] * b.x + a.m[9] * b.y + a.m[10]* b.z + a.m[11]* b.w;
		r.w = a.m[12]* b.x + a.m[13]* b.y + a.m[14]* b.z + a.m[15]* b.w;
	}
	else {
		r.x = a.m[0] * b.x + a.m[4] * b.y + a.m[8] * b.z + a.m[12] * b.w;
		r.y = a.m[1] * b.x + a.m[5] * b.y + a.m[9] * b.z + a.m[13] * b.w;
		r.z = a.m[2] * b.x + a.m[6] * b.y + a.m[10]* b.z + a.m[14] * b.w;
		r.w = a.m[3] * b.x + a.m[7] * b.y + a.m[11]* b.z + a.m[15] * b.w;
	}

	return r;
}

void OrthoNormalize(Matrix4 *R) {
	Vector3 x, y, z;

	if(TRANSPOSED) {
		x = SetV3(R->m[0], R->m[1], R->m[2]);
		y = SetV3(R->m[4], R->m[5], R->m[6]);
		z = Cross(x, y);
		z = Normalize(z);
		x = Normalize(x);
		y = Cross(z, x);
		R->m[0] = x.x;
		R->m[1] = x.y;
		R->m[2] = x.z;
		R->m[4] = y.x;
		R->m[5] = y.y;
		R->m[6] = y.z;
		R->m[8] = z.x;
		R->m[9] = z.y;
		R->m[10] = z.z;
		R->m[3] = 0.0;
		R->m[7] = 0.0;
		R->m[11] = 0.0;
		R->m[12] = 0.0;
		R->m[13] = 0.0;
		R->m[14] = 0.0;
		R->m[15] = 1.0;
	} 
    else {
		x = SetV3(R->m[0], R->m[4], R->m[8]);
		y = SetV3(R->m[1], R->m[5], R->m[9]);
		z = Cross(x, y);
		z = Normalize(z);
		x = Normalize(x);
		y = Cross(z, x);
		R->m[0] = x.x;
		R->m[4] = x.y;
		R->m[8] = x.z;
		R->m[1] = y.x;
		R->m[5] = y.y;
		R->m[9] = y.z;
		R->m[2] = z.x;
		R->m[6] = z.y;
		R->m[10] = z.z;
		R->m[3] = 0.0;
		R->m[7] = 0.0;
		R->m[11] = 0.0;
		R->m[12] = 0.0;
		R->m[13] = 0.0;
		R->m[14] = 0.0;
		R->m[15] = 1.0;
	}
}

Matrix4 TransposeM4(Matrix4 m) {
	Matrix4 a;
	a.m[0] = m.m[0];  a.m[4] = m.m[1];  a.m[8] =  m.m[2];  a.m[12] = m.m[3];
	a.m[1] = m.m[4];  a.m[5] = m.m[5];  a.m[9] =  m.m[6];  a.m[13] = m.m[7];
	a.m[2] = m.m[8];  a.m[6] = m.m[9];  a.m[10] = m.m[10]; a.m[14] = m.m[11];
	a.m[3] = m.m[12]; a.m[7] = m.m[13]; a.m[11] = m.m[14]; a.m[15] = m.m[15];
	return a;
}

Matrix3 TransposeM3(Matrix3 m) {
	Matrix3 a;
	a.m[0] = m.m[0]; a.m[3] = m.m[1]; a.m[6] = m.m[2];
	a.m[1] = m.m[3]; a.m[4] = m.m[4]; a.m[7] = m.m[5];
	a.m[2] = m.m[6]; a.m[5] = m.m[7]; a.m[8] = m.m[8];
	return a;
}

Matrix4 ArbritraryRotate(Vector3 axis, float fi) {
	Vector3 x, y, z;
	Matrix4 R, Rt, Raxis, m;

	if(axis.x < 0.0000001 && axis.x > -0.0000001) {
        if(axis.y < 0.0000001 && axis.y > -0.0000001) {
            if(axis.z > 0) {
                m = RotateZ(fi);
                return m;
            }
            else {
                m = RotateZ(-fi);
                return m;
            }
        }
    }

	x = Normalize(axis);
	z = SetV3(0, 0, 1);
	y = Normalize(Cross(z, x));
	z = Cross(x, y);

	if(TRANSPOSED) {
		R.m[0] = x.x; R.m[4] = x.y; R.m[8]  = x.z; R.m[12] = 0.0;
		R.m[1] = y.x; R.m[5] = y.y; R.m[9]  = y.z; R.m[13] = 0.0;
		R.m[2] = z.x; R.m[6] = z.y; R.m[10] = z.z; R.m[14] = 0.0;
		R.m[3] = 0.0; R.m[7] = 0.0; R.m[11] = 0.0; R.m[15] = 1.0;
	}
	else {
		R.m[0]  = x.x; R.m[1]  = x.y; R.m[2]  = x.z;  R.m[3]  = 0.0;
		R.m[4]  = y.x; R.m[5]  = y.y; R.m[6]  = y.z;  R.m[7]  = 0.0;
		R.m[8]  = z.x; R.m[9]  = z.y; R.m[10] = z.z;  R.m[11] = 0.0;
		R.m[12] = 0.0; R.m[13] = 0.0; R.m[14] = 0.0;  R.m[15] = 1.0;
	}

	Rt = TransposeM4(R);
	Raxis = RotateX(fi);

	m = MultM4(MultM4(Rt, Raxis), R);
	return m;
}

Matrix4 CrossM4(Vector3 a) {
	Matrix4 m;
	
	if (TRANSPOSED) {
		m.m[0] =   0; m.m[4] =-a.z; m.m[8] = a.y; m.m[12] = 0.0;
		m.m[1] = a.z; m.m[5] =   0; m.m[9] =-a.x; m.m[13] = 0.0;
		m.m[2] =-a.y; m.m[6] = a.x; m.m[10]=   0; m.m[14] = 0.0;
		m.m[3] = 0.0; m.m[7] = 0.0; m.m[11]= 0.0; m.m[15] = 0.0;
	}
	else {
		m.m[0]  =   0; m.m[1]  =-a.z; m.m[2] = a.y; m.m[3]  = 0.0;
		m.m[4]  = a.z; m.m[5]  =   0; m.m[6] =-a.x; m.m[7]  = 0.0;
		m.m[8]  =-a.y; m.m[9]  = a.x; m.m[10]=   0; m.m[11] = 0.0;
		m.m[12] = 0.0; m.m[13] = 0.0; m.m[14]= 0.0; m.m[15] = 0.0;
	}
	
	return m;
}

Matrix4 AddM4(Matrix4 a, Matrix4 b) {
	Matrix4 dest;
	
	int i;
	for (i = 0; i < 16; i++) {
		dest.m[i] = a.m[i] + b.m[i];
    }
	
	return dest;
}

void SetTransposed(char t) {
	TRANSPOSED = t;
}

Matrix4 LookAtVector(Vector3 p, Vector3 l, Vector3 v) {
	Vector3 n, u;
	Matrix4 rot, trans, m;

	n = Normalize(SubV3(p, l));
	u = Normalize(Cross(v, n));
	v = Cross(n, u);

	if (TRANSPOSED) {
        rot = SetM4(u.x, v.x, n.x, 0,
                    u.y, v.y, n.y, 0,
                    u.z, v.z, n.z, 0,
                    0  ,   0,   0, 1);
    }
	else {
        rot = SetM4(u.x, u.y, u.z, 0,
                    v.x, v.y, v.z, 0,
                    n.x, n.y, n.z, 0,
                    0  ,   0,   0, 1);
    }
    trans = Transform(-p.x, -p.y, -p.z);
    m = MultM4(rot, trans);

	return m;
}

Matrix4 LookAt( float px, float py, float pz, 
			    float lx, float ly, float lz,
			    float vx, float vy, float vz) {
	Vector3 p, l, v;
	p = SetV3(px, py, pz);
	l = SetV3(lx, ly, lz);
	v = SetV3(vx, vy, vz);
	return LookAtVector(p, l, v);
}

// http://www.opengl.org/wiki/GluPerspective_code
Matrix4 Perspective(float fovyInDeg, float aspectRatio, float znear, float zfar) {
	float f = 1 / tan(fovyInDeg / 2);
	Matrix4 m = SetM4(f / aspectRatio, 0, 0, 0, 0, f, 0, 0,
					  0, 0, (zfar + znear) / (znear - zfar), 
                      2 * zfar * znear / (znear - zfar),
					  0, 0, -1, 0);
	if (TRANSPOSED) {
		m = TransposeM4(m);
    }
	return m;
}

Matrix4 Frustum(float l, float r, float b, float t, float zn, float zf) {
    Matrix4 matrix;
    float tmp  = 2.0f * zn;
    float tmp2 = r - l;
    float tmp3 = t - b;
    float tmp4 = zf - zn;
    matrix.m[0] = tmp / tmp2;
    matrix.m[1] = 0.0;
    matrix.m[2] = 0.0;
    matrix.m[3] = 0.0;
    matrix.m[4] = 0.0;
    matrix.m[5] = tmp / tmp3;
    matrix.m[6] = 0.0;
    matrix.m[7] = 0.0;
    matrix.m[8] = (r + l) / tmp2;
    matrix.m[9] = (t + b) / tmp3;
    matrix.m[10] = (-zf - zn) / tmp4;
    matrix.m[11] = -1.0;
    matrix.m[12] = 0.0;
    matrix.m[13] = 0.0;
    matrix.m[14] = (-tmp * zf) / tmp4;
    matrix.m[15] = 0.0;
    
    if (!TRANSPOSED) {
    	matrix = TransposeM4(matrix);
    }
    
    return matrix;
}

Matrix4 Ortho(float left, float right, float bottom, float top, float near, float far) {
    float a = 2.0f / (right - left);
    float b = 2.0f / (top - bottom);
    float c = -2.0f / (far - near);
    float tx = - (right + left) / (right - left);
    float ty = - (top + bottom) / (top - bottom);
    float tz = - (far + near) / (far - near);
    Matrix4 o = SetM4(a, 0, 0, tx,
                      0, b, 0, ty,
                      0, 0, c, tz,
                      0, 0, 0, 1);

    if(TRANSPOSED) {
   		o = TransposeM4(o);
    }

    return o;
}

// http://www.dr-lex.be/random/matrix_inv.html
Matrix3 InvertM3(Matrix3 in) {
	Matrix3 out, nanout;
	
	float a11 = in.m[0];
	float a12 = in.m[1];
	float a13 = in.m[2];
	float a21 = in.m[3];
	float a22 = in.m[4];
	float a23 = in.m[5];
	float a31 = in.m[6];
	float a32 = in.m[7];
	float a33 = in.m[8];
	float DET = a11 * (a33 * a22 - a32 * a23) - a21 * (a33 * a12 - a32 * a13) + a31 * (a23 * a12 - a22 * a13);
	
    if(DET != 0) {
		out.m[0] =  (a33 * a22 - a32 * a23) / DET;
		out.m[1] = -(a33 * a12 - a32 * a13) / DET;
		out.m[2] =  (a23 * a12 - a22 * a13) / DET;
		out.m[3] = -(a33 * a21 - a31 * a23) / DET;
		out.m[4] =  (a33 * a11 - a31 * a13) / DET;
		out.m[5] = -(a23 * a11 - a21 * a13) / DET;
		out.m[6] =  (a32 * a21 - a31 * a22) / DET;
		out.m[7] = -(a32 * a11 - a31 * a12) / DET;
		out.m[8] =  (a22 * a11 - a21 * a12) / DET;
	}
	else {
		nanout = SetM3(NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN);
		out = nanout;
	}
	
	return out;
}

Matrix3 InverseTranspose(Matrix4 in) {
	Matrix3 out, nanout;
	
	float a11 = in.m[0];
	float a12 = in.m[1];
	float a13 = in.m[2];
	float a21 = in.m[4];
	float a22 = in.m[5];
	float a23 = in.m[6];
	float a31 = in.m[8];
	float a32 = in.m[9];
	float a33 = in.m[10];
	float DET = a11 * (a33 * a22 - a32 * a23) - a21 * (a33 * a12 - a32 * a13) + a31 * (a23 * a12 - a22 * a13);
	if(DET != 0) {
		out.m[0] =  (a33 * a22 -a32 * a23) / DET;
		out.m[3] = -(a33 * a12 -a32 * a13) / DET;
		out.m[6] =  (a23 * a12 -a22 * a13) / DET;
		out.m[1] = -(a33 * a21 -a31 * a23) / DET;
		out.m[4] =  (a33 * a11 -a31 * a13) / DET;
		out.m[7] = -(a23 * a11 -a21 * a13) / DET;
		out.m[2] =  (a32 * a21 -a31 * a22) / DET;
		out.m[5] = -(a32 * a11 -a31 * a12) / DET;
		out.m[8] =  (a22 * a11 -a21 * a12) / DET;
	}
	else {
		nanout = SetM3(NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN);
		out = nanout;
	}

	return out;
}

Matrix3 M4toM3(Matrix4 m) {
	Matrix3 res;
	res.m[0] = m.m[0];
	res.m[1] = m.m[1];
	res.m[2] = m.m[2];
	res.m[3] = m.m[4];
	res.m[4] = m.m[5];
	res.m[5] = m.m[6];
	res.m[6] = m.m[8];
	res.m[7] = m.m[9];
	res.m[8] = m.m[10];
	return res;
}

Matrix4 M3toM4(Matrix3 m) {
	Matrix4 res;
	res.m[0] = m.m[0];
	res.m[1] = m.m[1];
	res.m[2] = m.m[2];
	res.m[3] = 0;
	res.m[4] = m.m[3];
	res.m[5] = m.m[4];
	res.m[6] = m.m[5];
	res.m[7] = 0;
	res.m[8] = m.m[6];
	res.m[9] = m.m[7];
	res.m[10] = m.m[8];
	res.m[11] = 0;
	res.m[12] = 0;
	res.m[13] = 0;
	res.m[14] = 0;
	res.m[15] = 1;
	return res;
}

Vector3 V4toV3(Vector4 v) {
	Vector3 res;
	res.x = v.x;
	res.y = v.y;
	res.z = v.z;
	return res;
}

Vector4 V3toV4(Vector3 v) {
	Vector4 res;
	res.x = v.x;
	res.y = v.y;
	res.z = v.z;
	res.w = 1;
	return res;
}

// glMatrix (WebGL math unit)
Matrix4 InvertM4(Matrix4 a)
{
    Matrix4 b;
    
    float c=a.m[0],d=a.m[1],e=a.m[2],g=a.m[3],
	f=a.m[4],h=a.m[5],i=a.m[6],j=a.m[7],
	k=a.m[8],l=a.m[9],o=a.m[10],m=a.m[11],
	n=a.m[12],p=a.m[13],r=a.m[14],s=a.m[15],
	A=c*h-d*f,
	B=c*i-e*f,
	t=c*j-g*f,
	u=d*i-e*h,
	v=d*j-g*h,
	w=e*j-g*i,
	x=k*p-l*n,
	y=k*r-o*n,
	z=k*s-m*n,
	C=l*r-o*p,
	D=l*s-m*p,
	E=o*s-m*r,
	q=1/(A*E-B*D+t*C+u*z-v*y+w*x);

	b.m[0]=(h*E-i*D+j*C)*q;
	b.m[1]=(-d*E+e*D-g*C)*q;
	b.m[2]=(p*w-r*v+s*u)*q;
	b.m[3]=(-l*w+o*v-m*u)*q;
	b.m[4]=(-f*E+i*z-j*y)*q;
	b.m[5]=(c*E-e*z+g*y)*q;
	b.m[6]=(-n*w+r*t-s*B)*q;
	b.m[7]=(k*w-o*t+m*B)*q;
	b.m[8]=(f*D-h*z+j*x)*q;
	b.m[9]=(-c*D+d*z-g*x)*q;
	b.m[10]=(n*v-p*t+s*A)*q;
	b.m[11]=(-k*v+l*t-m*A)*q;
	b.m[12]=(-f*C+h*y-i*x)*q;
	b.m[13]=(c*C-d*y+e*x)*q;
	b.m[14]=(-n*u+p*B-r*A)*q;
	b.m[15]=(k*u-l*B+o*A)*q;

	return b;
};

void PrintM4(Matrix4 m) {
	unsigned int i;
	printf(" ---------------------------------------------------------------\n");
	for (i = 0; i < 4; i++) {
		int n = i * 4;
		printf("| %11.5f\t| %11.5f\t| %11.5f\t| %11.5f\t|\n", m.m[n], m.m[n+1], m.m[n+2], m.m[n+3]);
	}
	printf(" ---------------------------------------------------------------\n");
}

void PrintM3(Matrix3 m) {
	unsigned int i;
	printf(" ---------------------------------------------------------------\n");
	for (i = 0; i < 3; i++) {
		int n = i * 3;
		printf("| %11.5f\t| %11.5f\t| %11.5f\t| \n", m.m[n], m.m[n+1], m.m[n+2]);
	}
	printf(" ---------------------------------------------------------------\n");
}

void PrintV3(Vector3 in) {
	printf("(%f, %f, %f)\n", in.x, in.y, in.z);
}

#ifdef __cplusplus
Vector3::Vector3(Vector4 v) : x(v.x), y(v.y), z(v.z) {}

Matrix4::Matrix4(Matrix3 x) {
	m[0]  = x.m[0]; m[1]  = x.m[1]; m[2]  = x.m[2]; m[3]  = 0;
	m[4]  = x.m[3]; m[5]  = x.m[4]; m[6]  = x.m[5]; m[7]  = 0;
	m[8]  = x.m[6]; m[9]  = x.m[7]; m[10] = x.m[8]; m[11] = 0;
	m[12] =      0; m[13] =      0; m[14] =      0; m[15] = 1;
}
#endif

#endif
#endif