#include <stdio.h>

// This small piece of code writes a csv file with the NAND ECC (Hamming) mappings between kernel code and TI AM3505.
// It is included for completeness purposes.
// The ECC values have been determined empirically.

//                ~0xf4f488 = 0B0B77                ~0x47B83C = B847C3          ~0xBCBC22 = 4343DD        ~0x6767aa = 989855
char *chip_eccs[] = { "000010110000101101110111", "101110000100011111000011", "010000110100001111011101", "100110001001100001010101",
//                ~9x626200 = 9D9DFF                ~0xb3b311 = 4C4CEE          ~0x5ea15a = A15EA5        ~0x39c6b4 = C6394B
                      "100111011001110111111111", "010011000100110011101110", "101000010101111010100101", "110001100011100101001011" };

//                           fc033c                       9595aa                 ccc03f                       cf0f00
char *linux_eccs[] = { "111111000000001100111100", "100101011001010110101010", "110011001100000000111111", "110011110000111100000000",
//                           ff0fcf                       f0c3c3                 659aa6                       956a59
                       "111111110000111111001111", "111100001100001111000011", "011001011001101010100110", "100101010110101001011001" };


#define PAIRS 7

char possible_pos[24][24];

int main(int argc, char **argv){

  int i,j;

  // init
  for(i=0;i<24;++i)
    for(j=0;j<24;j++)
      possible_pos[i][j] = 'y';

  int pair, bit_lin, bit_chip, pos;
  for(pair=0;pair<PAIRS;++pair){
    
    for(bit_lin=0;bit_lin<24;bit_lin++){
      
      for(bit_chip=0;bit_chip<24;bit_chip++){
//        printf("%c %c\n", linux_eccs[pair][bit_lin], chip_eccs[pair][bit_chip]);
        if(linux_eccs[pair][bit_lin] != chip_eccs[pair][bit_chip])
        {
          possible_pos[bit_lin][bit_chip] = ' ';
        }
      }
    }

  }

//  printf("            \t0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3\n", bit_lin);
//  printf("===============================================================\n", bit_lin);
  for(bit_lin=0;bit_lin<24;++bit_lin){
//    printf("linux bit %d:\t", bit_lin);
    for(pos=0;pos<24;++pos)
      printf("%c;", possible_pos[bit_lin][pos]);
    printf("\n");
  }

  return 0;
}
