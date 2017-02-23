
#include "../includes/lib.h"

void on_error(int logPri, char *err) {
  syslog(logPri, "%s", err);
  exit(EXIT_FAILURE);
}
int ini_server(int port) {
  struct sockaddr_in server;
  int sockserv, opt_val = 1;

  syslog(LOG_INFO, "Creating socketServer");

  sockserv = socket(AF_INET, SOCK_STREAM, 0);
  if (sockserv < 0)
    on_error(LOG_ERR, "Could not create socket\n");

  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr.s_addr = htonl(INADDR_ANY);

  if (setsockopt(sockserv, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof opt_val) <
      0)
    on_error(LOG_ERR, "Error setsockopt\n");
  if (bind(sockserv, (struct sockaddr *)&server, sizeof(server)) < 0)
    on_error(LOG_ERR, "Could not bind socket\n");

  /*Procedemos a escuchar llamadas de clientes*/
  if (listen(sockserv, MAX_CLIENTS) < 0) {
    close(sockserv);
    on_error(LOG_ERR, "Error listening clients");
  }

  syslog(LOG_INFO, "Server is listening on %d", port);

  return sockserv;
}
int daemonizar(char *service) {

  int pid, i, fd;

  pid = fork();
  if (pid < 0)
    exit(EXIT_FAILURE);
  if (pid > 0)
    exit(EXIT_SUCCESS);

  // resetting File Creation Mask
  umask(0);
  setlogmask(LOG_UPTO(LOG_INFO));
  openlog(service, LOG_PID, LOG_LOCAL3);
  syslog(LOG_INFO, "Initiating_new_server.");

  if (setsid() < 0)
    on_error(LOG_ERR, "Error_creating_a_new_SID_for_the_child_process.");

  // Directorio raiz
  if ((chdir("/")) < 0)
    on_error(LOG_ERR, "Error_changing_the_current_working_directory_=_\"/\"");

  syslog(LOG_INFO, "Closing_standard_file_descriptors");

  // close all descriptors
  for (i = getdtablesize(); i >= 0; --i)
    close(i);

  fd = open("dev/null", O_RDWR, 0);

  if (fd != 1) {

    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);

    if (fd > 2)
      close(fd);
  }

  syslog(LOG_INFO, "Daemon creado");
  closelog();

  return EXIT_SUCCESS;
}
int accept_conex(int sock) {

  int length, socketClient;
  struct sockaddr_in client;

  length = sizeof(struct sockaddr_in);

  // Accept connection from an incoming client
  if ((socketClient =
           accept(sock, (struct sockaddr *)&client, (socklen_t *)&length)) < 0)
    on_error(LOG_ERR, "Error accept conexion cliente");

  return socketClient;
}
void *deal_cliente(void *sock) {
  int read, err, socket;
  char buf[BUFFER_SIZE];
  socket = *((int *)(&sock));
  while (1) {

    recibir(sock);

  }
}


/*Función que recibe un comando del cliente y realiza la acción correspondiente
Recibe el socket al que escuchar como argumento*/
int recibir(int sock) {

  char command[BUFFER_SIZE];
  char *pipe, *pipeCommand;
  int n_command = 0;

  /*Recibimos el comando*/
  if (recv(sock, command, BUFFER_SIZE, 0) == -1){

    close(sock);
    on_error(LOG_ERR, "Error al recibir comando.");
  }

  syslog(LOG_INFO, "Comando recibido: %s", command);

  /*Se llama a IRC_UnPipelineCommands*/

  syslog(LOG_INFO, "Probando UnPipeline...");
  pipe = IRC_UnPipelineCommands(command, &pipeCommand);
  syslog(LOG_INFO, "pipe = %s", pipe);
  syslog(LOG_INFO, "pipeCommand = %s", pipeCommand);

  doCommand(pipeCommand, sock);

  /*if ((pipe = IRC_UnPipelineCommands(command, &pipeCommand)) != NULL) {

    syslog(LOG_INFO, "Comando nº%d", n_command++);
    doCommand(pipeCommand, sock);

    while ((pipe = IRC_UnPipelineCommands(pipe, &pipeCommand)) != NULL) {

      syslog(LOG_INFO, "Comando nº%d", n_command++);
      doCommand(pipeCommand, sock);
    }

    if (pipe == NULL) {

      bzero(command, 512);
      syslog(LOG_INFO, "Comando nº%d", n_command++);
      doCommand(pipeCommand, sock);
    }

  } else {

    if (pipeCommand == NULL) {

      on_error(LOG_ERR, "Se han buscado comandos en cadena vacia.");
    } else {

      on_error(LOG_ERR, "Comando incompleto.");
    }

    doCommand(pipeCommand, sock);
  }*/

  free(pipeCommand);
  free(pipe);

  return 1;
}
