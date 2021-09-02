/* 
 * The more letters you use for the message the more it takes
 * 
 * cypher:
 * ./sssc HI
 * 
 * decypher:
 * ./sssc $:5:17:2
 * 
 * decim will automatically know to differentiate between messages and
 * encrypted keys
 * 
 * - superleaf1995
 * 
 */


/* gcc sssc.c -o sssc */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define BLOCK_SIZE 128

unsigned char *master_seed = "7a ssf705m5m 8uggf7 h8d9 fd8sfsn8  shgf sdmf sd89fhj gfjd kwnu35n8g uf idigo";
size_t brk = 0;

int
do_decim_iter(dest, src)
    unsigned char *dest;
    unsigned char *src;
{
    size_t i, j;

    for(i = 0; i < brk; i++) {
        for(j = 0; j < BLOCK_SIZE; j++) {
            dest[j] ^= (dest[j + 1] ^ src[j]) + src[j] + i * brk;
            src[j] = dest[j];
        }
    }
    
    for(i = 0; i < BLOCK_SIZE; i++) {
		dest[i] = (dest[i] % 27) + 'A';
		src[i] = (src[i] % 27) + 'A';
	}
    ++brk;
    
    if(brk == (size_t)-1) {
		return -1;
	}
    return 0;
}

int
main(argc, argv)
    int argc;
    char **argv;
{
    unsigned char *haystack, *seed;
    register size_t i;

    if(argc <= 1) {
        fprintf(stderr, "Usage: decim [key]\n");
        exit(EXIT_FAILURE);
    }
    
    /* Decypher mode */
    if(argv[1][0] == '$') {
		size_t iter, offset, needle_size;
		const char *ptr;
		
		haystack = malloc(BLOCK_SIZE);
		if(haystack == NULL) {
			fprintf(stderr, "Allocation error\n");
			exit(EXIT_FAILURE);
		}
		
		seed = malloc(BLOCK_SIZE);
		if(seed == NULL) {
			fprintf(stderr, "Allocation error\n");
			exit(EXIT_FAILURE);
		}
		memcpy(seed, master_seed, BLOCK_SIZE);
		
		ptr = argv[1];
		ptr = strchr(ptr, ':') + 1;
		iter = atoi(ptr);
		
		ptr = strchr(ptr, ':') + 1;
		offset = atoi(ptr);
		
		ptr = strchr(ptr, ':') + 1;
		needle_size = atoi(ptr);
		
		printf("Iteration: %zu, Offset: %zu, Needle: %zu\n", iter, offset, needle_size);

		while(1) {
			const void *key_needle;
			if(do_decim_iter(haystack, seed) != 0) {
				fprintf(stderr, "Key not found\n");
				exit(EXIT_FAILURE);
			}
			
			for(i = 0; i < BLOCK_SIZE; i++) {
				if(isalnum(haystack[i])) {
					putchar(haystack[i]);
				} else if(haystack[i] < 'A') {
					putchar('.');
				}
			}
			printf("\r");
			
			if(brk == iter) {
				printf("\n");
				for(i = 0; i < offset; i++) {
					putchar('~');
				}
				
				for(i = 0; i < needle_size; i++) {
					putchar('^');
				}
				
				printf("\n");
				break;
			}
		}
	}
	/* Cypher mode */
	else {
		unsigned char *key_16, *first_key;
		size_t needle_size;
		
		needle_size = strlen(argv[1]);
		if(needle_size >= BLOCK_SIZE) {
			fprintf(stderr, "Salt cannot be longer than cypher block\n");
			exit(EXIT_FAILURE);
		}

		key_16 = malloc(needle_size);
		if(key_16 == NULL) {
			fprintf(stderr, "Allocation error\n");
			exit(EXIT_FAILURE);
		}
		memset(key_16, 0, needle_size);
		memcpy(key_16, argv[1], needle_size);
		for(i = 0; i < needle_size; i++) {
			key_16[i] = toupper(key_16[i]);
		}
		
		haystack = malloc(BLOCK_SIZE);
		if(haystack == NULL) {
			fprintf(stderr, "Allocation error\n");
			exit(EXIT_FAILURE);
		}
		
		first_key = malloc(BLOCK_SIZE);
		if(first_key == NULL) {
			fprintf(stderr, "Allocation error\n");
			exit(EXIT_FAILURE);
		}
		
		seed = malloc(BLOCK_SIZE);
		if(seed == NULL) {
			fprintf(stderr, "Allocation error\n");
			exit(EXIT_FAILURE);
		}
		memcpy(seed, master_seed, BLOCK_SIZE);

		while(1) {
			const void *key_needle;
			if(do_decim_iter(haystack, seed) != 0) {
				fprintf(stderr, "Key not found\n");
				exit(EXIT_FAILURE);
			}
			
			if(brk == 1) {
				memcpy(first_key, haystack, BLOCK_SIZE);
			} else {
				if(memcmp(first_key, haystack, BLOCK_SIZE) == 0) {
					fprintf(stderr, "Cipher repeats at %zu\n", brk);
					exit(EXIT_FAILURE);
				}
			}
			
			for(i = 0; i < BLOCK_SIZE; i++) {
				if(isalnum(haystack[i])) {
					putchar(haystack[i]);
				} else if(haystack[i] < 'A') {
					putchar('.');
				}
			}
			printf("\r");
			
			/* Find the needle in the haystack */
			key_needle = memchr(haystack, key_16[0], BLOCK_SIZE);
			if(key_needle != NULL) {
				if(memcmp(key_needle, key_16, needle_size) == 0) {
					size_t offset = (size_t)((ptrdiff_t)key_needle - (ptrdiff_t)haystack);
					printf("\nCypher: $:%zu:%zu:%zu\n", brk, offset, needle_size);
					break;
				}
			}
		}
	}
    return 0;
}
