#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

#define _XOPEN_SOURCE /* See feature_test_macros(7) */

void mode_to_letters(int mode, char str[]) {
    strcpy(str, "----------"); /* default=no perms */
    if (S_ISDIR(mode)) str[0] = 'd'; /* is directory? */
    if (S_ISCHR(mode)) str[0] = 'c'; /* is char devices? */
    if (S_ISBLK(mode)) str[0] = 'b'; /* is block device? */
    if (mode & S_IRUSR) str[1] = 'r'; /* 3 bits for user */
    if (mode & S_IWUSR) str[2] = 'w';
    if (mode & S_IXUSR) str[3] = 'x';
    if (mode & S_IRGRP) str[4] = 'r'; /* 3 bits for group */
    if (mode & S_IWGRP) str[5] = 'w';
    if (mode & S_IXGRP) str[6] = 'x';
    if (mode & S_IROTH) str[7] = 'r'; /* 3 bits for other */
    if (mode & S_IWOTH) str[8] = 'w';
    if (mode & S_IXOTH) str[9] = 'x';
}

char * show_time(int timestamp) {
    struct tm tm;
    static char buffer[255];
    char time_s[255];
    sprintf(time_s, "%i", timestamp);
    strptime(time_s, "%s", &tm);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", &tm);
    return buffer;
}

char * uid_to_name(int uid) {
    struct passwd * p = getpwuid(uid);
    return p->pw_name;
}

char * gid_to_name(int gid) {
    struct group * g = getgrgid(gid);
    return g->gr_name;
}

char * bytes_to_human(int bytes) {
    static char human[255];
    if (bytes < 1024) {
        sprintf(human, "%i", bytes);
    } else {
        sprintf(human, "%i KB", bytes / 1024);
    }
    return human;
}

void show_stat_info(char *fname, struct stat *buf) {
    static char mode[255];
    mode_to_letters(buf->st_mode, mode);
    printf("    mode: %s\n", mode); /* type + mode */
    printf("   links: %d\n", buf->st_nlink); /* # links */
    printf("    user: %s\n", uid_to_name(buf->st_uid)); /* user name */
    printf("   group: %s\n", gid_to_name(buf->st_gid)); /* group name */
    printf("    size: %s\n", bytes_to_human(buf->st_size)); /* file size */
    printf("accessed: %s\n", show_time((int) buf->st_atime)); /* accessed */
    printf("modified: %s\n", show_time((int) buf->st_mtime)); /* modified */
    printf(" changed: %s\n", show_time((int) buf->st_ctime)); /* changed */
}

void show_stat_info_oneline(char *fname, struct stat *buf) {
    static char mode[255];
    mode_to_letters(buf->st_mode, mode);
    printf("%s %d %s %s %s\t%s %s\n", mode, buf->st_nlink, uid_to_name(buf->st_uid), gid_to_name(buf->st_gid), bytes_to_human(buf->st_size), show_time((int) buf->st_mtime), fname);
}

char * draw_line(char * word) {
    int length = strlen(word);
    char * line = malloc(length);
    for (int i = 0; i < length; i++) {
        strncat(line, "-", length);
    }
    return line;
}

void header(char * title) {
    printf("%s\n", title);
    printf("%s\n\n", draw_line(title));
}

void show_properties(char * filename) {
    struct stat buffer;
    stat(filename, &buffer);
    printf("%s\n", filename);
    printf("%s\n", draw_line(filename));
    printf(" > inode: %lld\n", buffer.st_ino);
    printf("\n");
}

int random_between(int min, int max) {
    return rand() % (max - min + 1) + min;
}

int file_exists(char * filename) {
    return access(filename, F_OK) != -1;
}

void err(char * s1, char * s2) {
    fprintf(stderr, "Error: %s ", s1);
    perror(s2);
    exit(1);
}

int file_byte(char * filename) {
    struct stat file;
    stat(filename, &file);
    return file.st_size;
}

int is_dot(char * name) {
    int single = strcmp(name, ".") == 0;
    int dbl = strcmp(name, "..") == 0;
    return single || dbl;
}

int is_dir(char * filename) {
    struct stat file;
    stat(filename, &file);
    return file.st_mode & S_IFDIR;
}

int is_file(char * filename) {
    struct stat file;
    stat(filename, &file);
    return file.st_mode & S_IFREG;
}

int is_symbolic(char * filename) {
    struct stat file;
    stat(filename, &file);
    return file.st_mode & S_IFLNK;
}

int is_socket(char * filename) {
    struct stat file;
    stat(filename, &file);
    return file.st_mode & S_IFSOCK;
}

