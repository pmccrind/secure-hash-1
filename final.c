/*
 * Sha-1.
 * Current: makes correct number of blocks,
 * blocks filled correctly, size is correct.
 * Working for all three files.
 * Patrick McCrindle
 * 5/4/2018
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define CHARS_IN_BLOCK 64
#define MAX_SIZE 1048576



unsigned int f0(unsigned int, unsigned int, unsigned int);
unsigned int f1(unsigned int, unsigned int, unsigned int);
unsigned int f2(unsigned int, unsigned int, unsigned int);
unsigned int f3(unsigned int, unsigned int, unsigned int);
unsigned int shift(unsigned int, int);
unsigned int blockCount(unsigned int);
unsigned int makeWord(unsigned char, unsigned char, unsigned char, unsigned char);

int main(int argc, char * argv[]) {
    
    //# of chars from ftell/fseek
    unsigned int x = 0;
    //to increment buffer
    unsigned int y = 0;
    //number of blocks from blockCount
    unsigned int blocks;
    //file read in
    FILE * filePointer = NULL;
    //File name from command line
    
    if (argc == 1) {
        printf("Please enter the name of the file you would like to hash.\n");
        exit(0);
    }
    
    char *fileName = argv[1]; 
    //incase of bad read
    int fileClosed = 0;
    //read in from buffer MUST BE SIGNED
    char ch0 = 'a';
    //r for read not editing file
    filePointer = fopen(fileName, "r"); 
    fseek(filePointer, 0, SEEK_END);
    //x = number of chars found
    x = ftell(filePointer);
    rewind(filePointer);
    //number of blocks found
    blocks = blockCount(x + 1);
    printf("Number of Blocks needed: %d\n", blocks);
    

    //arrays needed here
    unsigned int blockarr[blocks][16];
    unsigned int blockarr2[blocks][80];
    unsigned char buffer[MAX_SIZE];
    
    //constant K array
    unsigned int K[4];
    K[0] = 0x5A827999; //0 - 19
    K[1] = 0x6ED9EBA1; //20 - 39
    K[2] = 0x8F1BBCDC; //40-59
    K[3] = 0xCA62C1D6; //60 - 79
    
    //h array
    unsigned int H[5];
    H[0] = 0x67452301; //a 
    H[1] = 0xEFCDAB89; //b
    H[2] = 0x98BADCFE; //c
    H[3] = 0x10325476; //d
    H[4] = 0xC3D2E1F0; //e
    
    printf("Number of characters from ftell: %d\n", x);
    if (filePointer == NULL){
        printf("Cannot open.");
        exit(0);
    } else {
        while (((ch0= fgetc(filePointer)) != EOF) && (y < MAX_SIZE) ){
            //read in each char save to buffer
            //must be type cast
            buffer[y] = (unsigned char) ch0;
            //increment for buffer array
            //prints out each char going into the buffer
            //printf("Buffer hex value : %08x\n", buffer[y]);
            y++;
        }
        //check if file is too big
        if (y >= MAX_SIZE) {
            printf("File is too large to process.");
            exit(0);
        }
        //everything read in appead 80 char
        buffer[x] = 0x80;
        
        //increment for buffer
        unsigned int tempNum2 = 0;
        //counter for blocks
        unsigned int t;
        for (t = 0; t < blocks; t++) {
            //reset each time
            unsigned int tempNum = 0;
            //reset dummy array each time to ensure blank
            unsigned char temp[CHARS_IN_BLOCK];
            
            //for spilling into next block when buffer has all been read in
            //needs to br <= in case number of chars has excatly one left to
            //copy
            if (tempNum2 <= x){
                
                //read in 64 from buffer
                for (tempNum; tempNum < 64; tempNum++){
                    //assign from buffer to dummy array
                    temp[tempNum] = buffer[tempNum2];
                    //printf("Values going into the temp array: %08x\n", temp[tempNum]);
                    //if we hit the 80 char then the rest == 0
                    if (temp[tempNum] == 0x80) {
                        //increment past 80
                        tempNum++;
                        for (tempNum; tempNum < 64; tempNum++){
                            //make the rest 0's
                            temp[tempNum] = 0;
                        }
                    }
                    tempNum2++;
                }
            } else {
                //everything read in from buffer make the rest 0
                for (tempNum; tempNum < 64; tempNum++) {
                    temp[tempNum]= 0;
                }
            }
            //block number
            //printf("Block %d\n", m);
            
            //index in temp array
            unsigned int i;
            //word index
            unsigned int l = 0;
            // t = block count
            for (i = 0; l < 16; i += 4) {
                //saved directly to the 16 word array
                blockarr[t][l] = makeWord(temp[i], temp[i + 1] , temp[i + 2], temp[i + 3]);
                //used to print out each word
                //printf("W[%d] = %08x\n", l, blockarr[t][l]); 
                l++;
            }
        }
        //checks that the last block was == to m
        //printf("%d\n", m);
        
    }

    //word index
    unsigned int num4 = 0;
    // block index
    unsigned int num5 = 0;

    //Copy all words from 16 word array to 80 word array
    for (num5; num5 < blocks; num5++) {
        //Showing the number of blocks filled
        //printf("Block = %d\n", num5);
        for (num4 =0; num4 < 16; num4++){
            //from 16 word array of block size to a 80 word array of block size
            blockarr2[num5][num4] = blockarr[num5][num4];
            //printf("Words [%d] = %08x\n", num4, blockarr2[num5][num4]);
        }
    }
    
    //puts in size in the last word in the last block
    //outside the loop as to not reassign it each time
    blockarr2[(blocks - 1)][15] += (unsigned int)(x * 8);
    //checks the size that was appended to the last word in the last block
    printf("Size inserted into the last block in the last index: %08x \n", blockarr2[(blocks - 1)][15]);
    
    //step 7 part b
    unsigned int q;
    //Fill in 80 word array from indexes 16 - 79 with shift function
    for (q = 0; q < blocks; q++) {
        unsigned int t = 16;        
        for (t = 16; t < 80; t++) {
            blockarr2[q][t] = shift((blockarr2[q][t-3] ^ blockarr2[q][t-8] ^ blockarr2[q][t-14] ^ blockarr2[q][t - 16]), 1);   
            //Prints out each word scrambled with the ones previous
           // printf("Words [%d] = %08x\n", t, blockarr2[q][t]);
        }
    }
    

    //header for each part
   // printf("\n\t A \t\t B \t\t C \t\t D \t\t E \n");
    
    //step 7 part c 
    for (q = 0; q < blocks; q++) {
        unsigned int t;
        unsigned int TEMP;
        unsigned int A = H[0];
        unsigned int B = H[1];
        unsigned int C = H[2];
        unsigned int D = H[3];
        unsigned int E = H[4];
        for (t = 0; t < 80; t++) { 
            //t 0 -19
            if ((t >= 0) && (t <= 19)) {
                TEMP = shift(A, 5) + f0(B, C, D) + E + blockarr2[q][t] + K[0];
                E = D;  D = C; C = shift(B, 30); B = A; A = TEMP;
            }
            //t 20 - 39
            if ((t >= 20) && (t <= 39)) {
                TEMP = shift(A, 5) + f1(B, C, D) + E + blockarr2[q][t] + K[1];
                E = D;  D = C;   C = shift(B, 30); B = A; A = TEMP; 
            }
        
            //t 40 - 59
            if ((t >= 40) && (t <= 59)) {
                TEMP = shift(A, 5) + f2(B, C, D) + E + blockarr2[q][t] + K[2];
                E = D;  D = C;   C = shift(B, 30); B = A; A = TEMP;
            }
        
            //t 60 - 79
            if ((t >= 60) && (t <= 79)) {
                TEMP = shift(A, 5) + f3(B, C, D) + E + blockarr2[q][t] + K[3];
                E = D;  D = C;   C = shift(B, 30); B = A; A = TEMP;
            }
            //print out for each hash at each spot
  //          printf("\n t = %d: %08x \t %08x \t %08x \t %08x \t %08x \n", t, A, B, C, D, E);

        }
        //step 7 e
        H[0] = H[0] + A; H[1]= H[1] + B; H[2] = H[2] + C; H[3] = H[3] + D; H[4] = H[4] + E; 
   //    printf("\nH[0] = %08x \nH[1] = %08x\nH[2] = %08x\nH[3] = %08x\nH[4] = %08x\n",H[0], H[1], H[2], H[3], H[4]);
    }
    printf("\nMESSAGE DIGEST: \n\n%08x %08x %08x %08x %08x \n\n", H[0], H[1], H[2], H[3], H[4]);
    
    
    //close file and check if closed properly
    fileClosed = fclose(filePointer);
    if (0 == fileClosed){
        //successful close
    } else {
        printf("File connection %s not closed successfully.\n", fileName);
    }
}

//helper function 1 from tech document
unsigned int f0(unsigned int B, unsigned int C, unsigned int D) { 
    unsigned int x = B;
    unsigned int y = C;
    unsigned int z = D;
    unsigned int q; //may need to change
    q = (x & y) | ((~x) & z); //( 0 <= t <= 19)
    return q;
}

//helper function 2 from tech document
unsigned int f1(unsigned int B, unsigned int C, unsigned int D){
    unsigned int x = B;
    unsigned int y = C;
    unsigned int z = D;
    unsigned int q;

    q = (x ^ y ^ z); //(20<= t <= 39)
    return q;
}

//helper function 3 from tech document
unsigned int f2(unsigned int B, unsigned int C, unsigned int D){
    unsigned int x = B;
    unsigned int y = C;
    unsigned int z = D;
    unsigned int q;

    q = (x & y) | (x & z) | (y & z); //(40<= t <= 59)
    return q;
}

//helper function 4 from tech document
unsigned int f3(unsigned int B, unsigned int C, unsigned int D){
    unsigned int x = B;
    unsigned int y = C;
    unsigned int z = D;
    unsigned int q;

    q = (x ^ y ^ z); //(60<= t <= 79)
    return q;
}

//shift by n number with unsigned int x
unsigned int shift(unsigned int x, int n) {
    unsigned int q = x;
    unsigned int k = n;
    unsigned int f = 0;

    f = ((q << k) | (q >> (32 - k)));
    return f;
}

//Calculates number of blocks needed
unsigned int blockCount(unsigned int x){
    unsigned int q = x + 1; //one must be added for the 80 char
    unsigned int w;
    //first block always calculate
    w = floor((q/64)) + 1;
    
    if ((q % 64) > 56){
        w++;
    }
    return w; 
}


//make a word from 4 chars
unsigned int makeWord(unsigned char a, unsigned char b, unsigned char c, unsigned char d){
    unsigned int x= 0;
//    printf("%x %x %x %x\n", a, b, c, d);
    x = ((16 * 16) * a);
    x = x + b;
//    printf("%x\n", x);
    x *= (16 *16);
    x += c;
//    printf("%x\n", x);
    x *= (16 * 16);
    x += d;      
//    printf("%x\n", x);
    //one word made return the word
    return x;

}
