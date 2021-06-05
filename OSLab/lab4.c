#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <time.h>
#include <grp.h>

void printdir(char const *dir, int depth);
void mode2str(int mode, char str[]);

int main(int argc, char const *argv[])
{
    if(argc == 2)
    {
        printdir(argv[1],0);
    }
    else
    {
        printf("Arguments are unmatch\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}

void printdir(char const *dir, int depth)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    char base_path[255] = {0};
    char mode_str[11] = {0};
    char time_str[32] = {0};
    char dep_str[255] = {0};
    int total_val = 0;
    memset(dep_str,32,depth);
    
    if((dp = opendir(dir)) == NULL)
    {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    strcpy(base_path,dir);
    if(base_path[strlen(base_path)-1] != '/')
    {
        strcat(base_path,"/");
    }
    printf("%s|%s:\n",dep_str,base_path);

    while ((entry = readdir(dp)) != NULL)
    {
        if(!strcmp(entry->d_name,".") || !strcmp(entry->d_name,"..") || entry->d_name[0] == '.')
            continue;
        char cur_path[255] = {0};
        strcpy(cur_path,base_path);
        strcat(cur_path,entry->d_name);

        if(lstat(cur_path,&statbuf) == -1)
        {
            perror("lstat");
            exit(EXIT_FAILURE);
        }

        total_val += statbuf.st_blocks;

        memset(mode_str,0,sizeof(mode_str));
        mode2str(statbuf.st_mode,mode_str);

        struct passwd *pawd = getpwuid(statbuf.st_uid);
        struct group *grp = getgrgid(statbuf.st_gid);
        memset(time_str,0,sizeof(time_str));
        strcpy(time_str, ctime(&statbuf.st_mtime));
        time_str[strlen(time_str) - 1] = 0;

        printf("%s|%d",dep_str,depth);
        printf("%11s",mode_str);
        printf("%2d",statbuf.st_nlink);
        printf("%10s",pawd->pw_name);
        printf("%7s",grp->gr_name);
        printf("%9lld ",statbuf.st_size);
        printf("%6s ",time_str);
        printf("%s\n",entry->d_name);

        if(S_ISDIR(statbuf.st_mode)) //dir
        {
            printf("\n");
            printdir(cur_path,depth + 1);
        }

    }
    printf("%s|total:%d\n",dep_str,total_val);
    printf("\n");
    closedir(dp);
}

void mode2str(int mode, char str[])
{
    strcpy(str, "----------"); 
    if(S_ISDIR(mode)) str[0] = 'd';
    if(S_ISCHR(mode)) str[0] = 'c';
    if(S_ISBLK(mode)) str[0] = 'b';
    if(S_ISLNK(mode)) str[0] = 'l';
 
    if(mode & S_IRUSR) str[1] = 'r';
    if(mode & S_IWUSR) str[2] = 'w';
    if(mode & S_IXUSR) str[3] = 'x';
 
    if(mode & S_IRGRP) str[4] = 'r';
    if(mode & S_IWGRP) str[5] = 'w';
    if(mode & S_IXGRP) str[6] = 'x';
 
    if(mode & S_IROTH) str[7] = 'r';
    if(mode & S_IWOTH) str[8] = 'w';
    if(mode & S_IXOTH) str[9] = 'x';
}

