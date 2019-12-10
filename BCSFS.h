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

    static int bcsfs_getattr(const char *, struct stat *, struct fuse_file_info *);
    static int bcsfs_opendir(const char *, struct fuse_file_info *);
    static int bcsfs_readdir(const char *, void *, fuse_fill_dir_t, off_t, struct fuse_file_info *, enum fuse_readdir_flags);
    static int bcsfs_releasedir(const char *, struct fuse_file_info *);
    static int bcsfs_access(const char *, int);
    static int bcsfs_mkdir(const char *path, mode_t mode);
    static int bcsfs_rmdir(const char *path);

    static int bcsfs_open(const char *, struct fuse_file_info *);
    static int bcsfs_read(const char *, char *, size_t, off_t, struct fuse_file_info *);
    static int bcsfs_write(const char *, const char *, size_t, off_t,struct fuse_file_info *);
    static int bcsfs_create(const char *path, mode_t mode, struct fuse_file_info *fi);
    static int bcsfs_utimens(const char *, const struct timespec tv[2],struct fuse_file_info *fi);//创建文件时需要更新时间
    static int bcsfs_flush(const char *,struct fuse_file_info *);
    static int bcsfs_fsync(const char *path, int datasync, struct fuse_file_info *fi);
    static int bcsfs_mknod(const char *path, mode_t mode, dev_t dev);
    static int bcsfs_unlink(const char *path);

    static int bcsfs_symlink(const char *from, const char *to);
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
        //normal file test ok
        op.open = bcsfs_open;
        op.read = bcsfs_read;
        op.write = bcsfs_write;
        op.create = bcsfs_create;
        op.utimens = bcsfs_utimens;
        op.flush = bcsfs_flush;
        op.fsync = bcsfs_fsync;
        op.mknod = bcsfs_mknod;
        op.unlink = bcsfs_unlink;
        //
        op.symlink = bcsfs_symlink;




    }
};


#endif //BCSFS_BCSFS_H
