#include <lib.h>
inherit LIB_ROOM;

static void create() {
    room::create();
    SetClimate("indoors");
    SetAmbientLight(30);
    SetShort("Game room");
    SetLong("You are in the game room. The walls have been paneled "
      "in beautiful oak, and the thick carpeting "
      "feels quite nice under your feet.");
    SetItems( ([
        ({"wall","walls"}) : "It's beautiful oak paneling, "
        "with a deep, rich coat of stain.",
        ({"carpet","carpeting"}) : "It's plush, soft, and "
        "very comfortable."
      ]) );
    AddStuff( ({
        "/domains/town/obj/pool_table"
      }) );
    SetExits( ([
        "east" : "/domains/town/room/mansion_uhall2"
      ]) );
}
void init(){
    ::init();
}
