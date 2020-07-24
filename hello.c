#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#define PI 3.141592653589793238
#define P2 PI/2
#define P3 3 * PI/2
#define DR 0.0174533

float playerX, playerY, playerDeltaX, playerDeltaY, playerAngle; //player shit

void drawPlayer()
{
	glColor3f(1, 1, 0);
	glPointSize(8);
	glBegin(GL_POINTS);
	glVertex2i(playerX, playerY);
	glEnd();
	
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2i(playerX, playerY);
	glVertex2i(playerX + playerDeltaX * 5, playerY + playerDeltaY * 5);
	glEnd();
}

int mapX = 8, mapY = 8, mapS = 64;

int map[] = 
{
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 1, 0, 0, 0, 1,
	1, 0, 0, 1, 0, 0, 0, 1,
	1, 0, 1, 1, 1, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
};

void drawMap2D()
{
	int x, y, xo, yo;
	for(y = 0; y < mapY; y++)
	{
		for(x = 0; x < mapX; x++)
		{
			if (map[y * mapX + x] == 1) { glColor3f(1, 1, 1); } else { glColor3f(0, 0, 0); }
			xo = x * mapS; yo = y * mapS;
			glBegin(GL_QUADS);
			glVertex2i(xo        + 1, yo        + 1);
			glVertex2i(xo        + 1, yo + mapS - 1);
			glVertex2i(xo + mapS - 1, yo + mapS - 1);
			glVertex2i(xo + mapS - 1, yo        + 1);
			glEnd();
		}
	}
}

float dist(float angleX, float angleY, float bX, float bY, float angle)
{
	return( sqrt((bX - angleX) * (bX - angleX) + (bY - angleY) * (bY - angleY)));
}

void drawRays3D()
{
	int r, mX, mY, mP, dof; float rayX, rayY, rayAngle, xOffset, yOffset, disT;
	
	rayAngle = playerAngle;
	
	rayAngle = playerAngle - DR * 30;
	
	if (rayAngle < 0) {
		rayAngle += 2*PI;
	}
	
	if (rayAngle > 2 * PI) {
		rayAngle -= 2 * PI;
	}
	
	for(r = 0; r < 60; r++)
	{
		/*  Check horizontal lines  */
		
		dof = 0;
		
		float distH = 100000000,hX=playerX, hY = playerY;
		
		float aTan = -1/tan(rayAngle);
		
		if (rayAngle > PI) { rayY = (((int) playerY >> 6) <<6) - 0.0001; rayX = (playerY - rayY) * aTan + playerX; yOffset = -64; xOffset =- yOffset * aTan; } // if looking up
		if (rayAngle < PI) { rayY = (((int) playerY >> 6) <<6) + 64    ; rayX = (playerY - rayY) * aTan + playerX; yOffset =  64; xOffset =- yOffset * aTan; } // if looking down
		if (rayAngle == 0 || rayAngle == PI) { rayX = playerX; rayY = playerY; dof = 8; } // looking straight left/right
		while (dof < 8) 
		{
			mX = (int) (rayX) >> 6; mY = (int) (rayY) >> 6; mP = mY * mapX + mX;
			if (mP > 0 && mP < mapX * mapY && map[mP] == 1) { hX = rayX; hY = rayY; distH = dist(playerX, playerY, hX, hY, rayAngle); dof = 8; } // colission
			else { rayX += xOffset; rayY += yOffset; dof += 1; }
		}
		
		/*  Check vertical lines  */
		
		dof = 0;
		
		float distV = 100000000,vX=playerX, vY = playerY;
		
		float nTan = -tan(rayAngle);
		
		if (rayAngle > P2 && rayAngle < P3) { 
		rayX = (((int) playerX >> 6) <<6) - 0.0001; 
		rayY = (playerX - rayX) * nTan + playerY; 
		xOffset = -64; 
		yOffset =- xOffset * nTan; } // if looking left
		
		if (rayAngle < P2 || rayAngle > P3) { 
		rayX = (((int) playerX >> 6) <<6) + 64    ; 
		rayY = (playerX - rayX) * nTan + playerX; 
		xOffset =  64; 
		yOffset =- xOffset * nTan; 
		} // if looking right
		
		if (rayAngle == 0 || rayAngle == PI) { rayX = playerX; rayY = playerY; dof = 8; } // looking straight left/right
		while (dof < 8) 
		{
			mX = (int) (rayX) >> 6; mY = (int) (rayY) >> 6; mP = mY * mapX + mX;
			if (mP > 0 && mP < mapX * mapY && map[mP] == 1) { vX = rayX; vY = rayY; distV = dist(playerX, playerY, vX, vY, rayAngle); dof = 8; } // colission
			else { rayX += xOffset; rayY += yOffset; dof += 1; }
		}
		
		if (distV < distH) {
			rayX = vX; rayY = vY;
			disT = distV;
			glColor3f(0.3, 0, 0);
		}
		if (distH < distV) {
			rayX = hX; rayY = hY;
			disT = distH;
			glColor3f(0.7, 0, 0);
		}
		
		glLineWidth(1); glBegin(GL_LINES); glVertex2i(playerX, playerY); glVertex2i(rayX, rayY); glEnd();
		
		// draw 3D walls
		
		float ca = playerAngle - rayAngle;
		
		if (ca < 0) {
			ca += 2*PI;
		}
	
		if (ca > 2 * PI) {
			ca -= 2 * PI;
		}
		
		disT = disT * cos(ca);
		
		float lineH = (mapS * 320)/disT;
		
		if (lineH > 320) {
			lineH = 320;
		}
		
		float lineO = 160 - lineH/2;
		
		glLineWidth(8);
		glBegin(GL_LINES);
		glVertex2i(r * 8 + 530, lineO);
		glVertex2i(r * 8 + 530, lineH + lineO);
		glEnd();
		
		rayAngle += DR;
		
		if (rayAngle < 0) {
			rayAngle += 2*PI;
		}
	
		if (rayAngle > 2 * PI) {
			rayAngle -= 2 * PI;
		}
	}
}

void display()
{
 glClear(GL_COLOR_BUFFER_BIT);
 drawMap2D();
 drawRays3D();
 drawPlayer();
 glutSwapBuffers();
};

void buttons(unsigned char key, int x, int y)
{
	if (key == 'a') { playerAngle -= 0.1; if(playerAngle < 0) { playerAngle += 2 * PI; } playerDeltaX = cos(playerAngle) * 5; playerDeltaY = sin(playerAngle) * 5; }
	if (key == 'd') { playerAngle += 0.1; if(playerAngle > 2 * PI) { playerAngle -= 2 * PI; } playerDeltaX = cos(playerAngle) * 5; playerDeltaY = sin(playerAngle) * 5; }
	if (key == 'w') { playerX += playerDeltaX; playerY += playerDeltaY; }
	if (key == 's') { playerX -= playerDeltaX; playerY -= playerDeltaY; }
	glutPostRedisplay();
}

void init()
{
	glClearColor(0.3, 0.3, 0.3,0);
	gluOrtho2D(0, 1024, 512, 0);
	playerX = 300; playerY = 300; playerDeltaX = cos(playerAngle) * 5; playerDeltaY = sin(playerAngle) * 5;
}

int main(int argc, char** argv)
{ 
 glutInit(&argc, argv);
 glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
 glutInitWindowSize(1024,512);
 glutCreateWindow("Ray engine 9000");
 init();
 glutDisplayFunc(display);
 glutKeyboardFunc(buttons);
 glutMainLoop();
}
