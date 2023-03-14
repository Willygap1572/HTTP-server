#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "picohttpparser.h"
#include <fcntl.h>
#include <pthread.h>
#include <time.h>
#include <sys/stat.h>
#include <syslog.h>

/**
 * @brief Las variables globales que tienen la configuración del servidor.
 * 
 */
extern char server_root[512];
extern char max_clients[512];
extern char listen_port[512];
extern char server_signature[512];
extern int pair_socket;

/**
 * @brief Estructura que almacena la informacion de una peticion.
 * 
 */
typedef struct s_request
{
  char *method;
  size_t method_len;
  char *path;
  size_t path_len;
  struct phr_header headers[100];
  char post_arg[512];
  size_t num_headers;
  int version;
  int size;
} t_request;


void report_log(char *str);
void enviar_error_404(int *pair_socket, t_request *request);
void enviar_error_400(int *pair_socket, t_request *request);
void testing_output(void *str, size_t size);
void response(t_request *request, int *pair_socket);
t_request *process_request(int *pair_socket);
int setup_server(struct sockaddr_in *server);
void print_request(t_request *request, int pret);
int get_file_type(char *path, char *tipo);
int accept_connection(int sockval);
int demonizar(char *server);
void free_configuration();