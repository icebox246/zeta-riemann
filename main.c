#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>

#define PI 3.14159265

typedef struct {
    double x, y;
} C;

C C_add(C a, C b) { return (C){.x = a.x + b.x, .y = a.y + b.y}; }

C C_mult(C a, C b) {
    return (C){.x = a.x * b.x - a.y * b.y, .y = a.x * b.y + a.y * b.x};
}

C R_pow_C(double b, C e) {
    return C_mult((C){.x = pow(b, e.x), .y = 0},
                  (C){.x = cos(e.y * log(b)), .y = sin(e.y * log(b))});
}

C ZR(C s) {
    C acc = {0};
    for (int i = 1; i <= 100; i++) {
        acc = C_add(acc, R_pow_C(1.0 / i, s));
    }
    return acc;
}

double flerp(double x, double y, double t) { return x + (y - x) * t; }

C C_lerp_R2(C a, C b, double tx, double ty) {
    return (C){.x = flerp(a.x, b.x, tx), .y = flerp(a.y, b.y, ty)};
}

double C_abs(C c) { return sqrt(c.x * c.x + c.y + c.y); }

double C_arg(C c) { return atan(c.y / c.x); }

double fclamp(double x, double a, double b) {
    return x < a ? a : ((x > b) ? b : x);
}

int main(void) {
    C minE = {.x = -10, .y = -20};
    C maxE = {.x = 10, .y = 20};

    int window_height = 800;
    int window_width = 400;
    int should_quit = 0;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Event ev;

    SDL_Init(SDL_INIT_VIDEO);

    window =
        SDL_CreateWindow("Zeta", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         window_width, window_height, 0);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    for (int y = 0; y < window_height; y++) {
        for (int x = 0; x < window_width; x++) {
            C s = C_lerp_R2(minE, maxE, (double)x / window_width,
                            (double)y / window_height);
            C v = ZR(s);

            double sv = (fclamp(pow(log10(C_abs(v)) / 4, 4), -1, 1) + 1) / 2;

            double hv = fclamp(C_arg(v), -PI / 2, PI / 2);

            int r, g, b;

            r = 255 * fabs(cos(hv));
            g = 255 * fabs(cos(hv - PI / 3));
            b = 255 * fabs(cos(hv + PI / 3));

            r *= sv;
            g *= sv;
            b *= sv;

            SDL_SetRenderDrawColor(renderer, r, g, b, 255);

            SDL_RenderDrawPoint(renderer, x, y);
        }
    }

    SDL_RenderPresent(renderer);

    SDL_Surface* sshot =
        SDL_CreateRGBSurface(0, window_width, window_height, 32, 0x00ff0000,
                             0x0000ff00, 0x000000ff, 0xff000000);
    SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888,
                         sshot->pixels, sshot->pitch);
    SDL_SaveBMP(sshot, "zeta.bmp");
    SDL_FreeSurface(sshot);

    while (!should_quit) {
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
                case SDL_QUIT:
                    should_quit = 1;
                    break;
            }
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
