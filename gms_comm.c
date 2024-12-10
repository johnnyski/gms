#include	<stdio.h>
#include	<sys/types.h>
#include	<sys/ipc.h>
#include    <unistd.h>
#include    <fcntl.h>
#include    <string.h>
#include    <signal.h>
#include    <ctype.h>
#include    <errno.h>

#include    "gms_comm.h"
#include    "comm.h"



extern char *comm_stat_str;

extern int data_socket;
extern unsigned short data_portnum;
/*
extern int ps_info_pipe[];
*/
extern char local_host[];
int send_data_to_parent(char *fromhost, char *buf)
{
	int	n;
	int nbytes;
	char msg[MAX_DATA_CHILD_PAR_LEN];
	int t;

	n = 0;
/*
printf("enter send data to parent\n");
printf("... fromhost    = %s\n", fromhost);
printf("... strlen(buf) = %d\n", strlen(buf));
*/
	memset(msg, '\0',  MAX_DATA_CHILD_PAR_LEN);
	/* only get max host name for host */
	if (strlen(fromhost) >= MAX_HOSTNAME_LEN)
	  *(fromhost + MAX_HOSTNAME_LEN-1) = '\0';

	sprintf(msg, "%s %s", fromhost, buf);
	/*
	 * write msg 
	 */
	nbytes = strlen(msg)+1;

	/*
fprintf(stderr, "enter send data to parent: msg (%d bytes) = <<%s>>\n", nbytes, msg);
*/
/*
printf("call_socket(%s, %d);\n", local_host, data_portnum);
*/
	if ((t = call_socket(local_host, data_portnum)) < 0) {
	  perror(local_host);
	  fprintf(stderr, "Child failed to connect with parent\n");
	  exit_child(-2);
	}
	n = write_data(t, msg, nbytes);
	close(t);
	/* flush data */
	/*
	printf("wrote %d of %d bytes to parent.\n", n, nbytes);
	*/
	return  n;


} /* send_data_to_parent */


data_type_t get_data_from_child(char *fromhost, char *buf, int len)
{
  /* buf contains: fromhost data */
  static int n;
  static char msg[MAX_DATA_CHILD_PAR_LEN];
  char key[MAX_NAME_LEN];
  char *tmp;
  int t;
  struct sockaddr_in from;
  int from_len;
  char hostname[MAX_HOSTNAME_LEN];
  int hname_len;
/*
  printf("**** enter get status from child\n");
*/
  memset(key, '\0', MAX_NAME_LEN);
  memset(msg, '\0', MAX_DATA_CHILD_PAR_LEN);
  while (1) {
	if ((t= get_connection(data_socket, &from, &from_len)) < 0) { 
	  /* get a connection */
	  if (errno == EINTR) {
		continue; 
	  }  /* EINTR might happen on accept(), */
	  if (errno == EWOULDBLOCK) return UNDEFINED;  /* Nothing right now. */
	  fprintf(stderr, "parent gets connection to child: errno = %d\n", errno);
	  perror("accept");               /* bad */
	  exit_system(NULL, -1);
	}
	n = read_data(t, msg, MAX_DATA_CHILD_PAR_LEN-1);
	msg[n] = '\0'; /* read_data doesn't NULL terminate. */
	close(t);
	break;
  }
  /*
  fprintf(stderr, "get_data_from_child: read %d bytes.\n", n);
  fprintf(stderr, "get_data_from_child: msg = <<%s>>\n", msg);
  */
  /* extract hostname */
  memset(hostname,'\0', MAX_HOSTNAME_LEN);
  if (strchr(msg, ' ')) {
	hname_len = strlen(msg) - strlen((char *)strchr(msg, ' '));
	if (hname_len >= MAX_HOSTNAME_LEN)
	  hname_len = MAX_HOSTNAME_LEN - 1;
	strncpy(hostname,  msg, hname_len);
  }

  /* get local host name or the last 2 decimal of the 
   * dotted-decimal ip address */
  if (!isdigit(hostname[0])) {
	/* extract hostname (local name only) i.e. "radar". */
	if (strchr(hostname, '.'))
	  strncpy(fromhost,  hostname, (strlen(hostname) - strlen((char *)strchr(hostname, '.'))));
	else 

	  strcpy(fromhost,  hostname);
  }
  else {
	/* skip to the last 2 digits */
	if (strrchr(hostname, '.')) {
	  tmp = strrchr(hostname, '.');
	  tmp++; /* skip dot */
	}
	strcpy(fromhost, tmp);
  }

  /* skip full host name (i.e. trmm.gsfc.nasa.gov) until the actual data. */

  tmp = strchr(msg, ' ');
  tmp++;

  /* get key word */
  if (strchr(tmp, ':')) 
	strncpy(key, tmp, strlen(tmp) - strlen((char*) strchr(tmp, ':')) +1);

  /* copy the actual data received from monitorps to buf -- 
   * key word is not being removed here */

  strncpy(buf, tmp, len);

  /*

printf("get data from child: key = <%s>, from = <%s>, buf = <%s>\n", key, fromhost, buf);
*/
/*
printf("exit get data from child\n");
 */
  if (strcmp(key,PS_INFO_KEY) == 0) 
	return  PS_INFO;
  if (strcmp(key,STATUS_INFO_KEY) == 0) 
	return STATUS_INFO;
#if 0
  if (strcmp(key, LOG_FILE_KEY) == 0) 
	return LOG_FILE;
#endif 
  if (strcmp(key, DEL_JOB_KEY) == 0) 
	return DEL_JOB;
  if (strcmp(key, DATA_TRANS_KEY) == 0) 
	return DATA_TRANS_FLAG;
  if (strcmp(key, HOST_CURR_TIME_KEY) == 0) 
	return HOST_CURR_TIME;
  if (strcmp(key, LOCAL_CURR_TIME_KEY) == 0) 
	return LOCAL_CURR_TIME;
  return UNDEFINED;
} /* get_data_from_child */






