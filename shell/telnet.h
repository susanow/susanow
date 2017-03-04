
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <arpa/telnet.h>


/* Send WILL TELOPT_ECHO to remote server. */
static void
vty_will_echo (int fd)
{
  unsigned char cmd[] = { IAC, WILL, TELOPT_ECHO, '\0' };
  write(fd, cmd, sizeof(cmd));
}

/* Make suppress Go-Ahead telnet option. */
static void
vty_will_suppress_go_ahead (int fd)
{
  unsigned char cmd[] = { IAC, WILL, TELOPT_SGA, '\0' };
  write(fd, cmd, sizeof(cmd));
}

/* Make don't use linemode over telnet. */
static void
vty_dont_linemode (int fd)
{
  unsigned char cmd[] = { IAC, DONT, TELOPT_LINEMODE, '\0' };
  write(fd, cmd, sizeof(cmd));
}

/* Use window size. */
static void
vty_do_window_size (int fd)
{
  unsigned char cmd[] = { IAC, DO, TELOPT_NAWS, '\0' };
  write(fd, cmd, sizeof(cmd));
}
