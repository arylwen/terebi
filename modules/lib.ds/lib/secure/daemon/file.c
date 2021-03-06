#include <lib.h>
#include <config.h>
#include <daemons.h>
#include <runtime_config.h>

inherit LIB_DAEMON;
string *all_dirs = ({});
string *all_files = ({});
static string SaveFiles = "/secure/save/files.o";
int ftilt, dtilt;
string globaltemp;

static mixed Report();

static private void validate() {
    if( !((int)master()->valid_apply(({ "ASSIST" }))) ){
        log_file("adm/file","Illegal attempt to access FILE_D: "+get_stack()+" "+identify(previous_object(-1))+"\n");
        error("Illegal attempt to access FILE_D: "+get_stack()+" "+identify(previous_object(-1)));
    }
    if(query_os_type() == "windows"){
        error("The file daemon has been disabled for your mud "+
          "because it is running windows. Intensive file operations "+
          "in windows are not yet supported on Dead Souls.");
    }
}

void heart_beat(){
    foreach(mixed arr in call_out_info()){
        if(arr[0] == this_object()){
            return ;
        }
    }
    Report();
    unguarded( (: save_object(SaveFiles) :) );
    set_heart_beat(0);
}

mixed ReadDir(string str){
    string *current_level_dirs = ({});
    int iteration = 1;
#ifndef __FLUFFOS__
    return 0;
#endif

    validate();
    //log_file("adm/file","FILE_D ReadDir accessed and run by: "+identify(previous_object(-1))+"\n");

    if(!str ||  !sizeof(str)) str = "/";

    if(!query_heart_beat()) set_heart_beat(1);
    if(last(str,1) != "/") str += "/";
    if(directory_exists(str)){
        foreach(string element in get_dir(str)){
            if(file_exists(str+element)) all_files += ({ str+element });
            if(directory_exists(str+element) && 
              strsrch(str+element,"/realms") &&
              strsrch(str+element,"/estates") &&
              strsrch(str+element,"/secure/save") &&
              strsrch(str+element,"/secure/log") &&
              strsrch(str+element,"/secure/upgrades")) {
                all_dirs += ({ str+element });
                current_level_dirs += ({ str+element });
            }
        }
        if(sizeof(current_level_dirs)){
            foreach(string element in current_level_dirs){
                iteration++;
                globaltemp = element;
                call_out( (: ReadDir, globaltemp :), iteration );
            }
        }
    }
    return 1;
}

static mixed Report(){
    log_file("adm/file","FILE_D Report accessed and run by: "+identify(previous_object(-1))+"\n");
    foreach(mixed arr in call_out_info()){
        if(arr[0] == this_object()){
            write("File scan is not complete.");
            return 1;
        }
    }
    all_dirs = sort_array(all_dirs,1);
    all_dirs = distinct_array(all_dirs);
    all_files = sort_array(all_files,1);
    all_files = distinct_array(all_files);
    rm("/secure/tmp/dirs.txt");
    rm("/secure/tmp/files.txt");
    foreach(string dir in all_dirs){
        write_file("/secure/tmp/dirs.txt",dir+"\n");
    }
    foreach(string dir in all_files){
        write_file("/secure/tmp/files.txt",dir+"\n");
    }
    return 1;
}

string *GetFiles(){
    return all_files;
}

string *GetDirs(){
    return all_dirs;
}

int SearchFiles(string str){
    int cased = 1;
    int strict = 0;
    string ret = "";
#ifndef __FLUFFOS__
    return 0;
#endif

    //log_file("adm/file",timestamp()+" FILE_D search for "+str+" run by: "+identify(previous_object(-1))+"\n");

    if(!str || str == ""){
        write("Please specify the filename or substring you're looking for.");
        return 1;
    }

    if(grepp(str,"-i ")){
        cased = 0;
        str = replace_string(str,"-i ","");
    }

    if(grepp(str,"-s ")){
        strict = 1;
        str = replace_string(str,"-s ","");
    }

    foreach(string element in all_files){
        string tmpstr, tmpelement;

        if(!cased){ 
            tmpelement = lower_case(element);
            tmpstr = lower_case(str);
        } 
        else {
            tmpelement = element;
            tmpstr = str;
        }  

        if(!strict && grepp(last_string_element(tmpelement,"/"), tmpstr)) ret += element + "\n"; 
        else if(strict && last_string_element(tmpelement,"/") == tmpstr) ret += element + "\n";
    }
    write("Matches:");
    if(sizeof(ret)) write(ret);
    //write_file("/tmp/search.out", ret, 1);
    else write("None.");
    return 1;
}

static void create() {
    object fun_d = find_object(FUNCTION_D);
#ifndef __FLUFFOS__
    return 0;
#endif
    daemon::create();
    if(!file_exists(SaveFiles)){
        unguarded( (: save_object(SaveFiles) :) );
    }
    else restore_object(SaveFiles);
    //call_out((: ReadDir,"/" :), 1);
    catch( ReadDir("/") );
    if(!fun_d) fun_d = load_object(FUNCTION_D);
}

int eventDestruct(){
    unguarded( (: save_object(SaveFiles) :) );
    return ::eventDestruct();
}

