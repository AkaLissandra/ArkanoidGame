#include <SDL2/SDL.h>          // Sửa thành <SDL2/SDL.h>
#include <SDL2/SDL_image.h>   // Sửa thành <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>   // Sửa thành <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>     // Sửa thành <SDL2/SDL_ttf.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    // Khởi tạo SDL Video và Audio
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    } else {
         printf("SDL Initialized Successfully (Video & Audio)!\n");
    }

    // Thử khởi tạo các thư viện mở rộng (chỉ in ra lỗi nếu có, không dừng chương trình)
    if (!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG)) { // Yêu cầu hỗ trợ PNG và JPG
        printf("SDL_image could not initialize! IMG_Error: %s\n", IMG_GetError());
    } else {
        printf("SDL_image Initialized!\n");
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! Mix_Error: %s\n", Mix_GetError());
    } else {
         printf("SDL_mixer Initialized!\n");
    }

    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! TTF_Error: %s\n", TTF_GetError());
    } else {
         printf("SDL_ttf Initialized!\n");
    }

    // Tạo cửa sổ
    SDL_Window* window = SDL_CreateWindow(
        "ArkanoidGame Test",           // Tiêu đề cửa sổ
        SDL_WINDOWPOS_CENTERED,        // Vị trí x giữa màn hình
        SDL_WINDOWPOS_CENTERED,        // Vị trí y giữa màn hình
        640,                           // Chiều rộng
        480,                           // Chiều cao
        SDL_WINDOW_SHOWN               // Cờ hiển thị
    );

    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        // Dọn dẹp trước khi thoát nếu không tạo được cửa sổ
        TTF_Quit();
        Mix_CloseAudio(); // Dùng CloseAudio thay vì Mix_Quit nếu OpenAudio thành công
        IMG_Quit();
        SDL_Quit();
        return 1;
    } else {
        printf("Window created successfully!\n");
    }

    // Giữ cửa sổ hiển thị trong 3 giây (3000ms)
    printf("Window will close in 3 seconds...\n");
    SDL_Delay(3000);

    // Dọn dẹp
    printf("Cleaning up and closing.\n");
    SDL_DestroyWindow(window);
    TTF_Quit();
    Mix_CloseAudio(); // Đảm bảo đóng audio
    // Mix_Quit(); // Có thể gọi sau CloseAudio nếu cần
    IMG_Quit();
    SDL_Quit();

    return 0;
}