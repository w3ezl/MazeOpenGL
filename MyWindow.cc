#include <conio.h>
#include <stdio.h>
#include <cmath>

static constexpr double Pi = acos(-1.);

#include "MyWindow.h"

MyWindow::MyWindow(int width, int height)
: Window(width, height)
{
	_angle = 0.;
	_eye_level = 0.;
}

float brick[] = {0,0,0, 0,1,0, 1,1,0, 1,0,0, 0,0,1, 0,1,1, 1,1,1, 1,0,1};
GLuint brickInd[] = {0,1,2, 2,3,0, 4,5,6, 6,7,4, 3,6,2, 6,7,3,
0,1,5, 5,4,0, 1,2,6, 6,5,1, 0,3,7, 7,4,0};

struct {
    float x,y,z;
    float Xrot,Zrot;
} camera = {2.5,2.5,1.7,70,-40};

void Camera_Rotation(float xAngle, float zAngle)
{
    camera.Zrot += zAngle;
    if (camera.Zrot < 0) camera.Zrot += 360;
    if (camera.Zrot > 360) camera.Zrot -= 360;
    camera.Xrot += xAngle;
    if (camera.Xrot < 0) camera.Zrot = 0;
    if (camera.Xrot > 360) camera.Zrot = 180;
}

typedef struct {
    float r,g,b;
} TColor;

typedef struct {
    TColor clr;
} TCell;

TCell map[21][21];

void Map_Init()
{
    for (int i = 0; i < 21; i++)
        for (int j = 0; j < 21; j++)
        {
        	if (i == 2 && j == 2) {
        		map[i][j].clr.b = 0.4;
                map[i][j].clr.g = 0.1;
                map[i][j].clr.r = 0.1;
        	}
        	else if ((i == 18 && j == 18) || (i == 18 && j == 19) || (i == 19 && j == 19) || (i == 19 && j == 18)) {
                map[i][j].clr.b = 0.1;
                map[i][j].clr.g = 0.1;
                map[i][j].clr.r = 0.4;
        	}
        	else {
			float dc = (rand() % 20) * 0.002;
			map[i][j].clr.r = 0.85+dc;
			map[i][j].clr.g = 0.72+dc;
			map[i][j].clr.b = 0.47+dc;
			}
        }
}

void MyWindow::setup()
{
	Map_Init();
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.6, 0.8, 1, 0);
	glMatrixMode(GL_PROJECTION);
	gluPerspective(45., double(width()) / double(height()), 0.01, 20.0);
	glMatrixMode(GL_MODELVIEW);
}

void MyWindow::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glRotatef(-camera.Xrot, 1,0,0);
	glRotatef(-camera.Zrot, 0,0,1);
	glTranslatef(-camera.x, -camera.y, -camera.z);
	    glPushMatrix();
	        glEnableClientState(GL_VERTEX_ARRAY);
	        glVertexPointer(3, GL_FLOAT, 0, brick);
	        for (int i=0; i<21; i++)
	            for (int j=0; j<21; j++)
	        {
	            glPushMatrix();
	                glTranslatef(i,j,0);
	                glColor3f(map[i][j].clr.r, map[i][j].clr.g, map[i][j].clr.b);
	                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, brickInd);
	            glPopMatrix();
	        }
	        for (int i=0; i<21; i++)
	            for (int z=0; z<3; z++)
	            {
	                glPushMatrix();
	                    glTranslatef(i,0,z);
	                    glColor3f(0.8, 0.8, 0.6);
	                    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, brickInd);
	                glPopMatrix();
	            }
	        for (int i=0; i<21; i++)
	            for (int z=0; z<3; z++)
	            {
	                glPushMatrix();
	                    glTranslatef(0,i,z);
	                    glColor3f(0.8, 0.8, 0.6);
	                    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, brickInd);
	                glPopMatrix();
	            }
	        for (int i=0; i<21; i++)
	            for (int z=0; z<3; z++)
	            {
	                glPushMatrix();
	                    glTranslatef(i,21-1,z);
	                    glColor3f(0.8, 0.8, 0.6);
	                    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, brickInd);
	                glPopMatrix();
	            }
	        for (int i=0; i<21; i++)
	            for (int z=0; z<3; z++)
	            {
	                glPushMatrix();
	                    glTranslatef(21-1,i,z);
	                    glColor3f(0.8, 0.8, 0.6);
	                    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, brickInd);
	                glPopMatrix();
	            }

	        for (int j=4; j<21; j+=4){
	                int startWall, endWall;
	                if (j==4) {startWall = 1; endWall = 15;}
	                if (j==8) {startWall = 4; endWall = 20;}
	                if (j==12) {startWall = 1; endWall = 15;}
	                if (j==16) {startWall = 4; endWall = 20;}
	            for (int i=startWall; i<endWall; i++)
	                for (int z=0; z<3; z++)
	                {
	                    glPushMatrix();
	                        glTranslatef(i,j,z);
	                        glColor3f(0.8, 0.8, 0.6);
	                        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, brickInd);
	                    glPopMatrix();
	                }
	            }

	        glDisableClientState(GL_VERTEX_ARRAY);
	    glPopMatrix();
}
void MyWindow::handle_logic()
{
	_angle += 1.;
	if (_angle >= 360.)
		_angle -= 360.;

	_eye_level = sin(_angle / 180. * Pi);
}
void MyWindow::handle_keys(const Uint8 * keys)
{
	 float ugol = -camera.Zrot / 180 * Pi;
	    float speed = 0;
	    if (GetKeyState('W') < 0) speed = 0.1;
	    if (GetKeyState('S') < 0) speed = -0.1;
	    if (GetKeyState('A') < 0) {speed = 0.1; ugol -= Pi*0.5;}
	    if (GetKeyState('D') < 0) {speed = 0.1; ugol += Pi*0.5;}
	    if (camera.x < 1 || camera.y < 1)
	    {
	            speed = 0;
	            camera.x = camera.x + 0.1;
	            camera.y = camera.y + 0.1;
	    }
	    if (camera.x > 21-1 || camera.y > 21-1)
	    {
	            speed = 0;
	            camera.x = camera.x - 0.1;
	            camera.y = camera.y - 0.1;
	    }
	    if (camera.y > 3.9 && camera.y < 5.1){
	        if (camera.x < 15){
	            speed = 0;
	            if (camera.y > 5) camera.y += 0.1;
	            if (camera.y < 4) camera.y -= 0.1;
	        }
	    }
	    if (camera.y > 7.9 && camera.y < 9.1){
	        if (camera.x > 4){
	            speed = 0;
	            if (camera.y > 9) camera.y += 0.1;
	            if (camera.y < 8) camera.y -= 0.1;
	        }
	    }
	    if (camera.y > 11.9 && camera.y < 13.1){
	        if (camera.x < 15){
	            speed = 0;
	            if (camera.y > 13) camera.y += 0.1;
	            if (camera.y < 12) camera.y -= 0.1;
	        }
	    }
	    if (camera.y > 15.9 && camera.y < 17.1){
	        if (camera.x > 4){
	            speed = 0;
	            if (camera.y > 17) camera.y += 0.1;
	            if (camera.y < 16) camera.y -= 0.1;
	        }
	    }

	    if (speed != 0)
	    {
	        camera.x += sin(ugol)*speed;
	        camera.y += cos(ugol)*speed;
	    }

	    POINT cur;
	    static POINT base = {400, 300};
	    GetCursorPos(&cur);
	    Camera_Rotation((base.y - cur.y)/5.0, (base.x - cur.x)/5.0);
	    SetCursorPos(base.x, base.y);
}
