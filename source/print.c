#include "common.h"
#include "print.h"

void	printHeader(void)
{
	printf("\x1b[0;0H");
	printf("================================================================================");
	printf("%s%55s%s", CONSOLE_YELLOW, "Twili Disabler (By SegFault42)\n", CONSOLE_RESET);
	printf("================================================================================");
}

void	drawButton(char *string, char *color)
{
	printf("\x1b[20;0H");

	printf("%31s ________________\n", "");
	printf("%31s|                |\n", "");
	printf("%31s|                |\n", "");
	printf("%31s|%s%16s%s|\n", "", color, string, CONSOLE_RESET);
	printf("%31s|                |\n", "");
	printf("%31s|                |\n", "");
	printf("%31s ----------------\n", "");
}

void	warningMessage(char *str)
{
	int	align = 0;
	u64	kDown = 0;

	// align to vertical center
	if (strlen(str) < 80) {
		align = 40 - (strlen(str) / 2);
	}

	while (1) {
		hidScanInput();
		kDown = hidKeysDown(CONTROLLER_P1_AUTO);

		printf("\x1b[25;%dH", align);

		printf("%s%s%s", CONSOLE_RED, str, CONSOLE_RESET);

		printf("\x1b[27;32HPush + to exit !");

		if (kDown & KEY_PLUS) {
			break ;
		}
		consoleUpdate(NULL);
	}
}

void	progressBar(double TotalToDownload, double NowDownloaded)
{
	int		totaldotz=70;
	double	fractiondownloaded = NowDownloaded / TotalToDownload;
	int		dotz = round(fractiondownloaded * totaldotz);
	int		meter=0;

	// ensure that the file to be downloaded is not empty
	// because that would cause a division by zero error later on
	if (TotalToDownload <= 0.0) {
		return ;
	}

	printf("%3.0f%% [",fractiondownloaded*100);

	// part  that's full already
	for ( ; meter < dotz; meter++) {
		printf("=");
	}

	// remaining part (spaces)
	for ( ; meter < totaldotz; meter++) {
		printf(" ");
	}

	// and back to line begin
	printf("]\r");

	return ;
}

