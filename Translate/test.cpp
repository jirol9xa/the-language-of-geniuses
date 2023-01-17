#include "string.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int foo() {
  // FILE *fp = fopen("text_test.txt", "r");

  // long length = 0;

  // fseek(fp, 0L, SEEK_END);
  // length = ftell(fp);
  // fseek(fp, 0L, SEEK_SET);

  // fprintf(stderr, "length = %d\n", length);

  // char buff[64] = {};

  // fread(buff, sizeof(char), length, fp);
  // buff[length] = '\0';

  // for (int i = 0; i < length; ++i)
  // {
  //     fprintf(stderr, "%x ", buff[i]);
  //     //if ()
  // }

  // fprintf(stderr, "\n");

  // fprintf(stderr, "%x ", 'а');
  // fprintf(stderr, "%x ", 'б');
  // fprintf(stderr, "%x ", 'в');
  // fprintf(stderr, "%x ", 'г');

  // fprintf(stderr, "\n");

  char xui[64] = "bubu";

  printf("%s\n", xui);

  sprintf(xui, "hehe");

  fprintf(stderr, "%s\n", xui);

  return 0;
}

union Union_t {
  int number;
  struct {
    unsigned is_null : 1;
    unsigned is_LCanary : 1; // Error & (1 << 1)    Error.is_LCanary
    unsigned is_RCanary : 1;
  } field;
} Union;

#define TO_STR(param) ##param

int main() {
  Union_t bebra = {};
  bebra.field.is_RCanary = 1;

  printf("value = %ld\n", sizeof(bebra));

  TO_STR(876);

  printf("strcmp = %d\n", strcmp(TO_STR(xui), "xui"));

  return 0;
}
