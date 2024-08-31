#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
/* #include <stdlib.h> */
#include <string.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define INFO "Information Logging \n"
#define ERR "Error Logging \n"

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

int main() {

  const char *STATIC_PATH = "./static/";
  print_message(INFO, "Starting Server Execution");

  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in addr = {AF_INET, 0x901f, 0};

  int bind_ret = bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr));

  if (bind_ret == -1) {
    print_message(ERR, "Socket binding has failed");
    return bind_ret;
  }
  print_message(INFO, "Socket bind done");

  int listen_ret = listen(socket_fd, 10);

  if (listen_ret == -1) {
    print_message(ERR, "Unsuccessful in assigning passive socket and ip");
  }
  print_message(
      INFO,
      "Listening socket established successfully. Listening on port: 8080");

  while (1) {

    int client_fd = accept(socket_fd, 0, 0);

    char buffer[256] = {0};
    /*
     * return data from recv of the format:
     * GET /file.html
     */
    recv(client_fd, buffer, 256, 0);

    int open_fd = open("./static/index.html", O_RDONLY);

    struct stat file_stat;
    if (fstat(open_fd, &file_stat) < 0) {
      perror("fstat failed");
      return 1;
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
  }

  close(socket_fd);

  return 0;
}
