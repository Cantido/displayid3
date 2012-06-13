/******************************************************************************\
 * verifyid3.c - Displays information in an ID3v2 header
 * Author: Robert Richter - robert.c.richter@gmail.com
 * Date: June 2012
 * Description: Using the rules for an ID3v2 header given at
 *   http://id3.org/id3v2-00, this program reads and displays the information
 *   contained in a file's ID3v2 tag. Uses the same source as my verifyid3
 *   program, available at https://github.com/Cantido/verifyid3
\******************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define MEGABYTE 1048576
#define KILOBYTE 1024

int main(int argc, char *argv[]){
  int return_code;
  
  int fd;
  unsigned char id3header[10];
  unsigned char id3version[2];
  unsigned char id3flags;
  unsigned int id3size;
  
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
  
  id3size = id3header[6];
  id3size <<= 7;
  id3size |= id3header[7];
  id3size <<= 7;
  id3size |= id3header[8];
  id3size <<= 7;
  id3size |= id3header[9];
  id3size += 10; /* The size tag does not include the size of the header */
  
  printf("ID3 Information for file %s:\n", argv[1]);
  printf("ID3 version number: %i.%i\n", id3version[0], id3version[1]);
  
  printf("Using unsynchronization: ");
  if((id3flags & 0x80) == 0x80)
    printf("yes\n");
  else
    printf("no\n");
  
  printf("Using compression: ");
  if ((id3flags & 0x40) == 0x40)
    printf("yes (WARNING: The ID3v2 standard says decoders should ignore tags with this bit set!)\n");
  else
    printf("no\n");
  
  printf("Size of tag (including header): %i bytes", id3size);
  
  if(id3size > MEGABYTE)
    printf(" (%i MiB)", id3size / MEGABYTE);
  else if(id3size > KILOBYTE)
    printf(" (%i KiB)", id3size / KILOBYTE);
  
  printf("\n");
  
  (void) close(fd);
  
  return return_code;
}
