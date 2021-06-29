#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <bits/stdc++.h>

#define ColorLiteWood {0.749, 0.565, 0.0}
#define ColorDarkWood {0.706, 0.373, 0.024}
#define ColorMidWood {0.902, 0.59, 0.22}
#define ColorExtraDarkWood {0.471, 0.25, 0.016}
#define ColorFanDark {0.0824, 0.3333, 0.3412}
#define ColorFanLite {0.1333, 0.612, 0.6313}
#define ColorWall {0.576471, 0.678431, 0.203922}
#define ColorDarkWall {0.450981, 0.541177, 0.129412}
#define Yellow 245.0 / 255, 255.0/255, 48.0/255
#define White 1.0, 1.0, 1.0
#define M_PI (2*acos(0.0))

using namespace std;

const int windowWidth = 600;
const int windowHeight = 600;
double txVal = 0, tyVal = 0;
bool flagRotate = true;
double rval = 0, fanSpeed = 0;
bool openDrawer = true;

bool eye = true, cen = false;
double sec, minute, hour, cnt;
bool testOpen;
bool lightLeft = false, lightUp = false, spotLight = false;
bool amb = true, diff = true, spec = true;
double ty = 0, tz = 0, tx = 0;
double rx = 0, ry = 0, rz = 0;
double ds = 30;
GLfloat step = 0.5;
GLfloat stepAngle = 5;

vector<bool> OpenController(10, 0);

class myVector{
public:
	double x, y, z;
	myVector(){
	}
	myVector(double px, double py, double pz){
		x = px, y = py, z = pz;
	}
	void copyIt(myVector a){
		x = a.x, y = a.y, z = a.z;
	}

	myVector cross(myVector a){
		return myVector(y*a.z - z*a.y, z*a.x - x*a.z, x*a.y - y*a.x);
	}
	myVector add(myVector a){
		return myVector(x + a.x, y + a.y, z + a.z);
	}

	myVector multiply(double val){
		return myVector(val*x, val*y, val*z);
	}

	void Move(myVector to, float steps){
		x += (to.x)*steps;
		y += (to.y)*steps;
		z += (to.z)*steps;
	}

	void Rotate(myVector per, float angle){
		myVector t = cross(per);
		myVector m = *this;
		m = m.multiply(cos(angle*M_PI/180.0));
		t = t.multiply(sin(angle*M_PI/180.0));
		m = t.add(m);
		copyIt(m);
	}
};

myVector EYE(7.0, -7.0, 3.0), LOOK(0, 1, 0), UP(0,0,1), RIGHT(1, 0, 0);

void animate()
{
	if (flagRotate == true)
	{
		rval+= 0.05;
		if(rval > 360)
			rval -= 360;
	}

	fanSpeed += ds;
	if(fanSpeed >= 360 * ds) fanSpeed = 0;

    cnt -= 6;

    if(cnt == -90){
        sec -= 6;
        cnt = 0;
    }

    if(sec == -360){
        minute -= 6;
        sec = 0;
    }

    if(minute == -360){
        hour -= 6;
        minute = 0;
    }

	glutPostRedisplay();
}

static void resize(int width, int height)
{
    glViewport((width >= height) * ((float)width - height) / 2, (height >= width) * ((float)height - width) / 2, min(width, height), min(width, height));
}

vector<vector<GLfloat>> generatePoints8p(float x, float y, float z){
    vector<vector<GLfloat>> points(8, vector<GLfloat> (3));
    points = {
        {0.0, 0.0, 0.0}, // 0
        {x, 0.0, 0.0}, // 1
        {x, y, 0.0}, // 2
        {0.0, y, 0.0}, // 3
        {0.0, 0.0, z}, // 4
        {x, 0.0, z}, // 5
        {x, y, z}, // 6
        {0.0, y, z} // 7
    };

    return points;
}

vector<vector<GLubyte>> getMyIndicesForCube(){
    vector<vector<GLubyte>> ind(6, vector<GLubyte> (4));
    ind = {
        {1, 0, 3, 2},  // X-Y-down
        {6, 7, 4, 5},  // X-Y-up
        {5, 4, 0, 1},  // X-Z-front
        {2, 3, 7, 6},  // X-Z-back
        {2, 6, 5, 1},  // Y-Z-right
        {7, 3, 0, 4},  // Y-Z-left
    };

    return ind;
}

static void getNormal3p
(GLfloat x1, GLfloat y1,GLfloat z1, GLfloat x2, GLfloat y2,GLfloat z2, GLfloat x3, GLfloat y3,GLfloat z3)
{
    GLfloat Ux, Uy, Uz, Vx, Vy, Vz, Nx, Ny, Nz;

    Ux = x2-x1;
    Uy = y2-y1;
    Uz = z2-z1;

    Vx = x3-x1;
    Vy = y3-y1;
    Vz = z3-z1;

    Nx = Uy*Vz - Uz*Vy;
    Ny = Uz*Vx - Ux*Vz;
    Nz = Ux*Vy - Uy*Vx;

    glNormal3f(Nx,Ny,Nz);
}

void drawCube(float x, float y, float z, vector<GLfloat> &color){
    vector<vector<GLfloat>> points = generatePoints8p(x, y, z);
    vector<vector<GLubyte>> ind = getMyIndicesForCube();

    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient[] = { color[0] * 0.3, color[1] * 0.3, color[2] * 0.3, 1.0 };
    GLfloat mat_diffuse[] = { color[0], color[1], color[2], 1.0 };
    GLfloat mat_specular[] = { color[0], color[1], color[2], 1.0 };
    GLfloat mat_shininess[] = {60};

    glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess);

    glBegin(GL_QUADS);

        for(int i = 0; i < 6; i++){

            getNormal3p(points[ind[i][0]][0], points[ind[i][0]][1], points[ind[i][0]][2],
                    points[ind[i][1]][0], points[ind[i][1]][1], points[ind[i][1]][2],
                    points[ind[i][2]][0], points[ind[i][2]][1], points[ind[i][2]][2]);

            for(int j = 0; j < 4; j++){
                glVertex3fv(&points[ind[i][j]][0]);
            }
        }

    glEnd();
}

void drawCube(float x, float y, float z, vector<GLfloat> &color, bool glow){
    vector<vector<GLfloat>> points = generatePoints8p(x, y, z);
    vector<vector<GLubyte>> ind = getMyIndicesForCube();

    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient[] = { color[0] * 0.2, color[1] * 0.2, color[2] * 0.2, 1.0 };
    GLfloat mat_diffuse[] = { color[0] * 0.7, color[1] * 0.7, color[2] * 0.7, 1.0 };
    GLfloat mat_specular[] = { color[0] * 0.7, color[1] * 0.7, color[2] * 0.7, 1.0 };
    GLfloat mat_shininess[] = {30};
    GLfloat em[] = {0.5, 0.5, 0.5, 1.0};

    glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess);

    if(glow) glMaterialfv(GL_FRONT, GL_EMISSION, em);
    else glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);

    glBegin(GL_QUADS);

        for(int i = 0; i < 6; i++){

            getNormal3p(points[ind[i][0]][0], points[ind[i][0]][1], points[ind[i][0]][2],
                    points[ind[i][1]][0], points[ind[i][1]][1], points[ind[i][1]][2],
                    points[ind[i][2]][0], points[ind[i][2]][1], points[ind[i][2]][2]);

            for(int j = 0; j < 4; j++){
                glVertex3fv(&points[ind[i][j]][0]);
            }
        }

    glEnd();
}

void drawDrawer(float len, float width, float height, float thin, float Handlegap){
    //glBegin(GL_QUAD_STRIP);
        vector<GLfloat> color = ColorDarkWood;
        glPushMatrix();
            glTranslatef(0.0, 0.0, thin);
            drawCube(thin, width, height, color); // side Left
        glPopMatrix();

        glPushMatrix();
            glTranslatef(len - thin, 0.0, thin);
            drawCube(thin, width, height, color); // side Right
        glPopMatrix();

        color.clear();
        color = ColorLiteWood;

        glPushMatrix();
            glTranslatef(thin, 0.0, thin);
            drawCube(len - 2 * thin, thin, height - thin, color); // front
        glPopMatrix();

        glPushMatrix();
            glTranslatef(thin, width - thin, thin);
            drawCube(len - 2 * thin, thin, height - thin, color); // back
        glPopMatrix();

        color = ColorMidWood;

        glPushMatrix();
            drawCube(len, width, thin, color);  // floor
        glPopMatrix();

        color = ColorExtraDarkWood;

        glPushMatrix();
            glTranslatef(len, -2.0 * thin, 0.0);
            drawCube(2 * thin, width + 4 * thin, height + 2 * thin, color);  // Main Plate
        glPopMatrix();

        // Handle
        glPushMatrix();
            glTranslatef(len + 2 * thin, width / 3, 3 * height / 7);
            drawCube(Handlegap, thin, height / 7, color);  // Handle side front
        glPopMatrix();

        color = ColorDarkWood;

        glPushMatrix();
            glTranslatef(len + 2 * thin, 2 * width / 3 - thin, 3 * height / 7);
            drawCube(Handlegap, thin, height / 7, color);  // Handle side Back
        glPopMatrix();

        glPushMatrix();
            glTranslatef(len + 2 * thin + Handlegap, width / 3 - thin, 3 * height / 7 - thin);
            drawCube(1.5 * thin, width / 3 + 2 * thin, height / 7 + 2 * thin, color);  // Handle plate
        glPopMatrix();

    //glEnd();
}

void DrawHalfBedWithDrawer(float len, float width, float heigth, bool Open, float HandleGap = 0.1){
    float drawerLen = 2 * width / 3;
    float drawerWidth = 2 * len / 3;
    float drawerHeigth = 2 * heigth / 3;
    float drawerThin = drawerWidth / 80.0;
    vector<GLfloat> color = ColorDarkWood;

    glPushMatrix();
        glTranslatef( width / 3 + (0.5 * Open), len / 6, heigth / 6);
        drawDrawer(drawerLen, drawerWidth, drawerHeigth, drawerThin, HandleGap);
    glPopMatrix();

    glPushMatrix();
        drawCube(width, len, heigth / 6, color); // down - floor
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.0, 0.0, 5 * heigth / 6);
        drawCube(width, len, heigth / 6, color); // top - floor
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.0, 0.0, heigth / 6);
        drawCube(width, len / 6, 2 * heigth / 3, color); // front side
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.0, 5 * len / 6, heigth / 6);
        drawCube(width, len / 6, 2 * heigth / 3, color); // front side
    glPopMatrix();
}

void drawFlatPyramid(float dLen, float dWidth, float extraWidth, vector<GLfloat> colorNotUsed, vector<GLfloat> color){
    vector<vector<GLfloat>> points = generatePoints8p(dLen, dWidth, extraWidth);
    vector<vector<GLubyte>> ind = getMyIndicesForCube();

    points[4][0] -= extraWidth; points[4][1] -= extraWidth;
    points[5][0] += extraWidth; points[5][1] -= extraWidth;
    points[6][0] += extraWidth; points[6][1] += extraWidth;
    points[7][0] -= extraWidth; points[7][1] += extraWidth;

    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient[] = { color[0] * 0.5, color[1] * 0.3, color[2] * 0.3, 1.0 };
    GLfloat mat_diffuse[] = { color[0], color[1], color[2], 1.0 };
    GLfloat mat_specular[] = { color[0], color[1], color[2], 1.0 };
    GLfloat mat_shininess[] = {30};

    glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess);

    glBegin(GL_QUADS);

        for(int i = 0; i < 6; i++){

            getNormal3p(points[ind[i][0]][0], points[ind[i][0]][1], points[ind[i][0]][2],
                    points[ind[i][1]][0], points[ind[i][1]][1], points[ind[i][1]][2],
                    points[ind[i][2]][0], points[ind[i][2]][1], points[ind[i][2]][2]);

            for(int j = 0; j < 4; j++){
                glVertex3fv(&points[ind[i][j]][0]);
            }
        }

    glEnd();
}

void drawFlatPyramid(float dLen, float dWidth,float dheight, float extraWidth, vector<GLfloat> colorNotUsed, vector<GLfloat> color){
    vector<vector<GLfloat>> points = generatePoints8p(dLen, dWidth, dheight);
    vector<vector<GLubyte>> ind = getMyIndicesForCube();

    points[4][0] -= extraWidth; points[4][1] -= extraWidth;
    points[5][0] += extraWidth; points[5][1] -= extraWidth;
    points[6][0] += extraWidth; points[6][1] += extraWidth;
    points[7][0] -= extraWidth; points[7][1] += extraWidth;

    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient[] = { color[0] * 0.5, color[1] * 0.3, color[2] * 0.3, 1.0 };
    GLfloat mat_diffuse[] = { color[0], color[1], color[2], 1.0 };
    GLfloat mat_specular[] = { color[0], color[1], color[2], 1.0 };
    GLfloat mat_shininess[] = {30};

    glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess);

    glBegin(GL_QUADS);

        for(int i = 0; i < 6; i++){

            getNormal3p(points[ind[i][0]][0], points[ind[i][0]][1], points[ind[i][0]][2],
                    points[ind[i][1]][0], points[ind[i][1]][1], points[ind[i][1]][2],
                    points[ind[i][2]][0], points[ind[i][2]][1], points[ind[i][2]][2]);

            for(int j = 0; j < 4; j++){
                glVertex3fv(&points[ind[i][j]][0]);
            }
        }

    glEnd();
}

void drawFlatPyramid(float dLen, float dWidth,float dheight, float extraWidth, vector<GLfloat> color){
    vector<vector<GLfloat>> points = generatePoints8p(dLen, dWidth, dheight);
    vector<vector<GLubyte>> ind = getMyIndicesForCube();

    points[4][0] -= extraWidth; points[4][1] -= extraWidth;
    points[5][0] += extraWidth; points[5][1] -= extraWidth;
    points[6][0] += extraWidth; points[6][1] += extraWidth;
    points[7][0] -= extraWidth; points[7][1] += extraWidth;

    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient[] = { color[0] * 0.5, color[1] * 0.3, color[2] * 0.3, 1.0 };
    GLfloat mat_diffuse[] = { color[0], color[1], color[2], 1.0 };
    GLfloat mat_specular[] = { color[0], color[1], color[2], 1.0 };
    GLfloat mat_shininess[] = {30};

    glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess);


    glBegin(GL_QUADS);

        for(int i = 0; i < 6; i++){

            getNormal3p(points[ind[i][0]][0], points[ind[i][0]][1], points[ind[i][0]][2],
                    points[ind[i][1]][0], points[ind[i][1]][1], points[ind[i][1]][2],
                    points[ind[i][2]][0], points[ind[i][2]][1], points[ind[i][2]][2]);

            for(int j = 0; j < 4; j++){
                glVertex3fv(&points[ind[i][j]][0]);
            }
        }

    glEnd();
}

void drawBalish(float len, float width, float height){
    vector<GLfloat> color;
    color = {237.0 / 255, 255.0 / 255, 61.0 / 255};

    glPushMatrix();
        glTranslatef(0.2, 0, 0);
        drawCube(len - 0.4, width, height, color);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0,0.1,0.1);
        glRotatef(90, 0,1,0);
        glRotatef(90, 0, 0, 1);
        drawFlatPyramid(width - 0.2, height - 0.2, 0.2, 0.1, color);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(len, width - 0.1,0.1);
        glRotatef(-90, 0,1,0);
        glRotatef(-90, 0, 0, 1);
        drawFlatPyramid(width - 0.2, height - 0.2, 0.2, 0.1, color);
    glPopMatrix();
}

void drawBed(float len, float width, float height){
    vector<GLfloat> color = ColorExtraDarkWood;

    glPushMatrix();
        DrawHalfBedWithDrawer(len / 2, width, height, (openDrawer && OpenController[0]));
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.0, len / 2, 0.0);
        DrawHalfBedWithDrawer(len / 2, width, height, (openDrawer && OpenController[1]));
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.0, len, 0.0);
        drawCube(width, 0.2, 3 * height, color);
    glPopMatrix();

    color = {230.0 / 255, 235.0 / 255, 234.0 / 255};

    glPushMatrix(); // toshok
        glTranslatef(0.04, 0.04, height);
        drawCube(width - 0.08, len - 0.08, 0.3, color);
    glPopMatrix();

    float blen = width / 2 - (width / 10);
    float bwidth = 2 * blen / 3;
    float bheight = 0.3;

    glPushMatrix();
        glTranslatef(width / 20, len - bwidth - width / 20, height + 0.3);
        drawBalish(blen, bwidth, bheight);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(width / 2 + width / 20, len - bwidth - width / 20, height + 0.3);
        drawBalish(blen, bwidth, bheight);
    glPopMatrix();
}

void drawWardrobe(float length, float width, float heigth, int NumberOfDrawer, float extraWidth){
    float dHeigth = (length - extraWidth) / NumberOfDrawer;
    float dLen = length;
    float dWidth = width;

    for(int i = 0; i < NumberOfDrawer; i++){
        glPushMatrix();
            glTranslatef(0, 0, i * dHeigth);
            DrawHalfBedWithDrawer(dLen, dWidth, dHeigth, (openDrawer && OpenController[2 + i]));
        glPopMatrix();
    }

    glPushMatrix();
        glTranslatef(0,0, NumberOfDrawer * dHeigth);
        drawFlatPyramid(dWidth, dLen, extraWidth, ColorDarkWood, ColorExtraDarkWood);
    glPopMatrix();
}

void drawPizzaSlice(float len, float side, float heigth){
    vector<vector<GLfloat>> points(6, vector<GLfloat>(3));

    points = {
        {0, 0, 0},
        {side, len, 0},
        {-side, len, 0},
        {0, 0, heigth},
        {side, len, heigth},
        {-side, len, heigth}
    };

    vector<vector<GLubyte>> TriInd(2, vector<GLubyte> (3));
    TriInd = {
        {0, 2, 1},
        {3, 4, 5}
    };

    vector<vector<GLubyte>> RectInd(3, vector<GLubyte> (4));
    RectInd = {
        {0, 3, 5, 2},
        {0, 1, 4, 3},
        {1, 2, 5, 4}
    };

    vector<GLfloat> color = ColorFanDark;

    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient[] = { color[0] * 0.5, color[1] * 0.3, color[2] * 0.3, 1.0 };
    GLfloat mat_diffuse[] = { color[0], color[1], color[2], 1.0 };
    GLfloat mat_specular[] = { color[0], color[1], color[2], 1.0 };
    GLfloat mat_shininess[] = {30};

    glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess);

    glBegin(GL_QUADS);
        for(int i = 0; i < 3; i++){

            getNormal3p(points[RectInd[i][0]][0], points[RectInd[i][0]][1], points[RectInd[i][0]][2],
                    points[RectInd[i][1]][0], points[RectInd[i][1]][1], points[RectInd[i][1]][2],
                    points[RectInd[i][2]][0], points[RectInd[i][2]][1], points[RectInd[i][2]][2]);

            for(int j = 0; j < 4; j++){
                glVertex3fv(&points[RectInd[i][j]][0]);
            }
        }

    glEnd();

    glBegin(GL_TRIANGLES);
        for(int i = 0; i < 2; i++){

            getNormal3p(points[TriInd[i][0]][0], points[TriInd[i][0]][1], points[TriInd[i][0]][2],
                    points[TriInd[i][1]][0], points[TriInd[i][1]][1], points[TriInd[i][1]][2],
                    points[TriInd[i][2]][0], points[TriInd[i][2]][1], points[TriInd[i][2]][2]);

            for(int j = 0; j < 3; j++){
                glVertex3fv(&points[TriInd[i][j]][0]);
            }
        }

    glEnd();
}

void drawRoundPlate(float radius, float heigth, int numberOfTriangle = 360){
    float theta = 2.0 * M_PI / numberOfTriangle;
    float len = radius * cos(theta / 2.0);
    float side = radius * sin(theta / 2.0);

    for(int i = 0; i < numberOfTriangle; i++){
        glPushMatrix();
            glRotatef(i * theta * 180.0 / M_PI, 0, 0, 1);
            drawPizzaSlice(len, side, heigth);
        glPopMatrix();
    }
}

void drawBlade(float length, float width, float heigth = 0.05){
    float outLength = 7 * length / 8;
    float inLength = length / 8  + 2 * length / 10;
    float inWidth = width / 5;

    vector<GLfloat> color = ColorFanLite;

    glPushMatrix();
        drawCube(width, outLength, heigth, color);
    glPopMatrix();

    color = ColorFanDark;

    glPushMatrix();
        glTranslatef(inWidth, -inLength + (length / 10), heigth);
        drawCube(inWidth, inLength, heigth, color);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(3 * inWidth, -inLength + (length / 10), heigth);
        drawCube(inWidth, inLength, heigth, color);
    glPopMatrix();
}

void drawFan(float longHeigth, float shortHeigth, float radius, float bladeWidth, float numberOfBlades = 3){
    glPushMatrix();
        glTranslatef(0, 0, shortHeigth);
        drawRoundPlate(radius / 32, longHeigth);
    glPopMatrix();

    glPushMatrix();
        glRotatef(fanSpeed, 0, 0, 1);
        //glTranslatef()
        glPushMatrix();
            drawRoundPlate(radius / 4, shortHeigth);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(-(bladeWidth / 2), -(radius / 3 - 3 * radius / 40), shortHeigth);

            float theta = 360 / numberOfBlades;

            for(int i = 0; i < numberOfBlades; i++){
                glPushMatrix();
                    glTranslatef( bladeWidth / 2, +(radius / 3 - 3 * radius / 40), -shortHeigth);
                    glRotatef(i * theta, 0, 0, 1);
                    glTranslatef( -bladeWidth / 2, (radius / 3 - 3 * radius / 40), shortHeigth);
                    drawBlade(3 * radius / 4, bladeWidth);
                glPopMatrix();
            }

        glPopMatrix();
    glPopMatrix();
}

void drawClock(float len){
    float extraWidth = 0.1;
    len -= 2 * extraWidth;
    glPushMatrix(); //Whole Clock
        glTranslatef(len / 2, len / 2, 0);
        glRotatef(90, 0, 0, 1);

        glPushMatrix(); // Frame
            glTranslatef(len / 2, -len / 2, 0);

            glPushMatrix(); // Nicher Palla
                glRotatef(180, 0, 1, 0);
                drawFlatPyramid(len, len, extraWidth, ColorDarkWood, {1,1,1});
            glPopMatrix();

            glPushMatrix(); //Border
                //glTranslatef(-len, extraWidth, 0);
                glRotatef(90, 0, 1, 0);
                glRotatef(90, 0, 0, 1);
                drawFlatPyramid(len, 0.1, extraWidth, ColorLiteWood, ColorExtraDarkWood);
            glPopMatrix();

            glPushMatrix(); //Border
                glTranslatef(-len, 0, extraWidth);
                glRotatef(180, 0, 1, 0);
                glRotatef(90, 0, 1, 0);
                glRotatef(90, 0, 0, 1);
                drawFlatPyramid(len, 0.1, extraWidth, ColorLiteWood, ColorExtraDarkWood);
            glPopMatrix();

            glPushMatrix(); //Border
                glTranslatef(0, len, 0);
                glRotatef(90, 0, 0, 1);
                glPushMatrix();
                    //glTranslatef(-len, extraWidth, 0);
                    glRotatef(90, 0, 1, 0);
                    glRotatef(90, 0, 0, 1);
                    drawFlatPyramid(len, 0.1, extraWidth, ColorLiteWood, ColorExtraDarkWood);
                glPopMatrix();

                glPushMatrix();
                    glTranslatef(-len, 0, extraWidth);
                    glRotatef(180, 0, 1, 0);
                    glRotatef(90, 0, 1, 0);
                    glRotatef(90, 0, 0, 1);
                    drawFlatPyramid(len, 0.1, extraWidth, ColorLiteWood, ColorExtraDarkWood);
                glPopMatrix();
            glPopMatrix();
        glPopMatrix();

        float frameLen = len;
        len -= 4 * extraWidth;
        len /= 6;

        vector<GLfloat> color = {0,0,0}; //black

        glPushMatrix(); // second-stick
            glRotatef(sec, 0, 0, 1);
            glTranslatef(0, -len / 24, extraWidth);
            drawCube(3 * len, len / 12, extraWidth / 3, color);
        glPopMatrix();

        glPushMatrix(); // Minute-stick
            glRotatef(minute, 0, 0, 1);
            glTranslatef(0, -len / 16, extraWidth + extraWidth / 3);
            drawCube(2 * len, len / 8, extraWidth / 3, color);
        glPopMatrix();

        glPushMatrix(); //hour - stick
            glRotatef(hour, 0, 0, 1);
            glTranslatef(0, -len / 8, extraWidth + 2*extraWidth / 3);
            drawCube(1.5 * len, len / 4, extraWidth / 3, color);
        glPopMatrix();

        glPushMatrix(); //knob
            glTranslatef( 0, 0, extraWidth + 3*extraWidth / 3);
            drawRoundPlate(len / 2.5, 0.01);
        glPopMatrix();

        color = ColorExtraDarkWood;

        for(int i = 0; i < 12; i++){
            glPushMatrix();
                glRotatef(i * 30, 0, 0, 1);
                glTranslatef(3 * len - 0.1, 0, extraWidth);
                drawCube(0.1, 0.1, 0.02, color);
            glPopMatrix();
        }

    glPopMatrix();
}

void drawTiles(int dimX, int dimY, float indDim){
    vector<vector<GLfloat>> color(2);

    color[0] = {1,1,1};
    color[1] = {0,0,0};

    glPushMatrix();
        for(int i = 0; i < dimY; i++){
            glPushMatrix();
                glTranslatef(0, i * indDim, 0);
                for(int j = 0; j < dimX; j++){
                    glPushMatrix();
                        glTranslatef(j * indDim, 0, 0);
                        drawCube(indDim, indDim, 0.05, color[(i + j) % 2]);
                    glPopMatrix();
                }
            glPopMatrix();
        }
    glPopMatrix();
}

void drawAC(float len, float width, float height){
    glPushMatrix();
        glTranslatef(0, 0, width);
        glRotatef(-90, 1, 0, 0);
        drawFlatPyramid(len, width, height, 0.2, {236.0 / 255, 237.0/255, 230.0 / 255}, {1, 1, 1});
    glPopMatrix();
}

void drawBoxThatCanOpen(float len, float width, float height, bool openDoor = false){
    vector<GLfloat> color;

    color = {64.0 / 255, 28.0 / 255, 1.0 / 255};

    float palla = len / 10;

    glPushMatrix();
        drawCube(len, width, palla, color);
    glPopMatrix();

    color = {38.0/255, 17.0/255, 2.0/255};

    glPushMatrix();
        glTranslatef(0,0,palla);
        drawCube(palla, width, height - 2 * palla, color);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(9 * palla, 0, palla);
        drawCube(palla, width, height - 2 * palla, color);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(palla, width - palla, palla);
        drawCube(len - 2 * palla, palla, height - 2 * palla, color);
    glPopMatrix();

    //color[1] = {38.0/255, 17.0/255, 2.0/255};
    glPushMatrix();
        glTranslatef(0,0, height - palla);
        drawCube(len, width, palla, color);
    glPopMatrix();

    color.clear();

    color = {168.0/255, 122.0/255, 22.0/255};

    glPushMatrix();
        if(openDoor){
            glTranslatef(palla, 0.05, palla);
            glRotatef(-85, 0, 0, 1);
            glTranslatef(-palla, -0.05, -palla);
        }

        glTranslatef(palla, 0.05, palla);
        drawCube(len - 2 * palla, palla / 4, height - 2 * palla, color);

        glPushMatrix();
            if(openDoor){
                glTranslatef(len - palla - palla - 0.05 - palla, -0.05, height / 2);
                glRotatef(-85, 0, 0, 1);
                glTranslatef(-(len - palla - palla - 0.05 - palla), 0.05, -height / 2);
            }

            glTranslatef(len - palla - palla - 0.05 - palla, -0.05, height / 2);
            drawCube(palla, palla, palla, color);
        glPopMatrix();
    glPopMatrix();

    color = {(float)(rand() % 255)/255, (float)(rand() % 255)/255, (float)(rand() % 255)/255};

    glPushMatrix();
        glTranslatef(2 * palla, palla, palla);
        drawCube(len / 2, width / 2, height / 2, color);
    glPopMatrix();
}

void drawDressingTable(float len, float width, float height, int numberOfDrawer){
    float dlen = len / numberOfDrawer;
    float dheight = height / 3;

    for(int i = 0; i < numberOfDrawer; i++){
        glPushMatrix();
            glTranslatef(i * dlen, 0, 0);
            drawBoxThatCanOpen(dlen, width, dheight, (openDrawer && OpenController[6 + i]));
        glPopMatrix();
    }

    float palla = width / 6;

    vector<GLfloat> color = {89.0 / 255, 38.0 / 255, 8.0 / 255};

    float mirrorWidth = palla / 6;

    glPushMatrix();
        glTranslatef(0, width - palla / 2, height / 3);
        drawCube(palla, palla / 2, 2 * height / 3, color);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0, width - palla - mirrorWidth, height / 3);
        drawCube(palla, palla / 2, 2 * height / 3, color);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(len - palla, width - palla / 2, height / 3);
        drawCube(palla, palla / 2, 2 * height / 3, color);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(len - palla, width - palla - mirrorWidth, height / 3);
        drawCube(palla, palla / 2, 2 * height / 3, color);
    glPopMatrix();

    color = {192.0 / 255, 235.0 / 255, 234.0 / 255};

    glPushMatrix();
        glTranslatef(0, width - palla / 2 - mirrorWidth, height / 3);
        drawCube(len, mirrorWidth, 2 * height / 3, color);
    glPopMatrix();
}

void drawLamp(float height, float R){
    float unit = height / 3;
    vector<GLfloat> color = {23.0 / 255, 15.0 / 255, 7.0/255};
    glPushMatrix();
        glTranslatef(R/2 - R / 20,R/2 - R / 20,0.02);
        drawCube(R / 10, R / 10, unit * 2.3, color); //pillar
    glPopMatrix();

    glPushMatrix();
        glTranslatef(R/2 - R / 10, R / 2 - R / 10, 0);
        drawCube(R / 5, R / 5, 0.02, color); //floor
    glPopMatrix();

    float r = R / 6 + 2 * 0.02;

    color = {.7, 0, 0};

    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient[] = { color[0] * 0.3, color[1] * 0.3, color[2] * 0.3, 1.0 };
    GLfloat mat_diffuse[] = { color[0], color[1], color[2], 1.0 };
    GLfloat mat_specular[] = { color[0], color[1], color[2], 1.0 };
    GLfloat em[] = { color[0] * 0.06, color[1] * 0.001, color[2] * 0.001, 1.0 };
    GLfloat mat_shininess[] = {10};

    glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess);

    if(spotLight) glMaterialfv(GL_FRONT, GL_EMISSION, em);
    else glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);

    vector<pair<float, float>> pointsLow = {
        {0, 0},
        {R, 0},
        {R, R},
        {0, R}
    };

    float d = (R - r) / 2;

    vector<pair<float, float>> pointsUp = {
        {d, d},
        {R - d, d},
        {R - d, R - d},
        {d, R - d}
    };

    glPushMatrix();
        glTranslatef(0,0,height - unit * 1.2);
        glBegin(GL_QUADS);
            for(int i = 0; i < 4; i++){
                getNormal3p(pointsLow[i].first, pointsLow[i].second, 0,
                    pointsLow[(i + 1) % 4].first, pointsLow[(i + 1) % 4].second, 0,
                    pointsUp[(i + 1) % 4].first, pointsUp[(i + 1) % 4].second, unit * 1.2);

                glVertex3f(pointsLow[i].first, pointsLow[i].second, 0);
                glVertex3f(pointsLow[(i + 1) % 4].first, pointsLow[(i + 1) % 4].second, 0);
                glVertex3f(pointsUp[(i + 1) % 4].first, pointsUp[(i + 1) % 4].second, unit * 1.2);
                glVertex3f(pointsUp[i].first, pointsUp[i].second, unit * 1.2);
            }
        glEnd();
        glBegin(GL_TRIANGLES);
            for(int i = 0; i < 4; i++){
                getNormal3p(
                    R/2,R/2,unit * 1.2,
                    pointsUp[i].first, pointsUp[i].second, unit * 1.2,
                    pointsUp[i + 1].first, pointsUp[i + 1].second, unit * 1.2
                    );

                glVertex3f(R/2,R/2,unit * 1.2);
                glVertex3f(pointsUp[i].first, pointsUp[i].second, unit * 1.2);
                glVertex3f(pointsUp[(i + 1) % 4].first, pointsUp[(i + 1) % 4].second, unit * 1.2);
            }

        glEnd();
    glPopMatrix();
}

void light()
{
    GLfloat no_light[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_ambient[]  = { White, 1.0 };
    GLfloat light_diffuse[]  = { White, 1.0 };
    GLfloat light_specular[] = { White, 1.0 };
    GLfloat light_position[] = { 6.5, 5.5, 7.0 - 0.75 - 0.5, 1.0 };

    glEnable( GL_LIGHT0);

    if(lightUp){
        if(amb) glLightfv( GL_LIGHT0, GL_AMBIENT, light_ambient);
        else glLightfv( GL_LIGHT0, GL_AMBIENT, no_light);

        if(diff) glLightfv( GL_LIGHT0, GL_DIFFUSE, light_diffuse);
        else glLightfv( GL_LIGHT0, GL_DIFFUSE, no_light);

        if(spec) glLightfv( GL_LIGHT0, GL_SPECULAR, light_specular);
        else glLightfv( GL_LIGHT0, GL_SPECULAR, no_light);
    }
    else{
        glLightfv( GL_LIGHT0, GL_AMBIENT, no_light);
        glLightfv( GL_LIGHT0, GL_DIFFUSE, no_light);
        glLightfv( GL_LIGHT0, GL_SPECULAR, no_light);
    }

    glLightfv( GL_LIGHT0, GL_POSITION, light_position);

    GLfloat light1_ambient[]  = { White, 1.0 };
    GLfloat light1_diffuse[]  = { White, 1.0 };
    GLfloat light1_specular[] = { White, 1.0 };
    GLfloat light1_position[] = { .75 + 0.5, 4, 7.0 - 0.5, 1.0 };

    glEnable( GL_LIGHT1);

    if(lightLeft){
        if(amb) glLightfv( GL_LIGHT1, GL_AMBIENT, light1_ambient);
        else glLightfv( GL_LIGHT1, GL_AMBIENT, no_light);

        if(diff) glLightfv( GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
        else glLightfv( GL_LIGHT1, GL_DIFFUSE, no_light);

        if(spec) glLightfv( GL_LIGHT1, GL_SPECULAR, light1_specular);
        else glLightfv( GL_LIGHT1, GL_SPECULAR, no_light);
    }
    else{
        glLightfv( GL_LIGHT1, GL_AMBIENT, no_light);
        glLightfv( GL_LIGHT1, GL_DIFFUSE, no_light);
        glLightfv( GL_LIGHT1, GL_SPECULAR, no_light);
    }

    glLightfv( GL_LIGHT1, GL_POSITION, light1_position);


    GLfloat light2_ambient[]  = { Yellow, 1.0 };
    GLfloat light2_diffuse[]  = { Yellow, 1.0 };
    GLfloat light2_specular[] = { Yellow, 1.0 };
    GLfloat light2_position[] = { 7, 8, 3.4, 1.0 };
    glEnable(GL_LIGHT2);

    if(spotLight){
        if(amb) glLightfv( GL_LIGHT2, GL_AMBIENT, light2_ambient);
        else glLightfv( GL_LIGHT2, GL_AMBIENT, no_light);

        if(diff) glLightfv( GL_LIGHT2, GL_DIFFUSE, light2_diffuse);
        else glLightfv( GL_LIGHT2, GL_DIFFUSE, no_light);

        if(spec) glLightfv( GL_LIGHT2, GL_SPECULAR, light2_specular);
        else glLightfv( GL_LIGHT2, GL_SPECULAR, no_light);
    }
    else{
        glLightfv( GL_LIGHT2, GL_AMBIENT, no_light);
        glLightfv( GL_LIGHT2, GL_DIFFUSE, no_light);
        glLightfv( GL_LIGHT2, GL_SPECULAR, no_light);
    }

    glLightfv( GL_LIGHT2, GL_POSITION, light2_position);

    GLfloat spot_direction[] = { 0, 0.0, -1.0};
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot_direction);
    glLightf( GL_LIGHT2, GL_SPOT_CUTOFF, 30.0);
}

void display(void)
{
 glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glFrustum(-1,1,-1,1,1,50);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    gluLookAt(EYE.x, EYE.y, EYE.z,   EYE.x + LOOK.x, EYE.y + LOOK.y, EYE.z + LOOK.z,   UP.x,UP.y,UP.z);
    light();
    vector<GLfloat> color = ColorWall;

    glPushMatrix();
        glPushMatrix();
            drawTiles(12, 10, 1);
        glPopMatrix();

        glPushMatrix(); //Left Wall
            glTranslatef(0,0,0.05);
            drawCube(0.75, 10, 7, color);
        glPopMatrix();

        glPushMatrix(); //right Wall
            glTranslatef(12 - 0.75, 0, 0.05);
            drawCube(0.75, 10, 7, color);
        glPopMatrix();

        glPushMatrix(); //Back-Wall
            glTranslatef(0, 10 - 0.75, 0.05);
            drawCube(12, 0.75, 7, color);
        glPopMatrix();

        glPushMatrix(); //Up-Wall
            glTranslatef(0, 0, 7 + 0.05);
            drawCube(12, 10, 0.75, color);
        glPopMatrix();

        glPushMatrix(); // Bed
            glTranslatef(1.5, 3, 0.05); // 0.05 = Tiles height
            drawBed(6,4,1);
        glPopMatrix();

        glPushMatrix(); //Dressing Table
            glTranslatef(8, 10 - 0.75 - 2, 0.05);
            drawDressingTable(2, 1.5, 4.5, 2);
        glPopMatrix();

        glPushMatrix(); //Clock
            glTranslatef(1.5, 10 - 0.85, 4);
            glTranslatef(2, 0, 0.05);
            glRotatef(90, 1, 0, 0);
            drawClock(2);
        glPopMatrix();

        glPushMatrix(); // AC
            glTranslatef(12 - 0.75 - 0.5, 2.5 + 2, 5.5 + 0.05);
            glRotatef(-90, 0, 0, 1);
            drawAC(2.5, 0.75, 0.5);
        glPopMatrix();

        glPushMatrix(); // fan
            glTranslatef(4, 6, 7 + 0.05 - 1.2);
            drawFan(1, 0.2, 2, 0.5);
        glPopMatrix();

        glPushMatrix(); // wardrobe
            glTranslatef(12 - 0.75 - 0.2, 4, 0.05);
            glRotatef(180, 0, 0, 1);
            drawWardrobe(2, 1.5, 3.5, 4, 0.2);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(7.0 - 1.0 / 2, 8.0 - 1.0 / 2, 0.05);
            drawBoxThatCanOpen(1, 1, 2);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(7.0 - 0.75 / 2, 8.0 - 0.75 / 2, 0.05 + 2);
            drawLamp(1.4, .75);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(6.5, 7.5, 7.0 + 0.05 - 0.5);
            color = {White};
            drawCube(0.4,0.4,0.5, color, lightUp);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(0.75, 5.0 - 0.2, 7.0 - 0.75 - 0.2);
            color = {White};
            drawCube(0.5,0.4,0.4, color, false);
        glPopMatrix();

    glPopMatrix();

    glFlush();
    glutSwapBuffers();
}

void movewitharrow( int key, int x, int y )
{
	switch ( key )
	{
		case GLUT_KEY_DOWN:
			EYE.Move(LOOK, -step);
			break;
		case GLUT_KEY_UP:
			EYE.Move(LOOK, step);
			break;
		case GLUT_KEY_RIGHT:
			EYE.Move(RIGHT, step);
			break;
		case GLUT_KEY_LEFT:
			EYE.Move(RIGHT, -step);
			break;
		case GLUT_KEY_PAGE_UP:
			EYE.Move(UP, step);
			break;
		case GLUT_KEY_PAGE_DOWN:
			EYE.Move(UP, -step);
			break;

		case GLUT_KEY_END:	// Escape key
			exit(1);
	}

	glutPostRedisplay();
}

void myKeyboardFunc( unsigned char key, int x, int y )
{
	switch ( key )
	{
        case 'a':
            LOOK.Rotate(UP, -stepAngle);
			RIGHT.Rotate(UP, -stepAngle);
            break;
        case 's':
            LOOK.Rotate(UP, stepAngle);
			RIGHT.Rotate(UP, stepAngle);
            break;
        case 'w':
            LOOK.Rotate(RIGHT, -stepAngle);
			UP.Rotate(RIGHT, -stepAngle);
            break;
        case 'z':
            LOOK.Rotate(RIGHT, stepAngle);
			UP.Rotate(RIGHT, stepAngle);
            break;
        case 'q':
            UP.Rotate(LOOK, stepAngle);
			RIGHT.Rotate(LOOK, stepAngle);
			break;
        case 'x':
            UP.Rotate(LOOK, -stepAngle);
			RIGHT.Rotate(LOOK, -stepAngle);
        case 'o':
            for(int i = 0; i < 10; i++) OpenController[i] = !OpenController[i];
            break;
        case 'l':
            lightLeft = !lightLeft;
            break;
        case 'u':
            lightUp = !lightUp;
            break;
        case 't':
            spotLight = !spotLight;
            break;
        case 'f':
            if(ds < 60) ds += 1;
            break;
        case 'c':
            if(ds > 0) ds -= 1;
            break;
        case '0':
            amb = !amb;
            break;
        case '9':
            diff = !diff;
            break;
        case '8':
            spec = !spec;
            break;
		case 27:	// Escape key
			exit(1);
	}
	glutPostRedisplay();
}


int main (int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowPosition(300,80);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("HomeTask - 3");

    cout << "Manual: " << endl << endl;
    cout << "1 : Turn on/off left light" << endl;
    cout << "u : Turn on/off up light" << endl;
    cout << "t : Turn on/off spotlight" << endl;
    cout << "0 : Select / Unselect ambient light" << endl;
    cout << "9 : Select / Unselect diffuse light" << endl;
    cout << "8 : Select / Unselect specular light" << endl;

    cout << "Up Arrow : Move forward" << endl;
    cout << "Down Arrow : Move backward" << endl;
    cout << "Left Arrow : Move Left" << endl;
    cout << "Right Arrow : Move right" << endl;
    cout << "Page down : Move down" << endl;
    cout << "Page up : Move up" << endl;
    cout << "s : rotate right" << endl;
    cout << "a : rotate left" << endl;
    cout << "w : rotate up" << endl;
    cout << "z : rotate down" << endl;
    cout << "x : tilt clockwise" << endl;
    cout << "q : tilt counter clockwise" << endl;
    cout << "o : Open drawers" << endl;
    cout << "f : Increase Fan speed" << endl;
    cout << "c : Decrease Fan speed" << endl;

    glShadeModel( GL_SMOOTH );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_NORMALIZE );
    glEnable( GL_LIGHTING );
    //light();

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(myKeyboardFunc);
    glutSpecialFunc(movewitharrow);
    glutIdleFunc(animate);

    glutMainLoop();

    return 0;
}
