#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "id3v2.h"

int readcheck(FILE *fp, char *buffer, size_t nbytes){
  int return_code = 1;
  size_t charsread;
  
  charsread = fread(buffer, 1, nbytes, fp);
    
  if(charsread == nbytes){ /* successful read */
      return_code = 0;
  }
  else if(charsread == 0){ /* read hit end-of-file */
    fprintf(stderr, "%s: unexpectedly reached end of file %s\n",
            progname, filename);
    return_code = 1;
  }
  else if(charsread == -1){ /* read encountered an error */
    perror(progname);
    return_code = 1;
  }
  else{ /* did not read n bytes */
    fprintf(stderr, "%s: could not read %d bytes from file %s\n",
            progname, nbytes, filename);
    return_code = 1;
  }
  
  return return_code;
}

int getid(char *name){
  int id = 1;
  
  for(id = 1; id < 73; id++){
    if(strncmp(idlist[id], name, 4) == 0)
      return id;
  }
  
  return 0;
}

id3v2header *getheader(FILE *fp){
  static id3v2header header = {0, 0, 0, 0, 0, 0};
  char buffer[10];
  
  
  printf("reading now\n");
  
  if((fseek(fp, 0, SEEK_SET) == 0) &&
     (readcheck(fp, buffer, 10) == 0) &&
	 
  
     (buffer[0] == 'I') && // 0x49
     (buffer[1] == 'D') && // 0x44
     (buffer[2] == '3') && // 0x33
     (buffer[3] < 0xFF) &&
     (buffer[4] < 0xFF) &&
     ((buffer[5] & 0x0F) == 0x00) &&
     (buffer[6] < 0x80) &&
     (buffer[7] < 0x80) &&
     (buffer[8] < 0x80) &&
     (buffer[9] < 0x80)) {
     
    /* ID3v2 header is valid */
    header.version = buffer[3];
    header.revision = buffer[4];
    
    /* About size calculation: Multiplying by 128 is the same as shifting
        by 7 bits (which was my original method) but removes the worry of
        endianness of integer storage. Same with the addition (I OR'd before) */
    
    header.size = buffer[6];
    header.size *= 128;
    header.size += buffer[7];
    header.size *= 128;
    header.size += buffer[8];
    header.size *= 128;
    header.size += buffer[9];
    
    if((buffer[5] & 0x80) == 0x80)
      header.unsync = 1;
    else
      header.unsync = 0;
    
    if((buffer[5] & 0x40) == 0x40)
      header.extheader = 1;   // TODO: Parse extended header data
    else
      header.extheader = 0;
    
    if((buffer[5] & 0x20) == 0x20)
      header.experimental = 1;
    else
      header.experimental = 0; 
  }
  else{
    /* ID3v2 header is not valid or some other error occurred.*/
    return NULL;
  }
  
  return &header;
}

id3v2frame *getframe(FILE *fp){
  static id3v2frame frame = {0, 0, 0, 0, 0, 0, 0, 0, NULL};
  char buffer[10];
  
  if(frame.body != NULL){
    free(frame.body);
  }
    
  frame.body = NULL;
  frame.id = 0;
  frame.tag_alter_discard = 0;
  frame.file_alter_discard = 0;
  frame.read_only = 0;
  frame.compressed = 0;
  frame.encrypted = 0;
  frame.grouped = 0;
  
  if((readcheck(fp, buffer, 10) == 0) &&
     ((frame.id = getid(buffer)) != 0)){
    
    frame.size = buffer[4];
    frame.size *= 128;
    frame.size += buffer[5];
    frame.size *= 128;
    frame.size += buffer[6];
    frame.size *= 128;
    frame.size += buffer[7];
    
    if((buffer[8] & 0x80) == 0x80)
      frame.tag_alter_discard = 1;
    
    if((buffer[8] & 0x40) == 0x40)
      frame.file_alter_discard = 1;
    
    if((buffer[8] & 0x20) == 0x20)
      frame.read_only = 1;
    
    if((buffer[9] & 0x80) == 0x80)
      frame.compressed = 1;
    
    if((buffer[9] & 0x40) == 0x40)
      frame.encrypted = 1;
    
    if((buffer[9] & 0x20) == 0x20)
      frame.grouped = 1;
  
    if(((frame.body = (char *) malloc(frame.size)) == NULL) ||
       (readcheck(fp, frame.body, frame.size) != 0))
      return NULL;
  }
  return &frame;
}

void printheader(id3v2header *header){
  extern char *filename;
  
  printf("ID3v2 Information for file %s:\n", filename);
  printf("ID3v2 version number: %i.%i\n", header->version, header->revision);
  
  printf("Using unsynchronization: ");
  if(header->unsync == 1)
    printf("yes\n");
  else
    printf("no\n");
  
  if(header->extheader == 1)
    printf("Extended header present after current header\n");
  else
    printf("No extended header after current header\n");
  
  if(header->experimental == 1)
    printf("Tag is in an experimental state. WARNING: This means this program"
           " could fail!\n");
  else
    printf("Tag is not in an experimental state\n");

  
  printf("Size of tag (excluding 10B header): %d bytes", header->size);

  
  if(header->size > MEGABYTE)
    printf(" (%d MiB)", header->size / MEGABYTE);

  else if(header->size > KILOBYTE)
    printf(" (%d KiB)", header->size / KILOBYTE);

  
  printf("\n");
}

void printframe(id3v2frame *frame, int verbose){

  char *frametext;
  
  if(verbose == 1){  // TODO: #define constants for verbosity options
  
    printf("Frame ID: %s\n", idlist[frame->id]);
    printf("Size of frame (excluding 10B header): %d bytes", frame->size);
  
   if(frame->size > MEGABYTE)
     printf(" (%i MiB)", frame->size / MEGABYTE);
    else if(frame->size > KILOBYTE)
      printf(" (%i KiB)", frame->size / KILOBYTE);
  
    printf("\n");
  
    printf("Flags:\n");
  
    printf("If tag is altered: ");
   if(frame->tag_alter_discard == 1)
      printf("Frame should be discarded\n");
    else
      printf("Frame should be preserved\n");
    
    printf("If file is altered: ");
    if(frame->file_alter_discard == 1)
      printf("Frame should be discarded\n");
    else
      printf("Frame should be preserved\n");
  
    if(frame->read_only == 1)
      printf("Frame is intended to be read only\n");
    else
      printf("Frame is not intended to be read only\n");
    
    if(frame->compressed == 1)
      printf("Frame is compressed\n");
    else
      printf("Frame is not compressed\n");
    
    if(frame->encrypted == 1)
      printf("Frame is encrypted\n");
    else
      printf("Frame is not encrypted\n");
    
    if(frame->grouped == 1)
      printf("Frame is in a group with other frames\n");
    else
      printf("Frame is not in a group with other frames\n"); 
      
      
    printf("Frame content:\n");
  }
  
  if(frame->id > 21 && frame->id < 60){ /* frame is a text frame */
    frametext = malloc(frame->size);
    memcpy(frametext, frame->body+1, (frame->size)-1);
    frametext[(frame->size)-1] = '\0';
  }
  
  if (frame->id == 60)
    printf("Year recorded: %s\n", frametext);
  else if(frame->id == 30)
    printf("Encoded with: %s\n", frametext);
  else if(frame->id == 24)
    printf("Beats per minute: %s\n", frametext);
  else if(frame->id == 26)
    printf("Content type: %s\n", frametext);
  else if(frame->id == 47)
    printf("Lead Artist or Group: %s\n", frametext);
  else if(frame->id == 3){
    if(verbose == 1)
      printf("Comment language: %.*s\n", 3, frame->body+1);
    printf("Short comment: %s\n", frame->body+5);
  }
  return;
}
