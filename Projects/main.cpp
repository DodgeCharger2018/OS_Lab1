#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

#include <errno.h>
#include <limits.h>
 
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define MAX_BUF 1024

using namespace std;

int serachFiles(char *theDir);

int main(int argc, char *argv[])
{
    int pipefd[2];
    pid_t cpid;
    char buf;
    char buffer[MAX_BUF];
    char buffer1[200];
    if (argc != 2) {
    fprintf(stderr, "Usage: %s <string>\n", argv[0]);
    exit(EXIT_FAILURE);
    }
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (cpid == 0) {    /* Child reads from pipe */
        close(pipefd[1]);          /* Close unused write end */
        
        read(pipefd[0], buffer, sizeof(buffer));
        //buffer[(strlen(buffer))]='\0';
        printf("Received: %s\n", buffer);
        sprintf(buffer1,"%s",buffer);

        serachFiles(buffer);
        //cout<< "I`m on child"<<endl;
        close(pipefd[0]);
        _exit(EXIT_SUCCESS);
    } else {            /* Parent writes argv[1] to pipe */
        close(pipefd[0]);          /* Close unused read end */
        write(pipefd[1], argv[1], strlen(argv[1])+1);
        cout<<"I`m in parent"<<endl;
        close(pipefd[1]);          /* Reader will see EOF */
        wait(NULL);                /* Wait for child */

        cout<<"Again in parent"<<endl;
        exit(EXIT_SUCCESS);
    }
}

int serachFiles(char *theDir)
{
        DIR *dir = NULL;
        struct dirent entry;
        struct dirent *entryPtr = NULL;
        int retval = 0;
        unsigned count = 0;
        char pathName[PATH_MAX + 1];
        
        /* открыть указанный каталог, если возможно. */
        cout<<theDir<<" PATH"<<endl;
        dir = opendir( theDir );
        if( dir == NULL ) {
            printf( "Error opening %s: %s", theDir, strerror( errno ) );
            return 0;
        }

        retval = readdir_r( dir, &entry, &entryPtr );
        while( entryPtr != NULL ) {
            struct stat entryInfo;
     
            if( ( strncmp( entry.d_name, ".", PATH_MAX ) == 0 ) ||
                ( strncmp( entry.d_name, "..", PATH_MAX ) == 0 ) ) {
                /* Short-circuit the . and .. entries. */
                retval = readdir_r( dir, &entry, &entryPtr );
                continue;
            }
            (void)strncpy( pathName, theDir, PATH_MAX );
            (void)strncat( pathName, "/", PATH_MAX );
            (void)strncat( pathName, entry.d_name, PATH_MAX );
         
        if( lstat( pathName, &entryInfo ) == 0 ) {
            /* вызов stat() был успешным, так что продолжаем. */
            count++;
             
            if( S_ISDIR( entryInfo.st_mode ) ) {            
/* каталог */
                //printf( "processing %s/\n", pathName );
                //count += process_directory( pathName );
                printf("%s <DIR>\n",entry.d_name);
            } else if( S_ISREG( entryInfo.st_mode ) ) { 
/* обычный файл */
                //printf( "\t%s has %lld bytes\n",
                   // pathName, (long long)entryInfo.st_size );
                printf("%s\n",entry.d_name);    
            } else if( S_ISLNK( entryInfo.st_mode ) ) { 
/* символическая ссылка */
                char targetName[PATH_MAX + 1];
                if( readlink( pathName, targetName, PATH_MAX ) != -1 ) {
                    printf( "\t%s -> %s\n", pathName, targetName );
                } else {
                    printf( "\t%s -> (invalid symbolic link!)\n", pathName );
                }
            }
        } else {
            printf( "Error statting %s: %s\n", pathName, strerror( errno ) );
        }
        retval = readdir_r( dir, &entry, &entryPtr );
    }
    (void)closedir( dir );
    return count;
}