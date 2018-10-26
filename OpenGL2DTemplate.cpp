//#include "TextureBuilder.h"
#include <math.h>
#include <random>
#include <glut.h>
#include <string> 
#include <stdio.h>
#include <cstdlib>
#include <Windows.h>
#pragma comment(lib, "winmm.lib")

GLuint tex;

#define GLUT_KEY_ESCAPE 27

void drawCircle(int x, int y, float r, int c);
void KeyUp(unsigned char key, int x, int y);
void EnemyAttack(int value);
void Display();
void Special(int key, int x, int y);
void SpecialUp(int key, int x, int y);
void drawPlayer();
void drawLasers();
void drawRockets();
void drawEnemy();
void drawEnemyLasers();
void drawHealthbar();
void EnemySpawn(int value);
void print(int x, int y, char* string);
void drawScore();
void moveEnemy();
void drawDefender();
void spawnDefender(int value);
void drawDefenderLasers();
void DefenderAttack(int value);
void spawnPowerup(int value);
void drawPowerUps();
void drawPowerUpCircle(int type);
void printPowerUp(int x, int y, char *string);
void printRocketsAvailable();
void generateStar(int index);
void drawStars();
void drawFlyBy();
void spawnFlyBy(int value);

bool playerAlive = true;
int playerRotate = 0;
int playerX = 800;
int playerY = 70;
int playerLaserDmg = 2;
int playerRocketDmg = 10;
bool altWeapon = false;
int laserOn[100];
int laserX[100];
int laserY[100];
int rocketOn[100];
int rocketX[100];
int rocketY[100];
int laserCount = 0;
int rocketCount = 0;
int rocketsAvailable = 20;

bool enemyAlive = false;
int enemyFullHp = 25;
int enemyHp;
int enemyRotate = 0;
int enemyX = 800;
int enemyY = 830;
int enemyLaserX[100];
int enemyLaserY[100];
int enemyLaserOn[100];
int enemyLaserCount = 0;
float bezierCount = 0.0;
int p0[2];
int p1[2];
int p2[2];
int p3[2];

bool defAlive = false;
int defRotate = 0;
int defX = -20;
int defY = 600;
int defLaserX[100];
int defLaserY[100];
int defLaserOn[100];
int defLaserCount = 0;

int powerUpX[100];
int powerUpY[100];
int powerUpType[100];
int powerUpCount = 0;
int powerUpOn[100];

int starX[25];
int starY[25];
int starSize[25];
bool start = true;

bool flyBy = false;
int flyByX = 0;
int flyByY = 0;
bool flyByUp = false;

int score = 0;

bool directionDown[2];

int* bezier(float t, int* p0, int* p1, int* p2, int* p3)
{
	int res[2];
	res[0] = pow((1 - t), 3)*p0[0] + 3 * t*pow((1 - t), 2)*p1[0] + 3 * pow(t, 2)*(1 - t)*p2[0] + pow(t, 3)*p3[0];
	res[1] = pow((1 - t), 3)*p0[1] + 3 * t*pow((1 - t), 2)*p1[1] + 3 * pow(t, 2)*(1 - t)*p2[1] + pow(t, 3)*p3[1];
	return res;
}

void generateRandoms() {
	p0[0] = rand() % 1550 + 25;
	p1[0] = rand() % 1550 + 25;
	p2[0] = rand() % 1550 + 25;
	p3[0] = rand() % 1550 + 25;

	p0[1] = rand() % 400 + 525;
	p1[1] = rand() % 400 + 525;
	p2[1] = rand() % 400 + 525;
	p3[1] = rand() % 400 + 525;
}

void moveEnemy() {
	int* res = bezier(bezierCount, p0, p1, p2, p3);
	if (res[0] < enemyX) {
		enemyX -= 4;
		enemyRotate += 2;
	}
	else if (res[0] > enemyX) {
		enemyX += 4;
		enemyRotate -= 2;
	}
	if (res[1] < enemyY) {
		enemyY -= 4;
	}
	else if (res[1] > enemyY) {
		enemyY += 4;
	}
	bezierCount += 0.001;
	if (bezierCount >= 1.0) {
		generateRandoms();
		bezierCount = 0.0;
	}
}

void main(int argc, char** argr) {
	int time = (rand() % 1) * 1000;
	for (int i = 0; i < 25; i++) {
		generateStar(i);
	}
	start = false;
	glutInit(&argc, argr);

	glutInitWindowSize(1600, 900);
	glutInitWindowPosition(0, 0);
	generateRandoms();

	glutCreateWindow("Star Wars: Attack of the Death Star");
	glutDisplayFunc(Display);
	glutSpecialFunc(Special);
	glutSpecialUpFunc(SpecialUp);
	glutKeyboardUpFunc(KeyUp);
	glutTimerFunc(5000, EnemySpawn, 0);
	glutTimerFunc(10000, spawnDefender, 0);
	glutTimerFunc(time, spawnPowerup, 0);
	glutTimerFunc(10000, spawnFlyBy, 0);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glClearColor(1, 1, 1, 0);
	gluOrtho2D(0, 1600, 0, 900);

	glutMainLoop();
}

void Display() {

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glGenTextures(1, &tex);
	drawStars();
	if (playerAlive)
		drawPlayer();
	drawLasers();
	drawRockets();
	if (enemyAlive)
		drawEnemy();
	drawEnemyLasers();
	drawHealthbar();
	drawScore();
	if (defAlive || (!defAlive && (defX < 1600 && defX > 0))) {
		drawDefender();
	}
	drawDefenderLasers();
	drawPowerUps();
	drawFlyBy();
	printRocketsAvailable();
	glFlush();
	if (directionDown[0] && playerX > 60) {
		playerX -= 2;
		if (playerRotate <= 20)
			playerRotate += 1;
	}
	else if (directionDown[1] && playerX < 1540) {
		playerX += 2;
		if (playerRotate >= -20)
			playerRotate -= 1;
	}
	moveEnemy();
	glutPostRedisplay();
}

void drawCircle(int x, int y, float r, int c) {
	glPushMatrix();
	if (c == 0) {
		glColor3f(0.9, 0.9, 0.9);
	}
	else {
		glColor3f(0.525, 0.64, 0.756);
	}
	GLUquadric *quadObj = gluNewQuadric();
	gluDisk(quadObj, 0, r, 50, 50);
}

void EnemyAttack(int value) {
	if (enemyAlive && playerAlive) {
		int laser = 0;
		for (int i = 0; i < enemyLaserCount; i += 3)
		{
			if (enemyLaserOn[i] == 0)
			{
				enemyLaserX[i] = (enemyX - 60);
				enemyLaserX[i + 1] = (enemyX + 60);
				enemyLaserX[i + 2] = (enemyX);
				enemyLaserY[i] = (enemyY);
				enemyLaserY[i + 1] = (enemyY);
				enemyLaserY[i + 2] = (enemyY);
				enemyLaserOn[i] = 1;
				enemyLaserOn[i + 1] = 1;
				enemyLaserOn[i + 2] = 1;
				laser = 1;
				break;
			}
		}
		if (laser == 0)
		{
			enemyLaserX[enemyLaserCount] = (enemyX - 45);
			enemyLaserX[enemyLaserCount + 1] = (enemyX + 45);
			enemyLaserX[enemyLaserCount + 2] = (enemyX);
			enemyLaserY[enemyLaserCount] = (enemyY);
			enemyLaserY[enemyLaserCount + 1] = (enemyY);
			enemyLaserY[enemyLaserCount + 2] = (enemyY);
			enemyLaserOn[enemyLaserCount] = 1;
			enemyLaserOn[enemyLaserCount + 1] = 1;
			enemyLaserOn[enemyLaserCount + 2] = 1;
			enemyLaserCount += 3;
		}
		laser = 0;
		PlaySoundA("enemyL.WAV", NULL, SND_ASYNC | SND_FILENAME);
		glutTimerFunc(1000, EnemyAttack, 0);
	}
}

void KeyUp(unsigned char key, int x, int y) {
	if (key == ' ' && !altWeapon && playerAlive) {
		int laser = 0;
		for (int i = 0; i<laserCount; i += 2)
		{
			if (laserOn[i] == 0)
			{
				laserX[i] = (playerX - 45);
				laserX[i + 1] = (playerX + 45);
				laserY[i] = (playerY);
				laserY[i + 1] = (playerY);
				laserOn[i] = 1;
				laserOn[i + 1] = 1;
				laser = 1;
				break;
			}
		}
		if (laser == 0)
		{
			laserX[laserCount] = (playerX - 45);
			laserX[laserCount + 1] = (playerX + 45);
			laserY[laserCount] = (playerY);
			laserY[laserCount + 1] = (playerY);
			laserOn[laserCount] = 1;
			laserOn[laserCount + 1] = 1;
			laserCount += 2;
		}
		PlaySoundA("Xwing.wav", NULL, SND_ASYNC | SND_FILENAME);
		laser = 0;
	}
	else if (key == 'c') {
		if (altWeapon) {
			altWeapon = false;
		}
		else {
			altWeapon = true;
		}
	}
	else if (key == ' ' && altWeapon && rocketsAvailable > 0 && playerAlive) {
		int rocket = 0;
		for (int i = 0; i<rocketCount; i += 1)
		{
			if (rocketOn[i] == 0)
			{
				rocketX[i] = (playerX);
				rocketY[i] = (playerY);
				rocketOn[i] = 1;
				rocket = 1;
				break;
			}
		}
		if (rocket == 0)
		{
			rocketX[rocketCount] = (playerX);
			rocketY[rocketCount] = (playerY);
			rocketOn[rocketCount] = 1;
			rocketCount += 1;
		}
		rocket = 0;
		rocketsAvailable -= 1;
	}
	if (key == GLUT_KEY_ESCAPE)
		exit(EXIT_SUCCESS);
}

void Special(int key, int x, int y) {
	switch (key)
	{
	case GLUT_KEY_LEFT:
		if (playerX > 60) {
			playerX -= 2;
			directionDown[0] = true;
			directionDown[1] = false;
			if (playerRotate <= 20)
				playerRotate += 1;
		}
		break;
	case GLUT_KEY_RIGHT:
		if (playerX < 1540) {
			playerX += 2;
			directionDown[0] = false;
			directionDown[1] = true;
			if (playerRotate >= -20)
				playerRotate -= 1;
		}
		break;
	}
}

void SpecialUp(int key, int x, int y) {
	directionDown[0] = false;
	directionDown[1] = false;
	playerRotate = 0;
}

void drawPlayer() {

	glPushMatrix();															//1
	glTranslatef(playerX, playerY, 0);
	glRotatef(playerRotate, 0, 0, 1);
	glColor3f(0.74, 0.74, 0.56);
	glBegin(GL_POLYGON);
	glVertex3f(-50, -40, 0);
	glVertex3f(0, -50, 0);
	glVertex3f(50, -40, 0);
	glVertex3f(50, -20, 0);
	glVertex3f(0, -20, 0);
	glVertex3f(-50, -20, 0);
	glEnd();

	glPushMatrix();															//2
	glColor3f(0.69, 0.69, 0.563);
	glBegin(GL_POLYGON);
	glVertex3f(40, 20, 0);
	glVertex3f(40, -45, 0);
	glVertex3f(50, -45, 0);
	glVertex3f(50, 20, 0);
	glEnd();

	glPushMatrix();															//3
	glColor3f(0.69, 0.69, 0.563);
	glBegin(GL_POLYGON);
	glVertex3f(-40, 20, 0);
	glVertex3f(-40, -45, 0);
	glVertex3f(-50, -45, 0);
	glVertex3f(-50, 20, 0);
	glEnd();

	glPushMatrix();															//4
	glColor3f(0.69, 0.69, 0.563);
	glBegin(GL_POLYGON);
	glVertex3f(-17, -16, 0);
	glVertex3f(-17, -60, 0);
	glVertex3f(-12, -60, 0);
	glVertex3f(-12, -16, 0);
	glEnd();

	glPushMatrix();															//5
	glColor3f(0.69, 0.69, 0.563);
	glBegin(GL_POLYGON);
	glVertex3f(17, -16, 0);
	glVertex3f(17, -60, 0);
	glVertex3f(12, -60, 0);
	glVertex3f(12, -16, 0);
	glEnd();

	glPushMatrix();															//6
	glColor3f(0.938, 0.937, 0.69);
	glBegin(GL_POLYGON);
	glVertex3f(-5, 60, 0);
	glVertex3f(5, 60, 0);
	glVertex3f(10, -60, 0);
	glVertex3f(-10, -60, 0);
	glEnd();

	glPushMatrix();															//7
	glColor3f(0.69, 0.69, 0.563);
	glBegin(GL_TRIANGLES);
	glVertex3f(-10, 55, 0);
	glVertex3f(10, 55, 0);
	glVertex3f(0, 70, 0);
	glEnd();

	glPushMatrix();															//8
	glColor3f(0.3125, 0.4414, 0.6445);
	glBegin(GL_POLYGON);
	glVertex3f(-3, 10, 0);
	glVertex3f(3, 10, 0);
	glVertex3f(6, -10, 0);
	glVertex3f(-6, -10, 0);
	glEnd();

	glPushMatrix();															//10
	glColor3f(1, 0.4, 0.4);
	glBegin(GL_POLYGON);
	glVertex3f(35, -20, 0);
	glVertex3f(25, -20, 0);
	glVertex3f(25, -30, 0);
	glVertex3f(20, -35, 0);
	glVertex3f(20, -46, 0);
	glVertex3f(25, -44, 0);
	glVertex3f(25, -40, 0);
	glVertex3f(35, -35, 0);
	glEnd();

	glPushMatrix();															//10
	glColor3f(1, 0.4, 0.4);
	glBegin(GL_POLYGON);
	glVertex3f(-35, -20, 0);
	glVertex3f(-25, -20, 0);
	glVertex3f(-25, -30, 0);
	glVertex3f(-20, -35, 0);
	glVertex3f(-20, -46, 0);
	glVertex3f(-25, -44, 0);
	glVertex3f(-25, -40, 0);
	glVertex3f(-35, -35, 0);
	glEnd();

	glPushMatrix();															//11
	glColor3f(1, 0.4, 0.4);
	glBegin(GL_POLYGON);
	glVertex3f(-2, 60, 0);
	glVertex3f(2, 60, 0);
	glVertex3f(2, 20, 0);
	glVertex3f(-2, 20, 0);
	glEnd();

	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
}

void drawLasers() {
	for (int i = 0; i < laserCount; i++) {
		if (laserOn[i] == 1) {
			glPushMatrix();
			glColor3f(1, 0.2, 0.2);
			glTranslatef(laserX[i], laserY[i], 0);
			glBegin(GL_QUADS);
			glVertex3f(0, 0, 0);
			glVertex3f(0, 8, 0);
			glVertex3f(2, 8, 0);
			glVertex3f(2, 0, 0);
			glEnd();
			glPopMatrix();
			laserY[i] += 3;
			if (laserY[i] >= 900) {
				laserOn[i] = 0;
			}
			else if (laserX[i] <= (enemyX + 100) && laserX[i] >= (enemyX - 100) && laserY[i] <= (enemyY + 20) && laserY[i] >= (enemyY - 20) && enemyAlive) {
				if (enemyHp < playerLaserDmg) {
					score += enemyHp;
					enemyHp = 0;
				}
				else {
					enemyHp -= playerLaserDmg;
					score += playerLaserDmg;
				}
				laserOn[i] = 0;
				if (enemyHp <= 0) {
					enemyAlive = false;
					glutTimerFunc(5000, EnemySpawn, 0);
				}
			}
			else if (laserX[i] <= (defX + 20) && laserX[i] >= (defX - 20) && laserY[i] <= (defY + 20) && laserY[i] >= (defY - 20) && defAlive) {
				laserOn[i] = 0;
			}
		}
	}
}

void drawRockets() {
	for (int i = 0; i < rocketCount; i++) {
		if (rocketOn[i] == 1) {
			glPushMatrix();																			//1
			glColor3f(1, 1, 1);
			glTranslatef(rocketX[i], rocketY[i], 0);
			glScalef(1.5, 1.5, 1);
			glBegin(GL_QUADS);
			glVertex3f(0, 0, 0);
			glVertex3f(0, 10, 0);
			glVertex3f(4, 10, 0);
			glVertex3f(4, 0, 0);
			glEnd();

			glPushMatrix();
			glColor3f(1, 0, 0);
			glBegin(GL_POLYGON);
			glVertex3f(0, 0, 0);
			glVertex3f(4, 0, 0);
			glVertex3f(6, -3, 0);
			glVertex3f(-2, -3, 0);
			glEnd();

			glPushMatrix();
			glColor3f(0.5, 0.5, 0.5);
			glBegin(GL_TRIANGLES);
			glVertex3f(0, 10, 0);
			glVertex3f(4, 10, 0);
			glVertex3f(2, 13, 0);
			glEnd();

			glPopMatrix();
			glPopMatrix();
			glPopMatrix();

			rocketY[i] += 3;
			if (rocketX[i] > enemyX  && enemyAlive) {
				rocketX[i] -= 2;
			}
			else if (rocketX[i] < enemyX  && enemyAlive) {
				rocketX[i] += 2;
			}
			if (rocketY[i] >= 900) {
				rocketOn[i] = 0;
			}
			else if (rocketX[i] <= (enemyX + 100) && rocketX[i] >= (enemyX - 100) && rocketY[i] <= (enemyY + 20) && rocketY[i] >= (enemyY - 20) && enemyAlive) {
				enemyHp -= playerRocketDmg;
				score += playerRocketDmg;
				rocketOn[i] = 0;
				if (enemyHp <= 0) {
					enemyAlive = false;
					glutTimerFunc(5000, EnemySpawn, 0);
				}
			}
		}
	}
}

void drawEnemy() {

	glPushMatrix();																//1
	glTranslatef(enemyX, enemyY, 0);
	glRotatef(enemyRotate, 0, 0, 1);
	glColor3f(0.8, 0.8, 0.8);
	glBegin(GL_POLYGON);
	glVertex3f(-100, 10, 0);
	glVertex3f(-80, 40, 0);
	glVertex3f(-70, 35, 0);
	glVertex3f(-75, 10, 0);
	glVertex3f(-75, -10, 0);
	glVertex3f(-70, -35, 0);
	glVertex3f(-80, -40, 0);
	glVertex3f(-100, -10, 0);
	glEnd();

	glPushMatrix();																//2
	glColor3f(0.36, 0.36, 0.36);
	glBegin(GL_POLYGON);
	glVertex3f(-98, 11, 0);
	glVertex3f(-79, 38, 0);
	glVertex3f(-72, 33, 0);
	glVertex3f(-76, 11, 0);
	glEnd();

	glPushMatrix();																//3
	glColor3f(0.36, 0.36, 0.36);
	glBegin(GL_POLYGON);
	glVertex3f(-98, -11, 0);
	glVertex3f(-79, -38, 0);
	glVertex3f(-72, -33, 0);
	glVertex3f(-76, -11, 0);
	glEnd();

	glPushMatrix();																//4
	glColor3f(0.8, 0.8, 0.8);
	glBegin(GL_POLYGON);
	glVertex3f(100, 10, 0);
	glVertex3f(80, 40, 0);
	glVertex3f(70, 35, 0);
	glVertex3f(75, 10, 0);
	glVertex3f(75, -10, 0);
	glVertex3f(70, -35, 0);
	glVertex3f(80, -40, 0);
	glVertex3f(100, -10, 0);
	glEnd();

	glPushMatrix();																//5
	glColor3f(0.36, 0.36, 0.36);
	glBegin(GL_POLYGON);
	glVertex3f(98, 11, 0);
	glVertex3f(79, 38, 0);
	glVertex3f(72, 33, 0);
	glVertex3f(76, 11, 0);
	glEnd();

	glPushMatrix();																//6
	glColor3f(0.36, 0.36, 0.36);
	glBegin(GL_POLYGON);
	glVertex3f(98, -11, 0);
	glVertex3f(79, -38, 0);
	glVertex3f(72, -33, 0);
	glVertex3f(76, -11, 0);
	glEnd();

	glPushMatrix();																//7
	glColor3f(0.92, 0.92, 0.92);
	glBegin(GL_POLYGON);
	glVertex3f(-85, -10, 0);
	glVertex3f(0, -20, 0);
	glVertex3f(85, -10, 0);
	glVertex3f(85, 10, 0);
	glVertex3f(0, 20, 0);
	glVertex3f(-85, 10, 0);
	glEnd();

	glPushMatrix();																//8
	glColor3f(0.52, 0.52, 0.52);
	glBegin(GL_TRIANGLES);
	glVertex3f(-50, 5, 0);
	glVertex3f(-50, -5, 0);
	glVertex3f(-65, 0, 0);
	glEnd();

	glPushMatrix();																//9
	glColor3f(0.52, 0.52, 0.52);
	glBegin(GL_TRIANGLES);
	glVertex3f(50, 5, 0);
	glVertex3f(50, -5, 0);
	glVertex3f(65, 0, 0);
	glEnd();

	drawCircle(enemyX, enemyY, 30, 0);											//10
	drawCircle(enemyX, enemyY, 20, 1);											//11

	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
}

void drawEnemyLasers() {
	for (int i = 0; i < enemyLaserCount; i++) {
		if (enemyLaserOn[i] == 1) {
			glPushMatrix();
			glColor3f(0.2, 1, 0.2);
			glTranslatef(enemyLaserX[i], enemyLaserY[i], 0);
			glBegin(GL_QUADS);
			glVertex3f(0, 0, 0);
			glVertex3f(0, -8, 0);
			glVertex3f(2, -8, 0);
			glVertex3f(2, 0, 0);
			glEnd();
			glPopMatrix();
			enemyLaserY[i] -= 3;
			if (enemyLaserY[i] <= 0) {
				enemyLaserOn[i] = 0;
			}
			else if (enemyLaserX[i] <= (playerX + 50) && enemyLaserX[i] >= (playerX - 50) && enemyLaserY[i] <= (playerY + 20) && enemyLaserY[i] >= (playerY - 20)) {
				playerAlive = false;
				PlaySoundA("explosion.wav", NULL, SND_ASYNC | SND_FILENAME);
				enemyLaserOn[i] = 0;
			}
		}
	}
}

void drawHealthbar() {
	float eHP = (float)enemyHp;
	float rem = (eHP / enemyFullHp);
	float maxX = (rem * 100);
	glPushMatrix();
	glTranslatef(1480, 885, 0);
	glBegin(GL_QUADS);
	glVertex3f(0, -10, 0);
	glVertex3f(0, 10, 0);
	glVertex3f(maxX, 10, 0);
	glVertex3f(maxX, -10, 0);
	glEnd();
	glPopMatrix();
	print(1430, 876, "HP");
}

void print(int x, int y, char *string)
{
	int len, i;

	//set the position of the text in the window using the x and y coordinates
	glRasterPos2f(x, y);

	//get the length of the string to display
	len = (int)strlen(string);

	//loop to display character by character
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
}

void drawScore() {
	print(20, 870, "Score:");
	char str[80];
	sprintf(str, "%d", score);
	print(100, 870, str);
}

void EnemySpawn(int value) {
	enemyX = 800;
	enemyY = 830;
	enemyFullHp *= 2;
	enemyHp = enemyFullHp;
	enemyAlive = true;
	glutTimerFunc(1000, EnemyAttack, 0);
}

void drawDefender() {

	glPushMatrix();
	glTranslatef(defX, defY, 0);
	glRotatef(defRotate, 0, 0, 1);
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glVertex3f(-20, 30, 0);
	glVertex3f(-15, 30, 0);
	glVertex3f(-15, -30, 0);
	glVertex3f(-20, -30, 0);
	glEnd();

	glPushMatrix();
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glVertex3f(20, 30, 0);
	glVertex3f(15, 30, 0);
	glVertex3f(15, -30, 0);
	glVertex3f(20, -30, 0);
	glEnd();

	glPushMatrix();
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glVertex3f(20, 5, 0);
	glVertex3f(20, -5, 0);
	glVertex3f(-20, -5, 0);
	glVertex3f(-20, 5, 0);
	glEnd();

	drawCircle(defX, defY, 10, 0);
	drawCircle(defX, defY, 7, 1);

	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();

	if (defX < enemyX && defAlive) {
		defX += 4;
		defRotate -= 1;
	}
	else if (defX > enemyX && defAlive) {
		defX -= 4;
		defRotate += 1;
	}
	else if (!defAlive && defX > 900) {
		defX += 4;
	}
	else {
		defX -= 4;
	}
	if (defY < enemyY - 250) {
		defY += 4;
	}
	else if (defY > enemyY - 250) {
		defY -= 4;
	}
}

void spawnDefender(int value) {
	if (defAlive) {
		defAlive = false;
		int time = rand() % 10 * 1000;
		glutTimerFunc(time, spawnDefender, 0);
	}
	else {
		defAlive = true;
		defX = -20;
		defY = 600;
		defRotate = 0;
		int time = rand() % 10 * 1000;
		glutTimerFunc(time, spawnDefender, 0);
		glutTimerFunc(0, DefenderAttack, 0);
	}
}

void drawDefenderLasers() {
	for (int i = 0; i < defLaserCount; i++) {
		if (defLaserOn[i] == 1) {
			glPushMatrix();
			glColor3f(0.2, 1, 0.2);
			glTranslatef(defLaserX[i], defLaserY[i], 0);
			glBegin(GL_QUADS);
			glVertex3f(0, 0, 0);
			glVertex3f(0, -8, 0);
			glVertex3f(2, -8, 0);
			glVertex3f(2, 0, 0);
			glEnd();
			glPopMatrix();
			defLaserY[i] -= 3;
			if (defLaserY[i] <= 0) {
				defLaserOn[i] = 0;
			}
			else if (defLaserX[i] <= (playerX + 50) && defLaserX[i] >= (playerX - 50) && defLaserY[i] <= (playerY + 20) && defLaserY[i] >= (playerY - 20)) {
				playerAlive = false;
				PlaySoundA("explosion.wav", NULL, SND_ASYNC | SND_FILENAME);
			}
		}
	}
}

void DefenderAttack(int value) {
	if (defAlive && playerAlive) {
		int laser = 0;
		for (int i = 0; i < defLaserCount; i += 3)
		{
			if (defLaserOn[i] == 0)
			{
				defLaserX[i] = (defX);
				defLaserY[i] = (defY);
				defLaserOn[i] = 1;
				laser = 1;
				break;
			}
		}
		if (laser == 0)
		{
			defLaserX[defLaserCount] = (defX);
			defLaserY[defLaserCount] = (defY);
			defLaserOn[defLaserCount] = 1;
			defLaserCount += 1;
		}
		laser = 0;
		PlaySoundA("enemyL.WAV", NULL, SND_ASYNC | SND_FILENAME);
		glutTimerFunc(1000, DefenderAttack, 0);
	}
}

void spawnPowerup(int value) {
	int powerUp = rand() % 2;
	int x = rand() % 1400 + 100;
	int y = 900;
	bool done = false;
	for (int i = 0; i < powerUpCount; i++) {
		if (powerUpOn[i] == 0) {
			powerUpX[i] = x;
			powerUpY[i] = y;
			powerUpType[i] = powerUp;
			powerUpOn[i] = 1;
			done = true;
		}
	}
	if (!done) {
		powerUpX[powerUpCount] = x;
		powerUpY[powerUpCount] = y;
		powerUpType[powerUpCount] = powerUp;
		powerUpOn[powerUpCount] = 1;
		powerUpCount++;
	}
	int time = (rand() % 10) * 1000;
	glutTimerFunc(time, spawnPowerup, 0);
}

void drawPowerUps() {
	for (int i = 0; i < powerUpCount; i++) {
		if (powerUpOn[i] == 1) {
			if (powerUpType[i] == 0) {
				glPushMatrix();
				glTranslatef(powerUpX[i], powerUpY[i], 0);
				drawPowerUpCircle(powerUpType[i]);
				glPopMatrix();
				glPopMatrix();
				glPushAttrib(GL_CURRENT_BIT);
				glColor3f(1, 0.2, 0.2);
				printPowerUp(powerUpX[i] - 15, powerUpY[i], "+DMG");
				glPopAttrib();
			}
			else {
				glPushMatrix();
				glTranslatef(powerUpX[i], powerUpY[i], 0);
				drawPowerUpCircle(powerUpType[i]);
				glPopMatrix();
				glPopMatrix();
				glPushAttrib(GL_CURRENT_BIT);
				glColor3f(1, 1, 1);
				printPowerUp(powerUpX[i] - 15, powerUpY[i], "+Rocket");
				glPopAttrib();
			}
			powerUpY[i]--;
			if (powerUpY[i] < 0) {
				powerUpOn[i] = 0;
			}
			else if (powerUpX[i] <= (playerX + 50) && powerUpX[i] >= (playerX - 50) && powerUpY[i] <= (playerY + 20) && powerUpY[i] >= (playerY - 20)) {
				if (powerUpType[i] == 0) {
					playerLaserDmg += 2;
					powerUpOn[i] = 0;
				}
				else {
					rocketsAvailable++;
					printf("%d \n", rocketsAvailable);
					powerUpOn[i] = 0;
				}
			}
		}
	}
}

void drawPowerUpCircle(int type) {
	glPushMatrix();
	if (type == 0) {
		glColor3f(0.4, 0.9, 0.4);
	}
	else {
		glColor3f(0.4, 0.4, 0.9);
	}
	GLUquadric *quadObj = gluNewQuadric();
	gluDisk(quadObj, 0, 17, 50, 50);
}

void printPowerUp(int x, int y, char *string)
{
	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, string[i]);
	}
}

void printRocketsAvailable() {
	print(20, 20, "Rockets:");
	char str[80];
	sprintf(str, "%d", rocketsAvailable);
	print(120, 20, str);
}

void generateStar(int index) {
	int x = (rand() % 1500) + 50;
	int size = rand() % 3;
	if (start) {
		int y = (rand() % 850) + 25;
		starY[index] = y;
	}
	else {
		starY[index] = 900;
	}
	starX[index] = x;
	starSize[index] = size;
}

void drawStars() {
	for (int i = 0; i < 25; i++) {
		glPushMatrix();
		glTranslatef(starX[i], starY[i], 0);
		glPointSize(starSize[i]);
		glColor3f(1, 1, 1);
		glBegin(GL_POINTS);
		glVertex3f(0, 0, 0);
		glEnd();
		glPopMatrix();
		starY[i]--;
		if (starY[i] <= 0) {
			generateStar(i);
		}
	}
}

void drawFlyBy() {
	glPushMatrix();
	glTranslatef(flyByX, flyByY, 0);
	glScalef(0.4, 0.4, 0);
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glVertex3f(-20, 30, 0);
	glVertex3f(-15, 30, 0);
	glVertex3f(-15, -30, 0);
	glVertex3f(-20, -30, 0);
	glEnd();

	glPushMatrix();
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glVertex3f(20, 30, 0);
	glVertex3f(15, 30, 0);
	glVertex3f(15, -30, 0);
	glVertex3f(20, -30, 0);
	glEnd();

	glPushMatrix();
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glVertex3f(20, 5, 0);
	glVertex3f(20, -5, 0);
	glVertex3f(-20, -5, 0);
	glVertex3f(-20, 5, 0);
	glEnd();

	drawCircle(defX, defY, 10, 0);
	drawCircle(defX, defY, 7, 1);

	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();

	//=============================================================================

	glPushMatrix();
	glTranslatef(flyByX - 45, flyByY, 0);
	glScalef(0.4, 0.4, 0);
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glVertex3f(-20, 30, 0);
	glVertex3f(-15, 30, 0);
	glVertex3f(-15, -30, 0);
	glVertex3f(-20, -30, 0);
	glEnd();

	glPushMatrix();
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glVertex3f(20, 30, 0);
	glVertex3f(15, 30, 0);
	glVertex3f(15, -30, 0);
	glVertex3f(20, -30, 0);
	glEnd();

	glPushMatrix();
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glVertex3f(20, 5, 0);
	glVertex3f(20, -5, 0);
	glVertex3f(-20, -5, 0);
	glVertex3f(-20, 5, 0);
	glEnd();

	drawCircle(defX, defY, 10, 0);
	drawCircle(defX, defY, 7, 1);

	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();

	//=====================================================================

	glPushMatrix();
	glTranslatef(flyByX + 45, flyByY, 0);
	glScalef(0.4, 0.4, 0);
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glVertex3f(-20, 30, 0);
	glVertex3f(-15, 30, 0);
	glVertex3f(-15, -30, 0);
	glVertex3f(-20, -30, 0);
	glEnd();

	glPushMatrix();
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glVertex3f(20, 30, 0);
	glVertex3f(15, 30, 0);
	glVertex3f(15, -30, 0);
	glVertex3f(20, -30, 0);
	glEnd();

	glPushMatrix();
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glVertex3f(20, 5, 0);
	glVertex3f(20, -5, 0);
	glVertex3f(-20, -5, 0);
	glVertex3f(-20, 5, 0);
	glEnd();

	drawCircle(defX, defY, 10, 0);
	drawCircle(defX, defY, 7, 1);

	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();

	if (flyByUp) {
		flyByY+=2;
		if (flyByY > 900) {
			flyBy = false;
		}
	}
	else {
		flyByY-=2;
		if (flyByY < 0) {
			flyBy = false;
		}
	}
}

void spawnFlyBy(int value) {
	if (!flyBy) {
		int x = rand() % 2;
		int y = rand() % 2;
		if (x == 0) {
			flyByX = 150;
		}
		else {
			flyByX = 1450;
		}
		if (y == 0) {
			flyByY = 0;
			flyByUp = true;
		}
		else {
			flyByY = 900;
			flyByUp = false;
		}
		flyBy = true;
		PlaySoundA("flyby.wav", NULL, SND_ASYNC | SND_FILENAME);
	}
	int time = (rand() % 20) * 1000;
	glutTimerFunc(time, spawnFlyBy, 0);
}