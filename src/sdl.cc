#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <emscripten.h>
#include <emscripten/html5.h>

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
  if (!image)
  {
     printf("IMG_Load: %s\n", IMG_GetError());
     return 0;
  }
  ctx->owl_tex = SDL_CreateTextureFromSurface(ctx->renderer, image);
  ctx->dest.w = image->w;
  ctx->dest.h = image->h;

  SDL_FreeSurface (image);

  return 1;
}

/**
 * Processes the input events and sets the velocity
 * of the owl accordingly
 */
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

int frameCounter = 0;

/**
 * Loop handler that gets called each animation frame,
 * process the input, update the position of the owl and 
 * then render the texture
 */
void loop_handler(void *arg)
{
    struct context *ctx = (context*) arg;

    int vx = 0;
    int vy = 0;
    process_input(ctx);

    ctx->dest.x += ctx->owl_vx;
    ctx->dest.y += ctx->owl_vy;

    SDL_RenderClear(ctx->renderer);
    SDL_RenderCopy(ctx->renderer, ctx->owl_tex, NULL, &ctx->dest);
    SDL_RenderPresent(ctx->renderer);
    frameCounter++;
    if (frameCounter > 60) {
        frameCounter = 0;
    }

    if (frameCounter % 60 == 20) {
        printf("Once per second\n");
    }
}

EM_BOOL on_canvassize_changed(int eventType, const void *reserved, void *userData)
{
  int w, h;
  emscripten_get_canvas_element_size("#canvas", &w, &h);
  double cssW, cssH;
  emscripten_get_element_css_size("#canvas", &cssW, &cssH);
  printf("Canvas resized: WebGL RTT size: %dx%d, canvas CSS size: %02gx%02g\n", w, h, cssW, cssH);
  return 0;
}

int main()
{
    SDL_Window *window;
    struct context ctx;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(600, 400, SDL_WINDOW_RESIZABLE, &window, &ctx.renderer);
    SDL_SetWindowSize(window, 600, 400);
    SDL_SetRenderDrawColor(ctx.renderer, 255, 255, 255, 255);

    get_owl_texture(&ctx);
    ctx.active_state = NOTHING_PRESSED;
    ctx.dest.x = 200;
    ctx.dest.y = 100;
    ctx.owl_vx = 0;
    ctx.owl_vy = 0;

    emscripten_set_resize_callback("#canvas", nullptr, 0, [](int eventType, const EmscriptenUiEvent *uiEvent, void *userData) -> EM_BOOL {
        printf("RESIZE!\n");
        return 0;
    });
    
    SDL_AddEventWatch([](void*, SDL_Event* event) -> int{
        if((event->type == SDL_KEYDOWN) && event->key.keysym.sym == SDLK_F11) {
            emscripten_request_pointerlock("#canvas", 0);
            emscripten_request_fullscreen("#canvas", 0);
            printf("Going fullscreen\n");
        }
        return 1;
    }, 0);

    SDL_ShowCursor(SDL_ENABLE);
    emscripten_set_main_loop_arg(loop_handler, &ctx, -1, 1);

    return 0;
}
