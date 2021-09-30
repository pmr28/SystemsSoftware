#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int main() {
  FILE *tFile;
  tFile = fopen("traces/dave.trace", "r");

  char str[100];
  char operation[2];
  unsigned long address;
  unsigned int len;

  while (fgets(str, sizeof(str), tFile)) {
    sscanf(str, "%s %u, %u", operation, &address, &len);
    if (strcmp(operation, "L") == 0) {
      printf("%s, %u, %u\n", operation, address, len);
    }
  }

  char s[] = "String";
  printf("%c\n", s[2]);

  fclose(tFile);

  return 0;
}
