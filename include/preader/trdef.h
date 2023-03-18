#ifndef _TRDEF_H
#define _TRDEF_H
typedef struct
{
  unsigned short id, type;
  unsigned long fsize, dsize;
  unsigned int property;
  unsigned long address;
} file_type_t;

typedef struct {
    file_type_t fProperties;
    char pathname[32];
} file_info_t;
#endif