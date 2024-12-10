#include <unistd.h>
#include "comm.h"

/* code to establish a socket; originally from bzs@bu-cs.bu.edu
 */
#define MAXHOSTNAME 100
extern int gethostname(char *name, size_t len);


/***********************************************************************/
/*                                                                     */
/*                          portno                                     */
/*                                                                     */
/***********************************************************************/
unsigned short portno(int s)
{
  struct sockaddr_in sa;
  int n;
  n = sizeof(sa);
  if (getsockname(s, (struct sockaddr *)&sa, &n) < 0) perror("getsockname");
  return(ntohs(sa.sin_port));
}

/***********************************************************************/
/*                                                                     */
/*                          establish                                  */
/*                                                                     */
/***********************************************************************/
int establish(unsigned short portnum)
{ 
  char   myname[MAXHOSTNAME+1];
  int    s;
  struct sockaddr_in sa;
  struct hostent *hp;
  int yes_no;

  memset(&sa, 0, sizeof(struct sockaddr_in)); /* clear our address */
  gethostname(myname, MAXHOSTNAME);           /* who are we? */
  hp= gethostbyname(myname);                  /* get our address info */
  if (hp == NULL) { printf("hp==NULL\n");     /* we don't exist !? */
    return(-1);}
  sa.sin_family= hp->h_addrtype;              /* this is our host address */
  sa.sin_port= htons(portnum);                /* this is our port number */
  if ((s= socket(AF_INET, SOCK_STREAM, 0)) < 0) /* create socket */
   {  printf("socket failed\n"); return(-1);
	}
  yes_no = 1;
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes_no, sizeof(yes_no));
  if (bind(s,(struct sockaddr *)&sa,sizeof(struct sockaddr_in)) < 0) {
	perror("establish, bind failed");
    close(s);
    return(-1);                               /* bind address to socket */
  }
  listen(s, 5);                               /* max # of queued connects */
  return(s);
}

/***********************************************************************/
/*                                                                     */
/*                       make_socket_async                             */
/*                                                                     */
/***********************************************************************/
int make_socket_async(int s)
{

  if (fcntl(s, F_SETFL, FNDELAY) < 0) {
	perror("fcntl(s,F_SETFL, FNDELAY)");
	return -1;
	}

#ifdef __hpux
  {
	int pid;
	pid = getpid();
	if (ioctl(s, SIOCSPGRP, &pid) < 0) {
	  perror("ioctl SIOCSPGRP");
	  return(-1);
	}
  }
#else
  if (fcntl(s, F_SETOWN, getpid()) < 0) {
	perror("fcntl F_SETOWN");
	return(-1);
  }
#endif

  /* Allow receipt of ansync I/O signals. */
#ifdef __hpux
  {
	int yes_no;
	yes_no = 1;
	if (ioctl(s, FIOASYNC, &yes_no) < 0) {
	  perror("ioctl FIOASYNC, &yes_no");
	  return(-1);
	}
  }
#else
  if (fcntl(s, F_SETFL, FASYNC) < 0) {
	perror("fcntl F_SETFL, FASYNC");
	return(-1);
  }
#endif
  return s;
}

/***********************************************************************/
/*                                                                     */
/*                       establish_async                               */
/*                                                                     */
/***********************************************************************/
int establish_async(unsigned short portnum)
{ char   myname[MAXHOSTNAME+1];
  int    s;
  struct sockaddr_in sa;
  struct hostent *hp;
  int yes_no;

  memset(&sa, 0, sizeof(struct sockaddr_in)); /* clear our address */
  gethostname(myname, MAXHOSTNAME);           /* who are we? */
  hp= gethostbyname(myname);                  /* get our address info */
  if (hp == NULL) { printf("hp==NULL\n");     /* we don't exist !? */
    return(-1);}
  sa.sin_family= hp->h_addrtype;              /* this is our host address */
  sa.sin_port= htons(portnum);                /* this is our port number */

  if ((s= socket(AF_INET, SOCK_STREAM, 0)) < 0) /* create socket */
   {  printf("socket failed\n"); return(-1);
	}
  yes_no = 1;
  if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes_no, sizeof(yes_no))
	  < 0) {
	perror("establish_async, setsockopt");
	return(-1);
  }


  if (bind(s,(struct sockaddr *)&sa,sizeof(struct sockaddr_in)) < 0) {
	perror("establish_async, bind failed");
    close(s);
    return(-1);                               /* bind address to socket */
  }
  listen(s, 5);                               /* max # of queued connects */

  s = make_socket_async(s);
  return(s);
}

/***********************************************************************/
/*                                                                     */
/*                        get_connection                               */
/*                                                                     */
/***********************************************************************/
/* wait for a connection to occur on a socket created with establish()
 */
int get_connection(int s, struct sockaddr_in *from, int *from_len)
{ int t;                  /* socket of connection */

  *from_len = sizeof(struct sockaddr_in);
  if ((t = accept(s, (struct sockaddr *)from, from_len)) < 0)   /* accept connection if there is one */
    return(-1);
  return(t);
}


/***********************************************************************/
/*                                                                     */
/*                         call_socket                                 */
/*                                                                     */
/***********************************************************************/
int call_socket(char *hostname, unsigned short portnum)
{ struct sockaddr_in sa;
  struct hostent     *hp;
  int s;

  if ((hp= gethostbyname(hostname)) == NULL) { /* do we know the host's */
    return(-1);                                /* no */
  }

  memset(&sa,0,sizeof(sa));
  memcpy((char *)&sa.sin_addr,hp->h_addr,hp->h_length); /* set address */
  sa.sin_family= hp->h_addrtype;
  sa.sin_port= htons((u_short)portnum);

  if ((s= socket(hp->h_addrtype,SOCK_STREAM,0)) < 0)   /* get socket */
    return(-1);
  if (connect(s,(struct sockaddr *)&sa,sizeof(sa)) < 0) { /* connect */
    close(s);
    return(-1);
  }
  return(s);
}
/***********************************************************************/
/*                                                                     */
/*                        read_data                                    */
/*                                                                     */
/***********************************************************************/
int read_data(int s,     /* connected socket */
              char *buf, /* pointer to the buffer */
              int n      /* number of characters (bytes) we want */
             )
{ int bcount; /* counts bytes read */
  int br;     /* bytes read this pass */

  bcount= 0;
  br= 0;
  while (bcount < n) {             /* loop until full buffer */
    if ((br= read(s,buf,n-bcount)) > 0) {
      bcount += br;                /* increment byte counter */
      buf += br;                   /* move buffer ptr for next read */
    } else if (br <= 0) {              /* signal an error to the caller */
	  return(bcount);
	}
  }
  /* I have removed NULL terminating the data !!!  BEWARE  !!! */
  return(bcount);
}

/***********************************************************************/
/*                                                                     */
/*                         write_data                                  */
/*                                                                     */
/***********************************************************************/
int write_data(int s,     /* connected socket */
              char *buf, /* pointer to the buffer */
              int n      /* number of characters (bytes) we want */
             )
{ int bcount; /* counts bytes read */
  int br;     /* bytes read this pass */

  bcount= 0;
  br= 0;
  while (bcount < n) {             /* loop until full buffer */
    if ((br= write(s,buf,n-bcount)) > 0) {
      bcount += br;                /* increment byte counter */
      buf += br;                   /* move buffer ptr for next read */
    } else if (br <= 0) {              /* signal an error to the caller */
	  return(bcount);
	}
  }
  return(bcount);
}

