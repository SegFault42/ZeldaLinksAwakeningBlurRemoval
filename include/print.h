#ifndef PRINT_H
#define PRINT_H

#include "common.h"

typedef void (*ptrProgressBar)(double, double);

void	printHeader(void);
void	drawButton(char *string, char *color);
void	warningMessage(char *str);
void	progressBar(double TotalToDownload, double NowDownloaded);

#endif
