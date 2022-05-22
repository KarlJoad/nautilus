/* 
 * This file is part of the Nautilus AeroKernel developed
 * by the Hobbes and V3VEE Projects with funding from the 
 * United States National  Science Foundation and the Department of Energy.  
 *
 * The V3VEE Project is a joint project between Northwestern University
 * and the University of New Mexico.  The Hobbes Project is a collaboration
 * led by Sandia National Laboratories that includes several national 
 * laboratories and universities. You can find out more at:
 * http://www.v3vee.org  and
 * http://xstack.sandia.gov/hobbes
 *
 * Copyright (c) 2017, Peter Dinda
 * Copyright (c) 2017, The V3VEE Project  <http://www.v3vee.org> 
 *                     The Hobbes Project <http://xstack.sandia.gov/hobbes>
 * All rights reserved.
 *
 * Authors:        Peter Dinda <pdinda@northwestern.edu>
 *
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "LICENSE.txt".
 */
#include <nautilus/nautilus.h>
#include <nautilus/dev.h>
#include <nautilus/blkdev.h>
#include <nautilus/fs.h>

/* PAD - this is the glue code that will attach fatfs to the
   NK block device and file system abstraction layers
*/


#include "ff.h"


struct fatfs_state {
    struct nk_block_dev *dev;
    struct nk_block_dev_characteristics chars;
    struct nk_fs *fs;
    
    // ... state from our perspective
    // probably at least the mapping <-> blockdev / pdrv

} ; 

static ssize_t fatfs_read(void *state, void *file, void *srcdest, off_t offset, size_t num_bytes)
{
    struct fatfs_state *fs = (struct fatfs_state *)state;
    return -1;
}

static ssize_t fatfs_write(void *state, void *file, void *srcdest, off_t offset, size_t num_bytes)
{ 
    struct fatfs_state *fs = (struct fatfs_state *)state;
    return -1;
}

static int fatfs_stat_path(void *state, char *path, struct nk_fs_stat *st)
{
    struct fatfs_state *fs = (struct fatfs_state *)state;
    return -1;
}


static void *fatfs_create(void *state, char *path, int isdir)
{
    struct fatfs_state *fs = (struct fatfs_state *)state;
    return 0;
}

static void *fatfs_create_file(void *state, char *path)
{
    struct fatfs_state *fs = (struct fatfs_state *)state;
    return 0;
}

static int fatfs_create_dir(void *state, char *path)
{
    struct fatfs_state *fs = (struct fatfs_state *)state;
    return -1;
}

static int fatfs_exists(void *state, char *path)
{
    struct fatfs_state *fs = (struct fatfs_state *)state;
    return -1;
}

int fatfs_remove(void *state, char *path)
{
    struct fatfs_state *fs = (struct fatfs_state *) state;
    return -1;
}

static void * fatfs_open(void *state, char *path)
{
    struct fatfs_state *fs = (struct fatfs_state *)state;
    return 0;
}

static int fatfs_stat(void *state, void *file, struct nk_fs_stat *st)
{
    struct fatfs_state *fs = (struct fatfs_state *)state;
    return 0;
    return -1;
}

static int fatfs_truncate(void *state, void *file, off_t len)
{
    struct fatfs_state *fs = (struct fatfs_state *)state;
    return -1;
}

static void fatfs_close(void *state, void *file)
{
    struct fatfs_state *fs = (struct fatfs_state *)state;
    return;
    
    
}

static struct nk_fs_int fatfs_inter = {
    .stat_path = fatfs_stat_path,
    .create_file = fatfs_create_file,
    .create_dir = fatfs_create_dir,
    .exists = fatfs_exists,
    .remove = fatfs_remove,
    .open_file = fatfs_open,
    .stat = fatfs_stat,
    .trunc_file = fatfs_truncate,
    .close_file = fatfs_close,
    .read_file = fatfs_read,
    .write_file = fatfs_write,
};



int nk_fs_fatfs_attach(char *devname, char *fsname, int readonly)
{
    struct nk_block_dev *dev = nk_block_dev_find(devname);
    uint64_t flags = readonly ? NK_FS_READONLY : 0;
    
    if (!dev) { 
        ERROR("Cannot find device %s\n",devname);
        return -1;
    }
    
    struct fatfs_state *s = malloc(sizeof(*s));
    if (!s) { 
        ERROR("Cannot allocate space for fs %s\n", fsname);
        return -1;
    }
    
    memset(s,0,sizeof(*s));
    
    s->dev = dev;
    
    if (nk_block_dev_get_characteristics(dev,&s->chars)) { 
        ERROR("Cannot get characteristics of device %s\n", devname);
        free(s);
        return -1;
    }

    DEBUG("Device %s has block size %lu and numblocks %lu\n",dev->dev.name, s->chars.block_size, s->chars.num_blocks);
    
    // PAD - actualy do what's needed to mount
    
    s->fs = nk_fs_register(fsname, flags, &fatfs_inter, s);

    if (!s->fs) { 
	ERROR("Unable to register filesystem %s\n", fsname);
	free(s);
	return -1;
    }

    INFO("filesystem %s on device %s is attached (%s)\n", fsname, devname, readonly ?  "readonly" : "read/write");
    
    return 0;
}

int nk_fs_fatfs_detach(char *fsname)
{
    struct nk_fs *fs = nk_fs_find(fsname);
    if (!fs) {
        return -1;
    } else {
        return nk_fs_unregister(fs);
    }
}
