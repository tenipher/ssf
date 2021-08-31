// seg's string fucker

#include <stdio.h>
#include <stdint.h>
#include <string.h>

uint16_t hash_16(uint16_t hash) {
  hash ^= hash <<  5;
  hash ^= hash << 13;
  hash ^= hash <<  1;
  
  return hash;
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
    
    for (int j = 0; j < length; j++) {
      uint16_t sum = 0;
      
      for (int k = 0; k < length; k++) {
        if (k != j) {
          sum += (uint16_t)(argv[i][k]);
        }
      }
      
      uint16_t hash = hash_16((sum + 7) ^ key);
      key = hash_16(hash ^ key);
      
      printf("%04X", hash);
    }
    
    putchar('\n');
  }
  
  return 0;
}
