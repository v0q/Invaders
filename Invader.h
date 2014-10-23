#ifndef INVADER_H__
#define INVADER_H__

enum InvaderType{TYPE1,TYPE2,TYPE3};

#define SPRITEWIDTH 30
#define SPRITEHEIGHT 30
#define COLS 11
#define ROWS 5
#define GAP 10

typedef struct
{
  SDL_Rect pos;
  int active;
  int frame;
  enum InvaderType type;
}Invader;


#endif
