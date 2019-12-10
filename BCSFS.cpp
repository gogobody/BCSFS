//
// Created by Administrator on 2019/12/6.
//

#include <cstring>
#include <cstdio>
#include <iostream>
#include <dirent.h>
#include <unistd.h>
#include "BCSFS.h"

using namespace std;

static const string root_path = "/home/ubuntu/hello";

string fullpath(const char *path) {
    string fpath = root_path + path;
    return fpath;
}

int BCSFS::bcsfs_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi) {
    int res = 0;
    memset(stbuf, 0, sizeof(struct stat));
    puts("#getattr call");
    puts(path);
    string fpath = fullpath(path);
    if(fi)
        res = fstat(fi->fh, stbuf);
    else
        res = lstat(fpath.c_str(), stbuf);
    if (res == -1)
        return -errno;
    return 0;
}

int BCSFS::bcsfs_opendir(const char *path, struct fuse_file_info *fi) {
    puts("#opendir call");

    DIR *dir;
    string fpath = fullpath(path);
    dir = opendir(fpath.c_str());
    if (dir == nullptr) {
        return -errno;
    }
    fi->fh = reinterpret_cast<uint64_t>(dir);

    return 0;
}

int BCSFS::bcsfs_access(const char *path, int mask) {
    puts("#bcsfs_access call");

    string fpath = fullpath(path);
    int ret = access(fpath.c_str(), mask);
    if (ret < 0) {
        return -errno;
    }
    return ret;
}

int BCSFS::bcsfs_releasedir(const char *path, struct fuse_file_info *fi) {
    puts("#bcsfs_releasedir call");
    string fpath = fullpath(path);
    int ret = closedir(reinterpret_cast<DIR *>(fi->fh));
    if (ret < 0) {
        return -errno;
    }
    return ret;
}

int BCSFS::bcsfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi,
                         enum fuse_readdir_flags) {
    puts("#bcsfs_readdir call");
    DIR *dir;
    struct dirent *de;
    dir = reinterpret_cast<DIR *>(fi->fh);
    int ret = 0;
    while ((de = readdir(dir)) != nullptr) {
        ret = filler(buf, de->d_name, nullptr, 0, FUSE_FILL_DIR_PLUS);
        if (ret != 0) {
            return -errno;
        }
    }

    return ret;
}

int BCSFS::bcsfs_mkdir(const char *path, mode_t mode) {
    puts("#bcsfs_mkdir call");
    string fpath = fullpath(path);
    int ret = mkdir(fpath.c_str(),mode);
    if(ret<0) ret=-errno;
    return ret;
}

int BCSFS::bcsfs_rmdir(const char *path) {
    puts("#bcsfs_rmdir call");
    string fpath = fullpath(path);
    int ret = rmdir(fpath.c_str());
    if(ret<0) ret = -errno;
    return ret;
}

int BCSFS::bcsfs_open(const char *path, struct fuse_file_info *fi) {
    puts("#bcsfs_open call");
    string fpath = fullpath(path);
    int fd = open(fpath.c_str(), fi->flags);
    if (fd < 0) {
        return -errno;
    }
    fi->fh = fd;
    return 0;
}

int BCSFS::bcsfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    puts("#bcsfs_read call");
    //open 的时候设置了fh
    int ret = pread(fi->fh, buf, size, offset);
    if (ret < 0) return -errno;
    return ret;
}

int BCSFS::bcsfs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    puts("#bcsfs_write call");
    //open 的时候设置了fh
    int ret = pwrite(fi->fh, buf, size, offset);
    if (ret < 0) return -errno;
    return ret;
}

int BCSFS::bcsfs_flush(const char *path, struct fuse_file_info *fi) {

    return 0;
}

//fsync函数只对由文件描述符filedes指定的单一文件起作用，并且等待写磁盘操作结束，然后返回。fsync可用于数据库这样的应用程序，这种应用程序需要确保将修改过的块立即写到磁盘上。
//fdatasync函数类似于fsync，但它只影响文件的数据部分。而除数据外，fsync还会同步更新文件的属性。
int BCSFS::bcsfs_fsync(const char *path, int datasync, struct fuse_file_info *fi) {
    puts("#bcsfs_fsync call");

    int ret = 0;
    if (datasync)
        ret = fdatasync(fi->fh);
    else
        ret = fsync(fi->fh);
    if (ret < 0)
        return -errno;

    return ret;
}

//创建文件或者设备文件
int BCSFS::bcsfs_mknod(const char *path, mode_t mode, dev_t dev) {
    puts("#bcsfs_mknod call");
    string fpath = fullpath(path);
    int ret = 0;
    if (S_ISREG(mode)) {//是否是一个常规文件. https://blog.csdn.net/abcd1f2/article/details/49820889
        ret = open(fpath.c_str(), O_CREAT | O_EXCL | O_WRONLY, mode);
        if (ret < 0)
            ret = -errno;
        else {
            ret = close(ret);
            if (ret < 0)
                ret = -errno;
        }
    } else {
        if (S_ISFIFO(mode)) {
            ret = mkfifo(fpath.c_str(), mode);
            if (ret < 0) {
                ret = -errno;
            }
        } else { //创建设备文件
            ret = mknod(fpath.c_str(), mode, dev);
            if (ret < 0) ret = -errno;

        }
    }
    return ret;
}

//remove a file
int BCSFS::bcsfs_unlink(const char *path) {
    puts("#bcsfs_unlink call");
    string fpath = fullpath(path);
    int ret = unlink(fpath.c_str());
    if(ret<0) ret=-errno;

    return ret;
}

//create and open a file
int BCSFS::bcsfs_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
    puts("#bcsfs_create call");
    string fpath = fullpath(path);
    int fd = creat(fpath.c_str(),mode);
    int ret = 0;
    if(fd<0){
        ret= -errno;
    }
    fi->fh = fd;

    return ret;
}

int BCSFS::bcsfs_utimens(const char *path, const struct timespec tv[2],struct fuse_file_info *fi){
    puts("#bcsfs_create call");
    string fpath = fullpath(path);
    int res;

    /* don't use utime/utimes since they follow symlinks */
    if (fi)
        res = futimens(fi->fh, tv);/* Set file access and modification times of the file associated with FD.  */
    else
        res = utimensat(0, fpath.c_str(), tv, AT_SYMLINK_NOFOLLOW);//http://man7.org/linux/man-pages/man2/utimensat.2.html
    if (res == -1)
        return -errno;

    return 0;
}

int BCSFS::bcsfs_symlink(const char *from, const char *to) {
    int res;
    res = symlink(from, to);
    if (res == -1)
        return -errno;

    return 0;
}







