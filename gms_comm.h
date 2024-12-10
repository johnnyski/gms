
#ifndef __GMS_COMM_H__
#define __GMS_COMM_H__ 1

#include "forms.h"  /* used for exit_system */

/*
#ifdef __linux__
#define linux 1
#endif
*/

#define TIMEOUT_INT           30    /* seconds */
/* communications
 */
#define TO_MONITOR_PORT       5000  /* send to local monitorps */
#define FROM_GMS_CHILD_PORT   5003 /* Port that we communicate to the 
									  gms' child process. */
#define TO_GMS_PARENT_PORT    5003 /* Port that child communicates to the 
									 parent. */
#define MAX_DATA_LEN          20000 /* len of data received from port */
#ifndef MAX_HOSTNAME_LEN
#define MAX_HOSTNAME_LEN      40
#endif
#define MAX_PORT_STR_LEN      20
#define MAX_MSG_LEN           MAX_DATA_LEN - MAX_PORT_STR_LEN - 2
#define MAX_DATA_CHILD_PAR_LEN MAX_DATA_LEN + MAX_HOSTNAME_LEN + 2

/* Keys of messages received from monitorps */
#define PS_INFO_KEY           "ps:"
#define STATUS_INFO_KEY       "status:"
#if 0
#define LOG_FILE_KEY          "log:"
#endif
#define DEL_JOB_KEY           "command:"
#define DATA_TRANS_KEY        "dtrans:"
#define HOST_CURR_TIME_KEY    "time:"
#define LOCAL_CURR_TIME_KEY   "local_time:"

#define PS_INFO_STR           "ps"
#define STATUS_INFO_STR       "status"
#define LOG_FILE_STR          "log"

#ifndef MAX_NAME_LEN
#define MAX_NAME_LEN          40
#endif

/* Statuses of communication between monitorps and child&parent */
typedef enum {
  /* between child and monitorps */
  GET_CONNECTION_WITH_MONITOR, NO_DATA_FROM_MONITOR, 
  GET_CONNECTION_WITH_MONITOR_FAILED, READ_FROM_MONITOR, 
  DONE_READING_FROM_MONITOR, END_CONNECTION_WITH_MONITOR, 
  CONNECTED_WITH_MONITOR, 

  /* between child and parent */
  CONNECT_TO_MONITOR, WROTE_TO_MONITOR, WRITE_TO_MONITOR,
  CONNECT_TO_MONITOR_FAILED,  END_CONNECTION_TO_MONITOR,
  CONNECTED_TO_MONITOR, 

  NO_TRANS
} comm_status_type_t;

#define   IDLE_STR            "Done"   /* Status of data transmission */
#define   PROCESS_COMM_WITH_MONITOR_DIE_STR "Can't receive data from server. Child died."

/* Parent to monitorps -- strings associated with comm_status_type_t*/
#define   WROTE_TO_MONITOR_STR            "Done sending data. "
#define   WRITE_TO_MONITOR_STR            "Sending to monitorps..."
#define   CONNECT_TO_MONITOR_STR          "SND_CMD: Trying to connect..."
#define   CONNECTED_TO_MONITOR_STR        "SND_CMD: Connection established...."
#define   CONNECT_TO_MONITOR_FAILED_STR   "SND_CMD: Connection failed."
#define   END_CONNECTION_TO_MONITOR_STR   "SND_CMD: End connection."


/* Child with monitorps -- strings associated with comm_status_type_t*/

#define  GET_CONNECTION_WITH_MONITOR_STR  "RCV_DAT: Trying to connect..."
#define  NO_DATA_FROM_MONITOR_STR         "No data from monitorps."
#define  GET_CONNECTION_WITH_MONITOR_FAILED_STR  "RCV_DAT: Connection failed."
#define  CONNECTED_WITH_MONITOR_STR       "RCV_DAT: Connection established..."
#define  END_CONNECTION_WITH_MONITOR_STR  "RCV_DAT: End connection."
#define  READ_FROM_MONITOR_STR            "Receiving data..."
#define  DONE_READING_FROM_MONITOR_STR    "Received data"


/* Commands to  send to monitorps */
#define ADD_GUI_STR           "add gui "
#define DEL_GUI_STR           "del gui "
#define DEL_NEIGHBOR_STR      "del neighbor"
#define ADD_NEIGHBOR_STR      "add neighbor"
#define GET_LOG_FILES_STR     "get log files"
#define ADD_PSGUI_GUI_STR     "add psgui "
#define RESET_STR             "reset gui "
#define DEL_JOB_STR           "del job "

typedef enum data_type {
  STATUS_INFO, PS_INFO, LOG_FILE, ADD_GUI, DEL_GUI, ADD_NEIGHBOR, 
  DEL_NEIGHBOR, GET_LOG_FILES, DEL_JOB, DATA_TRANS_FLAG, HOST_CURR_TIME,
  LOCAL_CURR_TIME,  UNDEFINED
} data_type_t;

#ifndef MAX_CMD_LEN
#define MAX_CMD_LEN 200
#endif
typedef struct {
	int pid;
	int ppid;
	char user[9];
	char cmd[MAX_CMD_LEN];
	int sflag;     /* select flag */
  } Process;


void initialize_communication1();

int send_msg(char *tohost, char *msg);

void print_ps(char *buf);
data_type_t get_data_from_child(char *fromhost, char *buf, int len);
int send_data_to_parent(char *fromhost, char *buf);
void get_messages_from_monitor();
void exit_child(int code);

void exit_system(FL_OBJECT *obj, long value);
void add_msg_to_sys_stat_browser(char * msg);
void send_comm_status_to_parent(comm_status_type_t code);
void process_comm_status_msg(char *msg, comm_status_type_t stat_type, int msg_from_child);
#endif




