#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_audio.h"
#include <chrono>

#include <stdio.h>

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 368
#define BLOCK_SIDE 16
#define WORLD_WIDTH 10
#define WORLD_HEIGHT 18

#define LINES_PER_LEVEL 10
#define PIECE_MAX_SIZE 4
#define PIECE_VARIATIONS 7
#define COLOR_VARIATIONS 16


void init(int *world, int *level, int *lines, int *timer, char *rand, bool *dead);
void random(char *rand);
void newPiece(int *piece_x, int *piece_y, int *piece_color, int *piece_rotation, int *piece_type, char *rand);
void rotateLeft(int *piece_rotation, int piece_type, bool (*piece)[PIECE_MAX_SIZE][PIECE_MAX_SIZE]
  , const bool pieces[PIECE_VARIATIONS][4][PIECE_MAX_SIZE][PIECE_MAX_SIZE]);
void rotateRight(int *piece_rotation, int piece_type, bool (*piece)[PIECE_MAX_SIZE][PIECE_MAX_SIZE]
  , const bool pieces[PIECE_VARIATIONS][4][PIECE_MAX_SIZE][PIECE_MAX_SIZE]);
void updatePiece(int piece_rotation, int piece_type, bool (*piece)[PIECE_MAX_SIZE][PIECE_MAX_SIZE], const bool pieces[PIECE_VARIATIONS][4][PIECE_MAX_SIZE][PIECE_MAX_SIZE]);
bool collition(int piece_x, int piece_y, bool piece[PIECE_MAX_SIZE][PIECE_MAX_SIZE], int world[WORLD_HEIGHT][WORLD_WIDTH]);
void setPiece(bool piece[PIECE_MAX_SIZE][PIECE_MAX_SIZE], int (*world)[WORLD_HEIGHT][WORLD_WIDTH], int piece_x, int piece_y, int piece_color, int *level, int *lines);

void renderNumber(SDL_Renderer *renderer, SDL_Texture *font, int num, int x, int y);
void setColor(SDL_Renderer *renderer, int id);


int main(){
  bool running;

  //open window
  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_Window *window = SDL_CreateWindow("tetrominoes", 64, 64, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
  SDL_Texture *font = IMG_LoadTexture(renderer, "font.png");

  SDL_Surface *icon = IMG_Load("icon.ico");
  SDL_SetWindowIcon(window, icon);


  SDL_AudioSpec wavSpec;
  Uint32 wavLength;
  Uint8 *wavBuffer;
  SDL_LoadWAV("music.wav", &wavSpec, &wavBuffer, &wavLength);

  SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
  SDL_ClearQueuedAudio(deviceId);
  SDL_PauseAudioDevice(deviceId, 0);
  SDL_QueueAudio(deviceId, wavBuffer, wavLength);

  int world[WORLD_HEIGHT][WORLD_WIDTH];
  int off_x = 64;
  int off_y = 32;

  char rand = SDL_GetTicks() * 7;

  int piece_x, piece_y, piece_color, piece_rotation, piece_type;
  int level, lines, timer;
  bool dead, mute = false;

  bool piece[PIECE_MAX_SIZE][PIECE_MAX_SIZE];


  const bool pieces[PIECE_VARIATIONS][4][PIECE_MAX_SIZE][PIECE_MAX_SIZE] = {
    {
      {
      {0, 0, 1, 0}, //line
      {0, 0, 1, 0},
      {0, 0, 1, 0},
      {0, 0, 1, 0}
      },
      {
      {0, 0, 0, 0}, //line
      {0, 0, 0, 0},
      {1, 1, 1, 1},
      {0, 0, 0, 0}
      },
      {
      {0, 0, 1, 0}, //line
      {0, 0, 1, 0},
      {0, 0, 1, 0},
      {0, 0, 1, 0}
      },
      {
      {0, 0, 0, 0}, //line
      {0, 0, 0, 0},
      {1, 1, 1, 1},
      {0, 0, 0, 0}
      }
    },
    {
      {
      {0, 0, 0, 0}, //square
      {0, 1, 1, 0},
      {0, 1, 1, 0},
      {0, 0, 0, 0}
      },
      {
      {0, 0, 0, 0}, //square
      {0, 1, 1, 0},
      {0, 1, 1, 0},
      {0, 0, 0, 0}
      },
      {
      {0, 0, 0, 0}, //square
      {0, 1, 1, 0},
      {0, 1, 1, 0},
      {0, 0, 0, 0}
      },
      {
      {0, 0, 0, 0}, //square
      {0, 1, 1, 0},
      {0, 1, 1, 0},
      {0, 0, 0, 0}
      }
    },
    {
      {
      {0, 0, 0, 0}, //reverse L
      {0, 1, 1, 0},
      {0, 1, 0, 0},
      {0, 1, 0, 0}
      },
      {
      {0, 0, 0, 0}, //reverse L
      {1, 1, 1, 0},
      {0, 0, 1, 0},
      {0, 0, 0, 0}
      },
      {
      {0, 0, 1, 0}, //reverse L
      {0, 0, 1, 0},
      {0, 1, 1, 0},
      {0, 0, 0, 0}
      },
      {
      {0, 0, 0, 0}, //reverse L
      {0, 1, 0, 0},
      {0, 1, 1, 1},
      {0, 0, 0, 0}
      }
    },
    {
      {
      {0, 0, 0, 0}, //L
      {0, 1, 1, 0},
      {0, 0, 1, 0},
      {0, 0, 1, 0}
      },
      {
      {0, 0, 0, 0}, //L
      {0, 0, 1, 0},
      {1, 1, 1, 0},
      {0, 0, 0, 0}
      },
      {
      {0, 1, 0, 0}, //L
      {0, 1, 0, 0},
      {0, 1, 1, 0},
      {0, 0, 0, 0}
      },
      {
      {0, 0, 0, 0}, //L
      {0, 1, 1, 1},
      {0, 1, 0, 0},
      {0, 0, 0, 0}
      }
    },
    {
      {
      {0, 0, 0, 0}, //s
      {0, 0, 0, 0},
      {0, 1, 1, 0},
      {1, 1, 0, 0}
      },
      {
      {0, 0, 0, 0}, //s
      {0, 1, 0, 0},
      {0, 1, 1, 0},
      {0, 0, 1, 0}
      },
      {
      {0, 0, 0, 0}, //s
      {0, 0, 0, 0},
      {0, 1, 1, 0},
      {1, 1, 0, 0}
      },
      {
      {0, 0, 0, 0}, //s
      {0, 1, 0, 0},
      {0, 1, 1, 0},
      {0, 0, 1, 0}
      }
    },
    {
      {
      {0, 0, 0, 0},
      {0, 0, 0, 0}, //z
      {0, 1, 1, 0},
      {0, 0, 1, 1}
      },
      {
      {0, 0, 0, 0},
      {0, 0, 1, 0}, //z
      {0, 1, 1, 0},
      {0, 1, 0, 0}
      },
      {
      {0, 0, 0, 0},
      {0, 0, 0, 0}, //z
      {0, 1, 1, 0},
      {0, 0, 1, 1}
      },
      {
      {0, 0, 0, 0},
      {0, 0, 1, 0}, //z
      {0, 1, 1, 0},
      {0, 1, 0, 0}
      }
    },
    {
      {
      {0, 0, 0, 0},
      {0, 0, 0, 0}, //T
      {0, 1, 1, 1},
      {0, 0, 1, 0}
      },
      {
      {0, 0, 0, 0},
      {0, 0, 1, 0}, //T
      {0, 1, 1, 0},
      {0, 0, 1, 0}
      },
      {
      {0, 0, 0, 0},
      {0, 0, 1, 0}, //T
      {0, 1, 1, 1},
      {0, 0, 0, 0}
      },
      {
      {0, 0, 0, 0},
      {0, 0, 1, 0}, //T
      {0, 0, 1, 1},
      {0, 0, 1, 0}
      }
    }
  };

  uint32_t time_old = SDL_GetTicks(), time_new;

  SDL_Rect temp_rect;

  running = true;
  init(&world[0][0], &level, &lines, &timer, &rand, &dead);
  newPiece(&piece_x, &piece_y, &piece_color, &piece_rotation, &piece_type, &rand);
  updatePiece(piece_rotation, piece_type, &piece, pieces);
  while(running){
    //events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_KEYDOWN:
          switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
              running = false;
              break;
            case SDLK_r:
              init(&world[0][0], &level, &lines, &timer, &rand, &dead);
              newPiece(&piece_x, &piece_y, &piece_color, &piece_rotation, &piece_type, &rand);
              updatePiece(piece_rotation, piece_type, &piece, pieces);
              break;
            case SDLK_m:
              mute = !mute;
              break;

            case SDLK_RIGHT:
              if(!dead){
                piece_x++;
                if(collition(piece_x, piece_y, piece, world))
                  piece_x--;
              }
              break;
            case SDLK_LEFT:
              if(!dead){
                piece_x--;
                if(collition(piece_x, piece_y, piece, world))
                  piece_x++;
              }
              break;
            case SDLK_DOWN:
              if(!dead){
                piece_y++;
                if(collition(piece_x, piece_y, piece, world)){
                  piece_y--;
                  setPiece(piece, &world, piece_x, piece_y, piece_color, &level, &lines);
                  newPiece(&piece_x, &piece_y, &piece_color, &piece_rotation, &piece_type, &rand);
                  updatePiece(piece_rotation, piece_type, &piece, pieces);
                  if(collition(piece_x, piece_y, piece, world))
                    dead = true;
                }
              }
              break;
            case SDLK_z:
              if(!dead){
                rotateLeft(&piece_rotation, piece_type, &piece, pieces);
                if(collition(piece_x, piece_y, piece, world))
                  rotateRight(&piece_rotation, piece_type, &piece, pieces);
              }
              break;
            case SDLK_x:
              if(!dead){
                rotateRight(&piece_rotation, piece_type, &piece, pieces);
                if(collition(piece_x, piece_y, piece, world))
                  rotateLeft(&piece_rotation, piece_type, &piece, pieces);
              }
              break;

            /*
            case SDLK_a:
              piece_type++;
              piece_type %= PIECE_VARIATIONS;
              updatePiece(piece_rotation, piece_type, &piece, pieces);
              break;
            */
          }
          break;
        case SDL_QUIT:
          running = false;
      }
    }

    time_new = SDL_GetTicks();
    if(time_new - time_old > 1000/60){
      if(!dead){
        timer++;
        if((timer == 60 - level && level < 60) || level >= 60){
          piece_y++;
          if(collition(piece_x, piece_y, piece, world)){
            piece_y--;
            setPiece(piece, &world, piece_x, piece_y, piece_color, &level, &lines);
            newPiece(&piece_x, &piece_y, &piece_color, &piece_rotation, &piece_type, &rand);
            if(collition(piece_x, piece_y, piece, world))
              dead = true;
            updatePiece(piece_rotation, piece_type, &piece, pieces);
          }
          timer = 0;
        }

        if(mute)
          SDL_PauseAudioDevice(deviceId, 1);
        else if(SDL_GetAudioDeviceStatus(deviceId) != SDL_AUDIO_PLAYING)
          SDL_PauseAudioDevice(deviceId, 0);
      }
      else
        if(SDL_GetAudioDeviceStatus(deviceId) == SDL_AUDIO_PLAYING)
          SDL_PauseAudioDevice(deviceId, 1);
      time_old = SDL_GetTicks();
    }
    if(SDL_GetQueuedAudioSize(deviceId) == 0){
      SDL_QueueAudio(deviceId, wavBuffer, wavLength);
      SDL_PauseAudioDevice(deviceId, 0);
    }
    

    //render
    SDL_SetRenderDrawColor(renderer, 64, 0, 64, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 128, 64, 128, 255);
    temp_rect.x = off_x - BLOCK_SIDE;
    temp_rect.y = off_y;
    temp_rect.w = (WORLD_WIDTH + 2) * BLOCK_SIDE;
    temp_rect.h = (WORLD_HEIGHT + 1) * BLOCK_SIDE;
    SDL_RenderFillRect(renderer, &temp_rect);
    SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
    temp_rect.x = off_x;
    temp_rect.y = off_y;
    temp_rect.w = WORLD_WIDTH * BLOCK_SIDE;
    temp_rect.h = WORLD_HEIGHT * BLOCK_SIDE;
    SDL_RenderFillRect(renderer, &temp_rect);
    
    temp_rect.w = BLOCK_SIDE;
    temp_rect.h = BLOCK_SIDE;
    for(int j = 0; j < WORLD_HEIGHT; j++)
      for(int i = 0; i < WORLD_WIDTH; i++){
        temp_rect.x = off_x + (i * BLOCK_SIDE);
        temp_rect.y = off_y + (j * BLOCK_SIDE);
        setColor(renderer, world[j][i]);
        SDL_RenderFillRect(renderer, &temp_rect);
      }

    setColor(renderer, piece_color);
    for(int j = 0; j < PIECE_MAX_SIZE; j++)
      for(int i = 0; i < PIECE_MAX_SIZE; i++){
        if(piece[j][i]){
          temp_rect.x = off_x + ((i + piece_x) * BLOCK_SIDE);
          temp_rect.y = off_y + ((j + piece_y) * BLOCK_SIDE);
          SDL_RenderFillRect(renderer, &temp_rect);
        }
      }
    renderNumber(renderer, font, level, off_x + (WORLD_WIDTH + 13) * BLOCK_SIDE, off_y);
    renderNumber(renderer, font, (level * BLOCK_SIDE) + lines, off_x + (WORLD_WIDTH + 13) * BLOCK_SIDE, off_y + (WORLD_HEIGHT * 2));

    SDL_RenderPresent(renderer);
  }

  //end
  SDL_CloseAudioDevice(deviceId);
  SDL_FreeWAV(wavBuffer);
  SDL_Quit();

  return 0;
}

void init(int *world, int *level, int *lines, int *timer, char *rand, bool *dead){
  for(int j = 0; j < WORLD_HEIGHT; j++)
    for(int i = 0; i < WORLD_WIDTH; i++)
      *(world + i + (j * WORLD_WIDTH)) = 0;

  *level = 0;
  *lines = 0;
  *timer = 0;

  int temp = (unsigned int) *rand;
  for(int i = 0; i < temp; i++)
    random(rand);
  
  *dead = false;
}

void random(char *rand){
  *rand *= 5;
  *rand += 1;
}

void newPiece(int *piece_x, int *piece_y, int *piece_color, int *piece_rotation, int *piece_type, char *rand){
  *piece_x = WORLD_WIDTH / 2;
  *piece_x -= PIECE_MAX_SIZE / 2;
  *piece_y = 0;
  *piece_rotation = 0;
  random(rand);
  *piece_color = (unsigned int)*rand % COLOR_VARIATIONS;
  *piece_color += 1;
  random(rand);
  *piece_type = (unsigned int)*rand % PIECE_VARIATIONS;
}

void rotateLeft(int *piece_rotation, int piece_type, bool (*piece)[PIECE_MAX_SIZE][PIECE_MAX_SIZE]
  , const bool pieces[PIECE_VARIATIONS][4][PIECE_MAX_SIZE][PIECE_MAX_SIZE]){

  *piece_rotation -= 1;
  if(*piece_rotation == -1)
    *piece_rotation = 3;
  updatePiece(*piece_rotation, piece_type, piece, pieces);
}

void rotateRight(int *piece_rotation, int piece_type, bool (*piece)[PIECE_MAX_SIZE][PIECE_MAX_SIZE]
  , const bool pieces[PIECE_VARIATIONS][4][PIECE_MAX_SIZE][PIECE_MAX_SIZE]){

  *piece_rotation += 1;
  if(*piece_rotation == 4)
    *piece_rotation = 0;
  updatePiece(*piece_rotation, piece_type, piece, pieces);
}


void updatePiece(int piece_rotation, int piece_type, bool (*piece)[PIECE_MAX_SIZE][PIECE_MAX_SIZE], const bool pieces[PIECE_VARIATIONS][4][PIECE_MAX_SIZE][PIECE_MAX_SIZE]){
  for(int j = 0; j < PIECE_MAX_SIZE; j++){
    for(int i = 0; i < PIECE_MAX_SIZE; i++){
      (*piece)[j][i] = pieces[piece_type][piece_rotation][j][i];
    }
  }
}

bool collition(int piece_x, int piece_y, bool piece[PIECE_MAX_SIZE][PIECE_MAX_SIZE], int world[WORLD_HEIGHT][WORLD_WIDTH]){
  bool res = false;

  for(int j = 0; j < PIECE_MAX_SIZE; j++)
    for(int i = 0; i < PIECE_MAX_SIZE; i++){
      if(piece_x + i < 0){
        if(piece[j][i])
          res = true;
      }else if(piece_x + i >= WORLD_WIDTH)
        if(piece[j][i])
          res = true;

      if(piece_y + j < 0){
        if(piece[j][i])
          res = true;
      }else if(piece_y + j >= WORLD_HEIGHT){
        if(piece[j][i])
          res = true;
      }else{
        if(0 <= piece_x + i)
          if(piece_x + i < WORLD_WIDTH)
            if(piece[j][i])
              if(world[piece_y + j][piece_x + i] != 0)
                res = true;
      }
    }

  return res;
}

void setPiece(bool piece[PIECE_MAX_SIZE][PIECE_MAX_SIZE], int (*world)[WORLD_HEIGHT][WORLD_WIDTH], int piece_x, int piece_y, int piece_color, int *level, int *lines){
  for(int j = 0; j < PIECE_MAX_SIZE; j++)
    for(int i = 0; i < PIECE_MAX_SIZE; i++)
      if(piece[j][i])
        (*world)[piece_y + j][piece_x + i] = piece_color;

  bool line;
  for(int j = 1; j < WORLD_HEIGHT; j++){
    line = true;
    for(int i = 0; i < WORLD_WIDTH; i++){
      if((*world)[j][i] == 0)
        line = false;
    }
    if(line){
      for(int w = j; w > 0; w--)
        for(int i = 0; i < WORLD_WIDTH; i++)
          (*world)[w][i] = (*world)[w - 1][i];
      
      *lines += 1;
      if(*lines == LINES_PER_LEVEL){
        *level += 1;
        *lines = 0;
      }
    }
  }
}


void renderNumber(SDL_Renderer *renderer, SDL_Texture *font, int num, int x, int y){
  int temp_x = x;
  SDL_Rect source;
  source.w = BLOCK_SIDE;
  source.h = BLOCK_SIDE;
  SDL_Rect destiny = source;
  destiny.y = y;
  do{
    source.x = num % 5;
    source.y = num % 10;
    source.y /= 5;
    source.x *= BLOCK_SIDE;
    source.y *= BLOCK_SIDE;

    destiny.x = temp_x;

    SDL_RenderCopy(renderer, font, &source, &destiny);

    num /= 10;
    temp_x -= BLOCK_SIDE;
  }
  while(num != 0);
}


void setColor(SDL_Renderer *renderer, int id){
  switch(id){
    case 1:
      SDL_SetRenderDrawColor(renderer, 0, 0, 128, 255);
      break;
    case 2:
      SDL_SetRenderDrawColor(renderer, 0, 0, 192, 255);
      break;
    case 3:
      SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
      break;
    case 4:
      SDL_SetRenderDrawColor(renderer, 0, 128, 0, 255);
      break;
    case 5:
      SDL_SetRenderDrawColor(renderer, 0, 192, 0, 255);
      break;
    case 6:
      SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
      break;
    case 7:
      SDL_SetRenderDrawColor(renderer, 128, 0, 0, 255);
      break;
    case 8:
      SDL_SetRenderDrawColor(renderer, 192, 0, 0, 255);
      break;
    case 9:
      SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
      break;
    case 10:
      SDL_SetRenderDrawColor(renderer, 128, 0, 128, 255);
      break;
    case 11:
      SDL_SetRenderDrawColor(renderer, 192, 0, 192, 255);
      break;
    case 12:
      SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
      break;
    case 13:
      SDL_SetRenderDrawColor(renderer, 0, 128, 128, 255);
      break;
    case 14:
      SDL_SetRenderDrawColor(renderer, 0, 192, 192, 255);
      break;
    case 15:
      SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
      break;
    case 16:
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      break;
    default:
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  }
}