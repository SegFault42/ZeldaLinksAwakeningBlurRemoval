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

static bool	getModState(void)
{
	return (isFileExist("sdmc:/atmosphere/titles/01006BB00C6F0000/romfs/region_common/shader/postprocess.bfsha"));
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

bool	enableMod(void)
{
	mkdir("sdmc:/atmosphere/titles/01006BB00C6F0000", S_IRWXU);
	mkdir("sdmc:/atmosphere/titles/01006BB00C6F0000/romfs", S_IRWXU);
	mkdir("sdmc:/atmosphere/titles/01006BB00C6F0000/romfs/region_common", S_IRWXU);
	mkdir("sdmc:/atmosphere/titles/01006BB00C6F0000/romfs/region_common/shader", S_IRWXU);

	if (copyFile("sdmc:/atmosphere/titles/01006BB00C6F0000/romfs/region_common/shader/postprocess.bfsha", "sdmc:/switch/ZeldaLinksAwakeningBlurRemoval/postprocess.bfsha", progressBar) == false) {
		warningMessage("Copy of postprocess.bfsha failed");
		return (false);
	}

	return (true);
}


bool	disableMod(void)
{
	// remove mod
	remove("sdmc:/atmosphere/titles/01006BB00C6F0000/romfs/region_common/shader/postprocess.bfsha");

	return (true);
}

void	checkNeededFile(void)
{
	u64		kDown = 0;
	bool	err = false;
	char	files[] = "sdmc:/switch/ZeldaLinksAwakeningBlurRemoval/postprocess.bfsha";

	printf("Check requiered files :\n\n");

	// Check is file is present
	if (isFileExist(files) == false) {
		printf("[%sKO%s] %s\n", CONSOLE_RED, CONSOLE_RESET, files);
		err = true;
	} else {
		printf("[%sOK%s] %s\n", CONSOLE_GREEN, CONSOLE_RESET, files);
	}

	// If one of file is missing, print error
	if (err == true) {
		while (1) {
		hidScanInput();
			kDown = hidKeysDown(CONTROLLER_P1_AUTO);

			printf("%s\x1b[25;15H" "Please put missing files in /switch/ZeldaLinksAwakeningBlurRemoval/postprocess.bfsh", CONSOLE_YELLOW);
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

		if (getModState() == true) {
			drawButton("Mod Enabled !", CONSOLE_GREEN);
			printf("\x1b[43;0H");
			printf("Press A to %sDisable%s mod\n", CONSOLE_RED, CONSOLE_RESET);

			if (kDown & KEY_A) {
				if (disableMod() == false) {
					break ;
				}
			}
		} else {
			drawButton("Mod Disabled !", CONSOLE_RED);
			printf("\x1b[43;0H");
			printf("Press A to %sEnable%s mod\n", CONSOLE_GREEN, CONSOLE_RESET);

			if (kDown & KEY_A) {
				if (enableMod() == false) {
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
