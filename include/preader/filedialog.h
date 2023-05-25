#include <preader/common_definitions.h>
#ifndef _FILEDIALOG_H
#define _FILEDIALOG_H
extern int fileMenuSelect(file_info_t files[], int max_n, char *pathname, char *filter);
int openFileDialogDisclosure(const char *pathname, const char *filter, char *filename);
int browseAndOpenFileI(const char *path, const char *filter, char *filename_chosen);
int duplicateBackSlashes(char *str);
#endif