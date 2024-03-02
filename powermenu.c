#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include <unistd.h>
#include <sys/reboot.h>
#include <sys/sysinfo.h>

typedef enum {
    cancel,
    power_poweroff,
    power_reboot,
    power_suspend,
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
        case power_poweroff:
	    reboot(RB_POWER_OFF);
            break;
        case power_reboot:
	    reboot(RB_AUTOBOOT);
            break;
        case power_suspend:
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

const char *get_uptime() {
    struct sysinfo si;
    static char up[PATH_MAX / 4];

    if (sysinfo(&si)) {
        printf("There was an error retrieving the system's uptime\n");
	return default_prompt;
    }

    double days = si.uptime / 86400;
    double hours = (si.uptime / 3600) - (days * 24);
    double minutes = (si.uptime / 60) - (days * 1440) - (hours * 60);

    sprintf(up, "Uptime: %.0fd, %.0fh, %.0fm", days, hours, minutes);

    return up;
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        if (!strcmp(argv[1], "-p")) {
            handler(power_poweroff);
            return 0;
        } else if (!strcmp(argv[1], "-r")) {
            handler(power_reboot);
            return 0;
        } else if (!strcmp(argv[1], "-s")) {
            handler(power_suspend);
    	    return 0;
        } else if (!strcmp(argv[1], "-u")) {
            printf("%s\n", get_uptime());
    	    return 0;
        }
    }

    static char cmd[PATH_MAX]; 
    
    strcat(cmd, "echo \"");

    int i = 0;  
    for (; i < ((sizeof(options) / sizeof(options[0])) - 1); ++i) {
        strncat(cmd, options[i].name, strlen(options[i].name));
        strcat(cmd, "\n");
    }

    strncat(cmd, options[i].name, strlen(options[i].name));

    char final[PATH_MAX / 4]; 
    sprintf(final, "\" | dmenu -p \"%s\"", get_uptime());
    strncat(cmd, final, strlen(final));

    match(parse_o(cmd));

    return 0;
}
