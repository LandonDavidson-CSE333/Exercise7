// Copyright 2025 Landon Davidson
// landond@uw.edu

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

// Takes a directory name and returns a DIR* opened with POSIX opendir while handling errors
DIR* openDirectory(char*);

// Takes a string and returns true if it ends in .txt
bool isTextFile(char*);

// Takes a file name and opens it using POSIX calls, returns file descriptor
int openFile(char*);

// Print the contents of the given file descriptor using POSIX read and handle errors
void printFile(int, char*);

int main(int argc, char** argv) {
  // Requires on argument (A directory name)
  if (argc != 2) {
    fprintf(stderr, "Requires one argument");
    return EXIT_FAILURE;
  }

  // Normalize directory name to end in a /
  size_t size = strlen(argv[1]);
  if (argv[1][size - 1] != '/') {

  }

  // Open given directory
  DIR *dp = openDirectory(argv[1]);

  // Check that dp actually has entries in it
  struct dirent *ep = readdir(dp);
  if (ep == NULL) {
    fprintf(stderr, "Given directory %s doesn't have any entries", argv[1]);
    perror(": ");
    return EXIT_FAILURE;
  }

  // Loop through all entries ending in ".txt" and print to stdout
  while((ep = readdir(dp)) != NULL) {
    // Move to next entry if the file isn't a .txt
    if (!isTextFile(ep->d_name)) {
      continue;
    }

    // Open current file
    int fd = openFile(strcat(argv[1], ep->d_name));

    // Print the contents of the current file to standard out byte by byte
    printFile(fd, argv[1]);

    // Close current file
    close(fd);
  }
  // Close the directory now that we are done and return a success
  closedir(dp);
  return EXIT_SUCCESS;
}

DIR* openDirectory(char* dirName) {
  // Check that given directory opens successfully
  DIR *dp = opendir(dirName);
  if (dp == NULL) {
    fprintf(stderr, "Couldn't open the given directory %s", dirName);
    perror(": ");
    exit(EXIT_FAILURE);
  }
  return dp;
}

bool isTextFile(char* src) {
  // Get length from string length
  size_t length = strlen(src);
  // Check the name is long enough
  if (length < 4) {
    return false;
  }
  // Move pointer to the last four bytes and check it is .txt
  src = src + length - 4;
  if (strcmp(".txt", src) == 0) {
    return true;
  }
  return false;
}

int openFile(char* file) {
  // Attempt to open read only file and check for errors
  int fd = open(file, O_RDONLY);
  while (fd == -1) {
    // Retry on an EINTR and return failure otherwise
    if (errno != EINTR) {
      fprintf(stderr, "File %s failed to open", file);
      perror(": ");
      exit(EXIT_FAILURE);
    }
    fd = open(file, O_RDONLY);
  }
  return fd;
}

void printFile(int fd, char* name) {
  // Get the size of the current file
  off_t size = lseek(fd, 0, SEEK_END);
  // Return failure if lseek failed
  if (size == -1) {
    fprintf(stderr, "Failed to find the size of the file %s", name);
    perror(": ");
    close(fd);
    exit(EXIT_FAILURE);
  }
  // Move cursor back to start of file
  lseek(fd, 0, SEEK_SET);

  // Read whole file into buf
  char buf[size];
  long bytesRead = read(fd, buf, size);

  // On error return a failure
  if (bytesRead == -1) {
    fprintf(stderr, "Failed to read from file %s", name);
    perror("");
    close(fd);
    exit(EXIT_FAILURE);
  }
  // return failure when read didn't read the whole file
  if (bytesRead != size) {
    fprintf(stderr, "couldn't read whole file %s", name);
    perror("");
    close(fd);
    exit(EXIT_FAILURE);
  }

  // Print buf to stdout
  printf(buf);
}
