#ifndef CUBE_H
#define CUBE_H

struct TrigCache {
    float sinA, cosA, sinB, cosB, sinC, cosC;
};


float A, B, C;
float x, y, z;
float oneOnZ;
int pointX, pointY;
int idxBuffer;

const int SCREEN_WIDTH           = 160;
const int SCREEN_HEIGHT          = 44;
const int SCREEN_AREA            = SCREEN_WIDTH * SCREEN_HEIGHT;
const int SLEEP_TIME             = 1000;
const int DISTANCE_FROM_CAM      = 100;
const float CUBE_WIDTH           = 20;
const float INCREMENT_SPEED      = 0.2;
const float INCREMENT_ANGLE      = 0.05;
const float INCREMENT_ANGLE_C    = 0.01;
const float SCALE_FACTOR         = 40;
const char BACKGROUND_ASCII_CODE = ' ';

char character_buffer[SCREEN_AREA];
float depth_buffer[SCREEN_AREA];

float calculateZ(float i, float j, float k, const TrigCache &trig);
float calculateX(float i, float j, float k, const TrigCache &trig);
float calculateY(float i, float j, float k, const TrigCache &trig);

bool handleUserInput();

void drawCube();
void calculateForSurface(float cubeX, float cubeY, float cubeZ, int ch, const TrigCache &trig);
void processSurfaces(const TrigCache &trig);

TrigCache computeTrigCache();

#endif
