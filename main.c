#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define INFO "Information Logging \n"
#define ERR "Error Logging \n"

int socket_fd;

void print_message(char *type, char *message) {
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  printf("timestamp: %d-%02d-%02d %02d:%02d:%02d\t", tm.tm_year + 1900,
         tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
  printf("%s", type);
  printf("\t");
  printf(" MESSAGE: %s \n", message);
  fflush(stdout);
}

void handle_sigint(int signal) {
  print_message(INFO, "Shutting down server");
  close(socket_fd);
  exit(0);
}

void *handle_connection(void *client_fd_ptr) {
  char buffer[256] = {0};
  int client_fd = *(int *)client_fd_ptr;
  free(client_fd_ptr);
  /*
   * return data from recv of the format:
   * GET /file.html
   */
  recv(client_fd, buffer, 256, 0);

  int open_fd = open("./static/index.html", O_RDONLY);

  struct stat file_stat;
  if (fstat(open_fd, &file_stat) < 0) {
    perror("fstat failed");
    print_message(ERR, "fstat error has occured");
    return NULL;
  }

  off_t file_size = file_stat.st_size;

  // Convert file size to a string
  char content_length_header[64];
  snprintf(content_length_header, sizeof(content_length_header),
           "Content-Length: %ld\r\n", file_size);

  const char *http_response = "HTTP/1.1 200 OK\r\n"
                              "Content-Type: text/html\r\n";

  send(client_fd, http_response, strlen(http_response), 0);
  send(client_fd, content_length_header, strlen(content_length_header), 0);
  send(client_fd, "\r\n", 2, 0); // End of headers

  sendfile(client_fd, open_fd, NULL, file_size);

  close(client_fd);
  close(open_fd);

  return NULL;
}

int main() {

  signal(SIGINT, handle_sigint);

  const char *STATIC_PATH = "./static/";
  print_message(INFO, "Starting Server Execution");

  socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in addr = {0};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(8080);

  int bind_ret = bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr));

  if (bind_ret == -1) {
    print_message(ERR, "Socket binding has failed");
    return bind_ret;
  }
  print_message(INFO, "Socket bind done");

  int listen_ret = listen(socket_fd, 10);

  if (listen_ret == -1) {
    print_message(ERR, "Unsuccessful in assigning passive socket and ip");
    close(socket_fd);
    return 1;
  }
  print_message(
      INFO,
      "Listening socket established successfully. Listening on port: 8080");

  while (1) {

    int *client_fd = (int *)malloc(sizeof(int));
    *client_fd = accept(socket_fd, 0, 0);

    if (*client_fd == -1) {
      free(client_fd);
      print_message(ERR, "Client File Descriptor Failed");
      continue;
    }

    pthread_t thread_id;
    int thread_create_ret =
        pthread_create(&thread_id, NULL, handle_connection, client_fd);

    if (thread_create_ret != 0) {
      print_message(ERR, "Thread cration failed");
      close(*client_fd);
      free(client_fd);
      continue;
    } else {
      print_message(INFO, "Thread creation successful. Another client instance "
                          "is being processed.");
    }
    int thread_detach_ret = pthread_detach(thread_id);
    if (thread_detach_ret != 0) {
      print_message(
          ERR,
          "Thread detachment successful. Client session closed successfully");
    }
  }

  close(socket_fd);

  return 0;
}
