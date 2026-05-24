/** from: https://man7.org/linux/man-pages/man2/open_by_handle_at.2.html */
#define _GNU_SOURCE
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  int                 mount_id, fhsize, flags, dirfd;
  char               *path;
  struct file_handle *fhp;

  if(argc != 2) {
    fprintf(stderr, "Usage: %s path\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  path = argv[1];

  /* Allocate file_handle structure.  */

  fhsize = sizeof(*fhp);
  fhp    = malloc(fhsize);
  if(fhp == NULL) err(EXIT_FAILURE, "malloc");

  /* Make an initial call to name_to_handle_at() to discover
              the size required for file handle.  */

  dirfd             = AT_FDCWD; /* For name_to_handle_at() calls */
  flags             = 0;        /* For name_to_handle_at() calls */
  fhp->handle_bytes = 0;
  if(name_to_handle_at(dirfd, path, fhp, &mount_id, flags) != -1 ||
     errno != EOVERFLOW)
  {
    fprintf(stderr, "Unexpected result from name_to_handle_at()\n");
    exit(EXIT_FAILURE);
  }

  /* Reallocate file_handle structure with correct size.  */

  fhsize = sizeof(*fhp) + fhp->handle_bytes;
  fhp    = realloc(fhp, fhsize); /* Copies fhp->handle_bytes */
  if(fhp == NULL) err(EXIT_FAILURE, "realloc");

  /* Get file handle from pathname supplied on command line.  */

  if(name_to_handle_at(dirfd, path, fhp, &mount_id, flags) == -1)
    err(EXIT_FAILURE, "name_to_handle_at");

  /* Write mount ID, file handle size, and file handle to stdout,
              for later reuse by t_open_by_handle_at.c.  */

  printf("%d\n", mount_id);
  printf("%u %d   ", fhp->handle_bytes, fhp->handle_type);
  for(size_t j = 0; j < fhp->handle_bytes; j++)
    printf(" %02x", fhp->f_handle[j]);
  printf("\n");

  exit(EXIT_SUCCESS);
}