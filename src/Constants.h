#ifndef CONSTANTS_H
#define CONSTANTS_H

// Kích thước màn hình
const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 700;

// Thuộc tính của Paddle
const int PADDLE_WIDTH = 100;
const int PADDLE_HEIGHT = 20;
const int PADDLE_MARGIN_BOTTOM = 30;
const float PADDLE_SPEED = 5.0f;

// Thuộc tính của Ball (bóng)
const int BALL_WIDTH = 20;
const int BALL_HEIGHT = 20;

// Thuộc tính của Brick
const int BRICK_WIDTH = 60;
const int BRICK_HEIGHT = 25;
const int BRICK_AREA_TOP_OFFSET = 125; // Khoảng cách từ mép trên màn hình đến hàng gạch đầu tiên
const int NUM_BRICK_ROWS = 6;
const int NUM_BRICK_COLUMNS = 7; // (500 / (60 + spacing))
const int BRICK_SPACING = 5;
const int BRICK_AREA_START_X = (SCREEN_WIDTH - (NUM_BRICK_COLUMNS * BRICK_WIDTH + (NUM_BRICK_COLUMNS - 1) * BRICK_SPACING)) / 2; // Tọa độ X bắt đầu

// Kích thước Font chữ
const int FONT_SIZE_SCORE = 48;      // Kích thước lớn cho chữ Score
const int FONT_SIZE_MENU_ITEM = 32;  // Kích thước trung bình cho các mục Menu
const int FONT_SIZE_UI_TEXT = 24;    // Kích thước nhỏ hơn cho Best Score, Game Over text

// Các icon
const int HEART_ICON_WIDTH = 25;
const int HEART_ICON_HEIGHT = 25;
const int UI_TOP_MARGIN = 10;        // Khoảng cách từ mép trên màn hình tới các UI element
const int UI_SIDE_MARGIN = 15;       // Khoảng cách từ mép trái/phải màn hình
const int HEART_SPACING = 5;
const int SOUND_ICON_WIDTH = 25;  // Kích thước icon âm thanh, bằng với Pause cho đẹp
const int SOUND_ICON_HEIGHT = 25;
const int PAUSE_ICON_WIDTH = 25;
const int PAUSE_ICON_HEIGHT = 25;
const int ICON_SPACING = 10; // Pause - Sound
const int UI_BOTTOM_MARGIN = 10;

// Thuộc tính của Power-up
const int POWERUP_SIZE = 20;
const float POWERUP_FALL_SPEED = 2.0f;
const int MAX_LIVES = 3;
const int POWERUP_SPAWN_CHANCE_NORMAL = 1;
const int BONUS_POINTS_VALUE = 50; // Của Powerup

// Bg
const float MENU_BG_SCROLL_SPEED = 0.5f;

#endif