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

#define MEGABYTE 1048576
#define KILOBYTE 1024

#define MATCHFID( arg ) (strcmp(frameid, arg) == 0)
#define PRINT_FRAMETEXT( arg ) printf(arg "%.*s\n", framesize - 1, &frame[11])

typedef unsigned char BYTE;

typedef struct{
  BYTE version[2];
  BYTE unsync   : 1;
  BYTE extended : 1;
  BYTE exper    : 1;
  size_t size   : 28;
} id3v2header;

typedef struct{
  char* id[5];
  size_t size;
  BYTE *body;
} id3v2frame;

int fd;
unsigned char id3version[2];
unsigned char id3flags;
unsigned int id3size;

unsigned int calcsize(char *array){
  unsigned int size = array[0];
  size <<= 7;
  size |= array[1];
  size <<= 7;
  size |= array[2];
  size <<= 7;
  size |= array[3];
  return size;
}

/* return a pointer to bytes read from the frame, NULL if something screws up */
char *getframe(){

  extern int fd;
  extern unsigned int id3size;
  
  unsigned char frameheader[10];
  unsigned char *frame;
  unsigned int framesize;
  
  if (((id3size - 10) - lseek(fd, 0, SEEK_CUR)) <= 0){
    /* no more bytes left in the header */
    return NULL;
  }
  
  switch (read(fd, frameheader, 10)){
    case -1:
      /* read encountered an error */
      perror("displayid3 read error:");
      /* falls through */
    case 0:
      return NULL;
      break;
    case 10:
      /* no errors */
      break;
    default:
      /* unknown error occurred in read() */
      return NULL;
      break;
  }
  
  framesize = calcsize(&frameheader[4]) + 10;
  
  if((frame = (char *) malloc(framesize)) != NULL){
    memcpy(frame, frameheader, 10);
    read(fd, frame+10, framesize-10);
  }
  
  return frame;
}


void printframe(char *frame){
  unsigned char frameid[5];
  unsigned int framesize;
  unsigned char frameflags[2];

  frameid[0] = frame[0];
  frameid[1] = frame[1];
  frameid[2] = frame[2];
  frameid[3] = frame[3];
  frameid[4] = '\0';
  
  framesize = calcsize(&frame[4]);
  
  frameflags[0] = frame[8];
  frameflags[1] = frame[9];
  
  printf("Frame ID: %s\n", frameid);
  printf("Size of frame (excluding 10B header): %d bytes", framesize);
  
  if(framesize > MEGABYTE)
    printf(" (%i MiB)", framesize / MEGABYTE);
  else if(framesize > KILOBYTE)
    printf(" (%i KiB)", framesize / KILOBYTE);
  
  printf("\n");
  
  printf("Flags:\n");
  
  printf("Tag alter preservation: ");
  if((frameflags[0] & 0x80) == 0x80)
    printf("Frame should be discarded\n");
  else
    printf("Frame should be preserved\n");
  
  printf("File alter preservation: ");
  if((frameflags[0] & 0x40) == 0x40)
    printf("Frame should be discarded\n");
  else
    printf("Frame should be preserved\n");
  
  printf("Frame should be read only: ");
  if((frameflags[0] & 0x20) == 0x20)
    printf("yes\n");
  else
    printf("no\n");
  
  if((frameflags[1] & 0x80) == 0x80)
    printf("Frame is compressed\n");
  else
    printf("Frame is not compressed\n");
  
  if((frameflags[1] & 0x40) == 0x40)
    printf("Frame is encrypted\n");
  else
    printf("Frame is not encrypted\n");
  
  if((frameflags[1] & 0x20) == 0x20)
    printf("Frame is in a group with other frames\n");
  else
    printf("Frame is not in a group with other frames\n"); 
    
    
  printf("Frame content:\n");
  
  if (MATCHFID("TYER"))
    PRINT_FRAMETEXT("Year recorded: ");
  else if(MATCHFID("TENC"))
    PRINT_FRAMETEXT("Encoded with: ");
  else if(MATCHFID("TBPM"))
    PRINT_FRAMETEXT("Beats per minute: ");
  else if(MATCHFID("TCON"))
    PRINT_FRAMETEXT("Content type: ");
  else if(MATCHFID("TPE1"))
    PRINT_FRAMETEXT("Lead Artist or Group: ");
  else if(MATCHFID("COMM")){
    printf("Comment Language: %.*s\n", 3, &frame[11]);
    if(frame[14] != 0x00)
      printf("Short content descriptor: %s", &frame[14]);
    else
      printf("Comment: %.*s", framesize - 5, &frame[15]);
  }
  
  
  return;
}

int main(int argc, char *argv[]){
  int return_code;
  
  extern int fd;
  extern unsigned char id3version[2];
  extern unsigned char id3flags;
  extern unsigned int id3size;
  
  unsigned char id3header[10];
  unsigned char *frame;
  
  
  if(argc != 2){
    printf("Usage: %s filename", argv[0]);
    exit(EXIT_FAILURE);
  }
  
  if((fd = open(argv[1], O_RDONLY)) < 0){
    perror(argv[0]);
    exit(EXIT_FAILURE);
  }
  
  switch (read(fd, id3header, 10)){
    case 0:
      /* read hit end-of-file */
      printf("%s: The file %s has no content", argv[0], argv[1]);
      exit(EXIT_FAILURE);
      break;
    case -1:
      /* read encountered an error */
      perror(argv[0]);
      exit(EXIT_FAILURE);
      break;
    case 10:
      /* no errors */
      break;
    default:
      /* did not read 10 bytes */
      printf("%s: could not read 10 bytes from file %s", argv[0], argv[1]);
      exit(EXIT_FAILURE);
      break;
  }
  
  if( id3header[0] == 0x49 &&
      id3header[1] == 0x44 &&
      id3header[2] == 0x33 &&
      id3header[3] < 0xFF &&
      id3header[4] < 0xFF &&
      (id3header[5] & 0x3F) == 0x00 && /* only first two bits can be nonzero */
      id3header[6] < 0x80 &&
      id3header[7] < 0x80 &&
      id3header[8] < 0x80 &&
      id3header[9] < 0x80){
      /* ID3v2 tag exists */
  }
  else{
    printf("%s does not have an ID3v2 header.\n", argv[1]);
    exit(EXIT_FAILURE);
  }
  
  id3version[0] = id3header[3];
  id3version[1] = id3header[4];
  
  id3flags = id3header[5];
  
  id3size = calcsize(&id3header[6]);
  
  printf("ID3v2 Information for file %s:\n", argv[1]);
  printf("ID3v2 version number: %i.%i\n", id3version[0], id3version[1]);
  
  printf("Using unsynchronization: ");
  if((id3flags & 0x80) == 0x80)
    printf("yes\n");
  else
    printf("no\n");
  
  printf("Extended header present?: ");
  if((id3flags & 0x40) == 0x40)
    printf("yes\n");
  else
    printf("no\n");
  
  printf("Tag in experimental stage?: ");
  if ((id3flags & 0x20) == 0x20)
    printf("yes (WARNING: This program may have trouble interpreting this file!\n");
  else
    printf("no\n");
  
  printf("Size of tag (excluding 10B header): %i bytes", id3size);
  
  if(id3size > MEGABYTE)
    printf(" (%i MiB)", id3size / MEGABYTE);
  else if(id3size > KILOBYTE)
    printf(" (%i KiB)", id3size / KILOBYTE);
  
  printf("\n");
  
  
  int i;
  for(i = 0; i < 6; i++){
    if((frame = getframe()) != NULL){
      printframe(frame);
      free(frame);
      frame = NULL;
    }
  }
  
  (void) close(fd);
  
  return return_code;
}
