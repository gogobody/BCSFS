//
// Created by Administrator on 2019/12/6.
//
#ifndef FUSE_USE_VERSION
#define FUSE_USE_VERSION 32
#endif
#ifndef BCSFS_BCSFS_H
#define BCSFS_BCSFS_H

#include <fuse3/fuse.h>
#include <cstring>
#include <fuse.h>

class BCSFS {
public:
    BCSFS() {
        memset(&op, 0, sizeof(struct fuse_operations));
        load_operations();
    };

    ~BCSFS() = default;;
    //dir func
    static int bcsfs_getattr(const char *, struct stat *, struct fuse_file_info *);
    static int bcsfs_opendir(const char *, struct fuse_file_info *);
    static int bcsfs_readdir(const char *, void *, fuse_fill_dir_t, off_t, struct fuse_file_info *, enum fuse_readdir_flags);
    static int bcsfs_releasedir(const char *, struct fuse_file_info *);
    static int bcsfs_access(const char *, int);
    static int bcsfs_mkdir(const char *path, mode_t mode);
    static int bcsfs_rmdir(const char *path);
    //file func
    static int bcsfs_open(const char *, struct fuse_file_info *);
    static int bcsfs_read(const char *, char *, size_t, off_t, struct fuse_file_info *);
    static int bcsfs_write(const char *, const char *, size_t, off_t,struct fuse_file_info *);
    static int bcsfs_create(const char *path, mode_t mode, struct fuse_file_info *fi);
    static int bcsfs_utimens(const char *, const struct timespec tv[2],struct fuse_file_info *fi);//创建文件时需要更新时间
    static int bcsfs_flush(const char *,struct fuse_file_info *);
    static int bcsfs_fsync(const char *path, int datasync, struct fuse_file_info *fi);
    static int bcsfs_mknod(const char *path, mode_t mode, dev_t dev);
    static int bcsfs_unlink(const char *path);
    static int bcsfs_truncate(const char *path, off_t newsize, struct fuse_file_info *fi);
    //file extend attr func //文件扩展属性 setfattr getfattr
    static int bcsfs_getxattr(const char *path, const char *name, char *value, size_t size);
    static int bcsfs_setxattr(const char *path, const char *name, const char *value, size_t size, int flags);
    static int bcsfs_listxattr(const char *path, char *list, size_t size);
    static int bcsfs_removexattr(const char *path, const char *name);

    // link func
    static int bcsfs_symlink(const char *from, const char *to);
    static int bcsfs_link(const char *from, const char *to); //hard link
    static int bcsfs_readlink(const char *path, char *buf, size_t size);
    // change func
    static int bcsfs_rename(const char *path, const char *newpath, unsigned int flags);
    static int bcsfs_chown(const char *path, uid_t uid, gid_t gid, struct fuse_file_info *fi);
    static int bcsfs_chmod(const char *path, mode_t mode, struct fuse_file_info *fi);

    // get the filesystem information like #df
    static int bcsfs_statfs(const char *path, struct statvfs *statv);
    //other func I dont know
    static int bcsfs_fallocate(const char *path, int mode,off_t offset, off_t length, struct fuse_file_info *fi);
    static int bcsfs_read_buf(const char *path, struct fuse_bufvec **bufp,size_t size, off_t offset, struct fuse_file_info *fi);
    static int bcsfs_write_buf(const char *path, struct fuse_bufvec *buf,off_t offset, struct fuse_file_info *fi);
    static int bcsfs_lock(const char *path, struct fuse_file_info *fi, int cmd,struct flock *lock);
    static int bcsfs_flock(const char *path, struct fuse_file_info *fi, int op);

    int run(int argc, char **argv) {
        fuse_main(argc, argv, Operations(), nullptr);
        return 0;
    };

    fuse_operations *Operations() { return &op; };

private:
    struct fuse_operations op;

    void load_operations() {
        // dir func test ok
        op.getattr = bcsfs_getattr;
        op.opendir = bcsfs_opendir;
        op.readdir = bcsfs_readdir;
        op.access = bcsfs_access;
        op.releasedir = bcsfs_releasedir;
        op.mkdir = bcsfs_mkdir;
        op.rmdir = bcsfs_rmdir;
        //normal file func test ok
        op.open = bcsfs_open;
        op.read = bcsfs_read;
        op.write = bcsfs_write;
        op.create = bcsfs_create;
        op.utimens = bcsfs_utimens;
        op.flush = bcsfs_flush;
        op.fsync = bcsfs_fsync;
        op.mknod = bcsfs_mknod;
        op.unlink = bcsfs_unlink;
        op.truncate = bcsfs_truncate;
        //file extend attr func //文件扩展属性 setfattr getfattr
        op.getxattr = bcsfs_getxattr;
        op.setxattr = bcsfs_setxattr;
        op.listxattr = bcsfs_listxattr;
        op.removexattr = bcsfs_removexattr;

        //s/link test ok
        op.symlink = bcsfs_symlink;
        op.link = bcsfs_link;
        op.readlink = bcsfs_readlink;
        // change func test ok
        op.rename = bcsfs_rename;
        op.chown = bcsfs_chown;
        op.chmod = bcsfs_chmod;

        // get filesystem info
        op.statfs = bcsfs_statfs;

        //other func I dont understand those func
        op.fallocate = bcsfs_fallocate;
        op.read_buf = bcsfs_read_buf;
        op.write_buf = bcsfs_write_buf;
        op.lock = bcsfs_lock;
        op.flock = bcsfs_flock;


    }
};


#endif //BCSFS_BCSFS_H
