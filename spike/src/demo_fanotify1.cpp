/** need sudo. */
#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fanotify.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <exception>

#define BUF_SIZE      256
#define MONITOR_MASKS (FAN_CREATE | FAN_DELETE | FAN_MOVE | FAN_ONDIR)

int main(int argc, char *argv[])
{
  int                             fd, ret, event_fd, mount_fd;
  ssize_t                         size, path_len;
  char                            path[PATH_MAX];
  char                            events_buf[BUF_SIZE];
  struct file_handle             *file_handle;
  struct fanotify_event_metadata *metadata;
  struct fanotify_event_info_fid *fid;
  const char                     *file_name;
  unsigned long long              dir_inode;
  char                            procfd_path[PATH_MAX];
  struct stat                     sb;

  if(argc != 2) {
    fprintf(stderr, "Invalid number of command line arguments.\n");
    exit(EXIT_FAILURE);
  }

  mount_fd = open(argv[1], O_DIRECTORY | O_RDONLY);
  if(mount_fd == -1) {
    perror(argv[1]);
    exit(EXIT_FAILURE);
  }

  /* Create an fanotify file descriptor with FAN_REPORT_DFID_NAME as
              a flag so that program can receive fid events with directory
              entry name.  */

  fd = fanotify_init(FAN_CLASS_NOTIF | FAN_REPORT_DFID_NAME, O_RDONLY);
  if(fd == -1) {
    perror("fanotify_init");
    exit(EXIT_FAILURE);
  }

  /* Place a mark on the filesystem object supplied in argv[1].  */

  ret = fanotify_mark(fd,
                      FAN_MARK_ADD | FAN_MARK_FILESYSTEM,
                      MONITOR_MASKS,
                      AT_FDCWD,
                      argv[1]);
  if(ret == -1) {
    perror("fanotify_mark");
    exit(EXIT_FAILURE);
  }

  printf("Listening for events.\n");

  /* Read events from the event queue into a buffer.  */
  while(1) {
    size = read(fd, events_buf, sizeof(events_buf));
    if(size == -1 && errno != EAGAIN) {
      perror("read");
      exit(EXIT_FAILURE);
    }

    /* Process all events within the buffer.  */

    for(metadata = (struct fanotify_event_metadata *)events_buf;
        FAN_EVENT_OK(metadata, size);
        metadata = FAN_EVENT_NEXT(metadata, size))
    {
      fid         = (struct fanotify_event_info_fid *)(metadata + 1);
      file_handle = (struct file_handle *)fid->handle;

      /* Ensure that the event info is of the correct type.  */

      if(fid->hdr.info_type == FAN_EVENT_INFO_TYPE_FID ||
         fid->hdr.info_type == FAN_EVENT_INFO_TYPE_DFID)
      {
        file_name = NULL;
      } else if(fid->hdr.info_type == FAN_EVENT_INFO_TYPE_DFID_NAME) {
        file_name =
            (const char *)file_handle->f_handle + file_handle->handle_bytes;
        dir_inode = *(unsigned long long *)(file_handle->f_handle);
      } else {
        fprintf(stderr, "Received unexpected event info type.\n");
        exit(EXIT_FAILURE);
      }

      event_fd = open_by_handle_at(mount_fd, file_handle, O_RDONLY);
      if(event_fd == -1) {
        if(errno == ESTALE) {
          printf(
              "File handle is no longer valid.  "
              "File has been deleted\n");
        } else {
          perror("open_by_handle_at");
          exit(EXIT_FAILURE);
        }
      }

      if(metadata->fd >= 0) {
        snprintf(procfd_path, sizeof(procfd_path), "/proc/self/fd/%d", metadata->fd);
      } else {
        snprintf(procfd_path, sizeof(procfd_path), "/proc/self/fd/%d", event_fd);
      }

      /* Retrieve and print the path of the modified dentry.  */

      path_len = readlink(procfd_path, path, sizeof(path) - 1);
      if(path_len == -1) {
        perror("readlink");
        exit(EXIT_FAILURE);
      }

      if(path_len == 1 && path[0] == '/') {
        continue;
      }

      printf("filename[%d]: %s", strlen(file_name), file_name);

      // 打印事件类型
      printf("[事件通知] \n");
      if(metadata->mask & FAN_CREATE) printf("新增 (CREATE) \n");
      if(metadata->mask & FAN_DELETE) printf("删除 (DELETE) \n");
      if(metadata->mask & FAN_MODIFY) printf("修改 (MODIFY) \n");
      if(metadata->mask & FAN_MOVE) printf("移动/重命名 (MOVE) \n");

      if(metadata->mask & FAN_ONDIR) {
        printf("[目录] \n");
      } else {
        printf("[文件] \n");
      }

      path[path_len] = '\0';
      printf("\tDirectory '%s' has been modified.\n", path);

      if(file_name) {
        ret = fstatat(event_fd, file_name, &sb, 0);
        if(ret == -1) {
          if(errno == ENOENT) {
            printf("\tEntry '%s' does not exist.\n", file_name);
          }
        } else if((sb.st_mode & S_IFMT) == S_IFDIR) {
          printf("\tEntry '%s:%lu' is a subdirectory.\n", file_name, sb.st_ino);
        } else {
          printf("\tEntry '%s:%lu' is not a subdirectory.\n", file_name, sb.st_ino);
        }
      }

      /* Close associated file descriptor for this event.  */

      close(event_fd);
    }
    /* Close associated file descriptor for this event.  */

    close(event_fd);
  }

  printf("All events processed successfully.  Program exiting.\n");
  exit(EXIT_SUCCESS);
}