#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define COMLEN 1024

int rootexec(const char * restrict com);
int compare_password(const char *pass);

int
main(int argc, char **argv)
{
  /* INIT */
  char *com;

  /* NOT ENOUGH ARGUMENTS */
  if (argc == 1) {
    fputs("usage: flex [command] [command args]\n", stderr);
    return 1;
  }

  /* GET COMMAND */
  if (compare_password(getpass("Root password: "))) { /* RUN */ 
    com = malloc(COMLEN);

    strncpy(com, "", 1);
    for (argv++; *argv && strlen(com) < COMLEN;) {
      strncat(com, *argv++, COMLEN - strlen(com));
      strncat(com, " ", COMLEN - strlen(com));
    }

    if (!rootexec(com)) {
      free(com);
      return 1;
    }

    free(com);
    return 0;
  } 
    
  fputs("flex: Authentication failed\n", stderr);
  return 1;
}


int 
rootexec(const char *com) 
{
  /* MAKE AS ROOT */
  if (setuid(0) == -1) {
    fputs("Cannot be root\n", stderr);
    return 0;
  }

  /* RUN */
  system(com);

  /* RETURN TO DEFAULT USER */
  if (setuid(getuid()) == -1) {
    fputs("Cannot make user not root\n", stderr);
    return 0;
  }

  return 1;
}


#include <shadow.h>
#include <crypt.h>

#define equal(s1,s2)  (strcmp(s1, s2) == 0)

int 
compare_password(const char *pass) 
{
  struct spwd *root_entry;
  const char *stored_hash;
  char salt[256];
  const char *hashed_input;

  /* GET ROOT'S SHADOW ENTRY (REQUIRES ROOT PRIVILEGES) */
  if (!(root_entry = getspnam("root"))) {
    perror("Cannot compare");
    return 0;
  }

  /* EXTRACT SALT FROM STORED HASH (E.G., "$6$SOMESALT$...") */
  stored_hash = root_entry->sp_pwdp;
  strncpy(salt, stored_hash, sizeof(salt) - 1);
  salt[sizeof(salt)-1] = '\0';

  /* HASH THE INPUT PASSWORD WITH THE SAME SALT */
  if (!(hashed_input = crypt(pass, salt))) {
    perror("Cannot compare");
    return 0;
  }

  return equal(hashed_input, stored_hash);
}
