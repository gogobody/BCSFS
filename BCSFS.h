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

class BCSFS {
public:
    BCSFS() {
        memset(&op, 0, sizeof(struct fuse_operations));
        load_operations();
    };

    ~BCSFS() = default;;

    static int bcsfs_getattr(const char *, struct stat *, struct fuse_file_info *);

    static int bcsfs_opendir(const char *, struct fuse_file_info *);
    static int bcsfs_readdir(const char *, void *, fuse_fill_dir_t, off_t,
                             struct fuse_file_info *, enum fuse_readdir_flags);
    static int bcsfs_releasedir(const char *, struct fuse_file_info *);
    static int bcsfs_access(const char *, int);
    static int bcsfs_open(const char *, struct fuse_file_info *);
    static int bcsfs_read(const char *, char *, size_t, off_t, struct fuse_file_info *);

    int run(int argc, char **argv) {
        fuse_main(argc, argv, Operations(), nullptr);
        return 0;
    };

    fuse_operations *Operations() { return &op; };

private:
    struct fuse_operations op;

    void load_operations() {
        op.getattr = bcsfs_getattr;
        op.opendir = bcsfs_opendir;
        op.readdir = bcsfs_readdir;
        op.access = bcsfs_access;
        op.releasedir = bcsfs_releasedir;
    }
};


#endif //BCSFS_BCSFS_H
