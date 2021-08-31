// seg's string defucker

#include <stdio.h>
#include <stdint.h>
#include <string.h>

uint16_t hash_16(uint16_t hash) {
  hash ^= hash <<  5;
  hash ^= hash << 13;
  hash ^= hash <<  1;
  
  return hash;
}

uint16_t dexorshl(uint16_t x, int shift) {
  for (int i = shift; i < 16; i <<= 1) {
    x ^= x << i;
  }

  return x;
}

uint16_t dehash_16(uint16_t hash) {
  hash = dexorshl(hash,  1);
  hash = dexorshl(hash, 13);
  hash = dexorshl(hash,  5);
  
  return hash;
}

int dehex(char c) {
  if (c >= '0' && c <= '9') {
    return (c - '0');
  } else {
    return (c - 'A') + 10;
  }
}

int main(int argc, const char **argv) {
  if (argc < 2) return 1;
  uint16_t key = 7;
  
  while (*(argv[1])) {
    key += (uint16_t)(*(argv[1]));
    argv[1]++;
  }
  
  key = hash_16(key);
  
  for (int i = 2; i < argc; i++) {
    int length = strlen(argv[i]);
    uint32_t total = 0;
    
    uint16_t sums[length >> 2];
    
    for (int j = 0; j < length; j += 4) {
      uint16_t hash = 0;
      
      hash = (hash << 4) | (uint16_t)(dehex(argv[i][j + 0]));
      hash = (hash << 4) | (uint16_t)(dehex(argv[i][j + 1]));
      hash = (hash << 4) | (uint16_t)(dehex(argv[i][j + 2]));
      hash = (hash << 4) | (uint16_t)(dehex(argv[i][j + 3]));
      
      uint16_t sum = (dehash_16(hash) ^ key) - 7;
      sums[j >> 2] = sum;
      total += sum;
      
      key = hash_16(hash ^ key);
    }
    
    total /= ((length >> 2) - 1);
    
    for (int j = 0; j < length >> 2; j++) {
      putchar(total - sums[j]);
    }
    
    putchar('\n');
  }
  
  return 0;
}
