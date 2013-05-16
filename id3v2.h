/*  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *
 * This file is part of displayid3 
 * 
 * Displayid3 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Displayid3 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *  
 * You should have received a copy of the GNU General Public License
 * along with Displayid3. If not, see <http://www.gnu.org/licenses/>.
 *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  */
#ifndef ID3V2_H
#define ID3V2_H

#include <stdio.h>

#define MEGABYTE 1048576
#define KILOBYTE 1024

#define MATCHFID( arg ) (strcmp( arg , frame->id) == 0)
#define PRINT_FRAMETEXT( arg ) printf(arg "%.*s\n", (int) frame->size - 1, frame->body)

const char * idlist[] = {           // index of last in the row:
  NULL,   "AENC", "APIC", "COMM", "COMR", "ENCR", "EQUA", // 6
  "ETCO", "GEOB", "GRID", "IPLS", "LINK", "MCDI", "MLLT", // 13
  "OWNE", "PRIV", "PCNT", "POPM", "POSS", "RBUF", "RVAD", // 20
  "SYLT", "SYTC", "TALB", "TBPM", "TCOM", "TCON", "TCOP", // 27
  "TDAT", "TDLY", "TENC", "TEXT", "TFLT", "TIME", "TIT1", // 34
  "TIT2", "TIT3", "TKEY", "TLAN", "TLEN", "TMED", "TOAL", // 41
  "TOFN", "TOLY", "TOPE", "TORY", "TOWN", "TPE1", "TPE2", // 48
  "TPE3", "TPE4", "TPOS", "TPUB", "TRCK", "TRDA", "TRSN", // 55
  "TRSO", "TSIZ", "TSRC", "TSSE", "TYER", "TXXX", "UFID", // 62
  "USER", "USLT", "WCOM", "WCOP", "WOAF", "WOAR", "WOAS", // 69
  "WORS", "WPAY", "WPUB", "WXXX" // 74
};

typedef struct{
  short version;
  short revision;
  short size;
  short unsync;
  short extheader;
  short experimental;
} id3v2header;

typedef struct{
  int id;
  size_t size;
  short tag_alter_discard;
  short file_alter_discard;
  short read_only;
  short compressed;
  short encrypted;
  short grouped;
  char *body;
} id3v2frame;

extern char *progname;
extern char *filename;

int readcheck(FILE *fp, char *buffer, size_t nbytes);
int getid(char *name);

id3v2header *getheader(FILE *fp);
id3v2frame *getframe(FILE *fp);

void printheader(id3v2header *header);
void printframe(id3v2frame *frame, int verbose);

#endif
