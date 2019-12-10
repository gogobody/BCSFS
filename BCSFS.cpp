//
// Created by Administrator on 2019/12/6.
//
//#ifdef HAVE_LIBULOCKMGR
extern "C"{
#include <ulockmgr.h>
}
//#endif
#include <cstring>
#include <cstdio>
#include <iostream>
#include <dirent.h>
#include <unistd.h>
#include <sys/xattr.h>

#include <sys/file.h>

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

//change the file size
int BCSFS::bcsfs_truncate(const char *path, off_t newsize, struct fuse_file_info *fi) {
    puts("#bcsfs_truncate call");
    int ret = 0;
    string fpath = fullpath(path);
    if(fi){
        ret=ftruncate(fi->fh,newsize);
    }else{
        ret = truncate(fpath.c_str(),newsize);
    }
    if(ret<0) ret = -errno;
    return ret;
}

int BCSFS::bcsfs_symlink(const char *from, const char *to) {
    puts("#bcsfs_symlink call");

    int res;
    string pfrom = fullpath(from);
    string pto = fullpath(to);

    res = symlink(pfrom.c_str(), pto.c_str());
    if (res == -1)
        return -errno;

    return 0;
}

int BCSFS::bcsfs_link(const char *from, const char *to) {
    puts("#bcsfs_link call");
    int res;
    string pfrom = fullpath(from);
    string pto = fullpath(to);
    res = link(pfrom.c_str(), pto.c_str());
    if (res == -1)
        return -errno;
    return 0;
}

int BCSFS::bcsfs_readlink(const char *path, char *buf, size_t size) {
    puts("#bcsfs_readlink call");
    int res;
    string fpath = fullpath(path);
    res = readlink(fpath.c_str(), buf, size - 1);
    if (res == -1)
        return -errno;

    buf[res] = '\0';
    return 0;
}

int BCSFS::bcsfs_rename(const char *path, const char *newpath, unsigned int flags) {
    puts("#bcsfs_rename call");
    cout<<flags<<endl;
    int ret = 0;
    string fpath = fullpath(path);
    string fnewpath = fullpath(newpath);
    ret = rename(fpath.c_str(), fnewpath.c_str());
    if (ret < 0)
        ret = -errno;
    return ret;

}

int BCSFS::bcsfs_chown(const char *path, uid_t uid, gid_t gid, struct fuse_file_info *fi) {
    puts("#bcsfs_chown call");
    int ret = 0;
    string fpath = fullpath(path);
    if(fi){
        ret = fchown(fi->fh,uid,gid);
    }else{
        ret = chown(fpath.c_str(), uid, gid);
    }
    if (ret < 0)
        ret = -errno;

    return ret;
}

int BCSFS::bcsfs_chmod(const char *path, mode_t mode, struct fuse_file_info *fi) {
    puts("#bcsfs_chmod call");
    int ret = 0;
    string fpath = fullpath(path);
    if(fi){
        ret = fchmod(fi->fh,mode);
    }else{
        ret = chmod(fpath.c_str(), mode);
    }
    if (ret < 0)
        ret = -errno;

    return ret;
}

int BCSFS::bcsfs_statfs(const char *path, struct statvfs *statv) {
    puts("#bcsfs_statfs call");
    int ret;
    string fpath = fullpath(path);
    ret = statvfs(fpath.c_str(), statv);
    if (ret == -1)
        return -errno;
    return 0;
}

int BCSFS::bcsfs_getxattr(const char *path, const char *name, char *value, size_t size) {
    puts("#bcsfs_getxattr call");
    string fpath = fullpath(path);
    int ret = lgetxattr(fpath.c_str(), name, value, size);

    if (ret == -1)
        return -errno;
    return ret;
}

int BCSFS::bcsfs_setxattr(const char *path, const char *name, const char *value, size_t size, int flags) {
    puts("#bcsfs_setxattr call");
    string fpath = fullpath(path);
    int ret = lsetxattr(fpath.c_str(), name, value, size, flags);
    if (ret == -1)
        return -errno;
    return 0;
}

int BCSFS::bcsfs_listxattr(const char *path, char *list, size_t size) {
    puts("#bcsfs_listxattr call");
    string fpath = fullpath(path);
    int ret = llistxattr(fpath.c_str(), list, size);
    if (ret == -1)
        return -errno;
    return ret;
}

int BCSFS::bcsfs_removexattr(const char *path, const char *name) {
    puts("#bcsfs_removexattr call");
    string fpath = fullpath(path);
    int ret = lremovexattr(fpath.c_str(), name);
    if (ret == -1)
        return -errno;
    return 0;
}

//fallocate --help Allocates space for an open file
int BCSFS::bcsfs_fallocate(const char *path, int mode, off_t offset, off_t length, struct fuse_file_info *fi) {
    puts("#bcsfs_fallocate call");

    if (mode)
        return -EOPNOTSUPP;

    return -posix_fallocate(fi->fh, offset, length);
}


int BCSFS::bcsfs_read_buf(const char *path, struct fuse_bufvec **bufp,size_t size, off_t offset, struct fuse_file_info *fi) {
    puts("#bcsfs_read_buf call");
    struct fuse_bufvec *src;
    (void) path;

    src = static_cast<fuse_bufvec *>(malloc(sizeof(struct fuse_bufvec)));
    if (src == nullptr)
        return -ENOMEM;

    *src = FUSE_BUFVEC_INIT(size);

    src->buf[0].flags = static_cast<fuse_buf_flags>(FUSE_BUF_IS_FD | FUSE_BUF_FD_SEEK);
    src->buf[0].fd = fi->fh;
    src->buf[0].pos = offset;

    *bufp = src;

    return 0;
}

int BCSFS::bcsfs_write_buf(const char *path, struct fuse_bufvec *buf, off_t offset, struct fuse_file_info *fi) {
    puts("#bcsfs_write_buf call");

    struct fuse_bufvec dst = FUSE_BUFVEC_INIT(fuse_buf_size(buf));
    dst.buf[0].flags = static_cast<fuse_buf_flags>(FUSE_BUF_IS_FD | FUSE_BUF_FD_SEEK);
    dst.buf[0].fd = fi->fh;
    dst.buf[0].pos = offset;

    return fuse_buf_copy(&dst, buf, FUSE_BUF_SPLICE_NONBLOCK);
}

int BCSFS::bcsfs_lock(const char *path, struct fuse_file_info *fi, int cmd, struct flock *lock) {
    (void) path;
    return ulockmgr_op(fi->fh, cmd, lock, &fi->lock_owner, sizeof(fi->lock_owner));
}

int BCSFS::bcsfs_flock(const char *path, struct fuse_file_info *fi, int op) {
    int res;
    (void) path;

    res = flock(fi->fh, op);
    if (res == -1)
        return -errno;

    return 0;
}








