// sim3.c

#include <stdio.h>
#include <stdlib.h>

int read_one_line(unsigned *p)
// Read one line of the input stream.
// Return value is normally 'r' or 'w' to indicate read or write.
// In that case, *p contains the address read from the input line.
// Return value is 'e' to indicate that input failed at the end of
// the input stream.

{
  int nscan, rw;
  char buf[2];

  nscan = scanf("%1s%x", buf, p);
  if (nscan == EOF)
    return 'e';                 /* indicate end-of-file */
  else if (nscan != 2) {
    fprintf(stderr, "Format error in input stream.\n");
    exit(1);
  }

  rw = buf[0];
  if (rw != 'r' && rw != 'w') {
    fprintf(stderr, "Read/write character was neither r nor w.\n");
    exit(1);
  }
  return rw;
}

// These two arrays keep track of all the V-bits and stored tags in
// the array.  We don't need an array for data to count hits and misses.
// Because these arrays are external variables, it's safe to assume
// it will be initialized to all zeros before main starts.
char v_array[256];
unsigned tag_array[256];

int main(void)
{
  int read_count = 0, read_hits = 0;
  int write_count = 0, write_hits = 0;
  int access_count, miss_count; 
  int rw;
  unsigned address, tag, set;
  int hit;

  while (1) {
    rw = read_one_line(&address);
    if (rw == 'e') break;

	//256 blocks = 2^8 blocks
	//byte offset = log2(4) = 2 bits
	//block offset = log2(16) = 4 bits
	//set = log2(256) = 8 bits
	//tag = 32-8-4-2 = 18 bits
    set = (address & 0x00003fc0) >> 6; // bits 13-6
    tag = address >> 14;          // bits 31-14

    // Note: Next line results in either hit == 1 or hit == 0.
    hit = v_array[set] == 1 && tag_array[set] == tag;
    if (rw == 'r') {
      read_count++;
      read_hits += hit;
    }
    else {
      write_count++;
      write_hits += hit;
    }
    if (!hit) {                 // On a miss, update V-bit and tag.
      v_array[set] = 1;
      tag_array[set] = tag;
    }
  }

  printf("%d reads\n", read_count);
  printf("%d read hits\n", read_hits);  
  printf("%d writes\n", write_count);
  printf("%d write hits\n", write_hits);

  access_count = read_count + write_count;  
  miss_count = access_count - read_hits - write_hits;
  printf("overall miss rate: %.1f%%\n", 
         100.0 * (double) miss_count / access_count);

  return 0;
}
