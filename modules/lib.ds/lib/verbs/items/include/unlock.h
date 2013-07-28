#ifndef l_unlock_h
#define l_unlock_h

static void create();

//varargs mixed can_unlock_obj_with_obj(object ob, object key, mixed *args...);
mixed can_unlock_obj_with_obj();

//varargs mixed do_unlock_obj_with_obj(object ob, object key, mixed *args...);
mixed do_unlock_obj_with_obj(object target, object key);

string GetHelp(string str);

#endif /* l_unlock_h */
