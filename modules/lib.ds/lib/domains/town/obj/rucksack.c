#include <lib.h>

#include <armor_types.h>
#include <damage_types.h>
inherit LIB_WORN_STORAGE;

static void create() {
    ::create();
    SetKeyName("rucksack");
    SetId(({"ruck","sack"}));
    SetAdjectives(({"sturdy","green","olive","olive-green","od green","od-green"}));
    SetShort("a sturdy, olive-green rucksack");
    SetLong("This is a large, sturdy rucksack made of some sort of "
      "strong, canvas-like material. It appears suitable "+
      "for just about any adventure you'd find yourself in.");
    SetMass(100);
    SetDollarCost(400);
    SetCanClose(1);
    SetClosed(1);
    SetMaxCarry(2000);
    SetDamagePoints(100);
    SetArmorType(A_VEST);
    SetInventory(([
        "/domains/town/obj/torch" : 1,
        "/domains/town/obj/match" : 3,
        "/domains/town/obj/map" : 1,
        "/domains/town/meals/ale" : 3,
        "/domains/town/meals/poison" : 1,
      ]));
}
void init(){
    ::init();
}
