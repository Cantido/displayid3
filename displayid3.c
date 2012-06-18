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

/* start typedefs */

typedef unsigned char BYTE;

typedef union{
  struct{
    BYTE id[3]; /* this will always be 'I' 'D' '3' in a valid ID3 tag */
    BYTE version[2];
    BYTE flags;
    BYTE size[4];
  };
  BYTE buffer[10];
} id3v2header;

typedef union{
  struct{
    BYTE id[4];
    BYTE size[4];
    BYTE flags[2];
  };
  BYTE buffer[10];
} id3v2frameheader;

typedef struct{
  id3v2frameheader header;
  BYTE *body;
} id3v2frame;

/* end typedefs */

/* start external variable declaration */

char *progname;
char *filename;

/* end external variable declarations */

/* start function definitions */

size_t calctagsize(id3v2header *header){
  size_t size = header->size[0];
  size <<= 7;
  size |= header->size[1];
  size <<= 7;
  size |= header->size[2];
  size <<= 7;
  size |= header->size[3];
  return size;
}

size_t calcframesize(id3v2frameheader *frameheader){
  size_t size = frameheader->size[0];
  size <<= 7;
  size |= frameheader->size[1];
  size <<= 7;
  size |= frameheader->size[2];
  size <<= 7;
  size |= frameheader->size[3];
  return size;
}



id3v2header *getheader(int fd){
  extern char *progname;
  extern char *filename;
  id3v2header *header = NULL;
  
  if(fd < 0)
    return NULL;
  
  if(lseek(fd, 0, SEEK_SET) != 0)
    return NULL;
  
  switch (read(fd, header->buffer, 10)){
    case 0:
      /* read hit end-of-file */
      fprintf(stderr, "%s: The file %s has no content", progname, filename);
      return NULL;
      break;
    case -1:
      /* read encountered an error */
      perror(progname);
      return NULL;
      break;
    case 10:
      /* no errors */
      break;
    default:
      /* did not read 10 bytes */
      fprintf(stderr, "%s: could not read 10 bytes from file %s", progname, filename);
      return NULL;
      break;
  }
  
  /* verify that read data is a valid ID3v2 header */
  
  if( header->id[0] == 'I' && // 0x49
      header->id[1] == 'D' && // 0x44
      header->id[2] == '3' && // 0x33
      header->version[0] < 0xFF &&
      header->version[1] < 0xFF &&
      (header->flags & 0x0F) == 0x00 && // version 2.4 has 4 tags at beginning of byte
      header->size[0] < 0x80 &&
      header->size[1] < 0x80 &&
      header->size[2] < 0x80 &&
      header->size[3] < 0x80) {
      /* ID3v2 header is valid */
  }
  else{
    /* ID3v2 header is not valid */
    return NULL;
  }
  
  return header;
}

id3v2frame *getframe(int fd){

  id3v2frameheader *frameheader;
  id3v2frame *frame;
  char* framebody;
  size_t framesize;
  
  if(fd < 0)
    return NULL;
  
  switch (read(fd, frameheader->buffer, 10)){
    case 0:
      /* read hit end-of-file */
      fprintf(stderr, "%s: The file %s has no content", progname, filename);
      return NULL;
      break;
    case -1:
      /* read encountered an error */
      perror(progname);
      return NULL;
      break;
    case 10:
      /* no errors */
      break;
    default:
      /* did not read 10 bytes */
      fprintf(stderr, "%s: could not read 10 bytes from file %s", progname, filename);
      return NULL;
      break;
  }
  
  /* TODO: Check for frame's validity */
  
  framesize = calcframesize(frameheader) + 10;
  
  if((frame = (id3v2frame *) malloc(framesize)) != NULL){
    memcpy(frame, frameheader, 10);
    read(fd, frame+10, framesize-10);
  }
  
  return frame;
}

void printframe(id3v2frame *frame){

  char *idstring
  
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
  
  int fd;
  
  id3v2header current_header;
  id3v2frameheader current_fheader;
  id3v2frame current_frame;
  
  if(argc != 2){
    printf("Usage: %s filename", argv[0]);
    exit(EXIT_FAILURE);
  }
  
  if((fd = open(argv[1], O_RDONLY)) < 0){
    perror(argv[0]);
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
