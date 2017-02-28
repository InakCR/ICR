#include "../includes/commands.h"
char userNick[100];
void nick(char *string, int sock) {
  char *prefix, *nick, *msg, *ip;
  long parser;
  struct sockaddr_in addr;
  struct hostent *he;
  int sclient;
  sclient = sizeof(addr);
  parser = IRCParse_Nick(string, &prefix, &nick, &msg);

  getpeername(sock, (struct sockaddr *)&addr, &sclient);
  ip = inet_ntoa(addr.sin_addr);

  // cout << " The IP address from the accept is " << ip << std::endl;

  he = gethostbyaddr((char *)&addr.sin_addr, sizeof(addr.sin_addr), AF_INET);
  syslog(LOG_INFO, "parser = %ld", parser);
  syslog(LOG_INFO, "prefix = %s", prefix);
  syslog(LOG_INFO, "nick = %s", nick);
  syslog(LOG_INFO, "msg = %s", msg);
  syslog(LOG_INFO, "ip = %s", ip);
  syslog(LOG_INFO, "host = %s", he->h_name);

  strcpy(userNick, nick);

  if (parser == IRCERR_NOSTRING) {
    free(prefix);
    free(nick);
    free(msg);
    on_error(LOG_ERR,
             "***Error {nick()} No existe una cadena para usar como Nick.");
  } else if (parser == IRC_OK) {

    // Comprobar que el nick no existe (moviendose por todos los clientes)

    if (UTestNick(nick)) {
      syslog(LOG_INFO, " Ya existe el nick: %s", nick);
    } else {
      syslog(LOG_INFO, " No existe el nick: %s", nick);
    }
  }

  syslog(LOG_INFO, "Nick pasado correctamente, falta añadirlo al cliente");
}

void user(char *string, int sock) {
  char *prefix, *msg, **user, **modehost, **serverother, **realname;
  long parser;

  //  parser =
  //      IRCParse_User(string, &prefix, &user, &modehost, &serverother,
  //      &realname);

  // IRCTADUser_New(, userNick, *realname, NULL, host, char *IP, sock);
}
void doCommand(char *string, int sock) {

  if (string == NULL)
    return;

  switch (IRC_CommandQuery(string)) {

  case NICK:
    syslog(LOG_INFO, "NICK");
    nick(string, sock);
    break;
  case USER:
    syslog(LOG_INFO, "USER");
    user(string, sock);
    break;
  case PING:
    syslog(LOG_INFO, "PING");
    break;
  case TOPIC:
    syslog(LOG_INFO, "TOPIC");
    break;
  case LIST:
    syslog(LOG_INFO, "LIST");
    break;
  case JOIN:
    syslog(LOG_INFO, "JOIN");
    break;
  case WHOIS:
    syslog(LOG_INFO, "WHO IS");
    break;
  }
}
