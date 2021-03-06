#ifndef l_open_h
#define l_open_h

static void create();

//varargs mixed can_open_obj(object ob, mixed *args...);
//varargs mixed can_open_obj_with_obj(object ob, object with, mixed *args...);
mixed can_open_obj(string verb, string id);
mixed can_open_obj_with_obj(string verb, string id1, string id2);

varargs mixed do_open_obj(object ob, mixed *args...);
varargs mixed do_open_obj_with_obj(object ob, object with, mixed *args...);

#endif /* l_open_h */
