#ifndef _comm_h_
#define _comm_h_

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#if defined(__linux)
#include <asm/byteorder.h>
#endif
#include <string.h>
#include <netdb.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/file.h>
#include <arpa/inet.h>
#include <sys/time.h>

#ifdef __hpux
#include <sys/file.h>
#include <sys/socketvar.h>
#include <sys/param.h>
#endif

#ifdef __liNux
/*
 * Internet address
 *      This definition contains obsolete fields for compatibility
 *      with SunOS 3.x and 4.2bsd.  The presence of subnets renders
 *      divisions into fixed fields misleading at best.  New code
 *      should use only the s_addr field.
 */
struct in_addr {
        union {
                struct { u_char s_b1, s_b2, s_b3, s_b4; } S_un_b;
                struct { u_short s_w1, s_w2; } S_un_w;
                u_long S_addr;
        } S_un;
#define s_addr  S_un.S_addr             /* should be used for all code */
#define s_host  S_un.S_un_b.s_b2        /* OBSOLETE: host on imp */
#define s_net   S_un.S_un_b.s_b1        /* OBSOLETE: network */
#define s_imp   S_un.S_un_w.s_w2        /* OBSOLETE: imp */
#define s_impno S_un.S_un_b.s_b4        /* OBSOLETE: imp # */
#define s_lh    S_un.S_un_b.s_b3        /* OBSOLETE: logical host */
};

/*
 * Socket address, internet style.
 */
struct sockaddr_in {
        short   sin_family;
        u_short sin_port;
        struct  in_addr sin_addr;
        char    sin_zero[8];
};

#endif

/* Prototypes */

unsigned short portno(int s);
int make_socket_async(int s);
int establish(unsigned short portnum);
int establish_async(unsigned short portnum);
int establish_unix(char *socket_file);
int get_connection(int s, struct sockaddr_in *from, int *from_len);
int call_socket(char *hostname, unsigned short portnum);
int call_socket_unix(char *filename);
int read_data(int s,     /* connected socket */
              char *buf, /* pointer to the buffer */
              int n      /* number of characters (bytes) we want */
             );
int write_data(int s,     /* connected socket */
              char *buf, /* pointer to the buffer */
              int n      /* number of characters (bytes) we want */
             );
int listen_async(unsigned short s);

#endif
