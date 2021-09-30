#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int main() {
  int x[] = { 2, 6, 8, 10, 12 };
  int *p = x;
  int **pp = &p;
  (*pp)++;
  (*(*pp))++;
  printf("%d", *p);
}
