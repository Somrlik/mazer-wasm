#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <sstream>
#include <string>

#include <logger.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

/**
 * Inverse square root of two, for normalising velocity
 */
#define REC_SQRT2 0.7071067811865475 

#define NOTHING_PRESSED 0
#define UP_PRESSED 1
#define DOWN_PRESSED 1<<1
#define LEFT_PRESSED 1<<2
#define RIGHT_PRESSED 1<<3

/**
 * Context structure that will be passed to the loop handler
 */
struct context
{
    SDL_Renderer *renderer;
    SDL_Window *window;

    /**
     * Rectangle that the owl texture will be rendered into
     */
    SDL_Rect dest;
    SDL_Texture *owl_tex;

    int active_state;

    /**
     * x and y components of owl's velocity
     */
    int owl_vx;
    int owl_vy;
};

/**
 * Loads the owl texture into the context
 */
int get_owl_texture(struct context * ctx)
{
  SDL_Surface *image = IMG_Load("assets/preload/owl.png");
  if (!image) {
      log_error(IMG_GetError());
      return 0;
  }
  ctx->owl_tex = SDL_CreateTextureFromSurface(ctx->renderer, image);
  ctx->dest.w = image->w;
  ctx->dest.h = image->h;

  SDL_FreeSurface (image);

  return 1;
}

void process_input(struct context *ctx)
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.key.keysym.sym)
        {
            case SDLK_UP:
                if (event.key.type == SDL_KEYDOWN)
                    ctx->active_state |= UP_PRESSED;
                else if (event.key.type == SDL_KEYUP)
                    ctx->active_state ^= UP_PRESSED;
                break;
            case SDLK_DOWN:
                if (event.key.type == SDL_KEYDOWN)
                    ctx->active_state |= DOWN_PRESSED;
                else if (event.key.type == SDL_KEYUP)
                    ctx->active_state ^= DOWN_PRESSED;
                break;
            case SDLK_LEFT:
                if (event.key.type == SDL_KEYDOWN)
                    ctx->active_state |= LEFT_PRESSED;
                else if (event.key.type == SDL_KEYUP)
                    ctx->active_state ^= LEFT_PRESSED;
                break;
            case SDLK_RIGHT:
                if (event.key.type == SDL_KEYDOWN)
                    ctx->active_state |= RIGHT_PRESSED;
                else if (event.key.type == SDL_KEYUP)
                    ctx->active_state ^= RIGHT_PRESSED;
                break;
            default:
                break;
        }
    }

    ctx->owl_vy = 0;
    ctx->owl_vx = 0;
    if (ctx->active_state & UP_PRESSED)
        ctx->owl_vy = -5;
    if (ctx->active_state & DOWN_PRESSED)
        ctx->owl_vy = 5;
    if (ctx->active_state & LEFT_PRESSED)
        ctx->owl_vx = -5;
    if (ctx->active_state & RIGHT_PRESSED)
        ctx->owl_vx = 5;

    if (ctx->owl_vx != 0 && ctx->owl_vy != 0)
    {
        ctx->owl_vx *= REC_SQRT2;
        ctx->owl_vy *= REC_SQRT2;
    }
}

// (2^64/60) ~ 9.74e9 years
ulong frameCounter = 0;

/**
 * Loop handler that gets called each animation frame
 */
void loop_handler(void *arg)
{
    struct context *ctx = (context*) arg;

    process_input(ctx);

    ctx->dest.x += ctx->owl_vx;
    ctx->dest.y += ctx->owl_vy;

    SDL_SetRenderDrawColor(ctx->renderer, 0xff, 0xff, 0xff, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(ctx->renderer);
    SDL_RenderCopy(ctx->renderer, ctx->owl_tex, NULL, &ctx->dest);

    SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    int width, height;
    SDL_GetRendererOutputSize(ctx->renderer, &width, &height);
    SDL_RenderDrawLine(ctx->renderer, 0, 0, width, height);
    SDL_RenderDrawLine(ctx->renderer, 0, height, width, 0);

    SDL_RenderPresent(ctx->renderer);

    frameCounter++;

    if (frameCounter % 60 == 20) {
        log_debug("Once per second");
    }
}

#define DEFAULT_WIDTH 50
#define DEFAULT_HEIGHT 50

struct context ctx;

int main()
{
    log_set_level(LOG_LEVEL_ALL);
    log_set_handler([](std::string message, int level) -> void {
        std::cout << level << " " << message << std::endl;
    });

    SDL_Window *window;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(DEFAULT_WIDTH, DEFAULT_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &ctx.renderer);
    SDL_SetWindowSize(window, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    SDL_SetRenderDrawColor(ctx.renderer, 255, 255, 255, 255);

    get_owl_texture(&ctx);
    ctx.window = window;
    ctx.active_state = NOTHING_PRESSED;
    ctx.dest.x = 200;
    ctx.dest.y = 100;
    ctx.owl_vx = 0;
    ctx.owl_vy = 0;

    SDL_ShowCursor(SDL_ENABLE);
    log_info("Starting mainloop...");
    EM_ASM( window.notify_startingMainloop() );
    emscripten_set_main_loop_arg(loop_handler, &ctx, -1, 1);

    return 0;
}

#ifdef __EMSCRIPTEN__
extern "C" {
    void js_on_resize_callback(int width, int height) {
        std::stringstream ss;
        ss << "Resize to " << width << "x" << height;
        log_alert(ss.str());
        SDL_SetWindowSize(ctx.window, width, height);
    }
}
#endif
