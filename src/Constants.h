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

const int FONT_SIZE_SCORE = 48;      // Kích thước lớn cho chữ Score
const int FONT_SIZE_MENU_ITEM = 32;  // Kích thước trung bình cho các mục Menu
const int FONT_SIZE_UI_TEXT = 24;    // Kích thước nhỏ hơn cho Best Score, Game Over text

const int HEART_ICON_WIDTH = 25;     // Chiều rộng icon trái tim
const int HEART_ICON_HEIGHT = 25;    // Chiều cao icon trái tim
const int UI_TOP_MARGIN = 10;        // Khoảng cách từ mép trên màn hình tới các UI element
const int UI_SIDE_MARGIN = 15;       // Khoảng cách từ mép trái/phải màn hình
const int HEART_SPACING = 5;         // Khoảng cách giữa các icon trái tim

// Trong Constants.h
// ... (các hằng số UI_FONT_SIZE, HEART_ICON_WIDTH/HEIGHT, UI_TOP_MARGIN, UI_SIDE_MARGIN, HEART_SPACING như trước) ...
const int PAUSE_ICON_WIDTH = 25;  // Hoặc kích thước thực tế của icon bạn
const int PAUSE_ICON_HEIGHT = 25; // Hoặc kích thước thực tế của icon bạn
const int ICON_SPACING = 10;      // Khoảng cách giữa icon Pause và icon Sound sau này

const int UI_BOTTOM_MARGIN = 10;
#endif