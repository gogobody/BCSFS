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
    cout << fpath << endl;
    res = lstat(fpath.c_str(), stbuf);
    return res;
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

    string fpath  = fullpath(path);
    int ret = access(fpath.c_str(),mask);
    if(ret<0){
        return -errno;
    }
    return ret;
}

int BCSFS::bcsfs_releasedir(const char * path, struct fuse_file_info * fi) {
    puts("#bcsfs_releasedir call");
    string fpath = fullpath(path);
    int ret = closedir(reinterpret_cast<DIR *>(fi->fh));
    if(ret<0){
        return -errno;
    }
    return ret;
}

int BCSFS::bcsfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags) {
    puts("#bcsfs_readdir call");
    DIR *dir;
    struct dirent *de;
    dir = reinterpret_cast<DIR *>(fi->fh);
    int ret= 0;
    while ((de = readdir(dir))!= nullptr){
        ret = filler(buf,de->d_name, nullptr,0,FUSE_FILL_DIR_PLUS);
        if(ret!=0){
            return -errno;
        }
    }

    return ret;
}


int BCSFS::bcsfs_open(const char * path, struct fuse_file_info *fi) {
    puts("#bcsfs_open call");
    string fpath = fullpath(path);
    int fd = open(fpath.c_str(),fi->flags);
    if(fd<0){
        return -errno;
    }
    fi->fh = fd;
    return 0;
}

int BCSFS::bcsfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    puts("#bcsfs_read call");
    string fpath = fullpath(path);

    return 0;
}




