#include "common.h"
#include "print.h"
#include "reboot.h"

#define BUFF_SIZE (1024 * 16)

bool	isFileExist(const char *file)
{
	struct stat	st = {0};

	if (stat(file, &st) == -1) {
		return (false);
	}

	return (true);
}

bool	getTwiliState(void)
{
	return (isFileExist("sdmc:/atmosphere/titles/0100000000006480/flags/boot2.flag"));
}

bool	copyFile(const char *dest, const char *src, ptrProgressBar progress)
{
	int		fd_src = 0;
	int		fd_dest = 0;
	ssize_t	ret = 0;
	char	buff[BUFF_SIZE] = {0};
	double	downNow = 0;
	struct stat	st;

	fd_src = open(src, O_RDONLY);
	if (fd_src == -1) {
		return (false);
	}

	fd_dest = open(dest, O_RDWR | O_TRUNC | O_CREAT, 0600);
	if (fd_dest == -1) {
		close(fd_src);
		return (false);
	}

	if (fstat(fd_src, &st) == -1) {
		close(fd_src);
		close(fd_dest);
		return (false);
	}

	while ((ret = read(fd_src, &buff, BUFF_SIZE)) > 0) {
		downNow += ret;
		if (progress != NULL)
		{
			printf("\x1b[35;0H");
			(*progress)(st.st_size, downNow);
			consoleUpdate(NULL);
		}
		write(fd_dest, buff, ret);
	}

	close(fd_src);
	close(fd_dest);

	return (true);
}

bool	enableTwili(void)
{
	if (copyFile("sdmc:/atmosphere/hbl.nsp", "sdmc:/switch/twili_disabler/twili_hbl.nsp", progressBar) == false) {
		warningMessage("Copy of (twili) hbl.nsp failed");
		return (false);
	}

	if (copyFile("sdmc:/atmosphere/titles/0100000000006480/flags/boot2.flag", "sdmc:/switch/twili_disabler/boot2.flag", progressBar) == false) {
		warningMessage("Copy of boot2.flag failed");
		return (false);
	}

	return (true);
}


bool	disableTwili(void)
{
	// install stock hbl
	if (copyFile( "sdmc:/atmosphere/hbl.nsp", "sdmc:/switch/twili_disabler/stock_hbl.nsp", progressBar) == false) {
		warningMessage("Copy of (stock) hbl.nsp failed");
		return (false);
	}

	// remove boot2.flag
	remove("sdmc:/atmosphere/titles/0100000000006480/flags/boot2.flag");

	return (true);
}

void	checkNeededFile(void)
{
	u64		kDown = 0;
	bool	err = false;
	char	*files[] = {
		"sdmc:/switch/twili_disabler",
		"sdmc:/switch/twili_disabler/boot2.flag",
		"sdmc:/switch/twili_disabler/twili_hbl.nsp",
		"sdmc:/switch/twili_disabler/stock_hbl.nsp",
		NULL
	};

	printf("Check requiered files :\n\n");

	// Check all files
	for (int i = 0; files[i]; i++) {
		if (isFileExist(files[i]) == false) {
			printf("[%sKO%s] %s\n", CONSOLE_RED, CONSOLE_RESET, files[i]);
			err = true;
		} else {
			printf("[%sOK%s] %s\n", CONSOLE_GREEN, CONSOLE_RESET, files[i]);
		}
	}

	// If one of file is missing, print error
	if (err == true) {
		while (1) {
		hidScanInput();
			kDown = hidKeysDown(CONTROLLER_P1_AUTO);

			printf("%s\x1b[25;15H" "Please put missing files in /switch/twili_disabler/", CONSOLE_YELLOW);
			printf("\x1b[27;35H" "Press + to quit\n%s", CONSOLE_RESET);

			if (kDown & KEY_PLUS) {
				consoleExit(NULL);
				exit (-1);
			}
			consoleUpdate(NULL);
		}
	}
}

int main(void)
{
	u64	kDown = 0;

	consoleInit(NULL);

	// Check if file are here
	checkNeededFile();

	while (appletMainLoop()) {
		consoleClear();
		hidScanInput();
		kDown = hidKeysDown(CONTROLLER_P1_AUTO);

		printHeader();

		if (getTwiliState() == true) {
			drawButton("Twili Enabled !", CONSOLE_GREEN);
			printf("\x1b[43;0H");
			printf("Press A to %sDisable%s Twili\n", CONSOLE_RED, CONSOLE_RESET);

			if (kDown & KEY_A) {
				if (disableTwili() == false) {
					break ;
				}
			}
		} else {
			drawButton("Twili Disabled !", CONSOLE_RED);
			printf("\x1b[43;0H");
			printf("Press A to %sEnable%s Twili\n", CONSOLE_GREEN, CONSOLE_RESET);

			if (kDown & KEY_A) {
				if (enableTwili() == false) {
					break ;
				}
			}
		}

		/*printf("\x1b[49;0H");*/
		printf("Press + to %sExit%s\n", CONSOLE_RED, CONSOLE_RESET);
		/*printf("\x1b[50;0H");*/
		printf("Press - to %sReboot%s (Need to reboot after enable/dsable twili)", CONSOLE_RED, CONSOLE_RESET);

		if (kDown & KEY_PLUS) {
			break;
		}
		if (kDown & KEY_MINUS) {
			reboottopayload("/atmosphere/reboot_payload.bin");
		}

		consoleUpdate(NULL);
	}

	consoleExit(NULL);

	return (0);
}
