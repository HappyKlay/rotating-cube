#include "Cube.h"

#include <iostream>
#include <conio.h>
#include <unistd.h>
#include <thread>
#include <valarray>
#include <sstream>

void clearScreen() {
#ifdef _WIN32
    system("CLS");
#else
    system("clear");
#endif
}

using namespace std;
using namespace chrono;

int main(int argc, char const *argv[]) {
    clearScreen();
    cout << "\x1b[2J"; // clear entire screen | \x1b - escape character

    while (true) {
        if (handleUserInput()) {
            break;
        }

        drawCube();

        A += INCREMENT_ANGLE;
        B += INCREMENT_ANGLE;
        C += INCREMENT_ANGLE_C;

        this_thread::sleep_for(microseconds(SLEEP_TIME));
    }

    clearScreen();
    return 0;
}

void drawCube() {
    fill(characterBuffer.begin(), characterBuffer.end(), BACKGROUND_ASCII_CODE);
    fill(depthBuffer.begin(), depthBuffer.end(), 0);

    TrigCache trig = computeTrigCache();
    processSurfaces(trig);

    cout << "\x1b[H";
    stringstream bufferStream;


    for (int k = 0; k < SCREEN_AREA; ++k) {
        if (k % SCREEN_WIDTH == 0 && k != 0) {
            bufferStream << '\n';
        }
        bufferStream << characterBuffer[k];
    }
    bufferStream << "\x1b[H"; // move cursor to top-left corner
    cout << bufferStream.str();
}

void processSurfaces(const TrigCache &trig) {
    for (float cubeX = -CUBE_WIDTH; cubeX <= CUBE_WIDTH; cubeX += INCREMENT_SPEED) {
        for (float cubeY = -CUBE_WIDTH; cubeY <= CUBE_WIDTH; cubeY += INCREMENT_SPEED) {
            calculateForSurface(cubeX, cubeY, -CUBE_WIDTH, '@', trig);
            calculateForSurface(CUBE_WIDTH, cubeY, cubeX, '$', trig);
            calculateForSurface(-CUBE_WIDTH, cubeY, -cubeX, '~', trig);
            calculateForSurface(-cubeX, cubeY, CUBE_WIDTH, '#', trig);
            calculateForSurface(cubeX, -CUBE_WIDTH, -cubeY, ';', trig);
            calculateForSurface(cubeX, CUBE_WIDTH, cubeY, '+', trig);
        }
    }
}

bool handleUserInput() {
    if (_kbhit()) {
        int ch = _getch();
        // 27 is ESC key, 32 is SPACE key
        if (ch == 27 || ch == 32) {
            return true;
        }
    }
    return false;
}

float calculateX(float i, float j, float k, const TrigCache &trig) {
    return j * trig.sinA * trig.sinB * trig.cosC - k * trig.cosA * trig.sinB * trig.cosC +
           j * trig.cosA * trig.sinC + k * trig.sinA * trig.sinC + i * trig.cosB * trig.cosC;
}

float calculateY(float i, float j, float k, const TrigCache &trig) {
    return j * trig.cosA * trig.cosC + k * trig.sinA * trig.cosC -
           j * trig.sinA * trig.sinB * trig.sinC + k * trig.cosA * trig.sinB * trig.sinC -
           i * trig.cosB * trig.sinC;
}

float calculateZ(float i, float j, float k, const TrigCache &trig) {
    return k * trig.cosA * trig.cosB - j * trig.sinA * trig.cosB + i * trig.sinB;
}

void calculateForSurface(float cubeX, float cubeY, float cubeZ, int ch, const TrigCache &trig) {
    x = calculateX(cubeX, cubeY, cubeZ, trig);
    y = calculateY(cubeX, cubeY, cubeZ, trig);
    z = calculateZ(cubeX, cubeY, cubeZ, trig) + DISTANCE_FROM_CAM;

    oneOnZ = 1 / z;

    pointX = static_cast<int>(SCREEN_WIDTH / 2 + SCALE_FACTOR * oneOnZ * x * 2);
    pointY = static_cast<int>(SCREEN_HEIGHT / 2 + SCALE_FACTOR * oneOnZ * y);

    idxBuffer = pointX + pointY * SCREEN_WIDTH;

    if (idxBuffer >= 0 && idxBuffer < SCREEN_AREA) {
        if (oneOnZ > depthBuffer[idxBuffer]) {
            depthBuffer[idxBuffer] = oneOnZ;
            characterBuffer[idxBuffer] = ch;
        }
    }
}

TrigCache computeTrigCache() {
    return {
            sin(A), cos(A),
            sin(B), cos(B),
            sin(C), cos(C)
    };
}