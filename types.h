#ifndef TYPES_H
#define TYPES_H

typedef unsigned char BYTE;

typedef struct{
  BYTE version;
  BYTE revision;
  size_t size;
  BYTE unsync       : 1;
  BYTE extheader    : 1;
  BYTE experimental : 1;
} id3v2header;

typedef struct{
  char id[5];
  size_t size;
  BYTE tag_preserve  : 1;
  BYTE file_preserve : 1;
  BYTE read_only     : 1;
  BYTE compressed    : 1;
  BYTE encrypted     : 1;
  BYTE grouped       : 1;
  BYTE *body;
} id3v2frame;



#endif
