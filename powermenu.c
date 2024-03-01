#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include <unistd.h>
#include <sys/reboot.h>

typedef enum {
    cancel,
    poweroff,
    reboot,
    suspend,
} power_msg;

typedef struct {
    const char *name;
    power_msg msg;
} power_options;

/* no need to put the definitions in config! */
#include "config.h"

void handler(power_msg msg) {
    switch (msg) {
        case cancel:
            printf("Cancelled\n");
            break;
        case poweroff:
	    sync();
	    reboot(RB_POWER_OFF);
            break;
        case reboot:
	    sync();
	    reboot(RB_AUTOBOOT);
            break;
        case suspend:
	    sync();
	    reboot(RB_SW_SUSPEND);
            break;
        default:
            break;
    }
}

void match(const char *o) {
    if (o == NULL)
        exit(1);

    for (int i = 0; i < sizeof(options) / sizeof(options[0]); ++i) {
        if (!strncmp(options[i].name, o, strlen(options[i].name)))
            handler(options[i].msg);
    }
}

const char *parse_o(char *cmd) {
    static char buf[PATH_MAX];
    FILE *pf;

    if ((pf = popen(cmd, "r")) == NULL) {
        printf("Failed to open stream that can be used to read or write to the pipe!\n");
        exit(1);
    }

    while (fgets(buf, PATH_MAX, pf) != NULL);

    if (pclose(pf)) {
        printf("Command exited with an error status, or the user pressed ESC.\n");
        exit(1);
    }

    return buf;
}

int main(int argc, char *argv[]) {
    static char cmd[PATH_MAX]; 
    
    strcat(cmd, "echo \"");

    for (int i = 1; i < argc; ++i) {
	if (!strcmp(argv[i], "-p"))
	    handler(poweroff);
	else if (!strcmp(argv[i], "-r"))
	    handler(reboot);
	else if (!strcmp(argv[i], "-s"))
	    handler(suspend);
    }

    int i = 0;  
    for (; i < ((sizeof(options) / sizeof(options[0])) - 1); ++i) {
        strncat(cmd, options[i].name, strlen(options[i].name));
        strcat(cmd, "\n");
    }

    strncat(cmd, options[i].name, strlen(options[i].name));
    strcat(cmd, "\" | dmenu");

    match(parse_o(cmd));

    return 0;
}
