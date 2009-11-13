// On-disk file system format. 
// Both the kernel and user programs use this header file.

// Block 0 is unused.
// Block 1 is super block.
// Inodes start at block 2.

#define BSIZE 512
#define GROUP_BLOCKS 1024
#define GROUPS 8
#define FSSIZE (GROUPS * GROUP_BLOCKS)

// File system super block
struct superblock {
  uint size;         // Size of file system image (blocks)
  uint nblocks;      // Number of data blocks
  uint ninodes;      // Number of inodes.
  uint ngroups;      // Number of inodes.
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

// Inodes per group.
#define IPG 200

// Inodes per group.
#define DPG 994

// Block containing inode i
// 4 is: 1 for E block, 1 for SB, 1 for the ibitmap, 1 for the dbitmap
// Add the offset to the correct cyl grp too.
#define IBLOCK(i)     (((i) / IPB + 4) + ((i) / IPG) * GROUP_BLOCKS)

// Bitmap bits per block
#define BPB           (BSIZE*8)

// Bitmap Block containing the bit for block b
// 3 is for E block, SB, ibitmap block
#define BBLOCK(b) ((b/BPB + 3) + ((b) / DPG) * GROUP_BLOCKS)

// inode bitmap block containing the bit for inode i
// 2 is for E block, SB
#define IBBLOCK(i) ((i/BPB + 2) + ((i) / IPG) * GROUP_BLOCKS)

// Directory is a file containing a sequence of dirent structures.
#define DIRSIZ 14

struct dirent {
  ushort inum;
  char name[DIRSIZ];
};

