// On-disk file system format. 
// Both the kernel and user programs use this header file.

// Block 0 is unused.
// Block 1 is super block.

#define BSIZE  512
#define GROUPS 8
// Blocks per group
#define BPG    1024
// Inodes per group.
#define IPG 200
// Data blocks per group.
#define DPG 996
#define FSSIZE (GROUPS * BPG)
// Inode blocks per grp
#define INODE_BPG ((IPG / IPB) + (IPG % IPB == 0 ? 0 : 1))
// Inodes per block.
#define IPB           (BSIZE / sizeof(struct dinode))
// The number of blocks occupied by the empty block and the sb
#define EMPTY  1
#define SB     1

// The group an inode or data block is in.
// TESTED
#define GROUPI(i) ((i) / IPG)
// TESTED
#define GROUPB(b) ((b) / DPG)

// 8 means 8 bits per byte
// The modulus at the end makes sure a block isnt wasted when the number of
// inodes is divisible by the number of bits in the bitmap
// BOTH TESTED
#define IBITBLOCKS (IPG / (BSIZE * 8) + (IPG % (BSIZE * 8) == 0 ? 0 : 1))
#define DBITBLOCKS (DPG / (BSIZE * 8) + (DPG % (BSIZE * 8) == 0 ? 0 : 1))

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

// Block containing inode i
// IBLOCK lines
// Constant empty block
// Offset to start of cyl grp
// Constant offset into cyl grp
// offset into inode blocks.
#define IBLOCK(i)                   \
    (EMPTY +                        \
    (GROUPI(i)  * BPG) +            \
    SB + IBITBLOCKS + DBITBLOCKS +  \
    (((i) - GROUPI(i) * IPG) / IPB ))

// Map data block number to block address
#define DBLOCK(b)                               \
    (EMPTY +                                    \
	(GROUPB(b) * BPG) +                         \
	SB + IBITBLOCKS + DBITBLOCKS + INODE_BPG +  \
	( b - GROUPB(b) * DPG))

// Bitmap bits per block
#define BPB (BSIZE * 8)

// Bitmap Block containing the bit for block b
#define DBBLOCK(b)     \
    (EMPTY +           \
	GROUPB(b) * BPG +  \
	SB + IBITBLOCKS +  \
	(((b) - GROUPB(b) * DPG) / BPB ))

// inode bitmap block containing the bit for inode i
#define IBBLOCK(i)     \
    (EMPTY +           \
    GROUPI(i) * BPG +  \
    SB +               \
    ((i - GROUPI(i) * IPG) / BPB ))

// Directory is a file containing a sequence of dirent structures.
#define DIRSIZ 14

struct dirent {
  ushort inum;
  char name[DIRSIZ];
};

