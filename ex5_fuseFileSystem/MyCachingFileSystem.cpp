/*
  Big Brother File System
  Copyright (C) 2012 Joseph J. Pfeiffer, Jr., Ph.D. <pfeiffer@cs.nmsu.edu>

  This program can be distributed under the terms of the GNU GPLv3.
  See the file COPYING.

  This code is derived from function prototypes found /usr/include/fuse/fuse.h
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>
  His code is licensed under the LGPLv2.
  A copy of that code is included in the file fuse.h

  The point of this FUSE filesystem is to provide an introduction to
  FUSE.  It was my first FUSE filesystem as I got to know the
  software; hopefully, the comments in this code will help people who
  follow later to get a gentler introduction.

  This might be called a no-op filesystem:  it doesn't impose
  filesystem semantics on top of any other existing structure.  It
  simply reports the requests that come in, and passes them to an
  underlying filesystem.  The information is saved in a logfile named
  cachingfs.log, in the directory from which you run cachingfs.

  gcc -Wall `pkg-config fuse --cflags --libs` -o cachingfs cachingfs.c
 */

#define FUSE_USE_VERSION 26

#include <iostream>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <queue>
#include <sys/time.h>
#include <set>
#include <sstream>
#include <fuse.h>
#include <errno.h>

//#include "log.h"

#define ALIGN 512
#define ALIGNMENT(blockSize) (size_t)((ceil(blockSize / (double)ALIGN)) * ALIGN)

using namespace std;

struct block {
	timeval lastUsed;
	string path;
	unsigned int blockNumber;
	char* content;
	int size;

};


/**
 * compare two blocks by there time
 */
struct timeComparator{
	bool operator() (block* block1, block* block2){
		// the seconds are different
		if(block1->lastUsed.tv_sec != block2->lastUsed.tv_sec)
		{
			return block1->lastUsed.tv_sec < block2->lastUsed.tv_sec;
		}
		// the unisecs are different
		else if(block1->lastUsed.tv_usec != block2->lastUsed.tv_usec)
		{
			return block1->lastUsed.tv_usec < block2->lastUsed.tv_usec;
		}
		else
		{
			return block1->blockNumber < block2->blockNumber;
		}

	}
};


/**
 * compare two blocks by there pathname and block number
 */
struct blockComparator{
	bool operator() (block* block1, block* block2){

		int a = block1->path.compare(block2->path);

		if(a == 0)
		{
			return block1->blockNumber > block2->blockNumber;
		}
		else
		{
			return a > 0;
		}

	}
};

/**
 * contain two block pointer sets
 * and the pathes
 */
struct caching_state {
	FILE *logfile;
	char *rootdir;
	unsigned int numberOfBlocks;
	int blockSize;

	set<block*,blockComparator> *setPath;
	set<block*,timeComparator> *setTime;
};
#define CACHING_DATA ((struct caching_state *) fuse_get_context()->private_data)



struct fuse_operations caching_oper;


/*
 * Prints errors to the log file
 */
static int logError(string str)
{
	int ret = -errno;
	string s = "error in: " + str + "\n";
	fwrite(s.c_str(), sizeof(char), s.length(), CACHING_DATA->logfile);
	if (ferror(CACHING_DATA->logfile) != 0)
	{
		cerr << "system error: couldn't write to ioctloutput.log file" << endl;
	}

	return ret;

}



/**
 * Get attributes from an open file
 *
 * This method is called instead of the getattr() method if the
 * file information is available.
 *
 * Currently this is only called after the create() method if that
 * is implemented (see above).  Later it may be called for
 * invocations of fstat() too.
 *
 * Introduced in version 2.5
 */
// Since it's currently only called after caching_create(), and caching_create()
// opens the file, I ought to be able to just use the fd and ignore
// the path...
int caching_fgetattr(const char *path, struct stat *statbuf, struct fuse_file_info *fi)
{
	int retstat = 0;


	retstat = fstat(fi->fh, statbuf);
	if (retstat < 0)
	{
		retstat = logError("caching_fgetattr");
	}
	return retstat;
}




// Check whether the given user is permitted to perform the given operation on the given
//  All the paths I see are relative to the root of the mounted
//  filesystem.  In order to get to the underlying filesystem, I need to
//  have the mountpoint.  I'll save it away early on in main(), and then
//  whenever I need a path for something I'll call this to construct
//  it.
static void caching_fullpath(char fpath[PATH_MAX], const char *path)
{
	strcpy(fpath, CACHING_DATA->rootdir);
	strncat(fpath, path, PATH_MAX); // ridiculously long paths will break here

}


/** Get file attributes.
 *
 * Similar to stat().  The 'st_dev' and 'st_blksize' fields are
 * ignored.  The 'st_ino' field is ignored except if the 'use_ino'
 * mount option is given.
 */
int caching_getattr(const char *path, struct stat *statbuf)
{
	int retstat = 0;
	char fpath[PATH_MAX];

	caching_fullpath(fpath, path);

	retstat = lstat(fpath, statbuf);
	if (retstat != 0)
	{
		retstat = logError("caching_getattr");
	}

	return retstat;
}



/** Rename a file */
// both path and newpath are fs-relative
int caching_rename(const char *path, const char *newpath)
{
	int retstat = 0;
	char fpath[PATH_MAX];
	char fnewpath[PATH_MAX];

	caching_fullpath(fpath, path);
	caching_fullpath(fnewpath, newpath);

	retstat = rename(fpath, fnewpath);
	if (retstat < 0)
	{
		retstat = logError("caching_rename");
	}

	for (set<block*>::iterator itush = CACHING_DATA->setTime->begin();
			itush != CACHING_DATA->setTime->end(); itush++)
	{
		int find = (*itush)->path.find(fpath);
		if(find == 0)
		{
			CACHING_DATA->setPath->erase(*itush);
			(*itush)->path.replace(0,string(fpath).length(), string(fnewpath));
			CACHING_DATA->setPath->insert(*itush);
		}
	}


	return retstat;
}




/** File open operation
 * No creation, or truncation flags (O_CREAT, O_EXCL, O_TRUNC)
 * will be passed to open().  Open should check if the operation
 * is permitted for the given flags.  Optionally open may also
 * return an arbitrary filehandle in the fuse_file_info structure,
 * which will be passed to all file operations.
 *
 * Changed in version 2.2
 */
int caching_open(const char *path, struct fuse_file_info *fi)
{
	int retstat = 0;
	int fd;
	char fpath[PATH_MAX];
	caching_fullpath(fpath, path);
#ifndef O_DIRECT_MODE
	int flags = (fi->flags | O_SYNC) & ~(O_CREAT | O_EXCL | O_TRUNC);
#else
	int flags = (fi->flags | O_DIRECT | O_SYNC) & ~(O_CREAT | O_EXCL | O_TRUNC);
#endif
	fd = open(fpath, flags);
	if (fd < 0)
	{
		retstat = -1;
	}
	fi->fh = fd;
	return retstat;
}

/** Read data from an open file
 *
 * Read should return exactly the number of bytes requested except
 * on EOF or error, otherwise the rest of the data will be
 * substituted with zeroes.  An exception to this is when the
 * 'direct_io' mount option is specified, in which case the return
 * value of the read system call will reflect the return value of
 * this operation.
 *
 * Changed in version 2.2
 */
// I don't fully understand the documentation above -- it doesn't
// match the documentation for the read() system call which says it
// can return with anything up to the amount of data requested. nor
// with the fusexmp code which returns the amount of data also
// returned by read.
int caching_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{

	int retstat = 0;
	struct stat stat_buf;
	retstat = caching_fgetattr(path, &stat_buf, fi);
	if (retstat != 0)
	{
		return retstat;
	}


	unsigned int retRead;
	size_t blkSize = CACHING_DATA->blockSize;
	set<block*,blockComparator> * sPath = CACHING_DATA->setPath;
	set<block*,timeComparator> * sTime = CACHING_DATA->setTime;


	char fpath[PATH_MAX] = {'\0'};


	caching_fullpath(fpath, path);
	timeval currentTime;
	gettimeofday(&currentTime,NULL);

	unsigned int beginBlock = offset / blkSize;
	unsigned int endBlock = (offset + size - 1) / blkSize;

	char* tempContent;
	block temp;
	temp.blockNumber = beginBlock;
	temp.path = string(fpath);

	set<block*>::iterator itush;
	while(temp.blockNumber <= endBlock)
	{
		itush = sPath->find(&temp);
		// didn't find the blocks
		if(itush == sPath->end())
		{
			//initialize a new block and adds the relevant data
			block* toInsert = new block();
			toInsert->blockNumber = temp.blockNumber;
			toInsert->path = string(fpath);
			toInsert->lastUsed = currentTime;
#ifndef O_DIRECT_MODE
			toInsert->content = new char[blkSize];
#else
			if(posix_memalign((void**)&toInsert->content,ALIGN ,blkSize) != 0)
			{
				retstat = logError("caching_read");
			}
#endif
			retRead = pread(fi->fh, toInsert->content, blkSize, temp.blockNumber * blkSize);
			if (retRead < 0)
			{
				retstat = logError("caching_read");
			}

			toInsert->size = retRead;

			//only if the cache is full
			if(sTime->size() == CACHING_DATA->numberOfBlocks)
			{
				set<block*>::iterator iti2 = sPath->find((*sTime->begin()));
				sPath->erase(*iti2);
				sTime->erase(*(sTime->begin()));
				delete((*iti2)->content);
				delete((*iti2));
			}
			//add the block to the cache
			sTime->insert(toInsert);
			sPath->insert(toInsert);
			tempContent = toInsert->content;
		}
		//the block already exist. and the time is updated
		else
		{
			sTime->erase(*itush);
			(*itush)->lastUsed = currentTime;
			sTime->insert(*itush);
			tempContent = (*itush)->content;
			retRead = (*itush)->size;
		}



		//adds the content to the buffer.
		int beginFromBlock = 0;
		if(retRead > 0)
		{
			if(temp.blockNumber == beginBlock)
			{
				beginFromBlock = offset - temp.blockNumber * blkSize;
				retRead -= beginFromBlock;
			}
			if(temp.blockNumber == endBlock)
			{
				retRead = min((unsigned int)((offset + size) - (endBlock * blkSize)), retRead);
			}
			memcpy(buf + retstat, tempContent + beginFromBlock, retRead);
			retstat += retRead;
		}

		if(retRead < blkSize)
		{
			temp.blockNumber = endBlock;
		}
		temp.blockNumber++;

	}
	return retstat;
}





/** Possibly flush cached data
 *
 * BIG NOTE: This is not equivalent to fsync().  It's not a
 * request to sync dirty data.
 *
 * Flush is called on each close() of a file descriptor.  So if a
 * filesystem wants to return write errors in close() and the file
 * has cached dirty data, this is a good place to write back data
 * and return any errors.  Since many applications ignore close()
 * errors this is not always useful.
 *
 * NOTE: The flush() method may be called more than once for each
 * open().  This happens if more than one file descriptor refers
 * to an opened file due to dup(), dup2() or fork() calls.  It is
 * not possible to determine if a flush is final, so each flush
 * should be treated equally.  Multiple write-flush sequences are
 * relatively rare, so this shouldn't be a problem.
 *
 * Filesystems shouldn't assume that flush will always be called
 * after some writes, or that if will be called at all.
 *
 * Changed in version 2.2
 */
int caching_flush(const char *path, struct fuse_file_info *fi)
{
	int retstat = 0;

	return retstat;
}

/** Release an open file
 *
 * Release is called when there are no more references to an open
 * file: all file descriptors are closed and all memory mappings
 * are unmapped.
 *
 * For every open() call there will be exactly one release() call
 * with the same flags and file descriptor.  It is possible to
 * have a file opened more than once, in which case only the last
 * release will mean, that no more reads/writes will happen on the
 * file.  The return value of release is ignored.
 *
 * Changed in version 2.2
 */
int caching_release(const char *path, struct fuse_file_info *fi)
{
	int retstat = 0;


	// We need to close the file.  Had we allocated any resources
	// (buffers etc) we'd need to free them here as well.
	retstat = close(fi->fh);

	return retstat;
}



/** Open directory
 *
 * This method should check if the open operation is permitted for
 * this  directory
 *
 * Introduced in version 2.3
 */
int caching_opendir(const char *path, struct fuse_file_info *fi)
{
	DIR *dp;
	int retstat = 0;
	char fpath[PATH_MAX];

	caching_fullpath(fpath, path);

	dp = opendir(fpath);
	if (dp == NULL)
	{
		retstat = logError("caching_opendir");
	}
	fi->fh = (intptr_t) dp;


	return retstat;
}

/** Read directory
 *
 * This supersedes the old getdir() interface.  New applications
 * should use this.
 *
 * The filesystem may choose between two modes of operation:
 *
 * 1) The readdir implementation ignores the offset parameter, and
 * passes zero to the filler function's offset.  The filler
 * function will not return '1' (unless an error happens), so the
 * whole directory is read in a single readdir operation.  This
 * works just like the old getdir() method.
 *
 * 2) The readdir implementation keeps track of the offsets of the
 * directory entries.  It uses the offset parameter and always
 * passes non-zero offset to the filler function.  When the buffer
 * is full (or an error happens) the filler function will return
 * '1'.
 *
 * Introduced in version 2.3
 */
int caching_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset,
		struct fuse_file_info *fi)
{
	int retstat = 0;
	DIR *dp;
	struct dirent *de;


	// once again, no need for fullpath -- but note that I need to cast fi->fh
	dp = (DIR *) (uintptr_t) fi->fh;

	// Every directory contains at least two entries: . and ..  If my
	// first call to the system readdir() returns NULL I've got an
	// error; near as I can tell, that's the only condition under
	// which I can get an error from readdir()
	de = readdir(dp);
	if (de == 0)
	{
		retstat = logError("caching_readdir");
		return retstat;
	}


	do {
		if (filler(buf, de->d_name, NULL, 0) != 0)
		{
			return -ENOMEM;
		}
	} while ((de = readdir(dp)) != NULL);


	return retstat;
}

/** Release directory
 *
 * Introduced in version 2.3
 */
int caching_releasedir(const char *path, struct fuse_file_info *fi)
{
	int retstat = 0;

	if (closedir((DIR *) (uintptr_t) fi->fh) != 0)
	{
		retstat = logError("caching_releasedir");
	}

	return retstat;
}



/**
 * Initialize filesystem
 *
 * The return value will passed in the private_data field of
 * fuse_context to all file operations and as a parameter to the
 * destroy() method.
 *
 * Introduced in version 2.3
 * Changed in version 2.6
 */
// Undocumented but extraordinarily useful fact:  the fuse_context is
// set up before this function is called, and
// fuse_get_context()->private_data returns the user_data passed to
// fuse_main().  Really seems like either it should be a third
// parameter coming in here, or else the fact should be documented
// (and this might as well return void, as it did in older versions of
// FUSE).
void *caching_init(struct fuse_conn_info *conn)
{

	return CACHING_DATA;
}

/**
 * Clean up filesystem
 *
 * Called on filesystem exit.
 *
 * Introduced in version 2.3
 */
void caching_destroy(void *userdata)
{
	caching_state* data = (caching_state*) userdata;
	for (set<block*>::iterator itush = data->setTime->begin();
			itush != data->setTime->end(); itush++)
	{
		delete((*(itush))->content);
		delete(*(itush));
	}

	data->setTime->clear();
	data->setPath->clear();
	delete(data->setTime);
	delete(data->setPath);
	delete(data->rootdir);
	fclose(data->logfile);
	delete(data);
}

/**
 * Check file access permissions
 *
 * This will be called for the access() system call.  If the
 * 'default_permissions' mount option is given, this method is not
 * called.
 *
 * This method is not called under Linux kernel versions 2.4.x
 *
 * Introduced in version 2.5
 */
int caching_access(const char *path, int mask)
{
	int retstat = 0;
	char fpath[PATH_MAX];

	caching_fullpath(fpath, path);

	retstat = access(fpath, mask);

	if (retstat < 0)
		retstat = logError("caching_access");

	return retstat;
}





/*
 * convert time to the format that needed on the ioctloutpot.log file
 */
string timeToString(const timeval& time)
{
	char timebuf[64];
	tm* time_struct = localtime(&time.tv_sec);
	int ms = ((double) time.tv_usec) / 1000.0;

	snprintf(timebuf, sizeof(timebuf), "%02d:%02d:%02d:%02d:%02d:%03d",
			time_struct->tm_mon + 1, time_struct->tm_mday,
			time_struct->tm_hour, time_struct->tm_min, time_struct->tm_sec,
			ms);

	return string(timebuf);
}



/**
 * Ioctl
 *
 * flags will have FUSE_IOCTL_COMPAT set for 32bit ioctls in
 * 64bit environment.  The size and direction of data is
 * determined by _IOC_*() decoding of cmd.  For _IOC_NONE,
 * data will be NULL, for _IOC_WRITE data is out area, for
 * _IOC_READ in area and if both are set in/out area.  In all
 * non-NULL cases, the area is of _IOC_SIZE(cmd) bytes.
 *
 * Introduced in version 2.8
 */
int caching_ioctl (const char *, int cmd, void *arg,
		struct fuse_file_info *, unsigned int flags, void *data)
{
	timeval time;
	gettimeofday(&time, NULL);
	string output = timeToString(time) + "\n";

	set<block*>::iterator itush = CACHING_DATA->setTime->end();

	while(itush != CACHING_DATA->setTime->begin())
	{
		itush--;
		block* block = (*itush);

		output += block->path;
		output += "\t";

		char number_buffer[20];
		sprintf(number_buffer, "%d", block->blockNumber + 1);

		output += number_buffer ;
		output += "\t";
		output += timeToString(block->lastUsed);
		output += "\t";
		output += "\n";
	}
	fwrite(output.c_str(), sizeof(char), output.length(), CACHING_DATA->logfile);
	if (ferror(CACHING_DATA->logfile) != 0)
	{
		cerr << "system error: couldn't write to ioctloutput.log file" << endl;
	}
	fflush(CACHING_DATA->logfile);

	return 0;
}


/**
 * the usage is wrong
 * print the right usage
 */
void wrongUsage(caching_state* caching_data)
{
	cout << "usage: MyCachingFileSystem rootdir mountdir numberOfBlocks blockSize\n";

	delete(caching_data->setTime);
	delete(caching_data->setPath);
	delete(caching_data->rootdir);
	fclose(caching_data->logfile);
	delete(caching_data);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	struct caching_state *caching_data;

	// Initialize the operations
	caching_oper.getattr = caching_getattr;
	caching_oper.access = caching_access;
	caching_oper.open = caching_open;
	caching_oper.read = caching_read;
	caching_oper.flush = caching_flush;
	caching_oper.release = caching_release;
	caching_oper.opendir = caching_opendir;
	caching_oper.readdir = caching_readdir;
	caching_oper.releasedir = caching_releasedir;
	caching_oper.rename = caching_rename;
	caching_oper.init = caching_init;
	caching_oper.destroy = caching_destroy;
	caching_oper.ioctl = caching_ioctl;
	caching_oper.fgetattr = caching_fgetattr;


	caching_oper.readlink = NULL;
	caching_oper.getdir = NULL;
	caching_oper.mknod = NULL;
	caching_oper.mkdir = NULL;
	caching_oper.unlink = NULL;
	caching_oper.rmdir = NULL;
	caching_oper.symlink = NULL;
	caching_oper.link = NULL;
	caching_oper.chmod = NULL;
	caching_oper.chown = NULL;
	caching_oper.truncate = NULL;
	caching_oper.utime = NULL;
	caching_oper.write = NULL;
	caching_oper.statfs = NULL;
	caching_oper.fsync = NULL;
	caching_oper.setxattr = NULL;
	caching_oper.getxattr = NULL;
	caching_oper.listxattr = NULL;
	caching_oper.removexattr = NULL;
	caching_oper.fsyncdir = NULL;
	caching_oper.create = NULL;
	caching_oper.ftruncate = NULL;

	caching_data = new caching_state;

	// open the log file in the current folder
	caching_data->logfile = fopen("ioctloutput.log", "a");
	// failed to open log file
	if (caching_data->logfile  <= 0)
	{
		cerr << "system error: couldn't open ioctloutput.log file" << endl;
		delete(caching_data);
	}

	// checks if there are 4 arguments
	if (argc < 5)
	{
		wrongUsage(caching_data);
	}

	// initialize the struct
	caching_data->rootdir = realpath(argv[1],NULL);
	char* mountDir = realpath(argv[2],NULL);
	caching_data->numberOfBlocks = atoi(argv[3]);

#ifndef O_DIRECT_MODE
	caching_data->blockSize = atoi(argv[4]);
#else
	caching_data->blockSize = ALIGNMENT(atoi(argv[4]));
#endif


	// checks if the path is ok
	if(caching_data->rootdir == NULL || mountDir == NULL)
	{
		delete(mountDir);
		wrongUsage(caching_data);
	}
	delete(mountDir);

	struct stat root, mount;
	// checks if there are such directories
	if(stat(argv[1], &root) != 0 || stat(argv[2], &mount)  != 0)
	{
		int ret = logError("main");
		return ret;
	}

	if(!S_ISDIR(root.st_mode) || !S_ISDIR(mount.st_mode))
	{
		wrongUsage(caching_data);
	}
	// checks if the block size is negative
	if (caching_data->blockSize <= 0)
	{

		wrongUsage(caching_data);
	}

	caching_data->setTime = new set<block*,timeComparator>();
	caching_data->setPath = new set<block*,blockComparator>();

	argv[1] = argv[2];
	argv[3] = NULL;
	argv[4] = NULL;

	for (int i = 2; i< (argc - 3); i++){
		argv[i] = argv[i + 3];
	}
	argv[argc - 1] = NULL;


	argc -= 3;

	int fuse_stat = fuse_main(argc, argv, &caching_oper, caching_data);
	return fuse_stat;
}
