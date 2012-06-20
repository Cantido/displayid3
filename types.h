#ifndef TYPES_H
#define TYPES_H

typedef unsigned char BYTE;

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
  BYTE version;
  BYTE revision;
  size_t size;
  BYTE unsync       : 1;
  BYTE extheader    : 1;
  BYTE experimental : 1;
} id3v2header;

typedef struct{
  int id;
  size_t size;
  BYTE tag_alter_discard  : 1;
  BYTE file_alter_discard : 1;
  BYTE read_only          : 1;
  BYTE compressed         : 1;
  BYTE encrypted          : 1;
  BYTE grouped            : 1;
  BYTE *body;
} id3v2frame;



#endif
