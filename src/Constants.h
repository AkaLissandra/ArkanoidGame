#ifndef CONSTANTS_H
#define CONSTANTS_H

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 700;

const int PADDLE_WIDTH = 100;
const int PADDLE_HEIGHT = 20;
const int PADDLE_MARGIN_BOTTOM = 30;
const float PADDLE_SPEED = 5.0f;

const int BALL_WIDTH = 20;
const int BALL_HEIGHT = 20;

const int BRICK_WIDTH = 60;
const int BRICK_HEIGHT = 25;
const int BRICK_AREA_TOP_OFFSET = 125;

const int NUM_BRICK_ROWS = 6;
const int NUM_BRICK_COLUMNS = 7;   // (500 / (60 + spacing))
const int BRICK_SPACING = 5;
const int BRICK_AREA_START_X = (SCREEN_WIDTH - (NUM_BRICK_COLUMNS * BRICK_WIDTH + (NUM_BRICK_COLUMNS - 1) * BRICK_SPACING)) / 2;

#endif