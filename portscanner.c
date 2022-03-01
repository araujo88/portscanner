#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_PORT 65536

int port_number = 0;
pthread_mutex_t mutex;

void check_socket(int socket);
void *progress_bar();

int main(int argc, char *argv[])
{
  int web_socket;
  struct sockaddr_in s_in;
  pthread_t th;
  fd_set fdset;
  struct timeval tv;

  pthread_mutex_init(&mutex, 0);

  if (argc < 2)
  {
    printf("Please provide a target IP address\n");
    exit(EXIT_FAILURE);
  }
  if (argc < 3)
  {
    printf("Please provide a timeout\n");
    exit(EXIT_FAILURE);
  }

  printf("IP address: %s\n", argv[1]);

  s_in.sin_family = AF_INET;
  s_in.sin_addr.s_addr = inet_addr(argv[1]); // target IP

  pthread_create(&th, 0, &progress_bar, 0);

  for (port_number = 1; port_number < MAX_PORT; port_number++)
  {
    web_socket = socket(AF_INET, SOCK_STREAM, 0);
    check_socket(web_socket);

    fcntl(web_socket, F_SETFL, O_NONBLOCK);

    // address resolution

    s_in.sin_port = htons(port_number); // port

    connect(web_socket, (struct sockaddr *)&s_in, sizeof(s_in));

    FD_ZERO(&fdset);
    FD_SET(web_socket, &fdset);
    tv.tv_sec = atoi(argv[2]); /* 10 second timeout */
    tv.tv_usec = 0;

    if (select(web_socket + 1, NULL, &fdset, NULL, &tv) == 1)
    {
      int so_error;
      socklen_t len = sizeof so_error;

      getsockopt(web_socket, SOL_SOCKET, SO_ERROR, &so_error, &len);

      if (so_error == 0)
      {
        pthread_mutex_lock(&mutex);
        fputs("\033[A\033[2K", stdout);
        printf("%d\topen\n\n", port_number);
        pthread_mutex_unlock(&mutex);
      }
    }
    close(web_socket);
  }
  pthread_join(th, NULL);
  pthread_mutex_destroy(&mutex);
  return 0;
}

void check_socket(int socket)
{
  if (socket < 0)
  {
    perror("Socket criation error");
    printf("Error code: %d\n", errno);
    exit(EXIT_FAILURE);
  }
}

void *progress_bar()
{
  float progress;

  while (true)
  {
    pthread_mutex_lock(&mutex);
    progress = (float)((float)port_number / (float)MAX_PORT);
    printf("Progress %.2f %%\n", 100 * progress);
    fputs("\033[A\033[2K", stdout);
    // rewind(stdout);
    pthread_mutex_unlock(&mutex);
    if ((int)progress == 1)
    {
      break;
    }
  }
  pthread_exit(NULL);
}