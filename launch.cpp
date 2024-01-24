#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include <chrono>

#include <stdio.h>

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 368
#define BLOCK_SIDE 16
#define WORLD_WIDTH 10
#define WORLD_HEIGHT 18

#define PIECE_MAX_SIZE 4
#define PIECE_VARIATIONS 6


void init(int *world, int *level, int *lines, int *timer, char *rand, bool *dead);
void random(char *rand);
void newPiece(int *piece_x, int *piece_y, int *piece_color, int *piece_rotation, int *piece_type, char *rand);
void rotateLeft(int *piece_rotation, int piece_type, bool (*piece)[PIECE_MAX_SIZE][PIECE_MAX_SIZE]
  , const bool pieces[PIECE_VARIATIONS][4][PIECE_MAX_SIZE][PIECE_MAX_SIZE]);
void rotateRight(int *piece_rotation, int piece_type, bool (*piece)[PIECE_MAX_SIZE][PIECE_MAX_SIZE]
  , const bool pieces[PIECE_VARIATIONS][4][PIECE_MAX_SIZE][PIECE_MAX_SIZE]);
void updatePiece(int piece_rotation, int piece_type, bool (*piece)[PIECE_MAX_SIZE][PIECE_MAX_SIZE], const bool pieces[PIECE_VARIATIONS][4][PIECE_MAX_SIZE][PIECE_MAX_SIZE]);
bool collition(int piece_x, int piece_y, bool piece[PIECE_MAX_SIZE][PIECE_MAX_SIZE], int world[WORLD_HEIGHT][WORLD_WIDTH]);
void setPiece(bool piece[PIECE_MAX_SIZE][PIECE_MAX_SIZE], int (*world)[WORLD_HEIGHT][WORLD_WIDTH], int piece_x, int piece_y, int piece_color, bool *dead, int *level, int *lines);


int main(){
  bool running;

  //open window
  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_Window *window = SDL_CreateWindow("tetrominoes", 64, 64, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);


  int world[WORLD_HEIGHT][WORLD_WIDTH];
  int off_x = 128;
  int off_y = 32;

  char rand = SDL_GetTicks() * 7;

  int piece_x, piece_y, piece_color, piece_rotation, piece_type;
  int level, lines, timer;
  bool dead;

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
              break;

            case SDLK_RIGHT:
              piece_x++;
              if(collition(piece_x, piece_y, piece, world))
                piece_x--;
              break;
            case SDLK_LEFT:
              piece_x--;
              if(collition(piece_x, piece_y, piece, world))
                piece_x++;
              break;
            case SDLK_DOWN:
              piece_y++;
              if(collition(piece_x, piece_y, piece, world)){
                piece_y--;
                setPiece(piece, &world, piece_x, piece_y, piece_color, &dead, &level, &lines);
                newPiece(&piece_x, &piece_y, &piece_color, &piece_rotation, &piece_type, &rand);
                if(collition(piece_x, piece_y, piece, world))
                  dead = true;
                updatePiece(piece_rotation, piece_type, &piece, pieces);
              }
              break;
            case SDLK_z:
              rotateLeft(&piece_rotation, piece_type, &piece, pieces);
              if(collition(piece_x, piece_y, piece, world))
                rotateRight(&piece_rotation, piece_type, &piece, pieces);
            break;

            case SDLK_a:
              piece_type++;
              piece_type %= PIECE_VARIATIONS;
              updatePiece(piece_rotation, piece_type, &piece, pieces);
              break;
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
        if(timer == level){
          piece_y++;
          if(collition(piece_x, piece_y, piece, world)){
            piece_y--;
            setPiece(piece, &world, piece_x, piece_y, piece_color, &dead, &level, &lines);
            newPiece(&piece_x, &piece_y, &piece_color, &piece_rotation, &piece_type, &rand);
            if(collition(piece_x, piece_y, piece, world))
              dead = true;
            updatePiece(piece_rotation, piece_type, &piece, pieces);
          }
          timer = 0;
        }
      }
      time_old = SDL_GetTicks();
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
        switch(world[j][i]){
          case 1:
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            break;
          case 2:
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            break;
          case 3:
            SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
            break;
          case 4:
            SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
            break;
          case 5:
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            break;
          default:
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        }
        SDL_RenderFillRect(renderer, &temp_rect);
      }

    switch(piece_color){
      case 1:
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        break;
      case 2:
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        break;
      case 3:
        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
        break;
      case 4:
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
        break;
      case 5:
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        break;
    }
    for(int j = 0; j < PIECE_MAX_SIZE; j++)
      for(int i = 0; i < PIECE_MAX_SIZE; i++){
        if(piece[j][i]){
          temp_rect.x = off_x + ((i + piece_x) * BLOCK_SIDE);
          temp_rect.y = off_y + ((j + piece_y) * BLOCK_SIDE);
          SDL_RenderFillRect(renderer, &temp_rect);
        }
      }

    SDL_RenderPresent(renderer);
  }

  //end

  return 0;
}

void init(int *world, int *level, int *lines, int *timer, char *rand, bool *dead){
  for(int j = 0; j < WORLD_HEIGHT; j++)
    for(int i = 0; i < WORLD_WIDTH; i++)
      *(world + i + (j * WORLD_WIDTH)) = 0;

  *level = 60;
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
  *piece_y = 0;
  *piece_rotation = 0;
  random(rand);
  *piece_color = (unsigned int)*rand % 5;
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

void setPiece(bool piece[PIECE_MAX_SIZE][PIECE_MAX_SIZE], int (*world)[WORLD_HEIGHT][WORLD_WIDTH], int piece_x, int piece_y, int piece_color, bool *dead, int *level, int *lines){
  for(int j = 0; j < PIECE_MAX_SIZE; j++)
    for(int i = 0; i < PIECE_MAX_SIZE; i++)
      if(piece[j][i]){
        if(piece_y + j == 0)
          *dead = true;
        (*world)[piece_y + j][piece_x + i] = piece_color;
      }

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
      if(*lines == 10){
        if(*level != 1)
          *level -= 1;
        *lines = 0;
      }
    }
  }
}