#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fs.h"

char data1[512];
char data2[512];
int b,bi,m;

int balloc();
void bfree(uint b);

int main(int argc, char *argv[])
{
    int i;

    //for (i = 0; i < 20; i++) {
        //printf("IBLOCK(%d)=%d\n", i, IBLOCK(i));
    //}
    //for (i = 190; i < 190 + 20; i++) {
        //printf("IBLOCK(%d)=%d\n", i, IBLOCK(i));
    //}

    // OK
    //printf("IBITBLOCKS=%d\n", IBITBLOCKS);
    //printf("DBITBLOCKS=%d\n", DBITBLOCKS);

    // OK
    //for (i = 0; i < 20; i++) {
        //printf("GROUPI(%d)=%d\n", i, GROUPI(i));
    //}
    //for (i = 190; i < 190 + 20; i++) {
        //printf("GROUPI(%d)=%d\n", i, GROUPI(i));
    //}
    //for (i = 390; i < 390 + 20; i++) {
        //printf("GROUPI(%d)=%d\n", i, GROUPI(i));
    //}
    //for (i = 590; i < 590 + 20; i++) {
        //printf("GROUPI(%d)=%d\n", i, GROUPI(i));
    //}
    
    // OK
    //for (i = 0; i < 20; i++) {
        //printf("GROUPB(%d)=%d\n", i, GROUPB(i));
    //}
    //for (i = 986; i < 986 + 20; i++) {
        //printf("GROUPB(%d)=%d\n", i, GROUPB(i));
    //}
    //for (i = 1982; i < 1982 + 20; i++) {
        //printf("GROUPB(%d)=%d\n", i, GROUPB(i));
    //}

    //for (i = 0; i < 20; i++) {
        //printf("DBLOCK(%d)=%d\n", i, DBLOCK(i));
    //}
    //for (i = 986; i < 986 + 20; i++) {
        //printf("DBLOCK(%d)=%d\n", i, DBLOCK(i));
    //}
    //for (i = 1982; i < 1982 + 20; i++) {
        //printf("DBLOCK(%d)=%d\n", i, DBLOCK(i));
    //}

    //for (i = 0; i < 20; i++) {
        //printf("IBBLOCK(%d)=%d\n", i, IBBLOCK(i));
    //}
    //for (i = 190; i < 190 + 20; i++) {
        //printf("IBBLOCK(%d)=%d\n", i, IBBLOCK(i));
    //}
    //for (i = 390; i < 390 + 20; i++) {
        //printf("IBBLOCK(%d)=%d\n", i, IBBLOCK(i));
    //}

    //for (i = 0; i < 20; i++) {
        //printf("DBBLOCK(%d)=%d\n", i, DBBLOCK(i));
    //}
    //for (i = 986; i < 986 + 20; i++) {
        //printf("DBBLOCK(%d)=%d\n", i, DBBLOCK(i));
    //}
    //for (i = 1982; i < 1982 + 20; i++) {
        //printf("DBBLOCK(%d)=%d\n", i, DBBLOCK(i));
    //}
    
    memset(data1, 0, 512);
    memset(data2, 0, 512);
    int blocknums[1000];
    
    for(i = 0; i < 1000; i++){
        printblock(DBBLOCK(b));
        blocknums[i] = balloc();
        printf("balloc: %d\n", blocknums[i]);
        printblock(DBBLOCK(b));
        printf("\n");
    }

    printf("FREEING\n");

    for(i = 0; i < 1000; i++){
        printblock(DBBLOCK(blocknums[i]));
        bfree(blocknums[i]);
        printblock(DBBLOCK(blocknums[i]));
        printf("\n");
    }

    return 0;
}

char *getdb(int block)
{
    if (block == 3)
        return data1;
    else if (block == 1027)
        return data2;
    else {
        printf("bad block\n");
        exit(-1);
    }
}
void bin_prnt_byte(int x)
{
    int n;
    for(n=0; n<8; n++)
    {
        if((x & 0x80) !=0)
        {
            printf("1");
        }
        else
        {
            printf("0");
        }
        if (n==3)
        {
            printf(""); /* insert a space between nybbles */
        }
        x = x<<1;
    }
}

void printblock(int block)
{
    char *blk;
    if (block == 3)
        blk = data1;
    else if (block == 1027)
        blk = data2;
    else {
        printf("bad block\n");
        exit(-1);
    }

    int i;
    for (i = 0; i < 512; i++) {
        bin_prnt_byte(blk[i]);
        printf(" ");
    }
    printf("\n");
}

int balloc()
{
    char *datablock;
    for(b = 0; b < (DPG * 8) ; b += DPG){
        datablock = getdb(DBBLOCK(b));
        printf("balloc: bitmap block for %d is %d\n", b, DBBLOCK(b));
        //printblock(DBBLOCK(b));

        for(bi = 0; bi < DPG; bi++){
            m = 1 << (bi % 8);
            if((datablock[bi/8] & m) == 0){
                datablock[bi/8] |= m;
                return b + bi;
            }
        }
    }

    return -1;
}

void
bfree(uint b)
{
    int bi, m;
    char *datablock;

    datablock = getdb(DBBLOCK(b));
    bi = b % DPG;
    printf("block: %d, bi: %d, ", b, bi);
    m = 1 << (bi % 8);
    printf("m: ");
    bin_prnt_byte(m);
    printf("\n");
    
    printf("bfree: bitmap block for %d is %d\n", b, DBBLOCK(b));

    if((datablock[bi/8] & m) == 0) {
        printf("freeing free block\n");
        exit(-1);
    }
    datablock[bi/8] &= ~m;
}
