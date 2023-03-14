#include "../includes/server.h"

/**
 * @brief Demoniza el servidor.
 *
 * @param server la firma del servidor.
 * @return int 0 si todo ha ido bien, -1 en caso contrario
 */
int demonizar(char *server)
{
    char log[512];

    if (server == NULL)
        return -1;

    pid_t pid = fork();
    if (pid == -1)
        exit(EXIT_FAILURE);
    else if (pid > 0)
        exit(EXIT_SUCCESS);
    pid_t sid = setsid();
    if (sid == -1)
        exit(EXIT_FAILURE);

    umask(0);
    if (chdir("./") == -1)
        exit(EXIT_FAILURE);

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    openlog(server, LOG_PID, LOG_DAEMON);
    sprintf(log, "Demonization success [PID=%d]", sid);
    syslog(LOG_INFO, "%s\n", log);
    report_log(log);

    return 0;
}

/**
 * @brief Abre el fichero log/server.log y escribe en el el parametro.
 *
 * @param str la cadena de texto que se va a escribir en  server.log
 */
void report_log(char *str)
{
    char fecha[128];
    char *output = (char *)calloc(1024, sizeof(char));

    time_t tiempo = time(0);
    struct tm *t = gmtime(&tiempo);
    strftime(fecha, 128, "%d/%m/%Y-%H:%M:%S %Z", t);
    FILE *descriptor = fopen("log/server.log", "a");
    sprintf(output, "[%s]:\t%s\n", fecha, str);
    fwrite(output, sizeof(output[0]), strlen(output), descriptor);
    fclose(descriptor);
    free(output);
}

/**
 * @brief Envia por el socket par una respuesta del tipo 404.
 *
 * @param pair_socket el socket par generado por el accept.
 * @param request la petición a la que se responde.
 */
void enviar_error_404(int *pair_socket, t_request *request)
{
    char fecha[128];
    char buffer_response[1024];
    time_t tiempo = time(0);
    struct tm *t = gmtime(&tiempo);
    strftime(fecha, 128, "%d/%m/%Y-%H:%M:%S %Z", t);
    sprintf(buffer_response, "HTTP/1.%d 404 Not Found\r\nDate: %s\r\nServer: %s\r\nContent-Length: 80\r\n"
                             "Content-Type: text/html\r\n\r\n<html><b>404 Not Found</b></html>",
            request->version, fecha, "Server");
    send(*pair_socket, buffer_response, strlen(buffer_response), 0);
    memset(buffer_response, 0, strlen(buffer_response));
    free(request);
    exit(EXIT_SUCCESS);
}

/**
 * @brief Envia por el socket par una respuesta del tipo 400.
 *
 * @param pair_socket el socket par generado por el accept.
 * @param request la petición a la que se responde.
 */
void enviar_error_400(int *pair_socket, t_request *request)
{
    char fecha[128];
    char buffer_response[1024];
    time_t tiempo = time(0);
    struct tm *t = gmtime(&tiempo);
    strftime(fecha, 128, "%d/%m/%Y-%H:%M:%S %Z", t);
    sprintf(buffer_response, "HTTP/1.%d 400 Bad Request\r\nDate: %s\r\nServer: %s\r\nContent-Length: 80\r\n"
                             "Content-Type: text/html\r\n\r\n<html><b>400 Bad Request</b></html>",
            request->version, fecha, "Server");
    send(*pair_socket, buffer_response, strlen(buffer_response), 0);
    memset(buffer_response, 0, strlen(buffer_response));
    free(request);
    exit(EXIT_SUCCESS);
}

/**
 * @brief Funcion para comprobar el output de alguna funcion
 * o se el resultado de una cadena de texto (ES BLOQUEANTE).
 *
 * @param str La cadena de texto.
 * @param size El tamaño de la cadena.
 */
void testing_output(void *str, size_t size)
{
    FILE *descriptor = fopen("output.txt", "a");
    fwrite(str, sizeof(str[0]), size, descriptor);
    fwrite("\n", sizeof(char), 1, descriptor);
    fclose(descriptor);
    while (1)
        ;
}


/**
 * @brief Obtiene la configuracion del servidor guardando la
 * información en variables globales que no cambiaran.
 * 
 */
void setup_configuration()
{
    FILE *conf;

    conf = fopen("server.conf", "r");
    if (conf == NULL)
        exit(EXIT_FAILURE);
    fscanf(conf, "server_root = %s\n", server_root);
    fscanf(conf, "max_clients = %s\n", max_clients);
    fscanf(conf, "listen_port = %s\n", listen_port);
    fscanf(conf, "server_signature = %s\n", server_signature);
    fclose(conf);
}

// /**
//  * @brief Libera la memoria de la configuración
//  * 
//  */
// void free_configuration()
// {
//     free(server_root);
//     free(max_clients);
//     free(listen_port);
//     free(server_signature);
// }

/**
 * @brief Arranca el servidor abriendo el socket y
 * manteniendolo a la escucha.
 * 
 * @param server La estructura que almacenará la información basica del servidor.
 * @return int El identificador del socket o -1 en caso de que algo haya fallado.
 */
int setup_server(struct sockaddr_in *server)
{
    int tam;
    int server_socket;

    setup_configuration();

    server->sin_family = AF_INET;
    server->sin_port = htons(atoi(listen_port));
    server->sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(&(server->sin_zero), '\0', 8);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    tam = sizeof(struct sockaddr);

    if (bind(server_socket, (struct sockaddr *)server, tam) == -1)
    {
        printf("error BIND");
        return -1;
    }
    listen(server_socket, atoi(max_clients));
    return server_socket;
}

/**
 * @brief Imprime en la salida estandar una petición. Solo para debuggear.
 * 
 * @param request La petición
 * @param pret El tamaño en bytes de la peticion
 */
void print_request(t_request *request, int pret)
{
    int i;

    printf("request is %d bytes long\n", pret);
    printf("method is %.*s\n", (int)request->method_len, request->method);
    printf("path is %.*s\n", (int)request->path_len, request->path);
    printf("HTTP version is 1.%d\n", request->version);
    printf("headers:\n");
    for (i = 0; i <= (int)request->num_headers; ++i)
    {
        printf("%.*s: %.*s\n", (int)request->headers[i].name_len, request->headers[i].name,
               (int)request->headers[i].value_len, request->headers[i].value);
    }
}

/**
 * @brief Obtiene el tipo de archivo dependiendo de su extensión.
 * 
 * @param path La ruta que contiene el nombre del archivo y su extensión
 * @param tipo Variable donde se almacenará el tipo de archivo.
 * @return int 0 en caso de que todo ha salido bien, -1 en caso contrario.
 */
int get_file_type(char *path, char *tipo)
{
    const char *extension = strrchr(path, '.');
    if (extension == NULL)
        return -1;

    if (strcmp(extension, ".txt") == 0)
        strcpy(tipo, "text/plain");
    else if (strcmp(extension, ".html") == 0 || strcmp(extension, ".htm") == 0)
        strcpy(tipo, "text/html");
    else if (strcmp(extension, ".gif") == 0)
        strcpy(tipo, "image/gif");
    else if (strcmp(extension, ".jpeg") == 0 || strcmp(extension, ".jpg") == 0)
        strcpy(tipo, "image/jpeg");
    else if (strcmp(extension, ".mpeg") == 0 || strcmp(extension, ".mpg") == 0)
        strcpy(tipo, "video/mpeg");
    else if (strcmp(extension, ".doc") == 0 || strcmp(extension, ".docx") == 0)
        strcpy(tipo, "application/msword");
    else if (strcmp(extension, ".pdf") == 0)
        strcpy(tipo, "application/pdf");
    else
        return -1;

    return 0;
}
/**
 * @brief Acepta la conexión y obtiene el socket par.
 * 
 * @param sockval el socket principal creado por el servidor.
 * @return int el identificador del socket.
 */
int accept_connection(int sockval)
{

    int socket_client;
    struct sockaddr_in client;

    socklen_t length = sizeof(client);
    socket_client = accept(sockval, (struct sockaddr *)&client, &length);
    if (socket_client < 0)
        exit(1);
    return socket_client;
}