#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pwd.h>

#define uid_to_name(uid) (getpwuid(uid)->pw_name)

#define COMLEN   1024
#define PASSLEN  100

int exec(const char * restrict com, int uid);
int compare_password(const char *pass, int uid);
char *getcom(char *dst, char **src);
int
main(int argc, char **argv)
{
  /* INIT */
  char *com, *pass;
  int user_id;

  /* NOT ENOUGH ARGUMENTS */
  if (argc == 1) {
    fputs("usage: flex [command] [command args]\n", stderr);
    return 1;
  }

  /* CHECK ARGUMENTS */
  user_id = 0;
  if (*(*++argv+1) == 'u') {
    user_id = atoi(*++argv);
    argv++;
  }

  /* GET PASS */
  pass = malloc(PASSLEN);
  pass = NULL;
  if (user_id != getuid()) { /* IF NOT USERSELF */
    char prompt[256];
    snprintf(prompt, sizeof(prompt), "%s password: ", uid_to_name(user_id));
    pass = getpass(prompt);
  }

  /* RUN */
  if (compare_password(pass, user_id)) {
    com = malloc(COMLEN);
    getcom(com, argv);

    if (!exec(com, user_id)) {
      free(com); free(pass);
      return 1;
    }

    free(com);
    free(pass);
    return 0;
  } 
  
  /* FAIL */
  fputs("flex: Authentication failed\n", stderr);
  free(pass);
  return 1;
}


int 
exec(const char *com, int uid) 
{
  /* MAKE AS ROOT */
  if (setuid(uid) == -1) {
    fprintf(stderr, "Cannot run as %s\n", uid_to_name(uid));
    return 0;
  }

  /* RUN */
  system(com);

  /* RETURN TO DEFAULT USER */
  if (setuid(getuid()) == -1) {
    fputs("Cannot return user not root\n", stderr);
    return 0;
  }

  return 1;
}


#include <shadow.h>
#include <crypt.h>

#define equal(s1,s2)  (strcmp(s1, s2) == 0)

int 
compare_password(const char *pass, int uid) 
{
  if (!pass)
    return 1;

  struct spwd *root_entry;
  const char *stored_hash;
  char salt[256];
  const char *hashed_input;

  /* GET ROOT'S SHADOW ENTRY (REQUIRES ROOT PRIVILEGES) */
  if (!(root_entry = getspnam( getpwuid(uid)->pw_name ))) {
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



char 
*getcom(char *dst, char **src)
{
  strncpy(dst, "", 1);
  for (; *src && strlen(dst) < COMLEN;) {
    strncat(dst, *src++, COMLEN - strlen(dst));
    strncat(dst, " ", COMLEN - strlen(dst));
  }

  return dst;
}
