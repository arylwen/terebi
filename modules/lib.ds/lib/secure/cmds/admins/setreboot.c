#include <lib.h>
#include <daemons.h>

inherit LIB_DAEMON;

int cmd(string str) {
    int x;

    if( !((int)master()->valid_apply(({ "ASSIST" }))) ){
        return 0;
    }

    if(!str || str == "" || !sscanf(str, "%d", x)){
        this_object()->help();
    }
    x = (int)EVENTS_D->SetRebootInterval(x);
    message("info", "Reboot interval set to "+x+" hours.", this_player());
    return 1;
}

void help() {
    message("help", "Syntax: <setreboot [interval]>\n\n"
      "Sets up the interval at which the mud will automatically reboot.  "
      "The interval should be specified in hours.",
      this_player()
    );
}
