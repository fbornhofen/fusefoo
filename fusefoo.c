#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define FUSE_USE_VERSION 26
#include <fuse.h>
#include <sys/stat.h>

#define MAX(X, Y) ((X) < (Y) ? (Y) : (X))

int hello_open(const char* path, struct fuse_file_info *fi)
{
    fi->fh = 1337;
    return 0;
}

int hello_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    const char* s = "Hello World!";
    int len = strlen(s);
    snprintf(buf, size, "%s", s);
    return MAX(len, size);
}

int hello_getattr(const char *path, struct stat *st)
{
    printf("getattr on %s\n", path);
    memset(st, 0, sizeof(struct stat));
    st->st_mode = 0777;
    st->st_nlink = 1;
    if (strcmp("/", path) == 0) {
        st->st_mode |= S_IFDIR;
    } else {
        st->st_mode |= S_IFREG;
        st->st_ino = 1;
        st->st_blksize = 512;
        st->st_blocks = 1;
        st->st_size = strlen("Hello World!");
    }
    return 0;
}

int hello_opendir(const char *path, struct fuse_file_info *fi)
{
    return 0;
}

int hello_readdir(const char *path, void *buf, fuse_fill_dir_t fill, off_t offset,
                    struct fuse_file_info *fi)
{
    struct stat st;
    memset((void*)&st, 0, sizeof(struct stat));
    st.st_mode = 0777 | S_IFREG;
    st.st_nlink = 1;
    st.st_ino = 1;
    st.st_blksize = 512;
    st.st_blocks = 1;
    st.st_size = strlen("Hello World!");
    fill(buf, "Hello.txt", &st, 0);
    return 0;
}

void hello_mainloop(int argc, char** argv, struct fuse_operations* op, void* user_data)
{
    struct fuse *f;
    char* mountpoint;
    int mt;
    struct timespec interval;
    interval.tv_sec = 0;
    interval.tv_nsec = 20000000;
    f = fuse_setup(argc, argv, op, sizeof(struct fuse_operations), &mountpoint, &mt, user_data);
    for (;;) {
        struct fuse_cmd* cmd = fuse_read_cmd(f);
        if (fuse_exited(f)) {
            break;
        }
        fuse_process_cmd(f, cmd);
        nanosleep(&interval, NULL);
    }
    fuse_teardown(f, mountpoint);
}

struct fuse_operations hellofuse_ops = {
    .getattr = hello_getattr,
    .opendir = hello_opendir,
    .readdir = hello_readdir,
    .open = hello_open,
    .read = hello_read
};

int main(int argc, char** argv)
{
    printf("Hello FUSE\n");
    //fuse_main(argc, argv, &hellofuse_ops, NULL);
    hello_mainloop(argc, argv, &hellofuse_ops, NULL);
    return 0;
}
