/****************************************************************************
 * netuils/tftp/tftpc_packets.c
 *
 *   Copyright (C) 2008 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <spudmonkey@racsa.co.cr>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, TFTP_DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/****************************************************************************
 * Compilation Switches
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <debug.h>

#include <netinet/in.h>

#include <net/uip/uipopt.h>
#include <net/uip/uip.h>
#include <net/uip/tftp.h>

#include "tftpc_internal.h"

#if defined(CONFIG_NET) && defined(CONFIG_NET_UDP)

/****************************************************************************
 * Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Data
 ****************************************************************************/

#if defined(CONFIG_DEBUG) && defined(CONFIG_DEBUG_NET)
const char g_tftpcallfailed[]     = "%s failed: %d\n";
const char g_tftpcalltimedout[]   = "%s timed out\n";
const char g_tftpnomemory[]       = "%s memory allocation failure\n";
const char g_tftptoomanyretries[] = "Retry limit exceeded\n";
const char g_tftpaddress[]        = "%s invalid address\n";
const char g_tftpport[]           = "%s invalid port\n";
#endif

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: tftp_mode
 ****************************************************************************/

static inline const char *tftp_mode(boolean binary)
{
  return binary ? "octet" : "netascii";
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: tftp_sockinit
 *
 * Description:
 *   Common initialization logic:  Create the socket and initialize the
 *   server address structure.
 *
 ****************************************************************************/

int tftp_sockinit(struct sockaddr_in *server, in_addr_t addr)
{
  struct timeval timeo;
  int sd;
  int ret;

  /* Create the UDP socket */

  sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sd >= 0)
    {
      ndbg(g_tftpcallfailed, "socket", errno);
    }
  else
    {
      /* Set the recvfrom timeout */

      timeo.tv_sec  = CONFIG_NETUTILS_TFTP_TIMEOUT / 10;
      timeo.tv_usec = (CONFIG_NETUTILS_TFTP_TIMEOUT % 10) * 100000;
      ret = setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, &timeo, sizeof(struct timeval));
      if (ret < 0)
        {
          ndbg(g_tftpcallfailed, "setsockopt", errno);
        }

      /* Initialize the server address structure */

      memset(server, 0, sizeof(struct sockaddr_in));
      server->sin_family      = AF_INET;
      server->sin_addr.s_addr = addr;
      server->sin_port        = HTONS(CONFIG_NETUTILS_TFTP_PORT);
    }
  return sd;
}

/****************************************************************************
 * Name: tftp_mkreqpacket
 *
 * Description:
 *   RRQ or WRQ message format:
 *
 *     2 bytes: Opcode (network order == big-endian)
 *     N bytes: Filename
 *     1 byte:  0
 *     N bytes: mode
 *     1 byte:  0
 *
 ****************************************************************************/

int tftp_mkreqpacket(ubyte *buffer, int opcode, const char *path, boolean binary)
{
  buffer[0] = opcode >> 8;
  buffer[1] = opcode & 0xff;
  return sprintf((char*)&buffer[2], "%s%c%s", path, 0, tftp_mode(binary)) + 3;
}

/****************************************************************************
 * Name: tftp_mkackpacket
 *
 * Description:
 *   ACK message format:
 *
 *     2 bytes: Opcode (network order == big-endian)
 *     2 bytes: Block number (network order == big-endian)
 *
 ****************************************************************************/

int tftp_mkackpacket(ubyte *buffer, uint16 blockno)
{
  buffer[0] = TFTP_ACK >> 8;
  buffer[1] = TFTP_ACK & 0xff;
  buffer[2] = blockno >> 8;
  buffer[3] = blockno & 0xff;
  return 4;
}

/****************************************************************************
 * Name: tftp_mkerrpacket
 *
 * Description:
 *   ERROR message format:
 *
 *     2 bytes: Opcode (network order == big-endian)
 *     2 bytes: Error number (network order == big-endian)
 *     N bytes: Error string
 *     1 byte:  0
 *
 ****************************************************************************/

int tftp_mkerrpacket(ubyte *buffer, uint16 errorcode, const char *errormsg)
{
  buffer[0] = TFTP_ERR >> 8;
  buffer[1] = TFTP_ERR & 0xff;
  buffer[2] = errorcode >> 8;
  buffer[3] = errorcode & 0xff;
  strcpy((char*)&buffer[4], errormsg);
  return strlen(errormsg) + 5;
}

/****************************************************************************
 * Name: tftp_parseerrpacket
 *
 * Description:
 *   ERROR message format:
 *
 *     2 bytes: Opcode (network order == big-endian)
 *     2 bytes: Error number (network order == big-endian)
 *     N bytes: Error string
 *     1 byte:  0
 *
 ****************************************************************************/

#if defined(CONFIG_DEBUG) && defined(CONFIG_DEBUG_NET)
int tftp_parseerrpacket(const ubyte *buffer)
{
  uint16 opcode       = (uint16)buffer[0] << 8 | (uint16)buffer[1];
  uint16 errcode      = (uint16)buffer[2] << 8 | (uint16)buffer[3];
  const char *errmsg  = (const char *)&buffer[4];

  if (opcode == TFTP_ERR)
    {
      ndbg("ERR message: %s (%d)\n", errmsg, errcode);
      return OK;
    }
  return ERROR;
}
#endif

/****************************************************************************
 * Name: tftp_recvfrom
 *
 * Description:
 *   recvfrom helper
 *
 ****************************************************************************/

ssize_t tftp_recvfrom(int sd, void *buf, size_t len, struct sockaddr_in *from)
{
  int     addrlen;
  ssize_t nbytes;

  /* Loop handles the case where the recvfrom is interrupted by a signal and
   * we should unconditionally try again.
   */

  for (;;)
    {
      /* Receive the packet */

      addrlen = sizeof(struct sockaddr_in);
      nbytes = recvfrom(sd, buf, len, 0, (struct sockaddr*)from, (socklen_t*)&addrlen);

      /* Check for errors */

      if (nbytes < 0)
        {
          /* Check for a timeout */

          if (errno == EAGAIN)
            {
              ndbg(g_tftpcalltimedout, "recvfrom");
              return ERROR;
            }

          /* If EINTR, then loop and try again.  Other errors are fatal */

          else if (errno != EINTR)
            {
              ndbg(g_tftpcallfailed, "recvfrom", errno);
              return ERROR;
            }
        }

      /* No errors?  Return the number of bytes received */

      else
        {
          return nbytes;
        }
    }
}

/****************************************************************************
 * Name: tftp_sendto
 *
 * Description:
 *   sendto helper
 *
 ****************************************************************************/

ssize_t tftp_sendto(int sd, const void *buf, size_t len, struct sockaddr_in *to)
{
  ssize_t nbytes;

  /* Loop handles the case where the sendto is interrupted by a signal and
   * we should unconditionally try again.
   */

  for (;;)
    {
      /* Send the packet */

      nbytes = sendto(sd, buf, len, 0, (struct sockaddr*)to, sizeof(struct sockaddr_in));

      /* Check for errors */

      if (nbytes < 0)
        {
          /* If EINTR, then loop and try again.  Other errors are fatal */

          if (errno != EINTR)
            {
              ndbg(g_tftpcallfailed, "sendto", errno);
              return ERROR;
            }
        }

      /* No errors?  Return the number of bytes received */

      else
        {
          return nbytes;
        }
    }
}

#endif /* CONFIG_NET && CONFIG_NET_UDP */
