/******************************************************************************\
 * verifyid3.c - Displays information in an ID3v2 header
 * Author: Robert Richter - robert.c.richter@gmail.com
 * Date: June 2012
 * Description: Using the rules for an ID3v2 header given at
 *   http://id3.org/d3v2.3.0, this program reads and displays the information
 *   contained in a file's ID3v2 tag. Uses the same source as my verifyid3
 *   program, available at https://github.com/Cantido/verifyid3
\******************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "id3v2.h"

/* start external variable declaration */

char *progname;
char *filename;

/* end external variable declarations */

/* start function definitions */

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
  
  while(((frame = getframe(fp)) != NULL) && frame->size != 0){
    printframe(frame, 1);
    free(frame->body);
    frame->body = NULL;
  }
  
  (void) fclose(fp);
  
  
  return 0;
}
