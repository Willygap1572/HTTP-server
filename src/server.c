#include "../includes/server.h"

char server_root[512];
char max_clients[512];
char listen_port[512];
char server_signature[512];
int pair_socket;
/**
 * @brief gestiona si la petición es de tipo OPTIONS
 *
 * @param pair_socket el socket par que se genera al hacer el accept.
 * @param request la peticion ya parseada.
 */
void options(int *pair_socket, t_request *request)
{
    char fecha[128];
    char buffer_response[1024] = {0};
    time_t tiempo = time(0);
    struct tm *t = gmtime(&tiempo);
    strftime(fecha, 128, "%d/%m/%Y-%H:%M:%S %Z", t);

    sprintf(buffer_response, "HTTP/1.%d 200 OK\r\nAllow: GET, POST\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: GET, POST, OPTIONS\r\nAccess-Control-Allow-Headers: Content-Type, Atuthorization\r\nAccess-Control-Max-Age: 2\r\nDate: %s\r\nServer: %s\r\nContent-Length: 0\r\n\r\n", request->version, fecha, server_signature);

    send(*pair_socket, buffer_response, strlen(buffer_response), 0);
}

/**
 * @brief Genera la respuesta dependiendo del tipo de peticion.
 *
 * @param request la peticion ya parseada.
 * @param pair_socket el socket par que se genera al hacer el accept.
 */
void response(t_request *request, int *pair_socket)
{
    char response_date[128] = {0};
    char last_modified[128] = {0};
    char request_file_type[128] = {0};
    int ret = 0;
    int file = 0;
    char script_res[128] = {0};
    char *ptr = NULL;
    char args[128] = {0};
    char *argumentos = NULL;
    char script[1024] = {0};
    FILE *script_o = NULL;
    char fullpath[128] = {0};
    char bufenv[8192] = {0};
    char log[128] = {0};

    // OPTIONS
    if (strstr(request->method, "OPTION"))
        options(pair_socket, request);
    // GET/POST
    if (strstr(request->method, "POST") && strstr(request->path, "?"))
    {
        ptr = strtok(request->path, "?");
        strcpy(fullpath, ptr);
        argumentos = strtok(NULL, "?");
        strcpy(args, argumentos);
        sprintf(fullpath, "htmlfiles%s", request->path);

        sprintf(script, "echo %s | %s %s %s", request->post_arg, strstr(fullpath, ".py") ? "python3" : "php", fullpath, args);
        script_o = popen(script, "r");
        if (script_o == NULL)
        {
            report_log("[ERROR]\tUnable to open the script [500]");
            close(*pair_socket);
            free(request);
            exit(EXIT_FAILURE);
        }

        fread(script_res, 1, 1024, script_o);

        /*Date*/
        time_t tiempo = time(0);
        struct tm *t = gmtime(&tiempo);
        strftime(response_date, 128, "%d/%m/%Y-%H:%M:%S %Z", t);

        /*Last-modified*/
        struct stat s;
        stat(fullpath, &s);
        strftime(last_modified, 128, "%d/%m/%Y-%H:%M:%S %Z", gmtime(&s.st_mtime));

        sprintf(bufenv, "HTTP/1.%d 200 OK\r\nDate: %s\r\nServer: %s\r\nContent-Length: %ld\r\nLast-Modified: %s\r\nContent-Type: text/html\r\n\r\n%s\r\n",
                request->version, response_date, server_signature, strlen(script_res), last_modified, script_res);
        send(*pair_socket, bufenv, strlen(bufenv), 0);
        pclose(script_o);
    }
    // POST
    if (strstr(request->method, "POST") && !strstr(request->path, "?"))
    {
        sprintf(fullpath, "htmlfiles%s", request->path);
        sprintf(script, "echo %s | %s %s", request->post_arg, strstr(fullpath, ".py") ? "python3" : "php", fullpath);
        script_o = popen(script, "r");
        if (script_o == NULL)
        {
            report_log("[ERROR]\tUnable to open the script [500]");
            close(*pair_socket);
            free(request);
            exit(EXIT_FAILURE);
        }

        fread(script_res, 1, 1024, script_o);

        /*Date*/
        time_t tiempo = time(0);
        struct tm *t = gmtime(&tiempo);
        strftime(response_date, 128, "%d/%m/%Y-%H:%M:%S %Z", t);

        /*Last-modified*/
        struct stat s;
        stat(fullpath, &s);
        strftime(last_modified, 128, "%d/%m/%Y-%H:%M:%S %Z", gmtime(&s.st_mtime));

        sprintf(bufenv, "HTTP/1.%d 200 OK\r\nDate: %s\r\nServer: %s\r\nContent-Length: %ld\r\nLast-Modified: %s\r\nContent-Type: text/html\r\n\r\n%s\r\n",
                request->version, response_date, server_signature, strlen(script_res), last_modified, script_res);
        send(*pair_socket, bufenv, strlen(bufenv), 0);
        pclose(script_o);
    }
    // GET con argumentos
    if (strstr(request->method, "GET") && !strstr(request->method, "POST") && strstr(request->path, "?"))
    {
        ptr = strtok(request->path, "?");
        strcpy(fullpath, ptr);
        argumentos = strtok(NULL, "?");
        strcpy(args, argumentos);
        sprintf(fullpath, "htmlfiles%s", request->path);

        sprintf(script, "%s %s \"%s\"", strstr(fullpath, ".py") ? "python3" : "php", fullpath, args);

        script_o = popen(script, "r");
        if (script_o == NULL)
        {
            report_log("[ERROR]\tUnable to open the script [500]");
            close(*pair_socket);

            exit(EXIT_FAILURE);
        }

        fread(script_res, 1, 1024, script_o);

        /*Date*/
        time_t tiempo = time(0);
        struct tm *t = gmtime(&tiempo);
        strftime(response_date, 128, "%d/%m/%Y-%H:%M:%S %Z", t);

        /*Last-modified*/
        struct stat s;
        stat(fullpath, &s);
        strftime(last_modified, 128, "%d/%m/%Y-%H:%M:%S %Z", gmtime(&s.st_mtime));

        sprintf(bufenv, "HTTP/1.%d 200 OK\r\nDate: %s\r\nServer: %s\r\nContent-Length: %ld\r\nLast-Modified: %s\r\nContent-Type: text/html\r\n\r\n%s\r\n",
                request->version, response_date, server_signature, strlen(script_res), last_modified, script_res);
        send(*pair_socket, bufenv, strlen(bufenv), 0);
        pclose(script_o);
    }
    // GET sin argumentos
    else if (strstr(request->method, "GET") && !strstr(request->method, "POST") && !strstr(request->path, "?"))
    {
        strcpy(fullpath, server_root);
        strcat(fullpath, request->path);
        if (get_file_type(request->path, request_file_type) == -1)
        {
            sprintf(log, "File (%s) type (%s) not supported [400]", request->path, strchr(request->path, '.'));
            report_log(log);
            enviar_error_400(pair_socket, request);
            close(*pair_socket);
            free(request);
            exit(EXIT_SUCCESS);
        }
        file = open(fullpath, O_RDONLY);
        if (file == -1)
        {
            sprintf(log, "Unable to find the file (%s)[404]", request->path);
            report_log(log);
            enviar_error_404(pair_socket, request);
            close(*pair_socket);
            free(request);
            exit(EXIT_SUCCESS);
        }
        /*Date*/
        time_t tiempo = time(0);
        struct tm *t = gmtime(&tiempo);
        strftime(response_date, 128, "%d/%m/%Y-%H:%M:%S %Z", t);

        /*Last-modified*/
        struct stat s;
        stat(fullpath, &s);
        strftime(last_modified, 128, "%d/%m/%Y-%H:%M:%S %Z", gmtime(&s.st_mtime));

        /*Mandamos la cabecera de la respuesta*/
        sprintf(bufenv, "HTTP/1.%d 200 OK\r\nDate: %s\r\nServer: %s\r\nContent-Length: %ld\r\nLast-Modified: %s\r\nContent-Type: %s\r\n\r\n",
                request->version, response_date, server_signature, (long)s.st_size, last_modified, request_file_type);
        send(*pair_socket, bufenv, (int)strlen(bufenv), 0); //---Envia datos al cliente si se llego al final del archivo//
        while ((ret = read(file, bufenv, 8192)) > 0)
            send(*pair_socket, bufenv, ret, 0);
        close(file);
    }
    free(request);
    exit(EXIT_SUCCESS);
}

/**
 * @brief Parsea la peticion y inicia la respuesta.
 *
 * @param pair_socket el socket par que se generó al hacer el accept.
 * @return t_request* la peticion parseada.
 */

t_request *process_request(int *pair_socket)
{
    char buf[4096] = {0};
    int pret = 0;
    size_t buflen = 0;
    size_t prevbuflen = 0;
    ssize_t rret = 0;
    struct phr_header headers[128];
    size_t num_headers = 0;
    t_request *request = NULL;

    request = (t_request *)calloc(1, sizeof(t_request));
    if (request == NULL)
    {
        report_log("[ERROR]\tUnable alloc memory for the request [500]");
        return NULL;
    }
    while (1)
    {
        while ((rret = read(*pair_socket, buf + buflen, sizeof(buf) - buflen)) == -1)
            ;
        if (rret <= 0)
        {
            free(request);
            return NULL;
        }
        prevbuflen = buflen;
        buflen += rret;
        /* parse the request */
        num_headers = sizeof(headers) / sizeof(headers[0]);
        pret = phr_parse_request(buf, buflen, (const char **)&request->method, &request->method_len, (const char **)&request->path, &request->path_len,
                                 &request->version, headers, &num_headers, prevbuflen);
        if (pret > 0)
            break; /* successfully parsed the request */
        else if (pret == -1)
        {
            free(request);
            return NULL;
        }
        /* request is incomplete, continue the loop */
        assert(pret == -2);
        if (buflen == sizeof(buf))
        {
            free(request);
            return NULL;
        }
    }
    request->size = pret;
    /*Guardamos la ruta*/
    sprintf(request->path, "%.*s", (int)request->path_len, request->path);
    sprintf(request->method, "%.*s", (int)request->method_len, request->method);
    if (strstr(request->method, "POST"))
        sprintf(request->post_arg, "%s", buf + pret);
    response(request, pair_socket);
    return request;
}

/**
 * @brief Funcion principal del servidor,
 *  se encarga de arrancar el servidor, demonizarlo,
 *  aceptar las conexiones y parsear las respuestas
 *
 * @return devuelve 0 en caso de que todo haya salido
 * bien -1 en caso contrario
 */
int main()
{
    int server_socket = 0;
    struct sockaddr_in server = {0};
    t_request *request = NULL;
    mkdir("log",0777); //crea la carpeta log
    server_socket = setup_server(&server);

    // demonizar(server_signature); no funciona en ubuntu, en MACOS si

    while (1)
    {
        pair_socket = accept_connection(server_socket);
        if (fork() == 0)
        { // hijo
            request = process_request(&pair_socket);
            if (request == NULL)
            {
                free(request);
                report_log("[ERROR]\tProblem solving the request [500]");
                return -1;
            }
            close(pair_socket);
            free(request);
        }
        else
        {
            free(request);
            close(pair_socket);
        }
    }
    return 0;
}