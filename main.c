
#include<SDL2/SDL.h>
#include <stdio.h>
#include "op.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 320;
const char* GAME_TITLE = "chip-8";
const int FPS = 5;

int load_program(char *s)
{
  printf("Loading file %s\n", s);
  FILE *fp;
  long filelen;
  char *buffer;
  fp = fopen(s, "rb");
  if(!fp) {
    printf("Error loading file\n");
    return 1;
  }
  fseek(fp, 0, SEEK_END);
  filelen = ftell(fp);
  rewind(fp);
  printf("file length %d\n", filelen);
  buffer = (char *)malloc((filelen+1)*sizeof(char)); 
  if(!buffer) {
    printf("Memory error\n");
    return 1;
  }
  size_t cc = fread(buffer, filelen, 1, fp);
/*  if(cc != filelen) {
    printf("Reading error\n");
    return 1;
  }
*/
  if(filelen > 3584) {
    printf("File too big\n");
    return 1;
  }
  for(int n = 0; n < filelen; n++)
    memory[512+n] = buffer[n];
  free(buffer);
  fclose(fp);
  return 0;
}


void update(bool* quit, SDL_Event e) {
  while( SDL_PollEvent( &e ) != 0 )
    {
      if( e.type == SDL_QUIT )
	{
	  *quit = true;
	}
      else if( e.type == SDL_KEYDOWN )
	{
	  switch( e.key.keysym.sym )
	    {
	    case SDLK_1:
	      key[1] = 1;
	      break;
	    case SDLK_2:
	      key[2] = 1;
	      break;
	    case SDLK_3:
	      key[3] = 1;
	      break;
	    case SDLK_4:
	      key[12] = 1;
	      break;
	    case SDLK_q:
	      key[4] = 1;
	      break;
	    case SDLK_w:
	      key[5] = 1;
	      break;
	    case SDLK_e:
	      key[6] = 1;
	      break;
	    case SDLK_r:
	      key[13] = 1;
	      break;
	    case SDLK_a:
	      key[7] = 1;
	      break;
	    case SDLK_s:
	      key[8] = 1;
	      break;
	    case SDLK_d:
	      key[9] = 1;
	      break;
	    case SDLK_f:
	      key[14] = 1;
	      break;
	    case SDLK_z:
	      key[10] = 1;
	      break;
	    case SDLK_x:
	      key[0] = 1;
	      break;
	    case SDLK_c:
	      key[11] = 1;
	      break;
	    case SDLK_v:
	      key[15] = 1;
	      break;
	    }
	 
	}
      else if( e.type == SDL_KEYUP )
	{
	  switch( e.key.keysym.sym )
	    {
	    case SDLK_1:
	      key[1] = 0;
	      break;
	    case SDLK_2:
	      key[2] = 0;
	      break;
	    case SDLK_3:
	      key[3] = 0;
	      break;
	    case SDLK_4:
	      key[12] = 0;
	      break;
	    case SDLK_q:
	      key[4] = 0;
	      break;
	    case SDLK_w:
	      key[5] = 0;
	      break;
	    case SDLK_e:
	      key[6] = 0;
	      break;
	    case SDLK_r:
	      key[13] = 0;
	      break;
	    case SDLK_a:
	      key[7] = 0;
	      break;
	    case SDLK_s:
	      key[8] = 0;
	      break;
	    case SDLK_d:
	      key[9] = 0;
	      break;
	    case SDLK_f:
	      key[14] = 0;
	      break;
	    case SDLK_z:
	      key[10] = 0;
	      break;
	    case SDLK_x:
	      key[0] = 0;
	      break;
	    case SDLK_c:
	      key[11] = 0;
	      break;
	    case SDLK_v:
	      key[15] = 0;
	      break;
	    }
	}
    }
      cycle();
}
       void render(SDL_Window* w, SDL_Renderer* r) {
	if(draw_flag) {
    SDL_SetRenderDrawColor( r, 0, 0, 0, 0 );
    SDL_RenderClear( r );

    SDL_Rect rec;
    SDL_SetRenderDrawColor( r, 255, 255, 255, 255 );
    int xpad = SCREEN_WIDTH / 64;
    int ypad = SCREEN_HEIGHT / 32;
    int i = 0;
    for(int y = 0; y < 32; y++)
      {
	for(int x = 0; x < 64; x++)
	  {
	    if(gfx[y*64 + x] > 0) {
	      rec.x = x*xpad;
	      rec.y = y*ypad;
	      rec.w = xpad;
	      rec.h = ypad;
	      
	      SDL_RenderFillRect( r, &rec );
	    }
	  }
      }
    SDL_RenderPresent(r);
    draw_flag = false;
  }
  
  SDL_Delay( FPS );
}


void debug() {
  if(draw_flag)
    {
      for(int y = 0; y < 32; ++y)
	{
	  for(int x = 0; x < 64; ++x)
	    {
	      if(gfx[(y*64) + x] == 0) 
		printf("O");
	      else 
		printf(" ");
	    }
	  printf("\n");
	}
      printf("\n");
      draw_flag = false;
    }
  
}

int main( int argc, char* args[] )
{
  SDL_Window* window = NULL;
  SDL_Renderer* rend =  NULL;
  SDL_Event e;
  bool quit = false;

  if( argc < 2 ) {
    printf("input file\n");
    return 1;
  }
    
  if( SDL_Init( SDL_INIT_VIDEO ) < 0) {
    printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
  } else {
    window = SDL_CreateWindow( GAME_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    rend = SDL_CreateRenderer(window,
			      0,
			      SDL_RENDERER_ACCELERATED);
    if( window == NULL ) {
      printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
      return 0;
    } else if (rend == NULL) {
      printf ("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
    } else {
      op_init();
      if (load_program(args[1]))
	quit = true;
      while(!quit) {
	update(&quit, e);
	render(window, rend);
	//debug();
      }
    }
  }
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}


