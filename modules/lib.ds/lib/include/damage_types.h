#ifndef s_damage_types_h
#define s_damage_types_h

#define BLUNT              (1 << 1)
#define BLADE              (1 << 2)
#define KNIFE              (1 << 3)
#define WATER              (1 << 4)
#define SHOCK              (1 << 5)
#define COLD               (1 << 6)
#define HEAT               (1 << 7)
#define GAS                (1 << 8)
#define ACID               (1 << 9)
#define MAGIC              (1 << 10)
#define POISON             (1 << 11)
#define DISEASE            (1 << 12)
#define TRAUMA             (1 << 13)
#define PIERCE             (1 << 14)
#define PSIONIC            (1 << 15)
#define ANOXIA             (1 << 16)
#define DEATHRAY           (1 << 17)
#define EMOTIONAL          (1 << 18)
#define SONIC              (1 << 19)
#define BITE               (1 << 20)
#define OTHER              (1 << 21)

#define MAX_DAMAGE_BIT     OTHER

#define ALL_DAMAGE  BLUNT | BLADE | KNIFE | WATER | SHOCK | COLD | HEAT | GAS | ACID | MAGIC | POISON | DISEASE | TRAUMA | PIERCE | PSIONIC | ANOXIA | DEATHRAY | EMOTIONAL | SONIC | BITE | OTHER

#endif /* s_damage_types_h */
