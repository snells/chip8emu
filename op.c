#include "op.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>


unsigned short opcode;
unsigned short i;
unsigned short pc;
unsigned char delay_timer;
unsigned char sound_timer;
unsigned short sp;
unsigned char memory[4096];
unsigned char v[16];
unsigned char gfx[2048];
unsigned short stack[16];
unsigned char key[16];
bool clear_flag = false;
bool draw_flag = false;
unsigned char fontset[80] =
{ 
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};



void op_init()
{
  pc = 0x200;
  opcode = 0;
  i = 0;
  sp = 0;

  for (int i = 0; i < 4096; i++)
    {
      if(i < 80)
	memory[i] = fontset[i];
      else
	memory[i] = 0;

      if(i < 16) {
	v[i] = 0;
	stack[i] = 0;
	key[i] = 0;
      }

      if(i < 2048)
	gfx[i] = 0;
    }
  delay_timer = 0;
  sound_timer = 0;
  srand(time(NULL));
  draw_flag = true;
}








void cycle()
{
  opcode = memory[pc] << 8 | memory[pc+1];
  //  printf("%x\n", opcode);
  unsigned short nnn = opcode & 0xfff;
  unsigned short kk = opcode & 0xff;
  unsigned char x = (opcode & 0xf00) >> 8;
  unsigned char y = (opcode & 0xf0) >> 4;
  unsigned char l = opcode & 0xf;
  switch (opcode >> 12)
    {
    case 0:
      switch (l)
	{
	case 0x0:
	  for(int n = 0; n < 2048; n++)
	    gfx[i] = 0;
	  draw_flag = true;
	  pc+=2;
	  break;
	case 0xe:
	  pc = stack[--sp];
	  pc += 2;
	  break;
	}
      break;
    case 1:
      pc = nnn;
      break;
    case 2:
      stack[sp++] = pc;
      pc = nnn;
      break;
    case 3:
      pc+=2;
      if(v[x] == kk)
	pc+=2;
      break;
    case 4:
      pc+=2;
      if(v[x] != kk)
	pc+=2;
      break;
    case 5:
      pc+=2;
      if(v[x] == v[y])
	pc+=2;
      break;
    case 6:
      v[x] = kk;
      pc+=2;
      break;
    case 7:
      v[x] += kk;
      pc+=2;
      break;
    case 8:
      switch (l)
	{
	case 0:
	  v[x] = v[y];
	  pc+=2;
	  break;
	case 1:
	  v[x] |= v[y];
	  pc+=2;
	  break;
	case 2:
	  v[x] &= v[y];
	  pc+=2;
	  break;
	case 3:
	  v[x] ^= v[y];
	  pc+=2;
	  break;
	case 4:
	  v[0xf] = 0;
	  if(v[x] > 0xff - v[y])
	    v[0xf] = 1;
	  v[x] += v[y];
	  pc+=2;
	  break;
	case 5:
	  v[0xf] = 0;
	  if(v[x] > v[y])
	    v[0xf] = 1;
	  v[x] -= v[y];
	  pc+=2;
	  break;
	case 6:
	  v[0xf] = v[x] & 1;
	  v[x] >>= 1;
	  pc+=2;
	  break;
	case 7:
	  v[0xf] = 0;
	  if(v[y] > v[x])
	    v[0xf] = 1;
	  v[x] = v[y] - v[x];
	  pc+=2;
	  break;
	case 0xe:
	  v[0xf] = 0;
	  if(v[x] >> 7)
	    v[0xf] = 1;
	  v[x] <<= 1;
	  pc+=2;
	  break;
	}
      break;
    case 9:
      pc+=2;
      if(v[x] != v[y])
	pc+=2;
      break;
    case 10:
      i = nnn;
      pc+=2;
      break;
    case 11:
      pc = nnn + v[0];
      break;
    case 12:
      v[x] = kk & (rand() % 0xff);
      pc+=2;
      break;
    case 13:
      {
	unsigned short X = v[x];
	unsigned short Y = v[y];
	unsigned short pixel;
	v[0xf] = 0;
	for(int yline = 0; yline < l; yline++)
	  {
	    pixel = memory[i + yline];
	    for(int xline = 0; xline < 8; xline++)
	      {
		if((pixel & (0x80 >> xline)) != 0) {
		  if (gfx[X + xline + ((Y + yline) * 64)] == 1)
		    v[0xf] = 1;
		  gfx[X + xline + ((Y + yline) * 64)] ^= 1;
		  
		}
	      }
	  }
	draw_flag = true;
	pc+=2;
      }
      break;
    case 14:
      switch (kk)
	{
	case 0x9e:
	  pc+=2;
	  if(key[v[x]] != 0)
	    pc+=2;
	  break;
	case 0xa1:
	  pc+=2;
	  if(key[v[x]] == 0)
	    pc+=2;
	  break;
	}
      break;
    case 15:
      switch (kk)
	{
	case 0x07:
	  v[x] = delay_timer;
	  pc+=2;
	  break;
	case 0x0a:
	  {
	    bool press = true;
	    for(int n = 0; n < 16; n++)
	      {
		if(key[n] != 0) {
		  v[x] = i;
		  press = false;
		}
	      }
	    if(press) {
	      printf("waiting for key press\n");
	      return;
	    }
	    pc+=2;
	  }
	  break;
	case 0x15:
	  delay_timer = v[x];
	  pc+=2;
	  break;
	case 0x18:
	  sound_timer = v[x];
	  pc+=2;
	  break;
	case 0x1e:
	  v[0xf] = 0;
	  if(i + v[x] > 0xfff)
	    v[0xf] = 1;
	  i += v[x];
	  pc+=2;
	  break;
	case 0x29:
	  i = v[x] * 5;
	  pc+=2;
	  break;
	case 0x33:
	  memory[i] = v[x] / 100;
	  memory[i+1] = (v[x] / 10) % 10;
	  memory[i+2] = (v[x] / 100) % 10;
	  pc+=2;
	  break;
	case 0x55:
	  for(int n = 0; n <= x; n++)
	    memory[i+n] = v[n];
	  i+= x+1;
	  pc+=2;
	  break;
	case 0x65:
	  for(int n = 0; n < x; n++)
	    v[n] = memory[i+n];
	  i+= x+1;
	  pc+=2;
	  break;
	}
      break;
    default:
      printf("unknown opcode %x\n", opcode);
    }
  if(delay_timer > 0)
    delay_timer--;
  if(sound_timer == 0) {
    printf("sound\n");
    sound_timer--;
  }
}
