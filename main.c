#include "gameloader.h"
//#include "gamestate.h"
//#include "mprint.h"
//#include <dlfcn.h>
//#include <raylib.h>
//#include <stdbool.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <sys/stat.h>
//#include <time.h>
//#include <unistd.h>

//const char* libname = "./libgame.so";
//const char* lockfile = "./libgame.so.lockfile";
//const char* templib = "./templibgame.so";

//void* handle = NULL;

//void (*MyGame_gamestate_destroy)(Gamestate*) = NULL;
//void (*MyGameRun)() = NULL;

//time_t last_write_time = 0;
//int old_frame_count = 0;
//Gamestate* old_gamestate = NULL;

// get the last write time of a file
//time_t GetLastWriteTime(const char* filename) {
//    struct stat file_stat;
//    time_t retval = 0;
//    if(stat(filename, &file_stat) == 0) {
//        retval = file_stat.st_mtime;
//    }
//    return retval;
//}

//void LoadSymbols() {
//    MyGame_gamestate_destroy = (void (*)(Gamestate*))dlsym(handle, "Gamestate_destroy");
//    MyGameRun = dlsym(handle, "GameRun");
//    if(MyGame_gamestate_destroy == NULL || MyGameRun == NULL) {
//        fprintf(stderr, "dlsym failed: %s\n", dlerror());
//        exit(1);
//    }
//}

//void OpenHandle() {
//    handle = dlopen(libname, RTLD_LAZY);
//    if(!handle) {
//        fprintf(stderr, "dlopen failed: %s\n", dlerror());
//        exit(1);
//    }
//}

//void AutoReload() {
//    if(GetLastWriteTime(libname) > last_write_time) {
//        last_write_time = GetLastWriteTime(libname);
//        while(FileExists(lockfile)) {
//            printf("Library is locked\n");
//            sleep(1);
//        }
//        mPrint("Getting old gamestate");
//        old_gamestate = MyGame_get_gamestate();
//        mPrint("Closing window and unloading library");
//        MyCloseWindow();
//        mPrint("Unloading library");
//        dlclose(handle);
//        mPrint("Opening handle");
//        OpenHandle();
//        mPrint("Loading symbols");
//        LoadSymbols();
//        mPrint("Reloading window with old gamestate");
//        MyInitWindowWithGamestate(old_gamestate);
//    }
//}

int main() {
    //OpenHandle();
    //last_write_time = GetLastWriteTime(libname);
    //LoadSymbols();
    gamerun();
    //MyGame_gamestate_destroy(old_gamestate);
    //dlclose(handle);
    return 0;
}
