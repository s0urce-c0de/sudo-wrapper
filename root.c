#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>

char* get_abs_path(const char* rel_path) {
  char* abs_path = malloc(PATH_MAX);

  if (realpath(rel_path, abs_path) == NULL) {
    free(abs_path);
    return NULL;
  }

  return abs_path;
}

int exec_sudo(int argc, char* argv[]) {
  const char* command = "/usr/bin/sudo";
  char* abs_path = get_abs_path(argv[0]);
  
  if (abs_path != NULL) {
    struct stat info;
    if (stat(abs_path, &info) == 0) {
      int SUID = info.st_mode & S_ISUID;
      int SGID = info.st_mode & S_ISGID;
      if (SUID) {
        setuid(info.st_uid);
      }
      if (SGID) {
        setgid(info.st_gid);
      }
    } else {
      fprintf(stderr, "WARNING: Could not get stat information. Not changing UID or GID");
    }
    free(abs_path);
  } else {
    free(abs_path);
    printf("Could not get ABS Path");
  }
  // Construct the command with its arguments
  char full_command[256] = "";
  strcat(full_command, command);

  for (int i = 1; i < argc; i++) {
    strcat(full_command, " ");
    strcat(full_command, argv[i]);
  }

  int ret_code = system(full_command);
  if (ret_code == -1) {
    ret_code = 1;
  } else if (ret_code == 127) {} else {
    ret_code = ret_code >> 8;
  }

  return ret_code;
}

int main(int argc, char* argv[]) {
  return exec_sudo(argc, argv);
}
