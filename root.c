#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>
#include <stdarg.h>

void warn(const char *format, ...) {
  fprintf(stderr, "\033[1;33;40mWARNING: \033[1m");

  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);

  fprintf(stderr, "\033[0m\n");
}

char* get_abs_path(const char* rel_path) {
  char* abs_path = malloc(PATH_MAX);

  if (abs_path == NULL) {
    warn("Failed to allocate memory for fetching absolute path.");
    return NULL;
  }

  if (realpath(rel_path, abs_path) == NULL) {
    warn("Failed to get the absolute path.");
    free(abs_path);
    return NULL;
  }

  return abs_path;
}

int exec_sudo(int argc, char* argv[]) {
  const char* command = "/usr/bin/sudo";
  char* abs_path = get_abs_path(argv[0]);

  if (abs_path == NULL) {
    warn("Could not get absolute path of self. Not changing UID or GID.");
    return 1;
  }

  struct stat info;
  if (stat(abs_path, &info) != 0) {
    warn("Could not get stat information. Not changing UID or GID.");
    free(abs_path);
    return 1;
  }

  int SUID = info.st_mode & S_ISUID;
  int SGID = info.st_mode & S_ISGID;

  if (SUID) {
    if (setuid(info.st_uid) != 0) {
      warn("Setuid bit has been set, but appears that couldn't switch to it.");
    }
  }

  if (SGID) {
    if (setgid(info.st_gid) != 0) {
      warn("Setgid bit has been set, but appears that couldn't switch to it.");
    }
  }

  free(abs_path);

  // Calculate the required length for the full command
  size_t full_command_length = strlen(command) + 1; // +1 for null terminator
  for (int i = 1; i < argc; i++) {
    full_command_length += strlen(argv[i]) + 1; // +1 for space separator
  }

  // Allocate memory for the full command
  char* full_command = malloc(full_command_length);

  if (full_command == NULL) {
    warn("Failed to allocate memory for processing the sudo command.");
    return 1;
  }

  // Construct the command with its arguments
  strncpy(full_command, command, full_command_length);
  full_command[full_command_length - 1] = '\0'; // Ensure null termination

  for (int i = 1; i < argc; i++) {
    strncat(full_command, " ", full_command_length - strlen(full_command) - 1);
    strncat(full_command, argv[i], full_command_length - strlen(full_command) - 1);
  }

  int ret_code = system(full_command);
  if (ret_code == -1) {
    ret_code = 1;
  } else if (ret_code != 127 && WIFEXITED(ret_code)) {
    ret_code = WEXITSTATUS(ret_code);
  }

  free(full_command);

  return ret_code;
}

int main(int argc, char* argv[]) {
  return exec_sudo(argc, argv);
}
