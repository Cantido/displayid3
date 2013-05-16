/*  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *
 * verifyid3.c - Displays information in an ID3v2 header
 * Copyright (C) 2013 Robert Richter
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  */
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "id3v2.h"


char *progname;
char *filename;



int main(int argc, char *argv[]){

  extern char *progname;
  extern char *filename;
  
  FILE *fp;
  
  id3v2header *header;
  id3v2frame *frame;
  
  progname = argv[0];
  filename = argv[1];
  
  if(argc != 2){
    printf("Usage: %s filename\n", progname);
    exit(EXIT_FAILURE);
  }
  
  if((fp = fopen(filename, "r")) == NULL){
    perror(progname);
    exit(EXIT_FAILURE);
  }
  
  if((header = getheader(fp)) == NULL){
    fprintf(stderr, "%s error: not a valid MP3 file.\n", progname);
    exit(EXIT_FAILURE);
  }
  
  printheader(header);
  
  while(((frame = getframe(fp)) != NULL) && frame->body != NULL){
    printframe(frame, 1);
    free(frame->body);
    frame->body = NULL;
  }
  
  (void) fclose(fp);
  
  
  return 0;
}
