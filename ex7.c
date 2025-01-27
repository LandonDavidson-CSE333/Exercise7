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

// Print the contents of the given filename using POSIX read and handle errors
void printFile(char*);

int main(int argc, char** argv) {
  // Requires on argument (A directory name)
  if (argc != 2) {
    fprintf(stderr, "Requires one argument");
    return EXIT_FAILURE;
  }
  // Open given directory
  DIR *dp = openDirectory(argv[1]);
  // Check that dp actually has entries in it
  struct dirent *ep = readdir(dp);
  if (ep == NULL) {
    fprintf(stderr, "Given directory %s doesn't have any entries", argv[1]);
    perror("");
    return EXIT_FAILURE;
  }
  // Loop through all entries ending in ".txt" and print to stdout
  while(ep != NULL) {
    // Move to next entry if the file isn't a .txt
    if (!isTextFile(ep->d_name)) {
      continue;
    }
    // Open current file
    int fd = openFile(ep->d_name);

    // Close current file and move to the next while handling errors
    if (close(fd) == -1) {
      fprintf(stderr, "Closing the file %s failed", ep->d_name);
      perror("");
      exit(EXIT_FAILURE);
    }
    ep = readdir(dp);
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
    perror("");
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
  // Copy the last four bytes and check they equal .txt
  src = src + length - 4;
  char* extension = "";
  strcpy(extension, src);
  if (strcmp(".txt", extension) == 0) {
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
      perror("");
      exit(EXIT_FAILURE);
    }
    fd = open(file, O_RDONLY);
  }
  return fd;
}

void printFile(char* file) {
  return;
}
