#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>


#include "comm.h"
#include "gms_comm.h"
#include "gts_monitor.h"

extern unsigned short gms_portnum; /* use to listen to monitorps */
static sigset_t czeromask;
/* hostname for current machine */
extern char local_host[MAX_HOSTNAME_LEN];
extern char long_name_local_host[MAX_HOSTNAME_LEN];

extern int  gui_socket;

extern char *comm_stat_str; 

int errno;
extern  int  need_to_send_sigio;
static void io_handler(void);
extern pid_t gms_ppid;
int child_blocked_io;
static sigset_t csigset;
int child_send_msg(char *tohost, char *msg);
/***************************************************************************/
/*                                                                         */
/*                     child_block_io_sig                                  */
/*                                                                         */
/***************************************************************************/
long child_block_io_sig()
{
/*
  extern int sigblock(int);
*/

  long oldmask = 0;

  sigprocmask(SIG_SETMASK, &csigset, NULL);

/*
  oldmask = (long) sigblock(sigmask(SIGIO));
*/
  /* only block if it's not being blocked */
  child_blocked_io++;
  return oldmask;
}

/***************************************************************************/
/*                                                                         */
/*                     child_unblock_io_sig                                */
/*                                                                         */
/***************************************************************************/
void child_unblock_io_sig(long oldmask)
{ 
/*
  extern int sigsetmask(int);
*/
  /* don't unblock if it wasn't blocked */
  child_blocked_io--;
  if (child_blocked_io != 0) return;

  sigprocmask(SIG_SETMASK, &czeromask, NULL);
/*
  sigsetmask(sigmask(SIGIO));
*/

}

/***************************************************************************/
/*                                                                         */
/*                     exit_child                                          */
/*                                                                         */
/***************************************************************************/
void exit_child(int code)
{ 
  long old_mask;

/*
  extern int ps_info_pipe[];
  close(ps_info_pipe[1]);
*/
  old_mask = child_block_io_sig();
  if (code != 0) {
	kill(gms_ppid, SIGTERM);
  }
  child_unblock_io_sig(old_mask);

  exit(code);
}

/***************************************************************************/
/*                                                                         */
/*                     int_handler                                         */
/*                                                                         */
/***************************************************************************/
void int_handler()
{
  long old_mask;

  old_mask =  child_block_io_sig();
/*
  extern int ps_info_pipe[];
*/
  
  /* wait for all child processes to terminate */
  while (waitpid(-1, NULL, WNOHANG) > 0)
    ;
/*
  close(ps_info_pipe[1]);
*/
  child_unblock_io_sig(old_mask);
  exit_child(0);
}

/***************************************************************************/
/*                                                                         */
/*                     exit_handler                                        */
/*                                                                         */
/***************************************************************************/
void exit_handler()
{
  long old_mask;

  old_mask = child_block_io_sig();

  kill(gms_ppid, SIGTERM);

  child_unblock_io_sig(old_mask);
  exit_child(0);
}

/***************************************************************************/
/*                                                                         */
/*                     child_int_handler                                   */
/*                                                                         */
/***************************************************************************/
void child_int_handler()
{

  
  exit_child(0);

}

/***********************************************************************/
/*                                                                     */
/*                    fireman                                          */
/*                                                                     */
/***********************************************************************/
void fireman(void)
{
  long old_mask;
  old_mask = child_block_io_sig();

  while (waitpid(-1, NULL, WNOHANG) > 0)
    ;
  signal(SIGCHLD, (void(*)(int))fireman);
  child_unblock_io_sig(old_mask);
}

void timer(int n)
{
  long old_mask;
  old_mask = child_block_io_sig();
  signal(SIGALRM, (void(*)(int))timer);
  if (need_to_send_sigio) {
	/* Send sigio to itself to get the last message from monitorps. */
	need_to_send_sigio = 0;
	kill(getpid(), SIGIO);
  }
  child_unblock_io_sig(old_mask);
}

/***************************************************************************/
/*                                                                         */
/*                     pretty_print_list                                   */
/*                                                                         */
/***************************************************************************/
void pretty_print_list(Process p[], int n, int ppid, int i_start, int depth)
{
  int i, j;

  for (i=i_start; i<n; i++) {
	if (ppid == p[i].pid) continue;
	  
	if (ppid == p[i].ppid) {
	  for (j=0; j<=depth; j++) printf(".");
	  printf("%d %d %s\n", p[i].ppid, p[i].pid, p[i].cmd);
	  pretty_print_list(p, n, p[i].pid, 0, depth+1);
	}
  }
}

/***********************************************************************/
/*                                                                     */
/*                    print_ps                                         */
/*                                                                     */
/***********************************************************************/
void print_ps(char *buf)
{
  Process ps_list[1000];
  int n;
  char *token;
  char tmp_buf[MAX_DATA_LEN];

  strcpy(tmp_buf,buf);
  n = 0;
  token = strtok(tmp_buf, " ");
  token = strtok(NULL, " ");
  strncat(ps_list[n].user, token, 9);
  while (token) {
	
	token = strtok(NULL, " "); if (token == NULL) break;
	ps_list[n].pid = atol(token);

	token = strtok(NULL, " "); if (token == NULL) break;
	ps_list[n].ppid = atol(token);

	token = strtok(NULL, "\n"); if (token == NULL) break;
	strncat(ps_list[n].cmd, token, 200);

	n++; /* Another line? */
	token = strtok(NULL, " "); if (token == NULL) break;
	strncat(ps_list[n].user, token, 9);

  }

  pretty_print_list(ps_list, n, 0, 0, 0);
/*
  printf("\n====================================\n");
  for (i=0; i<n; i++) {
	printf("PS: %d ... %s  %d  %d  %s\n",
		   i,
		   ps_list[i].user,
		   ps_list[i].pid,
		   ps_list[i].ppid,
		   ps_list[i].cmd);
  }
*/

}

/***********************************************************************/
/*                                                                     */
/*                    do_something                                     */
/*                                                                     */
/***********************************************************************/
#define NOWAIT 0
void do_something(int s, char *from_who, struct sockaddr_in *from)
{
  extern pid_t gms_ppid;
  char *buf;
  char *fromhost;
  char last_status_msg[200];
  static struct hostent *hosten;
  int i, n, ioffset;
  int first_time = 1, send_trans_msg = 0;

  /*
printf("enter do_something\n");
*/
  if (from == NULL) return;



 /* Read 1 less byte ... data + '\0' */
  buf = (char *)calloc(MAX_DATA_LEN, sizeof(char));


/*
 * 4/10/96 -JHM.
 * We must buffer the last 'status:' message so that it is not
 * split when sending it to the parent.  Our buffer size is MAX_DATA_LEN,
 * however, monitorps could send us much more during this socket connection.
 */

  hosten = gethostbyaddr((char *) &from->sin_addr, sizeof(struct in_addr),
						 from->sin_family);
  fromhost = NULL;
  if (hosten) {
	/* uses string name for ip address, i.e., host.gsfc.nasa.gov */
	fromhost = (char *)strdup(hosten->h_name);
  }
  else {
	/* uses dotted decimal notation of ip address, i.e., ddd.ddd.dd.ddd */
	fromhost =  (char *)strdup(inet_ntoa(from->sin_addr));
  }
/*
printf("from host = <%s>\n", fromhost);
*/
  /* read data from monitorps and send it to parent gms -- through socket
   */

  ioffset = 0; /* On subsequent reads, this gets us past the saved
                * last split 'status:' message.  We expect that the 
				* new read will contain the rest of the 'status:'.
                */
  while((n = read_data(s, buf+ioffset, MAX_DATA_LEN-ioffset-1)) > 0) {


	if (first_time) {
	  /* Send msg once to parent indicating data transmission starts */	  
	  send_comm_status_to_parent(READ_FROM_MONITOR);
	  send_trans_msg = 1;
	  first_time = 0;
	}

/*
   print_ps(buf);
*/
	/* Find the last \n in the buffer.
     * Null terminate after that.  And copy whatever is left
     * to the begining of buf, after sending to the parent.
     */
	n += ioffset;
	buf[n] = '\0';

	for(i=n-1; i>=0; i--)
	  if (buf[i] == '\n') break;
	memcpy(last_status_msg, buf+i+1, n-i);
	buf[i+1] = '\0';
	(void)send_data_to_parent(fromhost, buf);

	ioffset = n-i-1;
	memcpy(buf, last_status_msg, ioffset);

  }
  if (ioffset > 0) { /* The last record must be sent. */
	(void)send_data_to_parent(fromhost, buf);
  }
  free(fromhost);
  free(buf);

  if (send_trans_msg) {
	/* Send msg to parent indicating data transmission ends */
	send_comm_status_to_parent(DONE_READING_FROM_MONITOR);
  }

/*
printf("exit do somethin\n");
*/

}

/***********************************************************************/
/*                                                                     */
/*                    read_socket                                      */
/*                                                                     */
/***********************************************************************/
#define forkchild 0
void read_socket(int s, char *from_who)
{
/*
  extern int ps_info_pipe[];
*/
  int t;
  struct sockaddr_in from;
  int from_len;

  while(1) {
	send_comm_status_to_parent(GET_CONNECTION_WITH_MONITOR);
	if ((t= get_connection(s, &from, &from_len)) < 0) { /* get a connection */
	  if (errno == EINTR) {
		fprintf(stderr, "EINTR get connection\n"); 
		continue; 
	  }  /* EINTR might happen on accept(), */
	  if (errno == EWOULDBLOCK) { 
		send_comm_status_to_parent(NO_DATA_FROM_MONITOR);
		return;  /* Nothing right now. */
	  }
	  fprintf(stderr, "child gets connection with monitorps: errno = %d\n", errno);
	  perror("accept");               /* bad */
	  send_comm_status_to_parent(GET_CONNECTION_WITH_MONITOR_FAILED);
	  exit_child(-2);
	}
	send_comm_status_to_parent(CONNECTED_WITH_MONITOR);
#if forkchild

	switch(fork()) {
 	case -1 : perror("fork"); close(s); close(t); exit_child(-1);
	case 0 : 

	  signal(SIGINT, int_handler);
	  close(s);
/*
printf("new pid = %ld\n", getpid());
*/
	  do_something(t, from_who, &from); 
	  exit_child(0); /* Child. */
	default : 
	  close(t); 
	  send_comm_status_to_parent(END_CONNECTION_WITH_MONITOR);
	  return; /* parent */
	}

#else

	do_something(t, from_who, &from); 
	close(t);
	send_comm_status_to_parent(END_CONNECTION_WITH_MONITOR);
	break;
#endif /* forkchild */
  }
} /* read_socket */



/***************************************************************************/
/*                                                                         */
/*                     do_io_handler                                       */
/*                                                                         */
/***************************************************************************/
void do_io_handler(void)
{
  fd_set read_template;
  struct timeval wait;
  int rc;

  wait.tv_sec = 0;
  wait.tv_usec = 0; /* Don't wait, there *is* something pending. */
  FD_ZERO(&read_template);
  FD_SET(gui_socket, &read_template);
#ifdef __hpux
#define SELECT_TYPE (int *)
#else
#define SELECT_TYPE (fd_set *)
#endif
  rc = select(FD_SETSIZE, SELECT_TYPE &read_template, SELECT_TYPE 0, SELECT_TYPE 0, &wait);

  /* Who caused this interrupt? */
  if (rc <= 0) {
	/* Possible error or timeout. */
	return;
  }
  
  if (FD_ISSET(gui_socket, &read_template)) {
	/* Big brother is watching. */
	/* Read data from monitorps' channel */
	read_socket(gui_socket, "monitorps");

  }
  /* Set flag to send sigio to itself to read the last msg */
  need_to_send_sigio = 1;  
}
/***********************************************************************/
/*                                                                     */
/*                    io_handler                                       */
/*                                                                     */
/***********************************************************************/
void io_handler(void)
{
  long oldmask;


  oldmask = child_block_io_sig();
  /*
printf("child: enter  io_handler= %ld \n", (long) time(NULL));
*/
  signal(SIGIO, (void(*)(int))io_handler);



  do_io_handler();
  /*
printf("child: exit  io_handler= %ld \n", (long) time(NULL));
*/
  child_unblock_io_sig(oldmask);

}

/***********************************************************************/
/*                                                                     */
/*                    initialize_communication                         */
/*                                                                     */
/***********************************************************************/
void initialize_communication(void)
{
  extern char local_host[MAX_HOSTNAME_LEN];
  sigset_t sigset;
  char msg[MAX_MSG_LEN];
  char buf[MAX_DATA_LEN];
  /*
  struct timeval timeout_value;
  */

  child_blocked_io = 0;

  sigemptyset(&csigset);
  sigemptyset(&czeromask);
  sigaddset(&csigset, SIGIO); 
  sigaddset(&csigset, SIGALRM); 

  signal(SIGIO, (void(*)(int))io_handler);
  signal(SIGCLD, (void(*)(int))fireman);
  signal(SIGALRM, (void(*)(int))timer);

  signal(SIGKILL, exit_handler);
  signal(SIGSTOP, exit_handler);
  signal(SIGINT, exit_handler);
  signal(SIGBUS, exit_handler);
  signal(SIGSEGV, exit_handler);
  signal(SIGTERM, exit_handler);
  signal(SIGABRT, exit_handler);
  signal(SIGWINCH, SIG_IGN);

  sigemptyset(&sigset);


#if 0

  sleep(1);  /* wait for parent to finish creating forms */
#endif


  /* Send local host's current time to parent. THe parent will use
   * this time to compute host's time difference.  This ensures we get the
   * exact host's time difference since monitorps will send its time 
   * right after it receives this add gui message--There is a delay before
   * the parent process receives this time msg, so the parent can't use
   * the time from gettimeofday as the local current time when it finally
   * receives this msg.
   */

  memset(buf, '\0', MAX_DATA_LEN);
  sprintf(buf, "%s %ld", LOCAL_CURR_TIME_KEY, (long) get_curr_time());
  /*
fprintf(stderr, "sending to parent: %s, time: %ld\n", buf, (long) get_curr_time());
*/
  send_data_to_parent(local_host, buf);

  /* child is ready to listen to monitorps -- send to monitor daemon. */
  /* This process sends 'add gui' to monitorps (local).  
   * Each monitorps in the network, once received this message by the 
   * local monitorps will start sending the local current time to this
   * process.  When the gms' parent process receives the current time for
   * a neighbor, it will send to monitorps (local) this message: 
   * 'reset gui <local_host>.<portnum> <neighbor_host>'.
   * When monitorps receives this reset message, it will start sending
   * jobs' status info to this process.
   */

  /* Make gui socket async here since its options are not inherited from 
   * the parent.
   */
  if (make_socket_async(gui_socket) < 0) exit_child(-2);

  /* Set the timeout for the socket */
  /*
  timeout_value.tv_sec = TIMEOUT_INT;
  timeout_value.tv_usec = 0;
  setsockopt(gui_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout_value, sizeof(struct timeval));
  */
  memset(msg, '\0', MAX_MSG_LEN);
  sprintf(msg, "%s %s.%d", ADD_GUI_STR, local_host, gms_portnum);
  if (child_send_msg(local_host, msg) < 0)
	  exit_child(-2);


  /* unblock signals blocked by parent */
  sigprocmask(SIG_SETMASK, &sigset, NULL);

} /*  initialize_communication */

/***********************************************************************/
/*                                                                     */
/*                   get_messages_from_monitor                         */
/*                                                                     */
/***********************************************************************/
void get_messages_from_monitor()
{  /* Each socket will be interrupt driven. */
  sigset_t zeromask;

  memset(&zeromask, 0, sizeof(zeromask));

  initialize_communication();

  timer(0);
  while(1) {
	if (need_to_send_sigio) alarm(2);
	else alarm(100000); /* Good night Snow White. */
	sigsuspend(&zeromask);
  }

}




/***************************************************************************/
/*                                                                         */
/*                     send_msg                                            */
/*                                                                         */
/***************************************************************************/
int send_msg(char *tohost, char *msg)
{
  /* returns 1 for msg sent; 0 for no msg sent; -1 for error.
   * Parent sends msg directly to monitorps.  This routine involves 
   * updating forms.
   */
  extern char local_host[MAX_HOSTNAME_LEN];
  extern int need_to_clean_xform;
  int rc;
  pid_t cpid2;
  
  /*
  struct timeval timeout_value;
  */
  /*
printf("enter send_msg, portnum = %d\n", gms_portnum);
*/
  /* Fork a child process to do communication with monitorps here to prevent 
   * the system from freezing  when the communication does not go through.
   */

  switch(cpid2 = fork()) {

  case -1: perror("fork"); exit_system(NULL, -1);         /* error */

  case 0:  /* child */
	/* Reset signals */
	signal(SIGINT, child_int_handler);
	signal(SIGTERM, child_int_handler);
	signal(SIGBUS, child_int_handler);
	signal(SIGSEGV, child_int_handler);
	signal(SIGSTOP, child_int_handler);
	signal(SIGKILL, child_int_handler);
	signal(SIGWINCH, SIG_IGN);
	signal(SIGCLD, SIG_IGN);
	signal(SIGIO, SIG_IGN);
	signal(SIGALRM,  SIG_IGN);
	need_to_clean_xform = 0;

	rc = child_send_msg(tohost, msg);
	exit(rc);

  default: /* parent */

	/* Parent will receive SIGCLD when child dies. Will handle status there
	 */
	break;
  }
  return 1;
}

/***************************************************************************/
/*                                                                         */
/*                     child_send_msg                                      */
/*                                                                         */
/***************************************************************************/
int child_send_msg(char *tohost, char *msg)
{
  /* returns 1 for msg sent; 0 for no msg sent; -1 for error.
   * Child send msg to monitorps. 
   */
  extern char local_host[MAX_HOSTNAME_LEN];
  int s;
  /*
  char tmpmsg1[MAX_DATA_LEN + 20];
  */


/*
printf("enter send_msg, portnum = %d\n", gms_portnum);
*/
  if (msg == NULL || strlen(msg) <=0 || tohost == NULL || 
	  strlen(tohost) == 0) return (-1);

 /* ignore dotted-decimal ip address.  */
  if (isdigit(*tohost)) {
	fprintf(stderr, "ignore sending to host <%s>\n", tohost);
	return 0;
  }

  send_comm_status_to_parent(CONNECT_TO_MONITOR);
  if (strcmp(tohost, local_host) == 0)
	/* send to local monitorps */
	s = call_socket(tohost, TO_MONITOR_PORT);
  else 
	/* send to non-local monitorps */
	/*	s = call_socket(tohost, T0_NEIGHBOR_PORT);  */
	;

  if (s < 0) {
	perror(tohost);
	fprintf(stderr, "Failed creating connection with monitorps\n");
	send_comm_status_to_parent(CONNECT_TO_MONITOR_FAILED);
	return (-1);
  }
  send_comm_status_to_parent(CONNECTED_TO_MONITOR);
  /*
  timeout_value.tv_sec = 30;
  timeout_value.tv_usec = 0;
  setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &timeout_value, sizeof(struct timeval));
  */
  /*
  memset(tmpmsg1, '\0', MAX_DATA_LEN+ 20);

  sprintf(tmpmsg1, "sent to %s: <<%s>>", tohost, msg);

fprintf(stderr, "%s\n", tmpmsg1);
*/
  send_comm_status_to_parent(WRITE_TO_MONITOR);
  write_data(s, msg, strlen(msg));
  send_comm_status_to_parent(WROTE_TO_MONITOR);
  close(s);
  send_comm_status_to_parent(END_CONNECTION_TO_MONITOR);
  return 1;
}




void send_comm_status_to_parent(comm_status_type_t stat)
{
  char buf[MAX_DATA_LEN];

  memset(buf, '\0', MAX_DATA_LEN);
  sprintf(buf, "%s %d", DATA_TRANS_KEY, stat);
  send_data_to_parent(long_name_local_host, buf);
} /* send_comm_status_to_parent */




