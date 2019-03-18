#include <QApplication>
#include <QFont>
#include <stdlib.h>
#include <signal.h>
#include <QDir>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <execinfo.h>
#include <string.h>

#include "ui_application.h"
#include "ui_frame.h"
#include "deftools/defcontrols.h"
#include "deftools/datastruct.h"

#include "ui_mainwindow.h"

int is_dir_exist(const char* dir_path)
{
    if(dir_path == NULL)
    {
        return -1;
    }
    if(-1 == access(dir_path, F_OK)){         
         if(-1 == mkdir(dir_path, 0777)){
             return -1;
         }
     }
    return 0;
}

void info_to_file(int sig)
{
    time_t tSetTime;
    time(&tSetTime);
    struct tm* ptm = localtime(&tSetTime);
    char fname[256] = {0};
    sprintf(fname, "core.%d-%d-%d_%d_%d_%d",
            ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday,
            ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    
    char *path = getenv("HOME");

    char *pangen_path = new char[22];
    strcpy(pangen_path, "/.pangen_gui/pgui_core");
    
    strcat(path, pangen_path);
    if (-1 == is_dir_exist(path))
    {
        printf("core dir make error!");
    }

    char *temppath = new char[1];
    strcpy(temppath, "/");
    strcat(path, temppath);
    strcat(path, fname);
    FILE* f = fopen(path, "a");
    if (f == NULL){
        return;
    }
    int fd = fileno(f);

    //lock file
    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;
    fl.l_pid = getpid();
    fcntl(fd, F_SETLKW, &fl);

    //output app path
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));
    int count = readlink("/proc/self/exe", buffer, sizeof(buffer));
    if(count > 0){
        buffer[count] = '\n';
        buffer[count + 1] = 0;
        fwrite(buffer, 1, count+1, f);
    }
    //output time
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "Dump Time: %d-%d-%d %d:%d:%d\n",
            ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday,
            ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    fwrite(buffer, 1, strlen(buffer), f);

    //thread and signal
    sprintf(buffer, "Curr thread: %u, Catch signal:%d\n",
            (int)pthread_self(), sig);
    fwrite(buffer, 1, strlen(buffer), f);

    //stack
    void* DumpArray[256];
    int nSize = backtrace(DumpArray, 256);
    sprintf(buffer, "backtrace rank = %d\n", nSize);
    fwrite(buffer, 1, strlen(buffer), f);
    if (nSize > 0){
        char** symbols = backtrace_symbols(DumpArray, nSize);
        if (symbols != NULL){
            for (int i=0; i<nSize; i++){
                fwrite(symbols[i], 1, strlen(symbols[i]), f);
                fwrite("\n", 1, 1, f);
            }
            free(symbols);
        }
    }

    //file unlock and unlock
    fl.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &fl);
    fclose(f);
}

void qt_gui_exit()
{
    logger_file("Quit the application");
     qDebug() << "Quit the application";
}

void signal_exit_handler(int handel)
{
    logger_file("Quit the application");
    qDebug() << "Quit the application, handler[SIGNAL]: "<< handel;

}

void signal_crash_handler(int handel)
{
    qDebug() << "GUI crash [SIGNAL]: "<< handel;
    const int len=1024;
    void *func[len];
    size_t size;
    char **funs;
    size = backtrace(func,len);
    funs = (char**)backtrace_symbols(func,size);
    qDebug() << "System error, Stack trace:";
    for(unsigned int i = 0; i < size; i ++)
    {
       fprintf(stderr,"%d %s \n",i,funs[i]);
    }
    free(funs);
    info_to_file(handel);
}

int main(int argc, char *argv[])
{
    ui::GuiApplication a(argc, argv);
    atexit(qt_gui_exit);
    signal(SIGTERM, signal_exit_handler);
 //   signal(SIGINT, signal_exit_handler);

    signal(SIGPIPE, SIG_IGN);
    signal(SIGBUS, signal_crash_handler);    
    signal(SIGSEGV, signal_crash_handler);   
    signal(SIGFPE, signal_crash_handler);     
    signal(SIGABRT, signal_crash_handler);   

    qApp->setStyle(new ui::ProxyStyle);
    QFont font;
    font.setFamily("Sans Serif");
    font.setPointSize (9);
    a.setFont(font);
    ui::MainWindow w;
    w.show();
    return a.exec();
}
