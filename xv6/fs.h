// On-disk file system format. 
// Both the kernel and user programs use this header file.

// Block 0 is unused.
// Block 1 is super block.
// Inodes start at block 2.

#define BSIZE 512  // block size
#define NUM_CYLINDER_GRPS 8

// File system super block
struct superblock {
  uint size;         // Size of file system image (blocks)
  uint nblocks;      // Number of data blocks
  uint ninodes;      // Number of inodes.
};

#define NADDRS (NDIRECT+1)
#define NDIRECT 12
#define INDIRECT 12
#define NINDIRECT (BSIZE / sizeof(uint))
#define MAXFILE (NDIRECT  + NINDIRECT)

// On-disk inode structure
struct dinode {
  short type;           // File type
  short major;          // Major device number (T_DEV only)
  short minor;          // Minor device number (T_DEV only)
  short nlink;          // Number of links to inode in file system
  uint size;            // Size of file (bytes)
  uint addrs[NADDRS];   // Data block addresses
};

#define T_DIR  1   // Directory
#define T_FILE 2   // File
#define T_DEV  3   // Special device

// Inodes per block.
#define IPB           (BSIZE / sizeof(struct dinode))

// Block containing inode i
// 4 is: 1 for E block, 1 for SB, 1 for the ibitmap, 1 for the dbitmap
#define IBLOCK(i)     ((i) / IPB + 4)

// Bitmap bits per block
#define BPB           (BSIZE*8)

// Bitmap Block containing the bit for block b
// 3 is for E block, SB, ibitmap block
#define BBLOCK(b) (b/BPB + 3)

// Bitmap Block containing the bit for block b
// 2 is for E block, SB
#define IBBLOCK(i) (i/BPB + 2)

// Directory is a file containing a sequence of dirent structures.
#define DIRSIZ 14

struct dirent {
  ushort inum;
  char name[DIRSIZ];
};

