#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include "types.h"
#include "fs.h"

int nblocks = DPG;
int ninodes = IPG;
int size = GROUP_BLOCKS;

int fsfd;
struct superblock sb;
char zeroes[512];
uint freeblock;
uint usedblocks;
uint ibitblocks;
uint dbitblocks;
uint freeinode = 1;

void balloc();
void wsect(uint, void*);
void winode(uint, struct dinode*);
void rinode(uint inum, struct dinode *ip);
void rsect(uint sec, void *buf);
uint ialloc(ushort type);
void iappend(uint inum, void *p, int n);

// convert to intel byte order
ushort
xshort(ushort x)
{
  ushort y;
  uchar *a = (uchar*) &y;
  a[0] = x;
  a[1] = x >> 8;
  return y;
}

uint
xint(uint x)
{
  uint y;
  uchar *a = (uchar*) &y;
  a[0] = x;
  a[1] = x >> 8;
  a[2] = x >> 16;
  a[3] = x >> 24;
  return y;
}

int
main(int argc, char *argv[])
{
  int i, cc, fd;
  uint rootino, inum, off;
  struct dirent de;
  char buf[512];
  struct dinode din;

  if(argc < 2){
    fprintf(stderr, "Usage: mkfs fs.img files...\n");
    exit(1);
  }

  assert((512 % sizeof(struct dinode)) == 0);
  assert((512 % sizeof(struct dirent)) == 0);

  fsfd = open(argv[1], O_RDWR|O_CREAT|O_TRUNC, 0666);
  if(fsfd < 0){
    perror(argv[1]);
    exit(1);
  }

  sb.size = xint(size * GROUPS);
  sb.nblocks = xint(nblocks * GROUPS); // so whole disk is size sectors
  sb.ninodes = xint(ninodes * GROUPS);
  //sb.size = xint(size);
  //sb.nblocks = xint(nblocks); // so whole disk is size sectors
  //sb.ninodes = xint(ninodes);
  sb.ngroups = xint(GROUPS);

  //SETUP BITMAP SIZES
  ibitblocks = ninodes / (BSIZE * 8) + 1;
  dbitblocks = nblocks / (BSIZE * 8) + 1;
  usedblocks = ninodes / IPB + 3 + dbitblocks + ibitblocks;
  freeblock = usedblocks;

  printf("used %d (dbit %d ibit %d ninode %lu) free %u total %d\n", usedblocks, dbitblocks, ibitblocks, ninodes/IPB + 1, freeblock, nblocks+usedblocks);
  printf("nblocks: %d, usedblocks: %d, size %d\n",nblocks,usedblocks,size);
  assert(nblocks + usedblocks == size);

  //ZERO DISK
  //for(i = 0; i < sb.size * 8; i++)
  for(i = 0; i < sb.size; i++)
    wsect(i, zeroes);

  //WRITE 8 SUPER BLOCKS TO DISK
  for (i = 0; i < sb.size; i += sb.size / 8)
	  wsect(i + 1, &sb);

  //ALLOCATE ROOT INODE, (WRITES TO DISK)
  rootino = ialloc(T_DIR);
  assert(rootino == 1);

  //ADDS DIRECTORY "." TO ROOT INODE
  bzero(&de, sizeof(de));
  de.inum = xshort(rootino);
  strcpy(de.name, ".");
  iappend(rootino, &de, sizeof(de));

  //ADDS DIRECTORY ".." TO ROOT INODE
  bzero(&de, sizeof(de));
  de.inum = xshort(rootino);
  strcpy(de.name, "..");
  iappend(rootino, &de, sizeof(de));


  //LOOP THROUGH COMMAND LINE FINDING FILES TO ADD TO FS
  for(i = 2; i < argc; i++){
    assert(index(argv[i], '/') == 0);  //ensures / does not exist in file name

    if((fd = open(argv[i], 0)) < 0){
      perror(argv[i]);
      exit(1);
    }
    
    // Skip leading _ in name when writing to file system.
    // The binaries are named _rm, _cat, etc. to keep the
    // build operating system from trying to execute them
    // in place of system binaries like rm and cat.
    if(argv[i][0] == '_')            //removes _ from start of file name
      ++argv[i];

    //ALLOC NEXT INODE FOR USER FILES
    inum = ialloc(T_FILE);           
    bzero(&de, sizeof(de));
    de.inum = xshort(inum);
    strncpy(de.name, argv[i], DIRSIZ);
    //ADD USER FILE NAME TO ROOT DIRECTORY
    iappend(rootino, &de, sizeof(de)); 

    //COPY USER DATA INTO DATA BLOCk
    while((cc = read(fd, buf, sizeof(buf))) > 0)
      iappend(inum, buf, cc);

    close(fd);
  }

  // fix size of root inode dir
  rinode(rootino, &din);
  off = xint(din.size);
  off = ((off/BSIZE) + 1) * BSIZE;
  din.size = xint(off);
  winode(rootino, &din);

  //WRITE OUT THE INODE AND DATA BITMAPS FOR EACH CYL GRP
  //for (i = 0; i < sb.size; i += sb.size / 8)
  balloc();

  exit(0);
}

void
wsect(uint sec, void *buf)
{
  if(lseek(fsfd, sec * 512L, 0) != sec * 512L){
    perror("lseek");
    exit(1);
  }
  if(write(fsfd, buf, 512) != 512){
    perror("write");
    exit(1);
  }
}

uint
i2b(uint inum)
{
  return (inum / IPB) + 2 + ibitblocks + dbitblocks;    //+2 comes from Empty block + super block (start writing at location 2)
}

void
winode(uint inum, struct dinode *ip)
{
  char buf[512];
  uint bn;
  struct dinode *dip;

  bn = i2b(inum);
  rsect(bn, buf);
  dip = ((struct dinode*) buf) + (inum % IPB);
  *dip = *ip;
  wsect(bn, buf);
}

void
rinode(uint inum, struct dinode *ip)
{
  char buf[512];
  uint bn;
  struct dinode *dip;

  bn = i2b(inum);
  rsect(bn, buf);
  dip = ((struct dinode*) buf) + (inum % IPB);
  *ip = *dip;
}

void
rsect(uint sec, void *buf)
{
  if(lseek(fsfd, sec * 512L, 0) != sec * 512L){
    perror("lseek");
    exit(1);
  }
  if(read(fsfd, buf, 512) != 512){
    perror("read");
    exit(1);
  }
}

uint
ialloc(ushort type)
{
  uint inum = freeinode++;
  struct dinode din;

  bzero(&din, sizeof(din));
  din.type = xshort(type);
  din.nlink = xshort(1);
  din.size = xint(0);
  winode(inum, &din);
  return inum;
}

//FUNCTION WILL FILL IN THE INODE BITMAP AND THE DATA BITMAP
void
balloc()
{
  uchar buf[512];
  int i;
  int group = 0;
  int global_start_addr = (sb.size / GROUPS) * group;

  //assert(used < 512);  // there is only one bitmap block, so cant have more than 512 blocks.
  
  int datablocks = freeblock - (ibitblocks + dbitblocks + (ninodes / IPB) + 3);
  //WRITE OUT DATA BITMAP
  bzero(buf, 512);
  for(i = 0; i < datablocks; i++) {
    buf[i/8] = buf[i/8] | (0x1 << (i%8));
  }
  wsect(global_start_addr + 3, buf);

  //WRITE OUT INODE BITMAP
  bzero(buf, 512);
  for(i=0; i < freeinode; i++) {
    buf[i/8] = buf[i/8] | (0x1 << (i%8));
  }
  wsect(global_start_addr + 2, buf);
}

#define min(a, b) ((a) < (b) ? (a) : (b))

void
iappend(uint inum, void *xp, int n)
{
  char *p = (char*) xp;
  //fbn - data block of the end of the file before we write to it.
  //off - current file size.
  uint fbn, off, n1;
  struct dinode din;
  char buf[512];
  uint indirect[NINDIRECT];
  uint x;

  rinode(inum, &din);

  off = xint(din.size);
  while(n > 0){
    fbn = off / 512;
    assert(fbn < MAXFILE);
    
    //FILE FITS INTO DIRECT BLOCKS
    if(fbn < NDIRECT) {
      if(xint(din.addrs[fbn]) == 0) {
        din.addrs[fbn] = xint(freeblock++);
        usedblocks++;
      }
      x = xint(din.addrs[fbn]);
    } else {
      
      //NEED TO ALLOCATE INDIRECT BLOCK
      if(xint(din.addrs[INDIRECT]) == 0) {
        // printf("allocate indirect block\n");
        din.addrs[INDIRECT] = xint(freeblock++);
        usedblocks++;
      }
      // printf("read indirect block\n");
      rsect(xint(din.addrs[INDIRECT]), (char*) indirect);
      
      //NEED TO ALLOCATE NEW ENTRY IN INDIRECT BLOCK
      if(indirect[fbn - NDIRECT] == 0) {
        indirect[fbn - NDIRECT] = xint(freeblock++);
        usedblocks++;
        wsect(xint(din.addrs[INDIRECT]), (char*) indirect);
      }
      x = xint(indirect[fbn-NDIRECT]);
    }
    n1 = min(n, (fbn + 1) * 512 - off);
    rsect(x, buf);
    bcopy(p, buf + off - (fbn * 512), n1);
    wsect(x, buf);
    n -= n1;
    off += n1;
    p += n1;
  }
  din.size = xint(off);
  winode(inum, &din);
}
