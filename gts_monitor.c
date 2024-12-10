/*****************************************************************************
 * gts_monitor.c 
 *
 *  This GUI program allows users to monitor the statuses of jobs started by 
 *  'all_tape_level_1n2' and 'all_tape_1C-51'.  In addition, it is capable of 
 *  showing the ps infomation.
 *
 *  Note:  
 *   1. This program can monitor (in realtime) any program (including 
 *      non-GVS' products).  However, it can only generate reports for GVS' 
 *      products only.  
 * 
 *   2. Job number starts at 1.
 *
 *   3. The recognized statuses are: 
 *        successful, failed, aborted, warning, finished.
 *
 *  Requirements:
 *  =============
 *  
 *  1. monitorps -- server that sends status info, plus other data,
 *     to this program.
 *  2. all_tape_level_1n2, all_tape_level_1--start jobs and call 'send_status'
 *     (Or any program that calls 'send_status')
 *  3. send_status -- sends job's status to monitorps.
 *  4. ~/.gms -- gms init file (see a).
 *  5. $GMS_ICONS_PATH -- environment variable (see b). 
 *  6. File, GMS_NETWORK_HOSTS_FILENAME, defined in the Makefile.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  (a) This program reads user's selected programs and neighbors for
 *      monitoring from "~/.gms" having the following format: 
 *
 *      <beginning of file>
 *       # Monitored Programs
 *       prog1
 *       prog2
 *        ...
 *       progN
 *       # Monitored Neighbors
 *       host1
 *       host2
 *        ...
 *       hostM
 *       # Options
 *       show_button_info: 0|1
 *       show_toolbar:     0|1
 *       blink_boxes:      0/1
 *       # GV Level Information (level# prog prog...)
 *       level# prog1 prog2 ... progN
 *        ...
 *      <end of file>
 *  where:
 *      0 = off (no) ; 1 = on (yes).
 *      level# = 1-3
 *
 *  (b) It loads pixmap files from $GMS_ICONS_PATH or 
 #      "/usr/local/trmm/gms/images" if $GMS_ICONS_PATH is not defined.
 *
 ****************************************************************************
 *---------------------------------------------------------------------------
 * Below lists the detail of program's process flow.
 *
 * Parent process' flow:
 * =====================
 *
 *   - Sets signal handlers
 *  
 *   - Block some relevant signals (i.e. SIGIO, SIGSIGALRM)
 *
 *   - Initialize lists, etc...
 *
 *   - Establishes a communication channel (1) (parent & child communication)
 *     Channel is ready to receive data.
 * 
 *   - Establishes a communication channel (2)(monitorps & child communication)
 *     Channel is ready to receive data.
 *
 *   - Forks a child process to handle receiving data  from monitorps.
 *
 *   - Sets IO & ALARM signal handle 
 *
 *   - Closes channel (2).
 *
 *   - Initialize Xform and show welcome form
 *
 *   - Initializes gui (create forms, etc.)
 *
 *   - Unblock signals
 *
 *   - Set alarm to update forms
 *
 *   - Goes to sleep and wakes up when there is an X event or SIGALRM, or
 *     SIGIO from child.
 *
 *   Notes:
 *
 *   - Parent updates forms in every 2-3 seconds (on average) when there are 
 *     data coming from monitorps. It only refreshes forms when the system 
 *     is idle, meaning when the program is not handling any signal i.e.
 *     IO, alarm, and X.
 *
 *   - Parent will send commands to monitorps -- In doing so, it will fork 
 *     a child process to handle this task. note: It will open a
 *     connection with monitorps (local), send data, and close this connection.
 * 
 *     The commands are:
 *         1. del gui host.portnum
 *         2. del neighbor host   (will remove this host from monitorps' list)
 *         3. add neighbor host   (will add this host to monitorps' list)
 *         4. get log files
 *         5. add psgui host.portnum
 *         6. reset gui local_host.portnum
 *         7. reset gui local_host.portnum neighbor_host
 *
 * Child process' flow:
 * ====================
 *   - Sets signals' handler
 *   
 *   - Unblocks signals blocked by parent
 *
 *   - Make gui_socket, created by parent, asynchronous
 *
 *   - Sends 'add gui local_host.portnum' to monitorps
 *     Monitorps will start sending status info. when it receives this msg.
 *
 *   - Suspends (sleeps) until SIGIO for monitorps to occur.
 * 
 *       - Reads data from monitorps 
 *     
 *       - Sends data to the parent process
 *
 *       Note: monitorps sends status messages in one connection.  The
 *             amount of data to be read might exceed the buffer size, 
 *             MAX_DATA_LEN.  As a result, status message might be splitted.
 *             Nevertherless, the child process will make sure that it will
 *             send to its parent a set of non-splitted status messages.
 *   
 *
 * Data flow between monitorps and gms:
 * 
 * The first few steps to get monitorps start sending data to gms are:
 *
 * 1. When the child is ready to listen to monitorps, it sends to the 
 *    local monitorps this message: 'add gui'.
 * 2. gms, now expects to receive this message:'time:secs' from each 
 *    monitorps in the network.
 * 3. When the child process receives a current time info, it will 
 *    send to its parent process. 
 * 4. When the parent process receives the current time for
 *    a neighbor host, it will send to monitorps (local) this message: 
 *    'reset gui <local_host>.<portnum> <neighbor_host>'.
 * 5. gms, now expects to receive jobs' status info from monitorps.
 *
 *                            -----
 * gms is waiting to receive from monitorps the following messages  whenever 
 * they become available:
 *   ps
 *   job's status
 *   del neighbor
 *   add neighbor
 *   
 *   
 * 
 ****************************************************************************
 *
 * Note:
 *  1. Set the environmental variable, GMS_DEBUG, if want to have messages 
 *     received  from monitorps be printed to 'gms.out'.  This is useful for 
 *     debugging.
 *
 *****************************************************************************
 *
 * by: Thuy Nguyen
 *     NASA/TRMM Office
 *     Thuy.Nguyen@gsfc.nasa.gov
 *      
 *     Copyright @ 1996, 1997
 *
 *****************************************************************************/


#include <strings.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/time.h>
#include <regex.h>
#include <math.h>
#include <forms.h>

#include "comm.h"
#include "gms_comm.h"
#include "gms_time.h"
#include "gts_monitor_form.h"
#include "gts_monitor.h" 


/*************************************************************************/
/*                                                                       */
/*                      Global data declarations section                 */
/*                                                                       */
/*************************************************************************/


/*
 ******************************* icons filename  *************************
 */
char delete_job_icon[MAX_FILENAME_LEN];
char show_detail_icon[MAX_FILENAME_LEN];
char help_icon[MAX_FILENAME_LEN];
char cancel_icon[MAX_FILENAME_LEN];
char print_red_icon[MAX_FILENAME_LEN];
char print_yellow_icon[MAX_FILENAME_LEN];
char print_all_icon[MAX_FILENAME_LEN];
char neighbors_icon[MAX_FILENAME_LEN];
char progs_icon[MAX_FILENAME_LEN];
char print_icon[MAX_FILENAME_LEN];
char exit_icon[MAX_FILENAME_LEN];
char sys_stat_icon[MAX_FILENAME_LEN];
char reset_icon[MAX_FILENAME_LEN];
char bars_icon[MAX_FILENAME_LEN];
char get_icon[MAX_FILENAME_LEN];
char test_icon[MAX_FILENAME_LEN];
char ps_icon[MAX_FILENAME_LEN];
char save_icon[MAX_FILENAME_LEN];
char report_icon[MAX_FILENAME_LEN];
char clear_icon[MAX_FILENAME_LEN];
char show_detail_icon[MAX_FILENAME_LEN];
char show_all_data_icon[MAX_FILENAME_LEN];
char exceeded_threshold_icon[MAX_FILENAME_LEN];

/*
 ****************************** Update Form Flags ************************
 */

int need_to_clean_xform = 0;
int show_main_form = 0;
int gms_busy_cursor = 0;
int refresh_status_form = 0;
int neighbor_list_changed = 0;
int prog_list_changed = 0;
int status_info_changed = 0;
int ps_info_changed = 0;
int logdata_changed = 0;
int gms_update_forms = 0;        /* applies for all forms */
int update_sys_stat_form = 0;
int need_show_new_data = 0;      /* When new data came in */
time_t last_set_alarm_time = 0;  /* Use to refresh forms */

int need_to_send_sigio = 1;
char *curr_selected_prod_groups[MAX_ITEMS];
char *log_selected_items[MAX_ITEMS];
char curr_selected_tape_group[MAX_BROWSER_LINE_LEN];
int load_status_forms = 0;
int something_is_running=0;

summary_type_t selected_summary_level_type = TAPE_SUMMARY;

/* Data transmission status related */
comm_status_type_t data_transmission_status = NO_TRANS;  /* Communication's status 
                                      * between monitorps and child process.
									  */
int update_data_transmission_status = 0; /* Update data trans status label on 
										  * the main form 
										  */
time_t trans_start_time;             /* Time of beginning of data transmission */
int set_data_transmission_inactive = 0;
char *comm_stat_str = IDLE_STR; /* Waiting, receiving, or idle */
time_t local_curr_time;    

/*
 ****************************** forms ******************************
 */
int form_count = 0;
FD_status_fields_selection_form *status_fields_selection_form;
FD_gv_levels_form * gv_levels_form;
FD_busy_form *busy_form = NULL;

FD_get_input_form *tape_get_input_form;
FD_select_item_form *select_host_for_ps_form;  /* show available hosts which
												* user can select to see ps info
												*/
FD_add_del_show_form *progs_form, *neighbors_form, *admin_form;

/* Current statuses related forms */
FD_msg_form *all_data_form;  /* show data of every job received from monitorps */

FD_ps_info_form * ps_form;
FD_gms_form *product_grouping_form;
FD_group_form *tape_grouping_form;
FD_logfile_form *job_grouping_form; /* show tapeid, prod, file 
												* range, and range's status 
												* -- info with key "status:" 
												*/

FD_msg_form *system_status_form;      /* contains system status messages 
									   * indicating what the system is doing
									   * i.e. receiving or processing data 
									   * from monitorps, idle, etc...
									   */

FD_msg_form *exceeded_threshold_form;   /* Show jobs that have runtime exceeds
									   * the threshold, specified by user.
									   */

/* Report related forms*/
FD_report_form *summary_report_form;     /* top level of report -- show tapeid, prod, status */
FD_logfile_form *logfile_report_form; /* Detail level of report -- show tapeid, prod, file range, 
									   * and range's status -- info with 
									   * key "log:" 
									   */

/*
FD_get_input_form *host_select_form;
*/



/* beginning: for testing only */
FD_test_form *test_form;
int write_to_file;
FILE *test_file;


char test_fname[MAX_FILENAME_LEN];
#if 0
int TEST_clear_msg;
int TEST;
int TEST_msg_count;
char thost[MAX_HOSTNAME_LEN];
char tprog[MAX_PROGNAME_LEN];
char tstatus[MAX_STATUS_STR_LEN];
pid_t tpid;
pid_t tppid;
char tinfile[MAX_FILENAME_LEN];

#endif

/* end: for testing only */

/*
 **************************** Menu related ************************************
 */
char windows_menu_items[MAX_WINDOWS_MENU_STR];
int show_button_info_flag = 0;   /* show/hide button_info_form */
int show_toolbar_flag = 0;       /* show/hide toolbar group */
int blink_boxes_flag = 0;        /* blink/unlink colored boxes */

/* view options for gms, group and status forms */
view_options_t forms_view_options;

/* 
 **************************** lists & tables & others ************************
 */

char last_job_status_msg[MAX_BROWSER_LINE_LEN]; /* last job status msg received
                                                 * from monitorps. 
												 */
/*
 **************************** buttons related *********************************
 */

FL_OBJECT *gms_save_button = NULL;

/* button_info_tbl: the order should be correspondent to the order of items in 
 * INFO_TBL_INDEX below.
 */
char *button_info_tbl[] = 
{
  "Quit",
  "Refresh data",
  "Print all",
  "Print failures",
  "Print all",
  "Print warnings",
  "Close form",
  "Save to file",
  "Show product groupings (bars)",
  "Show PS",
  "Show status summary report",
  "Add/Delete programs",
  "Add/Delete neighbors",
  "Show help",
  "Remove all job groupings",
  "Remove all product groupings",
  "Remove all tape groupings",
  "Show product's jobs",
  "Clear selections",
  "Show product groupings (text)",
  "Show all jobs sorted by time",
  "Show jobs running for too long",
  "Refresh data",
  "Show status summary of tapes",
  "Show status summary of GV levels",
  "Show status summary of products",
  "Remove status summary info",
  "",
};

/* INFO_TBL_INDEX: indeces to  button_info_tb above.
 */

enum INFO_TBL_INDEX {
  EXIT_INDEX, RESET_INDEX, PRINT_INDEX, PRINT_RED_INDEX, PRINT_ALL_INDEX,
  PRINT_YELLOW_INDEX, CANCEL_INDEX, SAVE_INDEX, BARS_INDEX, PS_INDEX,
  SHOW_REPORT_INDEX, PROGS_INDEX, 
  NEIGHBORS_INDEX, HELP_INDEX, DELETE_JOB_GRPING_INDEX, 
  DELETE_PROD_GRPING_INDEX, DELETE_TAPE_GRPING_INDEX, SHOW_DETAIL_INDEX,
  CLEAR_INDEX, RUN_DETAIL_INDEX, SHOW_ALL_DATA_INDEX, EXCEEDED_THRESHOLD_INDEX,
  REFRESH_INDEX, SUMMARY_TAPE_INDEX, SUMMARY_LEVEL_INDEX, SUMMARY_PRODUCT_INDEX, DELETE_SUMMARY_INFO_INDEX,
  NULL_INDEX};

FL_OBJECT *curr_button; /* button that mouse points to */		

/* array of forms and their name */
form_info_t gms_form_list[MAX_FORMS];

char gms_sys_stat_msg[MAX_SYS_STATUS_MSG_LEN];

/* prod_tbl is used for report only  */
char *prod_tbl[MAX_PRODS] = 
	{
	    "cdrom",
	    "1C-51",
		"2A-52i",
		"2A-53-w",
		"2A-53-d",
		"2A-53",
		"2A-54",
		"2A-55",
		"2A-56",
		"2A-57",
		"3A-53",
		"3A-54",
		"3A-55",
	};


level_info_t level_info_table[MAX_GV_LEVELS_NUM]; /* Store programs in the GV level order. Used
								* to create summary reports. Index is 'level-1'.
								*/

double threshold_time = 1.0;
/* hostname for current machine */
char local_host[MAX_HOSTNAME_LEN];
char long_name_local_host[MAX_HOSTNAME_LEN];

/* list of programs to be monitored - gms will monitor all descendents
 * of each program from selected list (ps info form). 
 */
char *Gselected_progs[MAX_PROGS]; /* user-selected list -- filter which progs to monitor. */
char *available_progs[MAX_PROGS]; /* availble list--from msg sent by monitorps */

/* list of hosts which are currently neighbors.*/
char *Gselected_hosts[MAX_HOSTS];  /* user-selected list--Filter which hosts to monitor. */
char *available_hosts[MAX_HOSTS]; /* availble list. These neighbors are communicating with each other via monitorps. */

/* List of hosts which are available in the network.  These hosts may not
 * already be communicating with each other via monitorps.
 */
char *Gavailable_network_hosts[MAX_HOSTS];


/* list of tape ids which are associated with loginfo files received 
 * from monitorps.
 */
char tapeids[MAX_TAPES][MAX_TAPEID_LEN];

/* Item from host info list gets created when user requested 'add neighbor'
 * and deleted when user requested 'del neighbor'.
 * 
 */
struct host_info_rec *host_info_list;

/* tape info list contains data from the logfile(s)
 */
struct tape_info_rec *tape_info_list;

/* gstatus_info contains all status information.
 */
struct graphic_status_rec *gstatus_info;
/* tape_groups groups of jobs belonging in the same group. 
 */
struct tape_grouping_rec *tape_groups;  
struct logfile_node *time_ordered_logfile_list;

/* statuses for all groups in tape_groups */
stat_type_t all_grps_stat = 0;

/*
 ***************************** communication section ************************
 */
int blocked;            /* block flag */
int cpid;               /* child pid */
int cpid_died = 0;      /* THis child died or not */
int gms_child_fd;
pid_t gms_ppid;         /* parent pid */

long gms_old_mask;       /* system's oldmask */
/*
int ps_info_pipe[2];
*/

/* data*: for comm. between child and parent processes 
 */
unsigned short data_portnum;  
int data_socket;

/* gms_portnum: for communication between gms' child process and monitorps 
 */
unsigned short gms_portnum; 
int gui_socket;
static int stat_info_count;
sigset_t lsigset;             /* signals to be blocked */
static sigset_t zeromask;     /* mask to unblock signals */
sigset_t gms_old_sigset;

/*************************************************************************/
/*                                                                       */
/*                            Function prototypes                        */
/*                                                                       */
/*************************************************************************/
void clear_level_info_table(level_info_t *level_info_table);
void initialize_xform(int argc, char **argv);
void time_secs2strs(time_t time, char *time_str, char *date_str);
void process_host_curr_time(char *fromhost, char *buf);
void blink_stat_light_buttons();
void reset_cursor_for_all_forms();
void set_cursor_for_all_forms(int cursor_type);
void do_nothing_io_handler(int sig, void *data);
struct logfile_rec * get_previous_logfile(struct logfile_rec *logfile);
int get_previous_logfile_index(struct logfile_rec **special_logfiles_array,
							   struct logfile_rec *logfile, int nlogfiles);
int get_nindexes_upward(struct logfile_rec **special_logfiles_array,
								 struct logfile_rec *logfile, int nlogfiles);
int get_nindexes_downward(struct logfile_rec **special_logfiles_array,
								 struct logfile_rec *logfile, int nlogfiles);
extern int  create_forms();
extern int process_status_info(char *fromhost, char *buf);
extern void process_ps_info(char *fromhost, char *msg, 
							struct ps_info_rec *sp_ps_info, 
							struct ps_info_rec *ap_ps_info);
extern long gms_block_ints();
extern void gms_exit_handler(int sig);
extern void child_term_handler(int sig);
extern struct logfile_rec * new_logfile_rec(char *fname, char *fromhost, 
											char *tapeid, char *prog);
extern int add_entry_to_log_rec(struct logfile_rec *logfile_ptr, 
		 char *jobnum, time_t time, char *time_str, char *date_str, 
								char *infile, stat_type_t stat);
extern void update_prod_group_form(int reload_form);
void free_ps_info_rec(struct ps_info_rec *rec);
void free_ps_bufs(struct ps_info_rec *rec);
extern struct ps_info_rec * new_ps_info_rec(int data_len);
void init_ps_info_rec(struct ps_info_rec *, int data_len);
extern void initialize_comm();
extern void initialize_main();
void process_incoming_data(data_type_t dtype, char *fromhost, char *buf);
void hide_chart(struct chart_rec *chart_rec);
static void parent_io_handler(int sig);
char * remove_leading_blanks(char *s);
char *remove_pathname(char *s);
int update_forms(XEvent *e, void *data);
void end_xform(); 
void do_alarm_handler(int sig);
void gms_child_exit_handler(int sig) ;
void show_comm_stat(char *stat_str);
/*************************************************************************/
/*                                                                       */
/*                            Functions Section                          */
/*                                                                       */
/*************************************************************************/

/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*                                 M A I N                                 */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
int main (int argc, char **argv)
{
  long old_mask;

/*
  char msg[MAX_MSG_LEN];
*/
  setpgrp();
  signal(SIGINT, gms_exit_handler);
  signal(SIGTERM, gms_exit_handler);
  signal(SIGBUS, gms_exit_handler);
  signal(SIGSEGV, gms_exit_handler);
  signal(SIGSTOP, gms_exit_handler);
  signal(SIGKILL, gms_exit_handler);
  signal(SIGWINCH, SIG_IGN);
  signal(SIGCLD, gms_child_exit_handler);
/*
  signal(SIGWINCH, gms_exit_handler);

  gms_old_mask = block_all_signals();
*/
  blocked = 0; 
  gms_busy_cursor = 0;

  /* set signals to be blocked */
  sigemptyset(&lsigset);
  sigemptyset(&zeromask);
  sigaddset(&lsigset, SIGIO); 
  sigaddset(&lsigset, SIGALRM); 
  sigaddset(&lsigset, SIGCLD); 

  /* block signals */

  gms_block_ints();

  initialize_main();

  initialize_comm(); /* communication between parent and child process.
					  * this routine must be called before forking child. */

  /* fork a process to listen for messages from monitor daemon */
  switch(cpid = fork()) {

  case -1: perror("fork"); exit_system(NULL, -1);         /* error */

  case 0:  /* child */
	/* connect to and receive data from monitorps using tcp/ip.  
	 * Send that data to parent process using tcp/ip */

	get_messages_from_monitor();  /* This routine never returns. */

	break;  

  default:                  /* parent */
	/* Set SIGIO handle to receive connection from child */
	signal(SIGIO, parent_io_handler );
	/* Set alarm handle to update forms */
	signal(SIGALRM, (void(*)(int))do_alarm_handler);

	close(gui_socket);  /* close socket from parent.  This is used by child */
	initialize_xform(argc, argv); /* Init xform and show busy form  */

	/* Create forms and set flag for displaying the main form when the 
	 * alarm goes off.
	 */
	initialize_gui ();
	/* Unblock signals */
	gms_unblock_ints(old_mask);

	/* set alarm to update forms.
	 * Note that forms status forms will be updated when
	 * the alarm goes off.
	 */
	alarm(UPDATE_FORMS_DEFAULT);

	/* main loop.  This main process sleeps and wakes up when an  X event 
	 * or SIGIO signal occurs.  Loop is necessary in case event, having 
	 * no associated callback, occurs.
	 */

	/* Only update forms whenever the system is not handling signal.  
	 * This prevents 'unexpected asyn' message to occur. 
	 * This seems to occur  whenenver the child and parent process' 
	 * communication is connected at the same time forms are being updated.  
     * The reason for this problem may be that the X server and client cannot
     * handle signal??.   Therefore, don't call X lib or Xform routines inside
	 * the signal handler routine--use flags instead and idle_callback 
	 * to workaround this problem. The update flags are set in the
	 * IO handler.  These flags are checked to update forms in 
	 * 'update_forms()'.
	 */
    fl_set_idle_callback(update_forms, (void *) NULL);
	while (1) {
	  fl_do_forms();
	}
  } /* switch */

  /* never reach next statement */
  exit_system(NULL, 0);
  return 0;
} /* main */


/***************************************************************************/
/*                                                                         */
/*                      child_term_handler                                 */
/*                                                                         */
/***************************************************************************/
void child_term_handler(int sig)
{
  long old_mask;
  old_mask = gms_block_ints();
  exit_system(NULL, 0);
  gms_unblock_ints(old_mask);
}

/***************************************************************************/
/*                                                                         */
/*                     gms_child_exit_handler                              */
/*                                                                         */
/***************************************************************************/
void gms_child_exit_handler(int sig) 
{
  /* Handles two different kinds of child processes.
   *  1. Child process handling the receiving of data  from monitorps.
   *  2. Child process handling the sending of command to monitorps.
   */
  pid_t cid;
  long old_mask;
  int status;
  char buf[MAX_DATA_LEN];

  old_mask = gms_block_ints();
  signal(SIGCLD, gms_child_exit_handler);
  cid = wait(&status);

  if (cid == cpid) {
	/* The child process handling the communication with monitorps died .
	 * Show at the status msg.
	 */
	show_comm_stat(PROCESS_COMM_WITH_MONITOR_DIE_STR);
	cpid_died = 1;
  }
  else {
	/* Child process that was forked to send command to monitorps.
	 * Get the status and show it to the system msg form.
	 */
	status >>= 8;

	memset(buf, '\0', MAX_DATA_LEN);
	if (status == 1)
	  sprintf(buf, "Sent command to monitorps successful.");
	else 
	  sprintf(buf, "Unsuccessful sending command to monitorps.");
	append_msg_to_sys_stat_msg(buf);
  }
  gms_unblock_ints(old_mask);
}

/***************************************************************************/
/*                                                                         */
/*                      gms_exit_handler                                   */
/*                                                                         */
/***************************************************************************/
void gms_exit_handler(int sig)
{
#ifdef linux
  extern char *strsignal(int sig);
#endif

  long old_mask;

  old_mask = gms_block_ints();
#if 0
#ifdef linux
  fprintf(stderr, "gms received signal: %s\n", strsignal(sig));
#else
  fprintf(stderr, "gms received signal: %d\n", sig);
#endif
#endif
  exit_system(NULL, 0);
  gms_unblock_ints(old_mask);
}



/***************************************************************************/
/*                                                                         */
/*                      form_atexit                                        */
/*                                                                         */
/***************************************************************************/
int form_atexit(struct forms_ *form, void *value)
{
  long old_mask;

  old_mask = gms_block_ints();
  exit_system(NULL, 0);
  gms_unblock_ints(old_mask);
  return 0;
}

/***************************************************************************/
/*                                                                         */
/*                             get_new_alarm_time                          */
/*                                                                         */
/***************************************************************************/
unsigned int get_new_alarm_time()
{
  unsigned int sec = 0;

  if (update_data_transmission_status) 
	sec = UPDATE_TRANS_STATUS_INTERVAL;
  else if (blink_boxes_flag)
	sec = UPDATE_BLINK_INTERVAL;
  else if (need_show_new_data)
	sec = UPDATE_FORMS_INTERVAL;
  else if  (something_is_running)
	sec = UPDATE_RUNTIME_INTERVAL;
  /*
fprintf(stderr, "sec ======= %d\n", sec);
*/
  return sec;
}

/***************************************************************************/
/*                                                                         */
/*                     set_alarm_to_update_forms                           */
/*                                                                         */
/***************************************************************************/
void set_alarm_to_update_forms()
{
  unsigned int sec;
  /*
  time_t curr_time;

  static unsigned int save_sec = 0;
  */
#if 0
  curr_time = get_curr_time();
#endif
  sec = get_new_alarm_time();
  /*
fprintf(stderr, " curr: %d, last: %d, %d\n", curr_time, last_set_alarm_time, save_sec);
*/
#if 0
  if ((curr_time - last_set_alarm_time) >= save_sec) {
	alarm(sec);  /* reset alarm */
	last_set_alarm_time = curr_time;
  }
  save_sec = sec;
#endif
  alarm(sec);  /* reset alarm */
}
/***************************************************************************/
/*                                                                         */
/*                        gms_atclose                                        */
/*                                                                         */
/***************************************************************************/
int gms_atclose(struct forms_ *form, void *value)
{
  long old_mask;
  /* cleanup before exit the application */
  old_mask = gms_block_ints();
  exit_system(NULL, 0);
  gms_unblock_ints(old_mask);
  return FL_IGNORE;
}

/***************************************************************************/
/*                                                                         */
/*                      form_atclose                                       */
/*                                                                         */
/***************************************************************************/
int form_atclose(struct forms_ *form, void *value)
{
  long old_mask;

  /* form is already closed by form library once in this routine */

  old_mask = gms_block_ints();

  /* remove form's name from windows menu */
  remove_name_from_windows_menu(form);

  /* activate button */
  if (form && form->u_vdata)
	fl_activate_object(form->u_vdata);
  gms_unblock_ints(old_mask);
  return 0;
}


/***************************************************************************/
/*                                                                         */
/*                      gms_unblock_ints                                   */
/*                                                                         */
/***************************************************************************/
void gms_unblock_ints(long oldmask)
{

  /* don't unblock if it wasn't blocked */
  blocked--;
  if (blocked != 0) return;
/*
printf("unblock int\n");
*/
/*
  reset_cursor_for_all_forms();
*/
  sigprocmask(SIG_SETMASK, &zeromask, NULL);

}


/***************************************************************************/
/*                                                                         */
/*                      gms_block_ints                                     */
/*                                                                         */
/***************************************************************************/
long gms_block_ints()
{
  long old_mask=0;

  /* block interrupts */

  sigprocmask(SIG_SETMASK, &lsigset, NULL);
  /* change to busy cursor if signals already blocked */
/*
  if (blocked)
	  set_cursor_for_all_forms(XC_watch);
*/
  /* do not block again if already blocked */
  blocked++;
/*
printf("block int\n");
*/

  return old_mask;
}


/***************************************************************************/
/*                                                                         */
/*                      parent_io_handler                                  */
/*                                                                         */
/***************************************************************************/
void parent_io_handler(int sig)
{
  long oldmask;

  fd_set read_template;
  struct timeval wait;
  int rc;
  char buf[MAX_DATA_LEN];
  char fromhost[MAX_HOSTNAME_LEN];
  data_type_t dtype;

  static int counter = 0;


  oldmask = gms_block_ints();
  signal(SIGIO, (void(*)(int))parent_io_handler);
  counter++;

  /*

printf("parent: enter  io_handler= %ld , counter = %d\n", (long) time(NULL), counter);
*/

  wait.tv_sec = 0;
  wait.tv_usec = 0; /* Don't wait, there *is* something pending. */

  FD_ZERO(&read_template);
  FD_SET(data_socket, &read_template);

  rc = select(FD_SETSIZE,
#ifdef __hpux
			  (int *)
#endif
			  &read_template, 0, 0, &wait);


  /* Who caused this interrupt? */
  if (rc <= 0) {
	/* Possible error or timeout. */
	/*
printf("parent: exit io_handler= %ld, counter = %d \n", (long) time(NULL), counter);
*/
	gms_unblock_ints(oldmask);
	return;
  }

  if (FD_ISSET(data_socket, &read_template)) {
	/* clean buffers */
/*
	printf("clean buffers ... routine parent_io_handler\n");
*/
	memset(buf, '\0', MAX_DATA_LEN);
	memset(fromhost, '\0', MAX_HOSTNAME_LEN);

	/* get data from child */
	dtype = get_data_from_child(fromhost, buf, MAX_DATA_LEN - 1);
	/* now buf contains data sent by the monitorps only, no hostname at
	 * the beginning.
	 */
	process_incoming_data(dtype, fromhost, buf);

  }
  /*
printf("parent: exit io_handler= %ld, counter = %d \n", (long) time(NULL), counter);
*/

  need_to_send_sigio = 1;
  gms_unblock_ints(oldmask);
} /* parent_io_handler */

/***************************************************************************/
/*                                                                         */
/*                      do_alarm_handler                                   */
/*                                                                         */
/***************************************************************************/
void do_alarm_handler(int sig)
{
  long old_mask;
  /*
  unsigned int sec;
  */


/*
  static time_t save_time = 0;

  time_t curr_time;
  */
  old_mask = gms_block_ints();
  /*
printf("in alarm handler######## time = %ld\n", (long) time(NULL));
*/


  /* set update form flag.  This will only update data required regular refresh
   */
  gms_update_forms = 1;
	
  signal(SIGALRM, (void(*)(int))do_alarm_handler);

/*
printf("exit alarm handler\n");
*/
  gms_unblock_ints(old_mask);
} /* do_alarm_handler */

/***************************************************************************/
/*                                                                         */
/*                      set_cursor_for_all_forms                           */
/*                                                                         */
/***************************************************************************/
void set_cursor_for_all_forms(int cursor_type)
{
  Display *display;

  display = fl_get_display();
  gms_busy_cursor = 1;

  if (ps_form && ps_form->ps_info_form &&
	   ps_form->ps_info_form->visible)
	fl_set_cursor(ps_form->ps_info_form->window, cursor_type);

  if (tape_grouping_form && tape_grouping_form->group_form &&
	  tape_grouping_form->group_form->visible)
	fl_set_cursor(tape_grouping_form->group_form->window, cursor_type);

  if (exceeded_threshold_form && exceeded_threshold_form->msg_form &&
	  exceeded_threshold_form->msg_form->visible)
	fl_set_cursor(exceeded_threshold_form->msg_form->window, cursor_type);

  if (product_grouping_form && product_grouping_form->gms_form &&
	  product_grouping_form->gms_form->visible) 
	fl_set_cursor(product_grouping_form->gms_form->window, cursor_type);

  if (neighbors_form && neighbors_form->add_del_show_form &&
	  neighbors_form->add_del_show_form->visible)
	fl_set_cursor(neighbors_form->add_del_show_form->window, cursor_type);

  if (progs_form && progs_form->add_del_show_form &&
	  progs_form->add_del_show_form->visible)
	fl_set_cursor(progs_form->add_del_show_form->window, cursor_type);

  if (tape_get_input_form && tape_get_input_form->get_input_form &&
	  tape_get_input_form->get_input_form->visible)
	fl_set_cursor(tape_get_input_form->get_input_form->window, cursor_type);

  if (summary_report_form && summary_report_form->report_form &&
	  summary_report_form->report_form->visible)
	fl_set_cursor(summary_report_form->report_form->window, cursor_type);

  if (gv_levels_form && gv_levels_form->gv_levels_form &&
	  gv_levels_form->gv_levels_form->visible)
	fl_set_cursor(gv_levels_form->gv_levels_form->window, cursor_type);

  if (status_fields_selection_form && status_fields_selection_form->status_fields_selection_form &&
	  status_fields_selection_form->status_fields_selection_form->visible)
	fl_set_cursor(status_fields_selection_form->status_fields_selection_form->window, cursor_type);

  if (logfile_report_form && logfile_report_form->logfile_form &&
	  logfile_report_form->logfile_form->visible)
	fl_set_cursor(logfile_report_form->logfile_form->window, cursor_type);

  if (job_grouping_form && job_grouping_form->logfile_form &&
	  job_grouping_form->logfile_form->visible)
	fl_set_cursor(job_grouping_form->logfile_form->window, cursor_type);

  if (system_status_form && system_status_form->msg_form &&
	  system_status_form->msg_form->visible)
	fl_set_cursor(system_status_form->msg_form->window, cursor_type);
  
  if (select_host_for_ps_form && select_host_for_ps_form->select_item_form &&
	  select_host_for_ps_form->select_item_form->visible)
	fl_set_cursor(select_host_for_ps_form->select_item_form->window, cursor_type);
  
  if (gstatus_info && gstatus_info->sform && gstatus_info->sform->status_form &&
	  gstatus_info->sform->status_form->visible)
	fl_set_cursor(gstatus_info->sform->status_form->window, cursor_type);

  if (test_form && test_form->test_form && test_form->test_form->visible)
	fl_set_cursor(test_form->test_form->window, cursor_type);

    XFlush(fl_get_display());

} /* set_cursor_for_all_forms */

/***************************************************************************/
/*                                                                         */
/*                      reset_cursor_for_all_forms                         */
/*                                                                         */
/***************************************************************************/
void reset_cursor_for_all_forms()
{
  if (gms_busy_cursor == 0) return;

  gms_busy_cursor = 0;

  if (ps_form && ps_form->ps_info_form &&
	   ps_form->ps_info_form->visible)
	fl_reset_cursor(ps_form->ps_info_form->window);

  if (product_grouping_form && product_grouping_form->gms_form &&
	  product_grouping_form->gms_form->visible) 
	fl_reset_cursor(product_grouping_form->gms_form->window);

  if (exceeded_threshold_form && exceeded_threshold_form->msg_form &&
	  exceeded_threshold_form->msg_form->visible)
	fl_reset_cursor(exceeded_threshold_form->msg_form->window);

  if (tape_grouping_form && tape_grouping_form->group_form &&
	  tape_grouping_form->group_form->visible)
	fl_reset_cursor(tape_grouping_form->group_form->window);

  if (progs_form && progs_form->add_del_show_form &&
	  progs_form->add_del_show_form->visible)
	fl_reset_cursor(progs_form->add_del_show_form->window);


  if (neighbors_form && neighbors_form->add_del_show_form &&
	  neighbors_form->add_del_show_form->visible)
	fl_reset_cursor(neighbors_form->add_del_show_form->window);

  if (tape_get_input_form && tape_get_input_form->get_input_form &&
	  tape_get_input_form->get_input_form->visible)
	fl_reset_cursor(tape_get_input_form->get_input_form->window);


  if (summary_report_form && summary_report_form->report_form &&
	  summary_report_form->report_form->visible)
	fl_reset_cursor(summary_report_form->report_form->window);

  if (gv_levels_form && gv_levels_form->gv_levels_form &&
	  gv_levels_form->gv_levels_form->visible)
	fl_reset_cursor(gv_levels_form->gv_levels_form->window);

  if (status_fields_selection_form && status_fields_selection_form->status_fields_selection_form &&
	  status_fields_selection_form->status_fields_selection_form->visible)
	fl_reset_cursor(status_fields_selection_form->status_fields_selection_form->window);

  if (logfile_report_form && logfile_report_form->logfile_form &&
	  logfile_report_form->logfile_form->visible)
	fl_reset_cursor(logfile_report_form->logfile_form->window);

  if (job_grouping_form && job_grouping_form->logfile_form &&
	  job_grouping_form->logfile_form->visible)
	fl_reset_cursor(job_grouping_form->logfile_form->window);

  if (system_status_form && system_status_form->msg_form &&
	  system_status_form->msg_form->visible)
	fl_reset_cursor(system_status_form->msg_form->window);
  
  if (select_host_for_ps_form && select_host_for_ps_form->select_item_form &&
	  select_host_for_ps_form->select_item_form->visible)
	fl_reset_cursor(select_host_for_ps_form->select_item_form->window);

  if (gstatus_info && gstatus_info->sform && gstatus_info->sform->status_form &&
	  gstatus_info->sform->status_form->visible)
	fl_reset_cursor(gstatus_info->sform->status_form->window);

  if (test_form && test_form->test_form && test_form->test_form->visible)
	fl_reset_cursor(test_form->test_form->window);

  XFlush(fl_get_display());
} /* reset_cursor_for_all_forms */

/***************************************************************************/
/*                                                                         */
/*                           update_trans_status                           */
/*                                                                         */
/***************************************************************************/
void update_trans_status()
{
  /* Update communication's status on the main form */

  time_t curr_time;
  char buf[MAX_LABEL_LEN], *label_str = NULL;
  double diff_time;
  static show_end_conn_str = 0;

  if (tape_grouping_form == NULL || 
	  tape_grouping_form->data_trans_status_label == NULL ||
	  tape_grouping_form->data_trans_stat_grp == NULL ||
	  comm_stat_str == NULL) return;

  memset(buf, '\0', MAX_LABEL_LEN);


  if (set_data_transmission_inactive) {
	/*
	label_str = comm_stat_str;
	*/

	switch (data_transmission_status) {
	case END_CONNECTION_WITH_MONITOR:
	case END_CONNECTION_TO_MONITOR:
	case NO_DATA_FROM_MONITOR:
	case GET_CONNECTION_WITH_MONITOR_FAILED:
	case CONNECT_TO_MONITOR_FAILED:
	  if (cpid_died == 1) 
		label_str = PROCESS_COMM_WITH_MONITOR_DIE_STR;
	  else
		label_str = IDLE_STR; /* Only done when connection is ended. */
	  break;
	default:
	  break;
	}
	set_data_transmission_inactive = 0;

  }
  else if (data_transmission_status == GET_CONNECTION_WITH_MONITOR ||
	  data_transmission_status == READ_FROM_MONITOR ||
	  data_transmission_status == DONE_READING_FROM_MONITOR ||
	  data_transmission_status == CONNECTED_WITH_MONITOR ||
	  data_transmission_status == CONNECT_TO_MONITOR ||
	  data_transmission_status == WRITE_TO_MONITOR ||
	  data_transmission_status == WROTE_TO_MONITOR ||
	  data_transmission_status ==CONNECTED_TO_MONITOR) {
	/* Status that continues on */
	curr_time = get_curr_time();
	diff_time = difftime(curr_time, trans_start_time);
	if (diff_time > 0)
	  sprintf(buf, "%s(%.0f secs)", comm_stat_str, diff_time);
	else
	  sprintf(buf, "%s( < 1 sec)", comm_stat_str);
	label_str = buf;

	switch (data_transmission_status) {
	case CONNECTED_TO_MONITOR:
	case CONNECTED_WITH_MONITOR:
	case DONE_READING_FROM_MONITOR:
	case WROTE_TO_MONITOR:
	case CONNECT_TO_MONITOR:
	case GET_CONNECTION_WITH_MONITOR:
	  show_end_conn_str = 1;
	  break;
	default:
	  break;
	}
  }
  else {
	/* Statuses that are not continuing--need to show only once. */
	set_data_transmission_inactive = 1;

	switch (data_transmission_status) {
	case END_CONNECTION_WITH_MONITOR:
	case END_CONNECTION_TO_MONITOR:
	  if (show_end_conn_str) {
		label_str = comm_stat_str;
		show_end_conn_str = 0;
	  }
	  else {
		/* Don't want to show these msg twice in a row */

		label_str = NULL;
		/*
		label_str = comm_stat_str;
		*/
	  }
	  break;

	case NO_DATA_FROM_MONITOR:
	case GET_CONNECTION_WITH_MONITOR_FAILED:
	case CONNECT_TO_MONITOR_FAILED:
	  label_str = comm_stat_str;
	  show_end_conn_str = 0;
	  break;

	default:
	  break;
	}
  }

  /*
  fprintf(stderr, "buf: <%s>, time: %d\n", label_str, time(NULL));
  */
  
  /* Show status msg on form*/
  if (label_str != NULL)
	fl_set_object_label(tape_grouping_form->data_trans_status_label, label_str);

} /* update_trans_status */

/***************************************************************************/
/*                                                                         */
/*                      update_forms                                       */
/*                                                                         */
/***************************************************************************/
int update_forms(XEvent *e, void *data)
{
  long oldmask;
  /*
  unsigned int sec_left, sec;
  */
  time_t curr_time;
  static time_t last_update_runtime_time=0;
  static time_t last_update_trans_stat_time=0;
  static time_t last_update_new_data_time=0;
  extern time_t last_set_alarm_time;


  /* Must block signals */
  gms_block_ints();
  /*
printf("enter update forms\n");
*/

  /* close info_form if mouse moved out of gms_save_button's boundary */
  do_pointermotion(gms_save_button);

  if (show_main_form && tape_grouping_form) {
	show_form(tape_grouping_form->ldata);
	show_main_form = 0;
	/* Hide busy form */
	if (busy_form) fl_hide_form(busy_form->busy_form);
	/* Set new alarm clock */
	set_alarm_to_update_forms();
  }

  if (need_show_new_data) {
	curr_time = get_curr_time();
	if ((curr_time - last_update_new_data_time) < UPDATE_FORMS_INTERVAL)
	  goto NEXT;

	last_update_new_data_time = curr_time;
	need_show_new_data = 0; /* reset flag */

	if (update_sys_stat_form) {
	  add_msg_to_sys_stat_browser(gms_sys_stat_msg);
	  memset(gms_sys_stat_msg, '\0', MAX_SYS_STATUS_MSG_LEN);
	  update_sys_stat_form = 0;
	}

	if (logdata_changed) {
	  /* info is added, so no need to clear & reload data to form--
	   * just update */
	  update_forms_affected_by_logfile_change(0);
      logdata_changed = 0;
    }
	if (ps_info_changed) {
	  update_ps_form();  
	  /* clear flags */
      ps_info_changed = 0;
    }
	if (status_info_changed) {
	  /* update forms--new status(es) have arrived. */
	  update_forms_affected_by_status_info_change(load_status_forms);

	  /* clear flags */
	  load_status_forms = 0;
      status_info_changed = 0;
	  stat_info_count = 0;
/*
printf("status info changed ####################@ %ld\n", (long) time(NULL));
*/
    }
	if (neighbor_list_changed) {
	  /* update other affected forms */
	  update_forms_affected_by_neighbor_change(AVAILABLE_HOSTS, 0);
	  /* clear flags */
      neighbor_list_changed = 0;
    }
	if (refresh_status_form) {
	  /* reload data to forms affected by delete */
	  reload_data_to_status_form();
	  update_forms_affected_by_status_info_change(load_status_forms);
	  /* clear flags */
	  refresh_status_form = 0;
	}
	/* Set new alarm clock */
	set_alarm_to_update_forms();
  } /* need show new data */

NEXT:

  if (gms_update_forms) {
	gms_update_forms = 0; /* REset flag */

	/* Refresh data required regular update */
	if (update_data_transmission_status) {
	  curr_time = get_curr_time();
	  /*
	  if ((curr_time - last_update_trans_stat_time) >= 
		  UPDATE_TRANS_STATUS_INTERVAL) {
		  */
	  if (1) {
		if (set_data_transmission_inactive) 
		  update_data_transmission_status = 0; /* Reset update flag */

		update_trans_status();
	  /*
 fprintf(stderr, "update trans stat:  time: %ld\n", time(NULL));
 */
#if 0
		if (update_data_transmission_status) {

		  /* Set alarm to update transmission time */
		  sec_left = alarm(0);
		  if (sec_left <= 0) 
			sec_left = get_new_alarm_time();
		  alarm(sec_left);  /* reset alarm with the remaining sec.*/
		}
#endif
		last_update_trans_stat_time = curr_time;
	  }
	  else gms_update_forms = 1;  /* Set it back */
	}

	/* Blink buttons and reset alarm for the next blink */
	if (blink_boxes_flag) {/* Blinking will waste a lot of CPU time */
	  curr_time = get_curr_time();
	  if ((curr_time - last_update_runtime_time) >= UPDATE_BLINK_INTERVAL) {
		blink_stat_light_buttons();	
#if 0
		sec_left = alarm(0);
		if (sec_left <= 0) 
		  sec_left = get_new_alarm_time();
		alarm(sec_left);  /* reset alarm with the remaining sec.*/
	  
#endif
	  }
	}
	/* Update these forms here -- updates runtime after UPDATE_RUNTIME_INTERVAL
	 * seconds.
	 */
	
	if (something_is_running) {
	  curr_time = get_curr_time();
	  if ((curr_time - last_update_runtime_time) >= UPDATE_RUNTIME_INTERVAL) {
		update_tape_grouping_form();
		update_prod_group_form(1);
		update_exceeded_threshold_form(NULL, 0);
		update_summary_report_form();
		last_update_runtime_time = curr_time;
	  }
	  else gms_update_forms = 1;  /* Set it back */
	}
	/* Set new alarm clock */
	set_alarm_to_update_forms();
  } /* gms update forms */


  /* Send sigio to itself to get the last message from child process. */
  if (need_to_send_sigio) {
	need_to_send_sigio = 0;
	kill(getpid(), SIGIO);
  }


/*
printf("exit update forms\n");
*/
  gms_unblock_ints(oldmask);

  return 0;
} /* update_forms */

void blink_stat_light_buttons()
{

  int i = 0;
  int failed_curr_stat, warning_curr_stat, running_curr_stat, 
	unknown_curr_stat;
  time_t last_time;

  /* blink light buttons on tape_grouping_form */
  if (!(tape_grouping_form == NULL || tape_grouping_form->group_form == NULL ||
		!tape_grouping_form->group_form->visible ||
		tape_grouping_form->failure_light_button == NULL ||
		tape_grouping_form->warning_light_button == NULL ||
		tape_grouping_form->running_light_button == NULL ||
		tape_grouping_form->unknown_light_button == NULL)){

	fl_redraw_object(tape_grouping_form->signal_grp);
	if (all_grps_stat & FAILED) {
	  failed_curr_stat = fl_get_button(tape_grouping_form->failure_light_button);
	  fl_set_button(tape_grouping_form->failure_light_button, !failed_curr_stat);

	}
	if (all_grps_stat & WARNING) {
	  warning_curr_stat = fl_get_button(tape_grouping_form->warning_light_button);
	  fl_set_button(tape_grouping_form->warning_light_button, !warning_curr_stat);
	}
	if (all_grps_stat & RUNNING) {
	  running_curr_stat = fl_get_button(tape_grouping_form->running_light_button);
	  fl_set_button(tape_grouping_form->running_light_button, !running_curr_stat);
	}

	if (all_grps_stat & UNKNOWN) {
	  unknown_curr_stat = fl_get_button(tape_grouping_form->unknown_light_button);
	  fl_set_button(tape_grouping_form->unknown_light_button, !unknown_curr_stat);
	}

	fl_redraw_object(tape_grouping_form->signal_grp);
	XFlush(fl_get_display());

	last_time = get_curr_time();
	/* make it wait a little bit (1 second) */
	for (i=0; i < 1000000;i++) {
	  if ((get_curr_time() - last_time) >= 1) break;
	  
	}
	/*
fprintf(stderr, "lasttime: %ld, time: %ld\n", last_time, time(NULL));
*/

	fl_redraw_object(tape_grouping_form->signal_grp);	
	if (all_grps_stat & FAILED) {
	  fl_set_button(tape_grouping_form->failure_light_button, failed_curr_stat);
	}
	if (all_grps_stat & WARNING) {
	  fl_set_button(tape_grouping_form->warning_light_button, warning_curr_stat);
	}
	if (all_grps_stat & RUNNING) {
	  fl_set_button(tape_grouping_form->running_light_button, running_curr_stat);
	}
	if (all_grps_stat & UNKNOWN) {
	  fl_set_button(tape_grouping_form->unknown_light_button, unknown_curr_stat);
	}
	fl_redraw_object(tape_grouping_form->signal_grp);
  }
} /* blink_stat_light_buttons */

/***************************************************************************/
/*                                                                         */
/*                      process_local_curr_time(                           */
/*                                                                         */
/***************************************************************************/
void process_local_curr_time(char *buf)
{
  char *time_str;

  if (buf == NULL || strstr(buf, LOCAL_CURR_TIME_KEY) == NULL) return;

  /* Skip key word */

  time_str = buf + strlen(LOCAL_CURR_TIME_KEY);
  time_str = remove_leading_blanks(time_str);
  if (time_str)
	local_curr_time = (time_t) atoi(time_str);
  
} /* process_local_curr_time */

/***************************************************************************/
/*                                                                         */
/*                    show_comm_stat                             */
/*                                                                         */
/***************************************************************************/
void show_comm_stat(char *stat_str)
{
  if (stat_str == NULL ||
	  tape_grouping_form == NULL || 
	  tape_grouping_form->data_trans_status_label == NULL ||
	  tape_grouping_form->data_trans_stat_grp == NULL)
	return;

  /* Show status msg on form here instead of waiting when the system is idle
   */

  fl_set_object_label(tape_grouping_form->data_trans_status_label, 
					  stat_str);
}
/***************************************************************************/
/*                                                                         */
/*                    update_busy_form                            */
/*                                                                         */
/***************************************************************************/
void update_busy_form(comm_status_type_t trans_status)
{
  if (busy_form != NULL && busy_form->busy_form != NULL && 
	  busy_form->busy_form->visible &&
	  busy_form->status_label) {
	if (trans_status == READ_FROM_MONITOR)
	  fl_set_object_label(busy_form->status_label,READ_FROM_MONITOR_STR );
	else  if (trans_status == GET_CONNECTION_WITH_MONITOR)
	  fl_set_object_label(busy_form->status_label, GET_CONNECTION_WITH_MONITOR_STR);
	else if (trans_status == WRITE_TO_MONITOR)
	  fl_set_object_label(busy_form->status_label,WRITE_TO_MONITOR_STR );
	else  if (trans_status == CONNECT_TO_MONITOR)
	  fl_set_object_label(busy_form->status_label, CONNECT_TO_MONITOR_STR);
	else
	  fl_set_object_label(busy_form->status_label, "Initializing...");
	fl_redraw_object(busy_form->status_label);
  }
}

/***************************************************************************/
/*                                                                         */
/*                     process_comm_status_msg                             */
/*                                                                         */
/***************************************************************************/
void process_comm_status_msg(char* msg, comm_status_type_t stat_type,
							 int msg_from_child)
{
  /* Set up so the comm status will be display on the main form.
   * It will display the status info right now if from_child == 0;
   * it will display the status info when the alarm goes off, otherwise.
   * In all cases, it will display the status on the busy form now.
   */
  /* stat_type is only used if msg_from_child == 0 */
  int rc;
  long old_mask;
  int stat;

  if (msg_from_child == 1) {
  if (msg == NULL) return;
	/* msg was constructed by child process */
	rc = sscanf(msg, "%*s %d",&stat);
	if (rc != 1) {
	  fprintf(stderr, "Data transmission message is obsolete.\n");
	  return;
	}
	data_transmission_status = stat;
  }
  else 
	/* msg was constructed by parent process */
	data_transmission_status = stat_type;
  /*
fprintf(stderr, "msg: <%s>, stattype:: %d\n", (char *)msg, stat_type);
*/

  if (msg_from_child == 1) {
	/* If msg received from child, set alarm so this form will be updated
	 * when the alarm goes off. 
	 */
	update_data_transmission_status = 1;

	/* Set new alarm clock -- this has higher priority. Calls this after
	 * update_data_transmission_status is set.
	 */
  
	set_alarm_to_update_forms();
  }

  switch (data_transmission_status) {
  case GET_CONNECTION_WITH_MONITOR:
	comm_stat_str = GET_CONNECTION_WITH_MONITOR_STR;
	break;
  case NO_DATA_FROM_MONITOR:
	comm_stat_str = NO_DATA_FROM_MONITOR_STR;
	break;
  case GET_CONNECTION_WITH_MONITOR_FAILED:
	comm_stat_str = GET_CONNECTION_WITH_MONITOR_FAILED_STR;
	break;
  case READ_FROM_MONITOR:
	comm_stat_str = READ_FROM_MONITOR_STR;
	break;
  case DONE_READING_FROM_MONITOR:
	comm_stat_str = DONE_READING_FROM_MONITOR_STR;
	break;
  case END_CONNECTION_WITH_MONITOR:
	comm_stat_str = END_CONNECTION_WITH_MONITOR_STR;
	break;
  case CONNECTED_WITH_MONITOR:
	comm_stat_str = CONNECTED_WITH_MONITOR_STR;
	break;
  case  CONNECT_TO_MONITOR:
	comm_stat_str =  CONNECT_TO_MONITOR_STR;
	break;
  case WROTE_TO_MONITOR:
	comm_stat_str =  WROTE_TO_MONITOR_STR;
	break;
  case WRITE_TO_MONITOR:
	comm_stat_str =  WRITE_TO_MONITOR_STR;
	break;
  case CONNECT_TO_MONITOR_FAILED:
	comm_stat_str = CONNECT_TO_MONITOR_FAILED_STR;
	break;
  case END_CONNECTION_TO_MONITOR:
	comm_stat_str = END_CONNECTION_TO_MONITOR_STR;
	set_data_transmission_inactive = 1;
	break;
  case CONNECTED_TO_MONITOR:
	comm_stat_str = CONNECTED_TO_MONITOR_STR;
	break;
  default: 
	break;
  }
	/*
fprintf(stderr, "com: %s\n", comm_stat_str);
*/
  if (data_transmission_status == READ_FROM_MONITOR ||
	  data_transmission_status == GET_CONNECTION_WITH_MONITOR ||
	  data_transmission_status == CONNECT_TO_MONITOR ||
	  data_transmission_status == WRITE_TO_MONITOR) {
	/* Status that continues */
	memset(&trans_start_time, '\0', sizeof(time_t));
	trans_start_time = get_curr_time();
  }
/*
	else trans_start_time = (time_t) 0;
	*/

  set_data_transmission_inactive = 0;


  old_mask = gms_block_ints();
  /* Update forms now rather than wait for the alarm to go off. */
  update_busy_form(data_transmission_status);

  if (msg_from_child == 0) {
	if (comm_stat_str != NULL) {
	  show_comm_stat(comm_stat_str);

	  /*
		fprintf(stderr, "comm: %s\n", comm_stat_str);
		*/
	  if (data_transmission_status == END_CONNECTION_TO_MONITOR) {
		update_data_transmission_status = 1;
		
		/* Set alarm clock -- this has higher priority. Calls this after
		 * update_data_transmission_status is set.
		 * This is required to update the status msg to IDLE_STR.
		 */
  
		set_alarm_to_update_forms();
	  }
	}
  }

	/*
fprintf(stderr, "received data trans: %d. time: %ld\n", data_transmission_status, time(NULL));
*/
  gms_unblock_ints(old_mask);
} /* process_comm_status_msg */


/***************************************************************************/
/*                                                                         */
/*                      process_incoming_data                              */
/*                                                                         */
/***************************************************************************/
void process_incoming_data(data_type_t dtype, char *fromhost, char *buf)
{
  /* buf contains message's key followed with the actual data. */
#if 0
  char prod[MAX_PRODNAME_LEN];
  char tapeid[MAX_TAPEID_LEN];
#endif
  struct ps_info_rec sp_ps_info;
  struct ps_info_rec ap_ps_info;
  extern time_t last_set_alarm_time;

#if 0
  int buf_len, change_cursor_len = 500;
#endif
  char msg[MAX_BROWSER_LINE_LEN];
  /*
  unsigned int sec_left;
  unsigned int sec;

  time_t curr_time;
  */

/*
printf("******** update ps info time = %ld \n", (long) time(NULL));
*/
  if (buf == NULL || strlen(buf) <= 0 || fromhost == NULL || 
	  strlen(fromhost) == 0) return;

  need_show_new_data = 1;
#if 0
  /* Set alarm clock to update forms */
  curr_time = get_curr_time();
  sec = get_new_alarm_time();
  if ((curr_time - last_set_alarm_time) >= sec) {
	alarm(sec);  /* reset alarm */
	last_set_alarm_time = curr_time;
  }
#endif
	
#if 0
{
char tmp_buf[30];
memset(tmp_buf, '\0', 30);
strncpy(tmp_buf, buf, 29);
/*
fprintf(stderr, "fromhost = %s, len - %d\n", fromhost, strlen(buf));
*/

fprintf(stderr, "parent host: %s, tmpbuf = <%s>, time: %ld\n", fromhost, tmp_buf, (long) get_curr_time());
}
#endif

  /* add host to host list if not done so 
   */

  if (add_new_host(available_hosts, fromhost) > 0) {
	neighbor_list_changed = 1;
  }

  switch (dtype) {
  case DATA_TRANS_FLAG:
	/*
fprintf(stderr, "buf: <%s>\n", buf);
*/
	process_comm_status_msg(buf, NO_TRANS, 1);
	break;
  case HOST_CURR_TIME:
	process_host_curr_time(fromhost, buf);
	break;
  case LOCAL_CURR_TIME:
	process_local_curr_time(buf);
	break;
  case PS_INFO:  /* current process status */
	memset(msg, '\0', MAX_BROWSER_LINE_LEN);
	sprintf(msg, "Received PS data from %s", fromhost);
	append_msg_to_sys_stat_msg(msg);

	if (!ps_form || !ps_form->ps_info_form || !ps_form->ps_info_form->visible)
	  break;
/*
	printf("buf = <%s>\n", buf);
*/
	init_ps_info_rec(&sp_ps_info, MAX_PS_BUF_LEN); /* selected progs list */
	init_ps_info_rec(&ap_ps_info, MAX_PS_BUF_LEN); /* all progs list */

	process_ps_info(fromhost, buf, &sp_ps_info, &ap_ps_info);

/*
printf("sp:topbuf: <%s>\n", ap_ps_info->all_levels);
*/
	ps_info_changed = 1;
	free_ps_bufs(&sp_ps_info);
	free_ps_bufs(&ap_ps_info);

	break;
#if 0
  case LOG_FILE:  /* history status: Read from a logfile. */
#if 0
	buf_len = strlen(buf);
	/* change to busy cursor */
	if (buf_len > change_cursor_len)
	  set_cursor_for_all_forms(XC_watch);
#endif
	memset(msg, '\0', MAX_BROWSER_LINE_LEN);
	sprintf(msg, "Received LOG data from %s", fromhost);
	append_msg_to_sys_stat_msg(msg);

	memset(tapeid, '\0', MAX_TAPEID_LEN);
	memset(prod, '\0', MAX_PRODNAME_LEN);
	process_log_file(fromhost, buf, tapeid, prod);
	logdata_changed = 1;
#if 0
	/* change to normal cursor */
	if (buf_len > change_cursor_len)
	  reset_cursor_for_all_forms();
#endif
	break;
#endif
  case STATUS_INFO: /* current status */
/*
printf("buf = <%s>\n", buf);
*/
#if 0
	buf_len = strlen(buf);
	/* change to busy cursor */
	if (buf_len > change_cursor_len)
	  set_cursor_for_all_forms(XC_watch);
#endif
	memset(msg, '\0', MAX_BROWSER_LINE_LEN);
	sprintf(msg, "Received STATUS data from %s", fromhost);
	append_msg_to_sys_stat_msg(msg);
/*
printf("before process stat info\n");
*/
	if (process_status_info(fromhost, buf) == 1) {
/*
printf("after process stat info\n");
*/

	/* Set status_info_update flag here but will 
	 # set gms_update_flag when the alarm goes off.  This eliminate flicker
	 * dues to too much update.
	 */
	status_info_changed = 1;
	}

#if 0
	/* change to normal cursor */
	if (buf_len > change_cursor_len)
	  reset_cursor_for_all_forms();
#endif
	break;

  case DEL_JOB:
	memset(msg, '\0', MAX_BROWSER_LINE_LEN);
	sprintf(msg, "Received Del job msg from %s", fromhost);
	append_msg_to_sys_stat_msg(msg);
	if (process_del_job_msg(buf)){
	  refresh_status_form = 1;
	  load_status_forms = 1;
	}
	break;

  default:
	need_show_new_data = 0;
	break;
  }

	
/*
printf("exit process incoming data\n");
*/
} /* process_incoming_data */

/***************************************************************************/
/*                                                                         */
/*                      data_type_to_str                                   */
/*                                                                         */
/***************************************************************************/
char *data_type_to_str(data_type_t dtype)
{
  switch (dtype) {
  case STATUS_INFO: return STATUS_INFO_STR;
  case PS_INFO: return PS_INFO_STR;
  case LOG_FILE: return LOG_FILE_STR;
  case ADD_GUI: return ADD_GUI_STR;
  case DEL_GUI: return DEL_GUI_STR;
  case ADD_NEIGHBOR: return ADD_NEIGHBOR_STR;
  case DEL_NEIGHBOR: return DEL_NEIGHBOR_STR;
  case GET_LOG_FILES: return GET_LOG_FILES_STR;
  default: return UNKNOWN_STR;
  }
} /* data_type_to_str */

/***************************************************************************/
/*                                                                         */
/*                        append_msg_to_sys_stat_msg                       */
/*                                                                         */
/***************************************************************************/
void append_msg_to_sys_stat_msg(char *msg)
{
  char buf[MAX_BROWSER_LINE_LEN];
  time_t curr_time;
  char *time_str;

  if (gms_sys_stat_msg == NULL || msg == NULL || strlen(msg) == 0) return;

  update_sys_stat_form = 1;
  curr_time = get_curr_time();
  /* time status from to datatype data_len */
/*
  sprintf(buf, "%s %9ld  %-70.70s", TEXT_FORMAT_STR, curr_time, msg);
*/
  time_str = ctime(&curr_time);
  /* skip day and remove trailing year from time_str */
  time_str += 4;
  *(time_str+16) = '\0';
  memset(buf, '\0', MAX_BROWSER_LINE_LEN);
  sprintf(buf, "%s %-16.16s  %-70.70s\n", TEXT_FORMAT_STR, time_str, msg);
  buf[MAX_BROWSER_LINE_LEN-1] = '\0';
  if ((strlen(gms_sys_stat_msg) + strlen(buf)) >= MAX_SYS_STATUS_MSG_LEN) {
	/* buffer limit exceeds, reinitialize buffer */
	memset(gms_sys_stat_msg, '\0', MAX_SYS_STATUS_MSG_LEN);
  }
  strcat(gms_sys_stat_msg, buf);


}
/***************************************************************************/
/*                                                                         */
/*                         add_msg_to_sys_stat_browser                     */
/*                                                                         */
/***************************************************************************/
void add_msg_to_sys_stat_browser(char * msg)
{


  if (system_status_form == NULL ||
	  system_status_form->msg_form == NULL ||
	  !system_status_form->msg_form->visible || msg == NULL) 
	return;

  fl_deactivate_object(system_status_form->browser);
  fl_freeze_form(system_status_form->msg_form);


  /* clear browser if max msg has reach */
  if (fl_get_browser_maxline(system_status_form->browser) > 
	  MAX_SYS_STATUS_MSGS)
	fl_clear_browser(system_status_form->browser);


  fl_addto_browser(system_status_form->browser, msg);
  fl_unfreeze_form(system_status_form->msg_form);
  fl_activate_object(system_status_form->browser);

} /* add_msg_to_sys_stat_browser */

/***************************************************************************/
/*                                                                         */
/*                      new_chart_rec                                      */
/*                                                                         */
/***************************************************************************/
struct chart_rec *new_chart_rec (int lindex, FL_OBJECT *chart, 
				FL_OBJECT *llabel, FL_OBJECT *blabel, FL_OBJECT *log_num_label)
{
  struct chart_rec *chart_info;

  if (chart == NULL || llabel == NULL || blabel == NULL || 
	  log_num_label == NULL)
	return NULL;

  chart_info = (struct chart_rec *) calloc(1, sizeof(struct chart_rec));
  if (chart_info == NULL) {
	perror ("calloc chart rec ");
	exit_system(NULL, -1);
  }
  chart_info->logfile = NULL;
  chart_info->chart = chart;
  chart_info->llabel_obj = llabel;
  chart_info->blabel_obj = blabel;
  chart_info->log_num_obj = log_num_label;

  fl_set_chart_maxnumb(chart, MAX_CHART_ITEMS);
  fl_set_chart_bounds(chart, 0, 0);
/*
  fl_set_chart_bounds(chart, 1, CHART_HEIGHT);
*/
/*
  fl_set_chart_autosize(chart, 0);*/  /* set to not autosize */


  clear_chart(chart_info);
  return chart_info;
}

/***************************************************************************/
/*                                                                         */
/*                             add_new_form_to_list                        */
/*                                                                         */
/***************************************************************************/
void add_new_form_to_list(char *form_name, FL_FORM *form)
{
  if (form_name == NULL || form == NULL) return;

    strcpy(gms_form_list[form_count].name, form_name);
	gms_form_list[form_count].form = form;
	gms_form_list[form_count].min_height = form->h;
	gms_form_list[form_count].min_width = form->w;

	form_count++;
} /* add_new_form_to_list */

/***************************************************************************/
/*                                                                         */
/*                      new_gstatus_rec                                    */
/*                                                                         */
/***************************************************************************/
struct graphic_status_rec * new_gstatus_rec()
{

  struct graphic_status_rec *gstat;
  int i;
  FD_status_form *stat_form;
  long val = 1;


  gstat = (struct graphic_status_rec *) calloc(1, sizeof(struct graphic_status_rec));
  if (gstat == NULL) {
	perror("calloc gstat");
	exit_system(NULL, -1);
  }

  /* init. default values */
  gstat->logfiles = NULL;
  gstat->nlogfiles = 0;
  gstat->scroll_per = 0.0;
  gstat->refresh_form = 0;
  gstat->bar_sorted_logfiles_array = NULL;
  gstat->gms_sorted_logfiles_array = NULL;
  gstat->nbar_array_items = 0;
  gstat->ngms_array_items = 0;

  /* create status form */
  stat_form = (FD_status_form *) create_form_status_form();
  if (stat_form == NULL || stat_form->status_form == NULL ||
	  stat_form->slider == NULL ||
	  stat_form->label == NULL || stat_form->chart1 == NULL ||
	  stat_form->chart2 == NULL ||stat_form->chart3 == NULL ||
	  stat_form->chart4 == NULL || stat_form->chart5 == NULL ||
	  stat_form->label1 == NULL ||stat_form->label2 == NULL ||
	  stat_form->label3 == NULL || stat_form->label4 == NULL ||
	  stat_form->label5 == NULL || stat_form->label11 == NULL ||
	  stat_form->label22 == NULL || stat_form->label33 == NULL ||
	  stat_form->label44 == NULL || stat_form->label55 == NULL ||
	  stat_form->bar1 == NULL || stat_form->bar2 == NULL ||
	  stat_form->bar3 == NULL || stat_form->bar4 == NULL ||
	  stat_form->bar5 == NULL || stat_form->action_menu == NULL ||
	  stat_form->help_menu == NULL) 
	return NULL;
  stat_form->ldata = form_count; /* index to form list */
  add_new_form_to_list(BARS_FORM_NAME, stat_form->status_form);


  /* menu related */
  /* Note: if change menu items below, change them in 
   *     do_action_menu()
   *     do_view_menu()
   *     do_help_menu()
   */

  fl_set_menu(stat_form->action_menu, "Close");
  fl_set_menu(stat_form->view_menu, "Sort by end time|Sort by host, tapeid, and program|----------------------------|Active groups only|Inactive groups only|----------------------------|Groups with something is failed|Groups with something is warning|Groups with something is running|Groups with everything was successful|Groups with something is unknown|All|----------------------------|One tape grouping");

  fl_set_menu_item_mode(stat_form->view_menu, VIEW_SEPARATOR1_MINDEX, FL_PUP_GRAY);
  fl_set_menu_item_mode(stat_form->view_menu, VIEW_SEPARATOR2_MINDEX, FL_PUP_GRAY);
  fl_set_menu_item_mode(stat_form->view_menu, VIEW_SEPARATOR3_MINDEX, FL_PUP_GRAY);
  set_view_menu_options(stat_form->view_menu, &(forms_view_options[STATUSF_VIEW_OPTION_IND]), STATUS_FORM_ID);

  stat_form->view_menu->u_vdata = (void *) stat_form;

  fl_set_menu(stat_form->help_menu, "About gms|Contexts");

  /* setup so can use generic do_close_form() */
  stat_form->action_menu->u_vdata = stat_form->status_form;

  stat_form->status_form->u_vdata = (void *) tape_grouping_form->bar_button;
  /*
  fl_set_slider_step(stat_form->slider, SLIDER_STEP);
  */

  fl_set_slider_return(stat_form->slider, FL_OFF);
  fl_set_slider_value(stat_form->slider, 0.0);
  fl_set_form_atclose(stat_form->status_form, form_atclose, (void *)&val);
  fl_add_io_callback(stat_form->status_form->window, 0, do_nothing_io_handler, "");
  gstat->sform = stat_form;

  /* initialize chart list */
  i = 0;
  gstat->chart_list[i++] = new_chart_rec(-1, stat_form->chart1, stat_form->label1, stat_form->label11, stat_form->bar1);
  gstat->chart_list[i++] = new_chart_rec(-1, stat_form->chart2, stat_form->label2, stat_form->label22, stat_form->bar2);
  gstat->chart_list[i++] = new_chart_rec(-1, stat_form->chart3, stat_form->label3, stat_form->label33, stat_form->bar3);
  gstat->chart_list[i++] = new_chart_rec(-1, stat_form->chart4, stat_form->label4, stat_form->label44, stat_form->bar4);
  gstat->chart_list[i++] = new_chart_rec(-1, stat_form->chart5, stat_form->label5,stat_form->label55, stat_form->bar5);

  return gstat;
}

/***************************************************************************/
/*                                                                         */
/*                      read_gms_init_file                                 */
/*                                                                         */
/***************************************************************************/
void read_gms_init_file()
{
  /* reads prog(s) and neighbor(s) from .gms file 
   * File format: 
   *    # Monitored Programs
   *    prog1
   *    prog2
   *    ...
   *    progN
   *    # Monitored Neighbors
   *    host1
   *    host2
   *     ...
   *    hostM
   *    # Options
   *    show_button_info: 0/1
   *    show_toolbar:     0/1
   *    blink_boxes:      0/1
   *    # GV Level Information (level# prog prog...)
   *    level# prog1 prog2 ... progN
   *    ...
   *    
   *  where:
   *      0 = off (no) ; 1 = on (yes).
   *      level# = 1-3
   */

  char fname[MAX_FILENAME_LEN];
  FILE *fp;
  char line[MAX_STR_LEN];
  int type = 0;  /* 0: program list; 1: neighbor list; 2: options */
  int val;
  char *tok, *prog;
  int p,ll, level_num;
  
  sprintf(fname,"%s/%s",getenv("HOME"), GMS_INIT_FILE);
  if ((fp = fopen(fname, "r")) == NULL) {
	sprintf(line, "Unable to read program(s) from %s. ",fname);
	fl_show_message("WARNING",line, "Select 'Configure Programs' to add program(s) then 'Refresh Data'to receive data.");
	return;
  }

  memset(line, '\0', MAX_STR_LEN);
  while (fgets(line, MAX_STR_LEN, fp) != NULL) {
  	if (strlen(line) == 0) continue;

	/* init file contains program name on each line */
	/* remove '\n' */
	line[strlen(line)-1]  = '\0';
/*
	printf("line = <%s>\n", line);
*/
	if (strcmp(SELECTED_PROGS_COMMENT_LINE, line) == 0) 
	  type = 0;
	else if (strcmp(SELECTED_HOSTS_COMMENT_LINE, line) == 0) 
	  type = 1;
	else if (strcmp(OPTIONS_COMMENT_LINE, line) == 0) 
	  type = 2;
	else if (strcmp(LEVEL_INFO_COMMENT_LINE, line) == 0) {
	  type = 3;
	  clear_level_info_table(level_info_table);
	}
	else {
	  /* read data */
	  switch (type) {
	  case 0: /* programs  list */
		add_new_prog(Gselected_progs, line);
		break;
	  case 1: /* host list */
		add_new_host(Gselected_hosts, line);
		break;
	  case 2: /* Option list */
		sscanf(line, "%*s %d", &val);
		if (val != 0 && val != 1)  {
		  fl_show_message("WARNING:", fname, " file is obsolete.");
		  break;
		}

		if (strstr(line, SHOW_TOOLBAR_STR) != NULL) {
		  show_toolbar_flag = val; 
		}
		else if (strstr(line, SHOW_BUTTON_INFO_STR) != NULL) {
		  show_button_info_flag = val;
		}
		else if (strstr(line, BLINK_BOXES_STR) != NULL) {
		  blink_boxes_flag = val;
		}
		break;

	  case 3: /* GV level info */
		/* Line format: level# prog1 prog2...progN, where level# > 0.
		 *   i.e., 1 level_1
		 *         2 2A-53 2A-54
		 */
		tok = strtok(line, " ");
		level_num = atoi(tok);
		if (level_num < 1) 
		  break; /* Skip */
		prog = strtok(NULL, " ");
		p = 0;
		ll = level_num - 1;
		while (prog != NULL) {
		  /* level info table is indexed with level_num-1. */
		  /* Add prog to that table */
		  if (prog[strlen(prog)-1] == '\n')
			prog[strlen(prog)-1] = '\0';
		  level_info_table[ll].prog_info[p].prog = (char *) strdup(prog);
		  level_info_table[ll].prog_info[p].stat = 0;
		  p++;
		  level_info_table[ll].nprogs = p;
		  prog = strtok(NULL, " ");
		  if (prog == NULL)
			prog = strtok(NULL, "\n"); /* The last prog on the line. */
		}
		break;
		  
	  }	/* switch */	  
	} /* else  data */
	memset(line, '\0', MAX_STR_LEN);
  }

  fclose(fp);
} /* read_gms_init_file */



/***************************************************************************/
/*                                                                         */
/*                      update_gms_init_file                               */
/*                                                                         */
/***************************************************************************/
void update_gms_init_file()
{
  /* reads selected prog(s) and neighbor(s) from .gms.
   * File format: 
   *    # Monitored Programs
   *    prog1
   *    prog2
   *    ...
   *    progN
   *    # Monitored Neighbors
   *    host1
   *    host2
   *     ...
   *    hostM
   *    # Options
   *    show_button_info: 0/1
   *    show_toolbar:     0/1
   *    blink_boxes:      0/1
   *    # GV Level Information (level# prog prog...)
   *    level# prog1 prog2 ... progN
   *    ...
   *    
   *  where:
   *      0 = off (no) ; 1 = on (yes).
   *      level# = 1-3
   */
  char fname[MAX_FILENAME_LEN];
  FILE *fp;
  int i, ll, p;

  sprintf(fname,"%s/%s",getenv("HOME"), GMS_INIT_FILE);
  if ((fp = fopen(fname, "w")) == NULL) {
	fl_show_message("WARNING", "Can't save program(s) in <%s>", fname);
	return;
  }
  /* write selected progs */
  fprintf(fp, "%s\n", SELECTED_PROGS_COMMENT_LINE);
  for (i = 0; i < MAX_PROGS; i++) {
	if (Gselected_progs[i] == NULL) continue;
	fprintf(fp, "%s\n", Gselected_progs[i]);
  }

  /* write selected neighbors */
  fprintf(fp, "%s\n", SELECTED_HOSTS_COMMENT_LINE);
  for (i = 0; i < MAX_HOSTS; i++) {
	if (Gselected_hosts[i] == NULL) continue;
	fprintf(fp, "%s\n", Gselected_hosts[i]);
  }

  /* write options */
  fprintf(fp, "%s\n%s %d\n%s %d\n%s %d\n", OPTIONS_COMMENT_LINE, SHOW_BUTTON_INFO_STR,
		  show_button_info_flag,SHOW_TOOLBAR_STR,show_toolbar_flag,
		  BLINK_BOXES_STR, blink_boxes_flag);

  /* Write GV level info */
  fprintf(fp, "%s\n", LEVEL_INFO_COMMENT_LINE);
  for (ll = 0; ll < MAX_GV_LEVELS_NUM; ll++) {
	fprintf(fp, "%d", ll+1);
	for (p = 0; p < MAX_PROGS_PER_LEVEL; p++) {
	  if (level_info_table[ll].prog_info[p].prog == NULL) continue;
	  fprintf(fp, " %s", level_info_table[ll].prog_info[p].prog);
	}
	fprintf(fp, "\n");
  }
  fclose(fp);
} /* update_gms_init_file */


/***************************************************************************/
/*                                                                         */
/*                    read_available_network_hosts_from_file               */
/*                                                                         */
/***************************************************************************/
void read_available_network_hosts_from_file()
{
  FILE *fp;
  int i = 0;
  char line[MAX_LINE_LEN];

  
  if ((fp = fopen(NETWORK_HOSTS_FILENAME, "r")) == NULL) {
	fprintf(stderr, "WARNING: Can't open file <%s> containing network hosts. Ignore.\n", NETWORK_HOSTS_FILENAME);
	return;
  }
  memset(line, '\0', MAX_LINE_LEN);
  i = 0;
  while (fgets(line, MAX_LINE_LEN, fp) != NULL) {
	if (*(line + strlen(line) -1) == '\n') 
	  *(line + strlen(line) -1) = '\0';  /* Remove newline char */
	if (line[0] == '-' && line[1] == '-') goto NEXT;

	if (i >= MAX_HOSTNAME_LEN) {
	  fprintf(stderr, "Error: too many network host. host <%s> is discarded\n", line);
	  break;
	}

	Gavailable_network_hosts[i]= (char *) calloc(1, MAX_HOSTNAME_LEN);
	if (Gavailable_network_hosts[i] == NULL)  {
	  perror("calloc network host");
	  exit_system(NULL, -1); 
	}
	strcpy(Gavailable_network_hosts[i], line);
	i++;
  NEXT:
	memset(line, '\0', MAX_LINE_LEN);
  }
  fclose(fp);
  
} /* read_available_network_hosts_from_file */



/***************************************************************************/
/*                                                                         */
/*                              clear_level_info_table                     */
/*                                                                         */
/***************************************************************************/
void clear_level_info_table(level_info_t *level_info_table)
{
  int ll, p;

  for (ll = 0; ll < MAX_GV_LEVELS_NUM; ll++) {
	for (p = 0; p < MAX_PROGS_PER_LEVEL; p++) {
	  if (level_info_table[ll].prog_info[p].prog) {
		free(level_info_table[ll].prog_info[p].prog);
		level_info_table[ll].prog_info[p].prog = NULL;
	  }
	}
	level_info_table[ll].nprogs = 0;
  }
} /* clear_level_info_table */


/***************************************************************************/
/*                                                                         */
/*                                level_info_table_set_default          */
/*                                                                         */
/***************************************************************************/
void level_info_table_set_default(level_info_t *level_info_table)
{
  /* Set default for level_info_table. Index starts at 0.*/
  int ll, p;

  for (ll = 0; ll < MAX_GV_LEVELS_NUM; ll++) {
	
	switch(ll) {
	case 0: /* Level 1 */
	  p = 0;
	  level_info_table[ll].prog_info[p].prog = (char *) strdup("level_1");
	  level_info_table[ll].prog_info[p].stat = 0;
	  level_info_table[ll].nprogs = p+1;
	  break;
	case 1: /* Level 2 */
	  p = 0;
	  level_info_table[ll].prog_info[p].prog = (char *) strdup("2A-52i");
	  level_info_table[ll].prog_info[p].stat = 0;
	  p++;
	  level_info_table[ll].prog_info[p].prog = (char *) strdup("2A-53");
	  level_info_table[ll].prog_info[p].stat = 0;
	  p++;
	  level_info_table[ll].prog_info[p].prog = (char *) strdup("2A-54");
	  level_info_table[ll].prog_info[p].stat = 0;
	  p++;
	  level_info_table[ll].prog_info[p].prog = (char *) strdup("2A-55");
	  level_info_table[ll].prog_info[p].stat = 0;
	  level_info_table[ll].nprogs = p+1;
	  break;
	case 2: /* Level 3 */
	  p = 0;
	  level_info_table[ll].prog_info[p].prog = (char *) strdup("3A-53");
	  level_info_table[ll].prog_info[p].stat = 0;
	  p++;
	  level_info_table[ll].prog_info[p].prog = (char *) strdup("3A-54");
	  level_info_table[ll].prog_info[p].stat = 0;
	  p++;
	  level_info_table[ll].prog_info[p].prog = (char *) strdup("3A-55");
	  level_info_table[ll].prog_info[p].stat = 0;
	  level_info_table[ll].nprogs = p+1;
	  break;
	default:
	  level_info_table[ll].nprogs = 0;
	  break;
	}
  }
} /* level_info_table_set_default */

/***************************************************************************/
/*                                                                         */
/*                     initialize_main                                     */
/*                                                                         */
/***************************************************************************/
void initialize_main()
{
  int i;
  char *s;

  gms_ppid = getpid();
  s = getenv((const char *) "GMS_DEBUG");
  if (s != NULL) {
	sscanf(s, "%d", &write_to_file);
  }
  else 
	write_to_file = 0;

  if (write_to_file) {
	sprintf(test_fname, "gms.out");
	if ((test_file = fopen(test_fname, "w+")) == NULL) {
	  fprintf(stderr, "failed to open %s for writing\n", test_fname);
	  exit(-1);
	}
  }
  host_info_list = NULL;
  tape_info_list = NULL;
  time_ordered_logfile_list = NULL;

  for (i = 0; i < MAX_HOSTS; i++) {
	Gselected_hosts[i] = NULL;
	available_hosts[i] = NULL;
	Gavailable_network_hosts[i] = NULL;
  }

  for (i = 0; i < MAX_TAPES; i++)  
	memset (tapeids[i], '\0', MAX_TAPEID_LEN);
  
  for (i = 0; i < MAX_PROGS; i++) {
	Gselected_progs[i] = NULL;
	available_progs[i] = NULL;
  }
  s = getenv((const char *)"HOST");
  if (s == NULL) {
	fprintf(stderr, "HOST, environment variable, is not set. Set it to your machine name, i.e., raindrop3\n");
	exit(-1);
  }
  strncpy(local_host, s, MAX_HOSTNAME_LEN-1);
  s = getenv((const char *)"HOSTNAME");
  if (s == NULL) {
	fprintf(stderr, "HOSTNAME, environment variable, is not set. Set it to your machine and domain name, i.e., raindrop3.gsfc.nasa.gov.\n");
	exit(-1);
  }
  strncpy(long_name_local_host, s, MAX_HOSTNAME_LEN-1);

  /* add to user selected neighbor list */
  add_new_host(Gselected_hosts, local_host);
  /* add to available neighbor list */
  add_new_host(available_hosts, local_host);

  /* Read available hosts in the network from a file */
  read_available_network_hosts_from_file();

  for (i = 0; i < MAX_ITEMS; i++) {
	curr_selected_prod_groups[i] = NULL;
	log_selected_items[i] = NULL;
  }
  memset(curr_selected_tape_group, '\0', MAX_BROWSER_LINE_LEN);
} /* initialize_main */

/***************************************************************************/
/*                                                                         */
/*                      initialize_comm                                    */
/*                                                                         */
/***************************************************************************/
void initialize_comm()
{

  /* communication between child and parent section */
  /* create a socket to communicate between child and parent */
  if ((data_socket = establish_async(0)) < 0) {
	perror("establish data socket");
	exit_system(NULL, -1);
  }
  data_portnum = portno(data_socket);
  if (write_to_file)
	fprintf(test_file, "Parent listening for child on port %d.\n", data_portnum);


  /* communication between child and monitorps section */
  /* have the system ready to listen to the monitorps */
  if ((gui_socket= establish_async(0)) < 0) {
    perror("parent establishes connection with  monitorps");
	exit_system(NULL, -1);
  }
  gms_portnum = portno(gui_socket);
  local_curr_time = get_curr_time();

}

/***************************************************************************/
/*                                                                         */
/*                             set_buttons_icon                            */
/*                                                                         */
/***************************************************************************/
void set_buttons_icon()
{
  char *path;

  /* define icons for pixmapbuttons */
  path = getenv("GMS_ICONS_PATH");
  if (path == NULL) {
	path = "/usr/local/trmm/gms/images";   /* default */
  }
  sprintf(help_icon, "%s/question.xpm", path);
  sprintf(neighbors_icon, "%s/neighbors.xpm", path);
  sprintf(print_icon, "%s/printer.xpm", path);
  sprintf(exit_icon, "%s/exit.xpm", path);
  sprintf(bars_icon, "%s/bars.xpm", path);
  sprintf(ps_icon, "%s/ps.xpm", path);
  sprintf(sys_stat_icon, "%s/sys_stat.xpm", path);
  sprintf(progs_icon, "%s/progs.xpm", path);
  sprintf(get_icon, "%s/get.xpm", path);
  sprintf(reset_icon, "%s/reset.xpm", path);
  sprintf(test_icon, "%s/test.xpm", path);
  sprintf(save_icon, "%s/save.xpm", path);
  sprintf(report_icon, "%s/report.xpm", path);
  sprintf(cancel_icon, "%s/cancel.xpm", path);
  sprintf(print_red_icon, "%s/print_red.xpm", path);
  sprintf(print_yellow_icon, "%s/print_yellow.xpm", path);
  sprintf(print_all_icon, "%s/print_all.xpm", path);
  sprintf(delete_job_icon, "%s/delete_job.xpm", path);
  sprintf(show_detail_icon, "%s/show_detail.xpm", path);
  sprintf(clear_icon, "%s/clear.xpm", path);
  sprintf(show_detail_icon, "%s/show_detail.xpm", path);
  sprintf(show_all_data_icon, "%s/show_all_data.xpm", path);
  sprintf(exceeded_threshold_icon, "%s/show_exceeded_threshold.xpm", path);

} /* set_buttons_icon */


/***************************************************************************/
/*                                                                         */
/*                                 set_fonts                               */
/*                                                                         */
/***************************************************************************/
void set_fonts()
{
  /* create fonts having fixed size and using 75 dpi
   */

  fl_set_font_name(LABEL_FONT_STYLE, "-misc-fixed-bold-r-normal--15-140-75-75-c-90-iso8859-1");

  /*-misc-fixed-medium-r-normal--20-200-75-75-c-100-iso8859-1");
   */
  /*
  fl_set_font_name(TEXT_FONT_STYLE, "-misc-fixed-medium-r-normal--15-140-75-75-c-90-iso8859-1");
  */

  fl_set_font_name(TEXT_FONT_STYLE, "-adobe-courier-bold-r-normal--12-120-75-75-m-70-iso8859-1");

  /*
  fl_set_font_name(TEXT_FONT_STYLE, "-misc-fixed-bold-r-normal-*-12-*-75-75-c-70-iso8859-1");
  */
  /*-misc-fixed-bold-r-normal--13-120-75-75-c-80-iso8859-1");

   */
  fl_set_font_name(TEXT_SMALL_FONT_STYLE, "-misc-fixed-medium-r-normal--13-120-75-75-c-70-iso8859-1");

}

/***************************************************************************/
/*                                                                         */
/*                               init_toolbars                             */
/*                                                                         */
/***************************************************************************/
void init_toolbars(int button_info_flag, int toolbar_flag)
{
  unsigned mode;

  /* tool bars related. */
  if (toolbar_flag) {
	mode = FL_PUP_CHECK;
  }
  else {
	mode = FL_PUP_BOX;
	/* hide tool bars */
	if (tape_grouping_form && tape_grouping_form->toolbar_grp &&
		tape_grouping_form->toolbar_grp->visible)
	  fl_hide_object(tape_grouping_form->toolbar_grp);
	if (product_grouping_form && product_grouping_form->toolbar_grp &&
		product_grouping_form->toolbar_grp->visible)
	  fl_hide_object(product_grouping_form->toolbar_grp);
	if (summary_report_form && summary_report_form->toolbar_grp &&
		summary_report_form->toolbar_grp->visible)
	  fl_hide_object(summary_report_form->toolbar_grp);
	if (logfile_report_form && logfile_report_form->toolbar_grp &&
		logfile_report_form->toolbar_grp->visible)
	  fl_hide_object(logfile_report_form->toolbar_grp);
	if (job_grouping_form && job_grouping_form->toolbar_grp &&
		job_grouping_form->toolbar_grp->visible)
	  fl_hide_object(job_grouping_form->toolbar_grp);

	hide_button_info_msg(); /* hide button's info form */

  }

  if (tape_grouping_form && tape_grouping_form->options_menu)
	fl_set_menu_item_mode(tape_grouping_form->options_menu, SHOW_TOOLBAR_MINDEX, mode);

  /* button info's window related*/
  if (button_info_flag)
	mode = FL_PUP_CHECK;
  else {
	mode = FL_PUP_BOX;
	hide_button_info_msg();
  }
  if (tape_grouping_form && tape_grouping_form->options_menu)
	fl_set_menu_item_mode(tape_grouping_form->options_menu, SHOW_BUTTON_INFO_MINDEX, mode);

} /* init_toolbars */

/***************************************************************************/
/*                                                                         */
/*                           end_xform                                     */
/*                                                                         */
/***************************************************************************/
void end_xform()
{
  if (need_to_clean_xform == 0) return;
  if (busy_form && busy_form->busy_form) {
	if (busy_form->busy_form->visible) {
	  fl_hide_form(busy_form->busy_form);
	}
	fl_free_form(busy_form->busy_form);
	free(busy_form);
	busy_form = NULL;
  }
  fl_finish();
  need_to_clean_xform = 0;
}
/***************************************************************************/
/*                                                                         */
/*                      initialize_xform                                   */
/*                                                                         */
/***************************************************************************/
void initialize_xform(int argc, char **argv)
{ 
  /* disable showing error msg by xform */
  int val = 0;
/*
  fl_show_errors(0);
*/
/*
  printf("initialize_gui\n");
*/

  fl_show_errors(1);
/*
  printf("fl_initialize(...)\n");
  printf("... argc == %d\n", argc);
  for (rc=0; rc<argc; rc++)
	printf("argv[%d] = %s\n", rc, argv[rc]);
*/

  fl_initialize(&argc, argv, 0, 0, 0); /* 0.8 of xforms */
  need_to_clean_xform = 1;
  fl_set_atclose(gms_atclose, (void *)&val);
  /* turns synchronous mode on for debug only */

  XSynchronize(fl_get_display(), 1);

  /* Create and display a busy form. */
  busy_form = (FD_busy_form *) create_form_busy_form();
  if (busy_form == NULL || busy_form->label == NULL ||
	  busy_form->status_label == NULL) {
	fprintf(stderr, "Can't create busy_form.\n");
	exit_system(NULL, -1);
  }
  fl_show_form(busy_form->busy_form, FL_PLACE_CENTER, FL_NOBORDER, "");

} /* initialize_xform */

/***************************************************************************/
/*                                                                         */
/*                      initialize_gui                                     */
/*                                                                         */
/***************************************************************************/
void initialize_gui()
{
  int rc;
  int i;

  memset(last_job_status_msg, '\0', MAX_BROWSER_LINE_LEN);


  /* initialize forms */
  ps_form = NULL;
  product_grouping_form = NULL;
  neighbors_form = NULL;
  admin_form = NULL;
  progs_form = NULL;
  summary_report_form = NULL;
  gv_levels_form = NULL;
  status_fields_selection_form = NULL;
  logfile_report_form = NULL;
  job_grouping_form = NULL;
  system_status_form = NULL;
  select_host_for_ps_form = NULL;
  tape_get_input_form = NULL;
  test_form = NULL;


  set_fonts();
  set_buttons_icon();

  /* options set in file, ~/.gms */
  show_button_info_flag = 1;
  show_toolbar_flag = 1;
  blink_boxes_flag = 0;

  for(i=0; i < MAX_FORMS_WITH_VIEW_MENU; i++) {
	forms_view_options[i] = VIEW_DEFAULT;
  }

  memset(windows_menu_items, '\0', MAX_WINDOWS_MENU_STR);
  curr_button = NULL;

  /* init. forms list */
  form_count =0;
  for (i = 0; i < MAX_FORMS; i++) {
	memset(gms_form_list[i].name, '\0', MAX_LABEL_LEN);
	gms_form_list[i].form = NULL;
  }
  /* initialize update form flags */
  load_status_forms = 1;  /* need to load status forms */
  refresh_status_form = 0;
  stat_info_count = 0;
  neighbor_list_changed = 0;
  update_sys_stat_form = 0;
  prog_list_changed = 0;
  status_info_changed = 0;
  ps_info_changed = 0;
  logdata_changed = 0;
  gms_update_forms = 0;
  need_show_new_data = 0;

  level_info_table_set_default(level_info_table);

  /* read progs, neighbors, and options  */

  read_gms_init_file();

  memset(gms_sys_stat_msg, '\0', MAX_SYS_STATUS_MSG_LEN);

  /* create all forms */
  rc = create_forms();
  if (rc == -1) {
	fprintf(stderr,"Error: Can't create forms\n");
	exit_system(NULL, -1);
  }

/*
  memset (msg, '\0', MAX_MSG_LEN);  
  sprintf(msg, "get log files");
  send_msg(msg);
*/
  /* for testing only */
#if 0
  TEST = 0;
  TEST_clear_msg = 1;
  TEST_msg_count = 0;
  strcpy(thost, "");
  strcpy(tprog, "");
  strcpy(tstatus, "");
  tpid = 0;
  tppid = 0;
  strcpy(tinfile, "file1.uf");
#endif


  init_toolbars(show_button_info_flag, show_toolbar_flag);
  /* popup main form */

  /* Set pixmap buttons for form */
  set_pixmapbuttons_for_tape_grouping_form();
#if 0
  show_form(tape_grouping_form->ldata);
  /* Hide busy form */
  fl_hide_form(busy_form->busy_form);
#else
  show_main_form = 1;
#endif

} /* initialize_gui */

/***************************************************************************/
/*                                                                         */
/*                      new_get_input_form                                 */
/*                                                                         */
/***************************************************************************/
FD_get_input_form *new_get_input_form()
{
  FD_get_input_form *get_input_form;
  long val;

  get_input_form = (FD_get_input_form * )create_form_get_input_form();
  if (get_input_form == NULL || get_input_form->cancel_button == NULL ||
	  get_input_form->clear_button == NULL || 
	  get_input_form->label == NULL || 
	  get_input_form->get_input_form == NULL || get_input_form->input == NULL ||
	  get_input_form->ok_button == NULL) return NULL;

  get_input_form->cancel_button->u_vdata = (void *) get_input_form->get_input_form;
  get_input_form->clear_button->u_vdata = (void *) get_input_form->input;
  get_input_form->ok_button->u_vdata = (void *) get_input_form;
  fl_set_form_atclose(get_input_form->get_input_form, form_atclose, (void *)&val);
  fl_add_io_callback(get_input_form->get_input_form->window, 0, do_nothing_io_handler, "");
  return get_input_form;
} /* new_get_input_form */

/***************************************************************************/
/*                                                                         */
/*                         do_nothing_io_handler                           */
/*                                                                         */
/***************************************************************************/

void do_nothing_io_handler(int sig, void *data)
{
/*
  long old_mask;

  old_mask = gms_block_ints();
  gms_unblock_ints(old_mask);
*/
}



/***************************************************************************/
/*                                                                         */
/*                       set_view_menu_options                             */
/*                                                                         */
/***************************************************************************/
void set_view_menu_options(FL_OBJECT *menu, 
						   view_option_type_t *view_opts, int form_id)

{
  unsigned mode, mode1;
  int nitems;

  if (menu == NULL || !menu->visible || view_opts == NULL) return;

  /* Unselect view all if other option was selected;
   * select view all if other options are not selected 
   */
  if (((!(*view_opts & VIEW_SUCCESSFULS) && !(*view_opts & VIEW_FAILURES) &&
		!(*view_opts & VIEW_RUNNINGS) && !(*view_opts & VIEW_UNKNOWNS) &&
		!(*view_opts & VIEW_WARNINGS) && !(*view_opts & VIEW_ONE_TAPE_GRPING))
	   &&  !(*view_opts & VIEW_ALL)) ||
	  /* Notthing is selected, then selecte view_all. */
	  (((*view_opts & VIEW_SUCCESSFULS) || (*view_opts & VIEW_FAILURES) ||
		(*view_opts & VIEW_RUNNINGS) || (*view_opts & VIEW_UNKNOWNS) ||
		(*view_opts & VIEW_WARNINGS) || (*view_opts & VIEW_ONE_TAPE_GRPING))
	   &&  (*view_opts & VIEW_ALL)))
    	/* Something is selected, then unselecte view_all. */
	(*view_opts) ^= VIEW_ALL;
  

  if (form_id != SUMMARY_REPORT_FORM_ID) {
	if (*view_opts & SORT_BY_ETIME) {
	  mode = (unsigned) FL_PUP_CHECK|FL_PUP_GRAY;
	  mode1 = (unsigned)FL_PUP_BOX;
	}
	else {
	  mode = (unsigned)FL_PUP_BOX;
	  mode1 = (unsigned)FL_PUP_CHECK|FL_PUP_GRAY;
	}
	fl_set_menu_item_mode(menu, VIEW_ETIME_SORTED_MINDEX, mode);
	fl_set_menu_item_mode(menu, VIEW_PROG_SORTED_MINDEX, mode1);
  }

  if (*view_opts & VIEW_ACTIVE_ONLY) 
	mode = (unsigned) FL_PUP_CHECK;
  else  mode = (unsigned) FL_PUP_BOX;
	fl_set_menu_item_mode(menu, VIEW_ACTIVE_ONLY_MINDEX, mode);

  if (*view_opts & VIEW_INACTIVE_ONLY) 
	mode = (unsigned) FL_PUP_CHECK;
  else  mode = (unsigned) FL_PUP_BOX;
	fl_set_menu_item_mode(menu, VIEW_INACTIVE_ONLY_MINDEX, mode);


  nitems = fl_get_menu_maxitems(menu);
  if (nitems >= 11) {

	if (*view_opts & VIEW_ALL)
	  mode = (unsigned) FL_PUP_CHECK|FL_PUP_GRAY;
	else 
	  mode = (unsigned) FL_PUP_BOX;
	fl_set_menu_item_mode(menu, VIEW_ALL_MINDEX, mode);

	if (*view_opts & VIEW_SUCCESSFULS)
	  mode = (unsigned) FL_PUP_CHECK;
	else 
	  mode = (unsigned) FL_PUP_BOX;
	fl_set_menu_item_mode(menu, VIEW_SUCCESSFULS_MINDEX, mode);

	if (*view_opts & VIEW_WARNINGS)
	  mode = (unsigned) FL_PUP_CHECK;
	else 
	  mode = (unsigned) FL_PUP_BOX;
	fl_set_menu_item_mode(menu, VIEW_WARNINGS_MINDEX, mode);

	if (*view_opts & VIEW_FAILURES)
	  mode = (unsigned) FL_PUP_CHECK;
	else 
	  mode = (unsigned) FL_PUP_BOX;
	fl_set_menu_item_mode(menu, VIEW_FAILURES_MINDEX, mode);

	if (*view_opts & VIEW_RUNNINGS)
	  mode = (unsigned) FL_PUP_CHECK;
	else 
	  mode = (unsigned) FL_PUP_BOX;
	fl_set_menu_item_mode(menu, VIEW_RUNNINGS_MINDEX, mode);

	if (*view_opts & VIEW_UNKNOWNS)
	  mode = (unsigned) FL_PUP_CHECK;
	else 
	  mode = (unsigned) FL_PUP_BOX;
	fl_set_menu_item_mode(menu, VIEW_UNKNOWNS_MINDEX, mode);
	
  }

  switch (form_id) {
  case GROUP_FORM_ID:
	break;
  case GMS_FORM_ID:
  case STATUS_FORM_ID:
	if (nitems < 10) break;
	if (*view_opts & VIEW_ONE_TAPE_GRPING)
	  mode = (unsigned) FL_PUP_CHECK;
	else 
	  mode = (unsigned) FL_PUP_BOX;
	fl_set_menu_item_mode(menu, VIEW_ONE_TAPE_GRPING_MINDEX, mode);
	break;
  default:
	break;
  }

} /* set_view_menu_options */

/***************************************************************************/
/*                                                                         */
/*                       create_product_grouping_form                           */
/*                                                                         */
/***************************************************************************/
int create_product_grouping_form()
{
  long val = 1;
  FD_get_input_form *get_input_form;

  /* returns -1 upon failure; 1 otherwise
   */
  product_grouping_form = create_form_gms_form();
  if (product_grouping_form == NULL || product_grouping_form->gms_form == NULL ||
	  product_grouping_form->label1 == NULL ||
	  product_grouping_form->browser == NULL ||
	  product_grouping_form->help_button == NULL ||
	  product_grouping_form->save_button == NULL ||
	  product_grouping_form->print_button == NULL ||
	  product_grouping_form->cancel_button == NULL ||
	  product_grouping_form->del_job_button == NULL ||
	  product_grouping_form->file_menu == NULL ||
	  product_grouping_form->action_menu == NULL ||
	  product_grouping_form->view_menu == NULL ||
	  product_grouping_form->help_menu == NULL ) {
	printf("Can't create  product_grouping_form\n");
	return -1;
  }
  product_grouping_form->ldata = form_count;  /* index to form list */
  add_new_form_to_list(GVS_MONITOR_FORM_NAME, product_grouping_form->gms_form);

  /* set font style */
  fl_set_object_lstyle(product_grouping_form->label1, LABEL_FONT_STYLE);

  /* set browser's label */
  fl_set_object_label(product_grouping_form->label1, 
"    Host     Tape ID         Product      Device   Date   Time    Runtime TotalJobs ");
  product_grouping_form->browser->u_vdata = (void *) product_grouping_form;

  fl_set_browser_fontstyle(product_grouping_form->browser,TEXT_FONT_STYLE);

  /* set callbacks */
  fl_set_object_callback(product_grouping_form->browser,
						  popup_file_status_report_form, STAT_INFO_FILE_RNG);
  fl_set_object_callback(product_grouping_form->del_job_button, 
						  do_del_job_button, PROD_GRPING);
  fl_set_form_atclose(product_grouping_form->gms_form, form_atclose, (void *)&val);

  fl_add_io_callback(product_grouping_form->gms_form->window, 0, do_nothing_io_handler, "");

  product_grouping_form->action_menu->u_ldata = PROD_GRPING;;

  /* set form's pulldown menus */
  /* Note: if change menu items below, change them in 
   *     do_file_menu()
   *     do_status_menu()
   *     do_view_menu();
   *     do_action_menu();
   *     do_help_menu();
   */
  fl_set_menu(product_grouping_form->file_menu, "Save|Print|Close");
  fl_set_menu(product_grouping_form->action_menu, "Remove all product groupings");
  fl_set_menu(product_grouping_form->view_menu, "Sort by end time|Sort by host, tapeid, and program|----------------------------|Active groups only|Inactive groups only|----------------------------|Groups with something is failed|Groups with something is warning|Groups with something is running|Groups with everything was successful|Groups with something is unknown|All|----------------------------|One tape grouping");
  fl_set_menu_item_mode(product_grouping_form->view_menu, VIEW_SEPARATOR1_MINDEX, FL_PUP_GRAY);
  fl_set_menu_item_mode(product_grouping_form->view_menu, VIEW_SEPARATOR2_MINDEX, FL_PUP_GRAY);
  fl_set_menu_item_mode(product_grouping_form->view_menu, VIEW_SEPARATOR3_MINDEX, FL_PUP_GRAY);

  set_view_menu_options(product_grouping_form->view_menu, &(forms_view_options[GMSF_VIEW_OPTION_IND]), GMS_FORM_ID);
  product_grouping_form->action_menu->u_vdata = (void *) product_grouping_form;
  product_grouping_form->view_menu->u_vdata = (void *) product_grouping_form;

  fl_set_menu(product_grouping_form->help_menu, "About gms|Contexts") ;

  get_input_form = new_get_input_form(); 
  if (get_input_form == NULL ) {
	printf("Can't create get_input_form--product_grouping_form\n");
	return -1;
  }
  get_input_form->ldata = form_count;  /* index to gms_form_list */
  add_new_form_to_list(INPUT_GVS_FORM_NAME, get_input_form->get_input_form);

  /* set these up so can use generic routines for writing browser to file*/
  product_grouping_form->file_menu->u_vdata = (void *) product_grouping_form;
  get_input_form->vdata = (void *) product_grouping_form;
  get_input_form->get_input_form->u_vdata = (void *) product_grouping_form->save_button;
  product_grouping_form->vdata = (void *) get_input_form;


  /* toolbar related.
   */
  

  product_grouping_form->cancel_button->u_vdata = (void *) product_grouping_form->gms_form;
  product_grouping_form->print_button->u_vdata = (void *) product_grouping_form;
  product_grouping_form->save_button->u_vdata = (void *) product_grouping_form;
  product_grouping_form->del_job_button->u_vdata = (void *) product_grouping_form;

  product_grouping_form->help_button->u_vdata = (void *) product_grouping_form;


  /* Set preemptive callback and button info table's index for button--
   * an info  window will be popup/disappear when the mouse is placed 
   * on/off the button.
   */

  product_grouping_form->cancel_button->u_ldata = (long) CANCEL_INDEX; /* index to button_info_tbl */

  fl_set_object_prehandler(product_grouping_form->cancel_button, 
						   do_button_info);


  product_grouping_form->print_button->u_ldata = (long) PRINT_INDEX; /* index to button_info_tbl */
  fl_set_object_prehandler(product_grouping_form->print_button, 
						   do_button_info);
  product_grouping_form->save_button->u_ldata = (long) SAVE_INDEX; /* index to button_info_tbl */
  fl_set_object_prehandler(product_grouping_form->save_button, 
						   do_button_info);

  product_grouping_form->del_job_button->u_ldata = DELETE_PROD_GRPING_INDEX; /* index to button_info_tbl */
  fl_set_object_prehandler(product_grouping_form->del_job_button,
						   do_button_info);


  product_grouping_form->help_button->u_ldata = (long) HELP_INDEX; /* index to button_info_tbl */
  fl_set_object_prehandler(product_grouping_form->help_button, 
						   do_button_info);

  return 1;

} /* create_product_grouping_form */

/***************************************************************************/
/*                                                                         */
/*                       create_tape_grouping_form                          */
/*                                                                         */
/***************************************************************************/
int create_tape_grouping_form()
{
  long val = 1;
  FD_get_input_form *get_input_form;
  unsigned mode;

  /* returns -1 upon failure; 1 otherwise
   */
  tape_grouping_form = create_form_group_form();
  if (tape_grouping_form == NULL || tape_grouping_form->group_form == NULL ||
	  tape_grouping_form->label1 == NULL ||
	  tape_grouping_form->label2 == NULL ||
	  tape_grouping_form->browser == NULL ||
	  tape_grouping_form->del_job_button == NULL ||
	  tape_grouping_form->all_data_button == NULL ||
	  tape_grouping_form->progs_button == NULL ||
	  tape_grouping_form->neighbors_button == NULL ||
	  tape_grouping_form->report_button == NULL ||
	  tape_grouping_form->ps_button == NULL ||
	  tape_grouping_form->bar_button == NULL ||
	  tape_grouping_form->reset_button == NULL ||
	  tape_grouping_form->help_button == NULL ||
	  tape_grouping_form->save_button == NULL ||
	  tape_grouping_form->print_button == NULL ||
	  tape_grouping_form->exit_button == NULL ||
	  tape_grouping_form->exceeded_threshold_button == NULL ||
	  tape_grouping_form->failure_light_button == NULL ||
	  tape_grouping_form->warning_light_button == NULL ||
	  tape_grouping_form->running_light_button == NULL ||
	  tape_grouping_form->unknown_light_button == NULL ||
	  tape_grouping_form->data_trans_status_label == NULL ||
	  tape_grouping_form->data_trans_stat_grp == NULL ||
	  tape_grouping_form->file_menu == NULL ||
	  tape_grouping_form->status_menu == NULL ||
	  tape_grouping_form->report_menu == NULL ||
	  tape_grouping_form->configure_menu == NULL ||
	  tape_grouping_form->action_menu == NULL ||
	  tape_grouping_form->options_menu == NULL ||
	  tape_grouping_form->windows_menu == NULL ||
	  tape_grouping_form->view_menu == NULL ||
	  tape_grouping_form->help_menu == NULL ) {
	printf("Can't create  tape_grouping_form\n");
	return -1;
  }
  tape_grouping_form->ldata = form_count;  /* index to form list */
  add_new_form_to_list(TAPE_GROUPING_FORM_NAME, tape_grouping_form->group_form);

  /* Turn light buttons off */
  fl_set_button(tape_grouping_form->failure_light_button, RELEASED);
  fl_set_button(tape_grouping_form->running_light_button, RELEASED);
  fl_set_button(tape_grouping_form->warning_light_button, RELEASED);
  fl_set_button(tape_grouping_form->unknown_light_button, RELEASED);
  fl_deactivate_object(tape_grouping_form->signal_grp); 

  /* set font style */
  fl_set_object_lstyle(tape_grouping_form->label1, LABEL_FONT_STYLE);
  fl_set_object_lstyle(tape_grouping_form->label2, LABEL_FONT_STYLE);
  fl_set_object_lstyle(tape_grouping_form->status_label, TEXT_SMALL_FONT_STYLE);

  fl_set_object_label(tape_grouping_form->status_label, EMPTY_STR);

  /* set browser's label */
  fl_set_object_label(tape_grouping_form->label1,
"                                      Current      Product   Product    Last");
  fl_set_object_label(tape_grouping_form->label2,
"    Host  Group ID        Runtime     Product      Runtime   Status  Job Runtime ");
  tape_grouping_form->browser->u_vdata = (void *) tape_grouping_form;

  fl_set_browser_fontstyle(tape_grouping_form->browser,TEXT_FONT_STYLE);

  /* set callbacks */

  fl_set_object_callback(tape_grouping_form->browser,
						  popup_product_grouping_form, 1);
  fl_set_object_callback(tape_grouping_form->del_job_button, 
						  do_del_job_button, TAPE_GRPING);
  fl_set_form_atclose(tape_grouping_form->group_form, form_atexit, (void *)&val);

  fl_add_io_callback(tape_grouping_form->group_form->window, 0, do_nothing_io_handler, "");

  tape_grouping_form->action_menu->u_ldata = TAPE_GRPING;

  /* set form's pulldown menus */
  /* Note: if change menu items below, change them in 
   *     do_file_menu()
   *     do_status_menu()
   *     do_report_menu();
   *     do_configure_menu()
   *     do_options_menu()
   *     do_action_menu()
   *     do_windows_menu();  remove_name_from_windows_menu();
   *     do_help_menu();
   */
  fl_set_menu(tape_grouping_form->file_menu, "Save|Print|Exit");
  fl_set_menu(tape_grouping_form->status_menu, "Show PS Info|Show System Messages|Show product groupings (text)|Show product groupings (bars)|Show all jobs sorted by time|Show jobs running for too long");
#if 0
  fl_set_menu(tape_grouping_form->report_menu, "Request Report|Show Summary Report");
#endif
  fl_set_menu(tape_grouping_form->report_menu, "Show Status Summary Report");
  fl_set_menu(tape_grouping_form->configure_menu, "Add/Delete Programs|Add/Delete Neighbors|---------------------|Administrative Functions");
  fl_set_menu_item_mode(tape_grouping_form->configure_menu, 3, FL_PUP_GRAY);
  fl_set_menu(tape_grouping_form->action_menu, "Remove all tape groupings");
  fl_set_menu(tape_grouping_form->windows_menu, "Close all others|---------------------------------");
  fl_set_menu_item_mode(tape_grouping_form->windows_menu, 2, FL_PUP_GRAY);
  fl_set_menu(tape_grouping_form->options_menu, "Show Toolbar|Show Button Description|Blink Boxes|Refresh data|Test");

  if (blink_boxes_flag) 
    mode = FL_PUP_CHECK;
  else mode = FL_PUP_BOX;
  fl_set_menu_item_mode(tape_grouping_form->options_menu,
						BLINK_BOXES_MINDEX, mode);

  fl_set_menu(tape_grouping_form->view_menu, "Sort by group's end time|Sort by host and tapeid|----------------------------|Active groups only|Inactive groups only|----------------------------|Groups with something is failed|Groups with something is warning|Groups with something is running|Groups with everything was successful|Groups with something is unknown|All");
  fl_set_menu_item_mode(tape_grouping_form->view_menu, VIEW_SEPARATOR1_MINDEX, FL_PUP_GRAY);
  fl_set_menu_item_mode(tape_grouping_form->view_menu, VIEW_SEPARATOR2_MINDEX, FL_PUP_GRAY);
  set_view_menu_options(tape_grouping_form->view_menu, &(forms_view_options[GROUPF_VIEW_OPTION_IND]), GROUP_FORM_ID);

  tape_grouping_form->view_menu->u_vdata = (void *) tape_grouping_form;
  tape_grouping_form->action_menu->u_vdata = (void *) tape_grouping_form;

  fl_set_menu(tape_grouping_form->help_menu, "About gms|Contexts") ;

  get_input_form = new_get_input_form(); 
  if (get_input_form == NULL ) {
	printf("Can't create get_input_form--tape_grouping_form\n");
	return -1;
  }
  get_input_form->ldata = form_count;  /* index to gms_form_list */
  add_new_form_to_list(INPUT_RUN_GRP_FORM_NAME, get_input_form->get_input_form);

  /* set these up so can use generic routines for writing browser to file*/
  tape_grouping_form->file_menu->u_vdata = (void *) tape_grouping_form;
  get_input_form->vdata = (void *) tape_grouping_form;
  get_input_form->get_input_form->u_vdata = (void *) tape_grouping_form->save_button;
  tape_grouping_form->vdata = (void *) get_input_form;

  
  /* toolbar related.
   */
  
  tape_grouping_form->exceeded_threshold_button->u_vdata = (void *) tape_grouping_form;
  tape_grouping_form->bar_button->u_vdata = (void *) tape_grouping_form;
  tape_grouping_form->exit_button->u_vdata = (void *) tape_grouping_form;
  tape_grouping_form->reset_button->u_vdata = (void *) tape_grouping_form;
  tape_grouping_form->print_button->u_vdata = (void *) tape_grouping_form;
  tape_grouping_form->save_button->u_vdata = (void *) tape_grouping_form;

  tape_grouping_form->ps_button->u_vdata = (void *) tape_grouping_form;
  tape_grouping_form->report_button->u_vdata = (void *) tape_grouping_form;
  tape_grouping_form->progs_button->u_vdata = (void *) tape_grouping_form;
  tape_grouping_form->neighbors_button->u_vdata = (void *) tape_grouping_form;
  tape_grouping_form->detail_button->u_vdata = (void *) tape_grouping_form;
  tape_grouping_form->all_data_button->u_vdata = (void *) tape_grouping_form;
  tape_grouping_form->del_job_button->u_vdata = (void *) tape_grouping_form;
  tape_grouping_form->help_button->u_vdata = (void *) tape_grouping_form;

  /* Set preemptive callback and button info table's index for button--
   * an info  window will be popup/disappear when the mouse is placed 
   * on/off the button.
   */
  tape_grouping_form->exceeded_threshold_button->u_ldata = (long) EXCEEDED_THRESHOLD_INDEX; /* index to button_info_tbl */

  fl_set_object_prehandler(tape_grouping_form->exceeded_threshold_button, 
						   do_button_info);

  tape_grouping_form->exit_button->u_ldata = (long) EXIT_INDEX; /* index to button_info_tbl */

  fl_set_object_prehandler(tape_grouping_form->exit_button, 
						   do_button_info);

  tape_grouping_form->bar_button->u_ldata = (long) BARS_INDEX; /* index to button_info_tbl */
  fl_set_object_prehandler(tape_grouping_form->bar_button, 
						   do_button_info);

  tape_grouping_form->reset_button->u_ldata = (long) RESET_INDEX; /* index to button_info_tbl */
  fl_set_object_prehandler(tape_grouping_form->reset_button, 
							 do_button_info);

  tape_grouping_form->print_button->u_ldata = (long) PRINT_INDEX; /* index to button_info_tbl */
  fl_set_object_prehandler(tape_grouping_form->print_button, 
						   do_button_info);
  tape_grouping_form->save_button->u_ldata = (long) SAVE_INDEX; /* index to button_info_tbl */
  fl_set_object_prehandler(tape_grouping_form->save_button, 
						   do_button_info);

  tape_grouping_form->ps_button->u_ldata = (long) PS_INDEX; /* index to button_info_tbl */
  fl_set_object_prehandler(tape_grouping_form->ps_button, 
						   do_button_info);
  tape_grouping_form->report_button->u_ldata = (long) SHOW_REPORT_INDEX; /* index to button_info_tbl */
  fl_set_object_prehandler(tape_grouping_form->report_button, 
						   do_button_info);
  tape_grouping_form->progs_button->u_ldata = (long) PROGS_INDEX; /* index to button_info_tbl */
  fl_set_object_prehandler(tape_grouping_form->progs_button, 
						   do_button_info);
  tape_grouping_form->neighbors_button->u_ldata = (long) NEIGHBORS_INDEX; /* index to button_info_tbl */
  fl_set_object_prehandler(tape_grouping_form->neighbors_button, 
						   do_button_info);

  tape_grouping_form->detail_button->u_ldata = (long) RUN_DETAIL_INDEX; /* index to button_info_tbl */
  fl_set_object_prehandler(tape_grouping_form->detail_button, 
						   do_button_info);

  tape_grouping_form->all_data_button->u_ldata = (long) SHOW_ALL_DATA_INDEX; /* index to button_info_tbl */
  fl_set_object_prehandler(tape_grouping_form->all_data_button, 
						   do_button_info);

  tape_grouping_form->del_job_button->u_ldata = DELETE_TAPE_GRPING_INDEX; /* index to button_info_tbl */
  fl_set_object_prehandler(tape_grouping_form->del_job_button,
						   do_button_info);

  tape_grouping_form->help_button->u_ldata = (long) HELP_INDEX; /* index to button_info_tbl */
  fl_set_object_prehandler(tape_grouping_form->help_button, 
						   do_button_info);



  return 1;

} /* create_tape_grouping_form */

FD_add_del_show_form *create_add_del_show_form(int type)
{
  FD_add_del_show_form *form;
  long val = 1;

  form = create_form_add_del_show_form();
  if (form  == NULL || form->add_del_show_form == NULL ||
	  form->cancel_button == NULL ||
	  form->add_button == NULL ||
	  form->delete_button == NULL ||
	  form->clear_button == NULL ||
	  form->input == NULL ||
	  form->label1 == NULL ||
	  form->label2 == NULL ||
	  form->available_browser == NULL ||
	  form->selected_browser == NULL ) {

	printf("Can't create form\n");
	return NULL;
  }

  form->ldata = form_count; /* index to form list **/
  form->cancel_button->u_vdata = (void *) form->add_del_show_form;
  form->available_browser->u_vdata = (void *) form;
  form->selected_browser->u_vdata = (void *) form;
  form->add_button->u_vdata  = (void *) form;
  form->delete_button->u_vdata  = (void *) form;
  form->clear_button->u_vdata  = (void *) form->input;
  fl_set_form_atclose(form->add_del_show_form, form_atclose, (void *)&val);
  fl_add_io_callback(form->add_del_show_form->window, 0, do_nothing_io_handler, "");
  switch (type) {
  case PROGS:
	add_new_form_to_list(PROGS_FORM_NAME, form->add_del_show_form);

	fl_set_object_callback(form->add_button,  update_monitoring_config, ADD_PROG);
	fl_set_object_callback(form->delete_button, update_monitoring_config, DEL_PROG);
	form->add_del_show_form->u_vdata = (void *) tape_grouping_form->progs_button;
	
	break;
  case HOSTS:
	add_new_form_to_list(NEIGHBORS_FORM_NAME, form->add_del_show_form);
	fl_set_object_callback(form->add_button,  update_monitoring_config, ADD_HOST);
	fl_set_object_callback(form->delete_button, update_monitoring_config, DEL_HOST);
	form->add_del_show_form->u_vdata = (void *) tape_grouping_form->neighbors_button;


	break;
  case ADMIN:
	add_new_form_to_list(ADMIN_FORM_NAME, form->add_del_show_form);
	fl_set_object_callback(form->add_button, update_monitoring_config, ADD_NETWORK_HOST);
	fl_set_object_callback(form->delete_button, update_monitoring_config, DEL_NETWORK_HOST);
	form->add_del_show_form->u_vdata = (void *) NULL;  /* This is not associated with a button */
	fl_set_object_label(form->label1, "Available Hosts:");
	fl_set_object_label(form->label2, "Current Neighbors:");
	break;
  default:
	free(form);
	form = NULL;
  }

  return form;
} /* create_add_del_show_form */


/***************************************************************************/
/*                                                                         */
/*                      create_ps_form                                     */
/*                                                                         */
/***************************************************************************/
int create_ps_form()
{
  long val = 1;

  /* returns -1 upon failure; 1 otherwise
   */

  ps_form = (FD_ps_info_form *) create_form_ps_info_form();
  if (ps_form == NULL ||
	  ps_form->ps_info_form == NULL ||
	  ps_form->every_level_button == NULL ||
	  ps_form->progs_only_button == NULL ||
	  ps_form->label1 == NULL ||
	  ps_form->action_menu == NULL ||
	  ps_form->configure_menu == NULL ||
	  ps_form->help_menu == NULL) {
	printf("Can't create ps_form\n");
	return -1;
  }
  ps_form->ldata = form_count; /* index to form list */
  add_new_form_to_list(PS_INFO_FORM_NAME, ps_form->ps_info_form);

  /* menu related */
  /* Note: if change menu items below, change them in 
   *     do_action_menu()
   *     do_configure_menu()
   *     do_help_menu()
   */
  fl_set_menu(ps_form->action_menu, "Close");
  fl_set_menu(ps_form->configure_menu, "Select Host(s)");
  fl_set_menu(ps_form->help_menu, "About gms|Contexts");

  /* setup so can use generic do_close_form() */
  ps_form->action_menu->u_vdata = (void *)ps_form->ps_info_form;
  


  ps_form->ps_info_form->u_vdata = (void *) tape_grouping_form->ps_button;
  fl_set_object_lstyle(ps_form->label1, LABEL_FONT_STYLE);
  fl_set_browser_fontstyle(ps_form->browser, TEXT_FONT_STYLE);
  fl_set_object_label(ps_form->label1, 
		 "Date    Time        Host     User    PID   PPID   Command");
  fl_set_button(ps_form->progs_only_button, PUSHED);
  fl_set_button(ps_form->every_level_button, RELEASED);
  fl_set_form_atclose(ps_form->ps_info_form, form_atclose, (void *)&val);
  fl_add_io_callback(ps_form->ps_info_form->window, 0, do_nothing_io_handler, "");

  return 1;
} /* create_ps_form */


/***************************************************************************/
/*                                                                         */
/*                          create_select_item_form                        */
/*                                                                         */
/***************************************************************************/
FD_select_item_form * create_select_item_form(int type)
{
  /* returns NULL upon failure; form otherwise
   */
  long val = 1;
  FD_select_item_form *form;

  form = create_form_select_item_form();
  if (form == NULL ||
	  form->select_item_form == NULL ||
	  form->cancel_button == NULL ||
	  form->clear_button == NULL ||
	  form->ok_button == NULL ||
	  form->browser == NULL) return NULL;

  form->ldata = form_count; /* Index to form list */
  switch(type) {
  case HOST_FOR_PS:
	add_new_form_to_list(SELECT_NEIGHBOR_LIST_FORM_NAME, 
						 form->select_item_form);
	fl_set_object_callback(form->ok_button, 
						   process_ps_request,0);
	break;

  default:
	return NULL;
  }

  form->cancel_button->u_vdata = (void *) form->select_item_form;
  form->ok_button->u_vdata = (void *) form;


  form->clear_button->u_vdata  = (void *) form->browser;
  fl_set_browser_fontstyle(form->browser,TEXT_FONT_STYLE);
  form->browser->u_vdata = (void *) form;
  fl_set_form_atclose(form->select_item_form, form_atclose, (void *)&val);
  fl_add_io_callback(form->select_item_form->window, 0, do_nothing_io_handler, "");

  return form;
} /* create_select_item_form */


/***************************************************************************/
/*                                                                         */
/*                           create_summary_report_form                       */
/*                                                                         */
/***************************************************************************/
int create_summary_report_form()
{
  /* returns -1 upon failure; 1 otherwise
   */
  long val = 1;
  char label[MAX_LABEL_LEN];
  FD_get_input_form *get_input_form;

  /* this form is used to display the history report which contains: 
   * tapeid, product, and prod's status.
   */

  summary_report_form = (FD_report_form *) create_form_report_form();
  if (summary_report_form == NULL || 
	  summary_report_form->report_form == NULL ||
	   summary_report_form->cancel_button == NULL ||
	  summary_report_form->browser == NULL ||
	  summary_report_form->label == NULL ||
	  summary_report_form->refresh_button == NULL ||
	  summary_report_form->save_button == NULL ||
	  summary_report_form->del_job_button == NULL ||
	  summary_report_form->tape_button == NULL ||
	  summary_report_form->level_button == NULL ||
	  summary_report_form->product_button == NULL ||
	  summary_report_form->print_red_button == NULL ||
	  summary_report_form->print_yellow_button == NULL ||
	  summary_report_form->print_all_button == NULL ||
	  summary_report_form->file_menu == NULL ||
	  summary_report_form->configure_menu == NULL ||
	  summary_report_form->view_menu == NULL ||
	  summary_report_form->help_menu == NULL) {
	fprintf(stderr, "can't create summary_report_form\n");
	return -1;
  }
  summary_report_form->ldata = form_count; /* index to form list */
  add_new_form_to_list(SUMMARY_REPORT_FORM_NAME, summary_report_form->report_form);

  get_input_form = new_get_input_form(); 
  if (get_input_form == NULL ) {
	fprintf(stderr, "can't create get_input_form -- summary_report_form \n");
	  return -1;
  }
  get_input_form->ldata = form_count;  /* index to gms_form_list */
  add_new_form_to_list(INPUT_SUMMARY_RPT_FORM_NAME, get_input_form->get_input_form);

  get_input_form->vdata = summary_report_form;
  get_input_form->get_input_form->u_vdata = (void *) summary_report_form->save_button;
  summary_report_form->vdata = (void *) get_input_form;
  tape_get_input_form = get_input_form;

  /* labels */
  fl_set_object_lstyle(summary_report_form->label, LABEL_FONT_STYLE);
  memset(label, '\0', MAX_LABEL_LEN);

  sprintf(label, " ");

  fl_set_object_label(summary_report_form->label, label);


  /* toolbar related */
  summary_report_form->report_form->u_vdata = (void *) tape_grouping_form->report_button;
  summary_report_form->cancel_button->u_vdata = (void *) summary_report_form->report_form;
  summary_report_form->refresh_button->u_vdata = (void *) summary_report_form;

  summary_report_form->save_button->u_vdata = (void *) summary_report_form;
  summary_report_form->del_job_button->u_vdata = (void *) summary_report_form;
  summary_report_form->tape_button->u_vdata = (void *) summary_report_form;
  summary_report_form->level_button->u_vdata = (void *) summary_report_form;
  summary_report_form->product_button->u_vdata = (void *) summary_report_form;
  summary_report_form->print_red_button->u_vdata = (void *) summary_report_form;
  summary_report_form->print_yellow_button->u_vdata = (void *) summary_report_form;
  summary_report_form->print_all_button->u_vdata = (void *) summary_report_form;

  /* browser */
  fl_set_browser_fontstyle(summary_report_form->browser,TEXT_FONT_STYLE);
  summary_report_form->browser->u_vdata = (void *) summary_report_form;


  /* Set preemptive callback and button info table's index for button--
   * an info  window will be popup/disappear when the mouse is placed 
   * on/off the button.
   */
  summary_report_form->cancel_button->u_ldata = CANCEL_INDEX; /* index to button_info_tbl */
  fl_set_object_prehandler(summary_report_form->cancel_button,
						   do_button_info);
  summary_report_form->save_button->u_ldata = SAVE_INDEX; /* index to button_info_tbl */
  fl_set_object_prehandler(summary_report_form->save_button,
						   do_button_info);

  summary_report_form->del_job_button->u_ldata = DELETE_SUMMARY_INFO_INDEX; /* index to button_info_tbl */
  fl_set_object_prehandler(summary_report_form->del_job_button,
						   do_button_info);

  summary_report_form->tape_button->u_ldata = SUMMARY_TAPE_INDEX; /* index to button_info_tbl */
  fl_set_object_prehandler(summary_report_form->tape_button,
						   do_button_info);
  summary_report_form->level_button->u_ldata = SUMMARY_LEVEL_INDEX; /* index to button_info_tbl */
  fl_set_object_prehandler(summary_report_form->level_button,
						   do_button_info);
  summary_report_form->product_button->u_ldata = SUMMARY_PRODUCT_INDEX; /* index to button_info_tbl */
  fl_set_object_prehandler(summary_report_form->product_button,
						   do_button_info);

  summary_report_form->print_red_button->u_ldata = PRINT_RED_INDEX; /* index to button_info_tbl */
  fl_set_object_prehandler(summary_report_form->print_red_button,
						   do_button_info);
  summary_report_form->print_yellow_button->u_ldata = PRINT_YELLOW_INDEX; /* index to button_info_tbl */
  fl_set_object_prehandler(summary_report_form->print_yellow_button,
						   do_button_info);
  summary_report_form->print_all_button->u_ldata = PRINT_ALL_INDEX; /* index to button_info_tbl */
  fl_set_object_prehandler(summary_report_form->print_all_button,
						   do_button_info);
  summary_report_form->refresh_button->u_ldata = REFRESH_INDEX; /* index to button_info_tbl */

  fl_set_object_prehandler(summary_report_form->refresh_button,
						   do_button_info);

  summary_report_form->help_button->u_ldata = HELP_INDEX; /* index to button_info_tbl */
  fl_set_object_prehandler(summary_report_form->help_button,
						   do_button_info);


  /* menus related */
  /* Note: if change menu items below, change them in 
   *     do_file_menu()
   *     do_action_menu()
   *     do_help_menu();
   */
  fl_set_menu(summary_report_form->file_menu, 
			  "Save|Print Failures|Print Warnings|Print All|Close");
  fl_set_menu(summary_report_form->configure_menu, "Define GV Levels") ;

  fl_set_menu(summary_report_form->view_menu, "Sort by end time|Sort by host, tapeid, and program|----------------------------|Active groups only|Inactive groups only|----------------------------|Groups with something is failed|Groups with something is warning|Groups with something is running|Groups with everything was successful|Groups with something is unknown|All");
  fl_set_menu(summary_report_form->help_menu, "About gms|Contexts") ;
  fl_set_menu_item_mode(summary_report_form->view_menu, VIEW_ETIME_SORTED_MINDEX, FL_PUP_GRAY);
  fl_set_menu_item_mode(summary_report_form->view_menu, VIEW_PROG_SORTED_MINDEX, FL_PUP_GRAY);
  fl_set_menu_item_mode(summary_report_form->view_menu, VIEW_SEPARATOR1_MINDEX, FL_PUP_GRAY);
  fl_set_menu_item_mode(summary_report_form->view_menu, VIEW_SEPARATOR2_MINDEX, FL_PUP_GRAY);

  set_view_menu_options(summary_report_form->view_menu, &(forms_view_options[SUMMARYF_VIEW_OPTION_IND]), SUMMARY_REPORT_FORM_ID);
  /* set these up so can use generic routines for writing browser to file*/
  summary_report_form->file_menu->u_vdata = (void *)summary_report_form;
  summary_report_form->configure_menu->u_vdata = (void *)summary_report_form;
  summary_report_form->view_menu->u_vdata = (void *)summary_report_form;


  /* callbacks */
  fl_set_object_callback(summary_report_form->print_red_button, 
						  do_print_failures_from_report, 1);
  fl_set_object_callback(summary_report_form->print_yellow_button, 
						  do_print_warnings_from_report, 1);
  fl_set_object_callback(summary_report_form->print_all_button, 
						  do_print_all_from_browser, 1);
  fl_set_form_atclose(summary_report_form->report_form, form_atclose, (void *)&val);
  fl_add_io_callback(summary_report_form->report_form->window, 0, do_nothing_io_handler, "");

  fl_set_button(summary_report_form->tape_button, 1);
  return 1;
} /* create_summary_report_form */


FD_logfile_form *create_logfile_form(int type)
{
  FD_logfile_form *form;
  long val = 1;
  char label[MAX_LABEL_LEN];
  FD_get_input_form *get_input_form;

  form = create_form_logfile_form();

  if (form == NULL ||
	  form->label == NULL ||
	  form->logfile_form == NULL ||
	  form->cancel_button == NULL ||
	  form->browser == NULL ||
	  form->save_button == NULL ||
	  form->print_red_button == NULL ||
	  form->print_yellow_button == NULL ||
	  form->print_all_button == NULL ||
	  form->del_job_button == NULL ||
	  form->file_menu == NULL ||
	  form->action_menu == NULL ||
	  form->help_menu == NULL) {
	fprintf(stderr, "can't create form\n");
	return NULL;
  }

  form->ldata = form_count; /* index to form list */
  fl_add_io_callback(form->logfile_form->window, 0, do_nothing_io_handler, "");

  get_input_form = new_get_input_form();
  if (get_input_form == NULL) {
	fprintf(stderr, "can't create get input form\n");
	free(form);
	return NULL;
  }

  get_input_form->vdata = (void *) form;
  get_input_form->get_input_form->u_vdata = (void *) form->save_button;
  form->vdata = get_input_form;

  fl_set_object_lstyle(form->label, LABEL_FONT_STYLE);
  fl_set_browser_fontstyle(form->browser,TEXT_FONT_STYLE);
  sprintf(label, "   Date   Time     Tape ID   Device       Product      Job Num");
  fl_set_object_label(form->label, label);

  /* Toolbar related */
  form->cancel_button->u_vdata = (void *) form->logfile_form;
  form->save_button->u_vdata = (void *) form;
  form->print_red_button->u_vdata = (void *) form;
  form->print_yellow_button->u_vdata = (void *) form;
  form->print_all_button->u_vdata = (void *) form;
  form->browser->u_vdata = (void *) form;

  /* Set preemptive callback and button info table's index for button--
   * an info  window will be popup/disappear when the mouse is placed 
   * on/off the button.
   */
  form->cancel_button->u_ldata = CANCEL_INDEX; /* index to button_info_tbl */
  fl_set_object_prehandler(form->cancel_button,
						   do_button_info);
  form->save_button->u_ldata = SAVE_INDEX; /* index to button_info_tbl */
  fl_set_object_prehandler(form->save_button,
						   do_button_info);
  form->print_red_button->u_ldata = PRINT_RED_INDEX; /* index to button_info_tbl */
  fl_set_object_prehandler(form->print_red_button,
						   do_button_info);
  form->print_yellow_button->u_ldata = PRINT_YELLOW_INDEX; /* index to button_info_tbl */
  fl_set_object_prehandler(form->print_yellow_button,
						   do_button_info);
  form->print_all_button->u_ldata = PRINT_ALL_INDEX; /* index to button_info_tbl */
  fl_set_object_prehandler(form->print_all_button,
						   do_button_info);
  form->del_job_button->u_ldata = DELETE_JOB_GRPING_INDEX; /* index to button_info_tbl */
  fl_set_object_prehandler(form->del_job_button,
						   do_button_info);
  form->help_button->u_ldata = HELP_INDEX; /* index to button_info_tbl */
  fl_set_object_prehandler(form->help_button,
						   do_button_info);


  /* menus related */
  /* Note: if change menu items below, change them in 
   *     do_file_menu()
   *     do_action_menu()
   *     do_help_menu();
   */
  fl_set_menu(form->file_menu, 
			  "Save|Print Failures|Print Warnings|Print All|Close");
  fl_set_menu(form->action_menu, "Remove all job groupings");
  fl_set_menu(form->help_menu, "About gms|Contexts") ;

  /* set these up so can use generic routines for writing browser to file*/
  form->file_menu->u_vdata = (void *)form;
  form->action_menu->u_vdata = (void *)form;


  /* set callbacks */
  fl_set_object_callback(form->print_red_button, 
						  do_print_failures_from_report, 0);
  fl_set_object_callback(form->print_yellow_button, 
						  do_print_warnings_from_report, 0);
  fl_set_object_callback(form->print_all_button, 
						  do_print_all_from_browser, 0);

  fl_set_form_atclose(form->logfile_form, form_atclose, (void *)&val);



  switch (type) {
  case LOGFILE:
	add_new_form_to_list(FILE_REPORT_FORM_NAME, form->logfile_form);
	get_input_form->ldata = form_count;  /* index to gms_form_list */
	add_new_form_to_list(INPUT_FILE_RPT_FORM_NAME, get_input_form->get_input_form);
	fl_set_object_callback(form->cancel_button, do_close_form, 
						   LOGFILE_REPORT_FORM_ID);
	break;
  case CURR_FILE_STAT:
	form->logfile_form->u_vdata = (void *) NULL;
	add_new_form_to_list(FILE_STAT_FORM_NAME, form->logfile_form);

	get_input_form->ldata = form_count;  /* index to gms_form_list */
	add_new_form_to_list(INPUT_FILE_STAT_FORM_NAME, get_input_form->get_input_form);
	fl_set_object_callback(form->cancel_button, do_close_form, 
						   JOB_GROUP_FORM_ID);
	break;
  default:
	free(form);
	return NULL;
  }

  return form;
} /* create_logfile_form */

/***************************************************************************/
/*                                                                         */
/*                          create_msg_form                                */
/*                                                                         */
/***************************************************************************/
FD_msg_form *create_msg_form(int form_id, char *form_name)
{

  long val = 1;
  FD_msg_form *msg_form;
  FD_get_input_form *get_input_form;


  /* create system status form */
  msg_form = (FD_msg_form *) create_form_msg_form();
  if (msg_form == NULL ||
	  msg_form->msg_form == NULL ||
	  msg_form->browser == NULL ||
	  msg_form->label == NULL ||
	  msg_form->file_menu == NULL ||
	  msg_form->help_menu == NULL) {
	fprintf(stderr, "can't create system_status form\n");
	return NULL;
  }
  msg_form->ldata = form_count;  /* index to form list */
  add_new_form_to_list(form_name, msg_form->msg_form);

  /* menu related */
  /* Note: if change menu items below, change them in 
   *     do_help_menu()
   *     do_file_menu()
   */

  fl_set_menu(msg_form->help_menu, "About gms|Contexts");

  /* setup so can use generic do_close_form() */
  msg_form->file_menu->u_vdata = (void *)msg_form;

/*
  msg_form->msg_form->u_vdata = (void *)product_grouping_form->stat_button ;
*/
  msg_form->msg_form->u_vdata = (void *) NULL;

  fl_set_browser_fontstyle(msg_form->browser,TEXT_FONT_STYLE);

  msg_form->browser->u_vdata = (void *) msg_form;


  fl_set_object_lstyle(msg_form->label, LABEL_FONT_STYLE);

  fl_set_form_atclose(msg_form->msg_form, form_atclose, 
					  (void *) &val);
  fl_add_io_callback(msg_form->msg_form->window, 0, do_nothing_io_handler, "");

  fl_set_object_callback(msg_form->file_menu, do_file_menu, form_id);

  switch (form_id) {
  case SYS_MSG_FORM_ID:
	fl_set_object_label(msg_form->label, " Date    Time    Message ");
	fl_hide_object(msg_form->legend_grp);
	fl_set_menu(msg_form->file_menu, "Close");
	fl_hide_object(msg_form->threshold_grp);
	break;
  case ALL_DATA_FORM_ID:
	fl_set_menu(msg_form->file_menu, "Save|Print|Close");
	fl_set_object_label(msg_form->label, " Date    Time    Tape ID    Tape Device Product     Job Number  Filename");
	fl_hide_object(msg_form->threshold_grp);
	get_input_form = new_get_input_form();
	if (get_input_form == NULL) {
	  fprintf(stderr, "can't create get input form\n");
	  free(msg_form);
	  return NULL;
	}
	
	get_input_form->vdata = (void *) msg_form;
	get_input_form->get_input_form->u_vdata = (void *) NULL;
	msg_form->vdata = get_input_form;
	get_input_form->ldata = form_count;  /* index to gms_form_list */
	add_new_form_to_list(INPUT_FILE_ALL_DATA_FORM_NAME, get_input_form->get_input_form);
	break;
  case EXCEEDED_THRESHOLD_FORM_ID:
	fl_set_menu(msg_form->file_menu, "Save|Print|Close");
	fl_set_object_label(msg_form->label, "  Date   Time    Runtime  Tape ID      Device    Product Job No. Filename");
	fl_set_counter_precision(msg_form->threshold_counter, 2);
	fl_set_counter_value(msg_form->threshold_counter, threshold_time);
	fl_set_counter_bounds(msg_form->threshold_counter, -0.10, 1000.0);

	get_input_form = new_get_input_form();
	if (get_input_form == NULL) {
	  fprintf(stderr, "can't create get input form\n");
	  free(msg_form);
	  return NULL;
	}

	get_input_form->vdata = (void *) msg_form;
	get_input_form->get_input_form->u_vdata = (void *) NULL;
	msg_form->vdata = get_input_form;
	get_input_form->ldata = form_count;  /* index to gms_form_list */
	add_new_form_to_list(INPUT_FILE_EXCEEDED_THRESHOLD_FORM_NAME, 
						 get_input_form->get_input_form);
	break;
  default:
	break;
  }

  return msg_form;
} /* create_msg_form */

/***************************************************************************/
/*                                                                         */
/*                           create_test_form                              */
/*                                                                         */
/***************************************************************************/

int create_test_form()
{
  long val = 1;

  test_form = (FD_test_form *) create_form_test_form();
  if (test_form == NULL ||
	  test_form->test_form == NULL ||
	  test_form->update_history_button == NULL ||
	  test_form->flush_button == NULL ||
	  test_form->show_loglist_button == NULL ||
	  test_form->log_files_button == NULL ||
	  test_form->get_stat_info_button == NULL||
	  test_form->cancel_button == NULL  ) {
	fprintf(stderr, "can't create test form\n");
	return -1;
  }
  test_form->ldata = form_count; /* index to form list */
  add_new_form_to_list(TEST_FORM_NAME, test_form->test_form);

/*
  test_form->test_form->u_vdata = (void *) product_grouping_form->test_button;
*/
  test_form->update_history_button->u_vdata = (void *) test_form;
  test_form->log_files_button->u_vdata = (void *) test_form;
  test_form->get_stat_info_button->u_vdata = (void *) test_form;
  test_form->cancel_button->u_vdata = (void  *) test_form->test_form;
  test_form->flush_button->u_vdata = (void  *) test_form;
/*
  test_form->vdata = (void *) product_grouping_form->test_button;
*/
  fl_set_form_atclose(test_form->test_form, form_atclose, (void *)&val);
  fl_add_io_callback(test_form->test_form->window, 0, do_nothing_io_handler, "");
  return 1;
} /* create_test_form */


/***************************************************************************/
/*                                                                         */
/*                      create_gv_levels_form                              */
/*                                                                         */
/***************************************************************************/

int create_gv_levels_form()
{
  long val = 1; 

  gv_levels_form = (FD_gv_levels_form *) create_form_gv_levels_form();

  if (gv_levels_form == NULL ||
	  gv_levels_form->label == NULL ||
	  gv_levels_form->level_browser == NULL ||
	  gv_levels_form->available_progs_browser == NULL ||
	  gv_levels_form->gv_levels_form == NULL ||
	  gv_levels_form->program_input == NULL ||
	  gv_levels_form->level_input == NULL ||
	  gv_levels_form->add_button == NULL ||
	  gv_levels_form->delete_button == NULL ||
	  gv_levels_form->clear_button == NULL ||
	  gv_levels_form->cancel_button == NULL) {
	fprintf(stderr, "Can't create gv_levels form \n");
	return -1;
  }
  gv_levels_form->ldata = form_count;/* index to form list */
  add_new_form_to_list(GV_LEVELS_FORM_NAME, gv_levels_form->gv_levels_form);

  gv_levels_form->cancel_button->u_vdata = (void*)gv_levels_form->gv_levels_form;
  gv_levels_form->add_button->u_vdata = (void *)gv_levels_form;
  gv_levels_form->delete_button->u_vdata = (void *)gv_levels_form;
  gv_levels_form->clear_button->u_vdata = (void *)gv_levels_form->program_input;
  gv_levels_form->level_browser->u_vdata = (void *)gv_levels_form;
  gv_levels_form->available_progs_browser->u_vdata = (void *)gv_levels_form;

  fl_set_form_atclose(gv_levels_form->gv_levels_form, form_atclose, (void *) &val);
  fl_add_io_callback(gv_levels_form->gv_levels_form->window, 0, do_nothing_io_handler, "");

  return 1;
} /* create_gv_levels_form */


/***************************************************************************/
/*                                                                         */
/*                     update_status_fields_selection_form                 */
/*                                                                         */
/***************************************************************************/
void update_status_fields_selection_form() 
{
  if (status_fields_selection_form == NULL || 
	  status_fields_selection_form->status_fields_selection_form == NULL) 
	return;
  update_active_fields();
}

/***************************************************************************/
/*                                                                         */
/*                     create_status_fields_selection_form                 */
/*                                                                         */
/***************************************************************************/
int create_status_fields_selection_form() 
{
  long val = 1; 

  status_fields_selection_form = (FD_status_fields_selection_form *)
	create_form_status_fields_selection_form();
  if (status_fields_selection_form == NULL ||
	  status_fields_selection_form->status_fields_selection_form == NULL ||
	  status_fields_selection_form->ok_button == NULL ||
	  status_fields_selection_form->clear_button == NULL ||
	  status_fields_selection_form->cancel_button == NULL ||
	  status_fields_selection_form->tapeid_input == NULL ||
	  status_fields_selection_form->host_input == NULL ||
	  status_fields_selection_form->product_input == NULL ||
	  status_fields_selection_form->level_input == NULL ||
	  status_fields_selection_form->sdate_input == NULL)  return -1;

  status_fields_selection_form->ldata = form_count;/* index to form list */
  add_new_form_to_list(STATUS_FIELDS_SELECTION_FORM_NAME, status_fields_selection_form->status_fields_selection_form);
  status_fields_selection_form->cancel_button->u_vdata = 
	(void *)status_fields_selection_form->status_fields_selection_form;
  status_fields_selection_form->clear_button->u_vdata = 
	(void *)status_fields_selection_form;
  status_fields_selection_form->ok_button->u_vdata = 
	(void *)status_fields_selection_form;
  fl_set_form_atclose(status_fields_selection_form->status_fields_selection_form, form_atclose, (void *) &val);
  fl_add_io_callback(status_fields_selection_form->status_fields_selection_form->window, 0, do_nothing_io_handler, "");
  
  return 1;
} /* create_status_fields_selection_form */


/***************************************************************************/
/*                                                                         */
/*                      create_forms                                       */
/*                                                                         */
/***************************************************************************/

int create_forms()
{
  /* Creates all the forms that this program will need.
   * Returns -1 upon failure; 1 otherwise.
   */

  /* run grouping  form */
  if (create_tape_grouping_form() < 0) return -1;

  /* gts monitoring  form */
  if (create_product_grouping_form() < 0) return -1;

  /* bars form */
  gstatus_info = new_gstatus_rec();
  if (gstatus_info == NULL) {
	perror("calloc gstatus info");
	return -1;
  }

  if ((progs_form = create_add_del_show_form(PROGS)) == NULL) {
	fprintf(stderr, "progs_form is NULL\n");
	return -1;
  }

  if ((neighbors_form = create_add_del_show_form(HOSTS)) == NULL) {
	fprintf(stderr, "neighbors_form is NULL\n");
	return -1;
  }
  if ((admin_form = create_add_del_show_form(ADMIN) ) == NULL) {
	fprintf(stderr, "admin_form is NULL\n");
	return -1;
  }

  if (create_ps_form() < 0) return -1;

  if ((select_host_for_ps_form = create_select_item_form(HOST_FOR_PS)) == NULL) {
	fprintf(stderr, "select_host_for_ps_form is null\n");
	return -1;
  }


  if (create_summary_report_form() < 0) return -1;
  if (create_gv_levels_form() < 0) return -1;
  if (create_status_fields_selection_form() < 0) return -1;

  /* create logfile report form: this form is used to display 
   * tapeid, prod, range of files, and range's status.
   */
  if ((logfile_report_form = create_logfile_form(LOGFILE)) == NULL) {
	fprintf(stderr, "logfile_report_form is NULL\n");
	return -1;
  }

  fl_hide_object(logfile_report_form->del_job_button);
  fl_hide_object(logfile_report_form->action_menu);

  /* create file status report form: this form is used to display 
   * tapeid, prod, range of files, and range's status.
   */
  if ((job_grouping_form = create_logfile_form(CURR_FILE_STAT)) == NULL) {
	fprintf(stderr, "job_grouping_form is NULL\n");
	return -1;
  }

  fl_set_object_callback(job_grouping_form->del_job_button, 
						  do_del_job_button, JOB_GRPING);
  job_grouping_form->del_job_button->u_vdata = (void *) product_grouping_form;
  job_grouping_form->action_menu->u_ldata = JOB_GRPING;

  if ((system_status_form = create_msg_form(SYS_MSG_FORM_ID, SYSTEM_MSG_FORM_NAME)) == NULL) 
	 return -1;

  if ((all_data_form = create_msg_form(ALL_DATA_FORM_ID,ALL_DATA_FORM_NAME)) == NULL) 
	 return -1;

  if ((exceeded_threshold_form = create_msg_form(EXCEEDED_THRESHOLD_FORM_ID,
												EXCEEDED_THRESHOLD_FORM_NAME)) == NULL) 
	 return -1;

#if 1
  if (create_test_form() < 0) return -1;
#endif

  return (1);
} /* create_forms */

/***************************************************************************/
/*                                                                         */
/*                             cmd_match_prog                              */
/*                                                                         */
/***************************************************************************/
int cmd_match_prog(char *cmd, char *prog)
{
  char tmp_cmd[MAX_CMD_LEN], *tmp;

  if (cmd == NULL || prog == NULL) return 0;
  memset(tmp_cmd, '\0', MAX_CMD_LEN);
  strcpy(tmp_cmd, cmd);
  tmp = tmp_cmd;

  /* match if cmd contains /usr/bin/prog, ../bin/prog, prog at the beginning */

  /* get the first string */
  if (strchr(tmp, ' ') != NULL) {
	tmp = strchr(tmp, ' ');
	*tmp = '\0';
	tmp = tmp_cmd;
  }
#if 0
  /* if pefectly match */
  if (strncmp(tmp, prog, strlen(prog)) == 0)
	return 1;  

  /* else remove pathname before checking */
  tmp = remove_pathname(tmp);

  if (strncmp(tmp, prog, strlen(prog)) == 0)
	return 1;  
#endif

  /* if pefectly match */
  if (strcmp(tmp, prog) == 0)
	return 1;  

  /* else remove pathname before checking */
  tmp = remove_pathname(tmp);

  if (strcmp(tmp, prog) == 0)
	return 1;  

  return 0;
} /* cmd_match_prog */


/***************************************************************************/
/*                                                                         */
/*                      get_ parent                                         */
/*                                                                         */
/***************************************************************************/
/* parent having the same command name as child */
int get_parent(Process ps_list[], int nlines, int pid, char *prog)
{
  int i;

  if (prog == NULL || strlen(prog) == 0) return -1;

  for (i = 0; i < nlines; i++) {
	if (ps_list[i].sflag) continue;

	if (pid == ps_list[i].pid && cmd_match_prog(ps_list[i].cmd, prog))
	  return ps_list[i].ppid;
  }
  return -1;
}

/***************************************************************************/
/*                                                                         */
/*                      find_top_ancestor                                  */
/*                                                                         */
/***************************************************************************/
void find_top_ancestor(
					   Process ps_list[], int nlines, 
					   int pid, char *prog, int *ancestor)
{
  int ppid;
  int tppid;

  if (prog == NULL || strlen(prog) == 0) return;

  tppid = -1;
  while (1) {

	if ((ppid = get_parent(ps_list, nlines, pid, prog))  == -1) 
	  break;
	tppid = pid;
	pid = ppid;
#ifdef linux
	if (ppid == 1) break;
#else
	if (ppid == 0) break;  /* execption */
#endif
  }
  *ancestor = tppid;

} /* find_top_ancestor */


/***************************************************************************/
/*                                                                         */
/*                      extract_ps_info_of_descentors                      */
/*                                                                         */
/***************************************************************************/
void extract_ps_info_of_descentors(char *fromhost,
								 Process ps_list[], int nlines, 
								 int ppid, char *prog_info,int start,
								 int depth, int max_data_len, char *time_str)
{
  int i, j;
  char indent[MAX_SUB_LEVELS];
  char stat_line[MAX_STATUS_LINE_LEN];
  char pref_str[20 + MAX_SUB_LEVELS];


  if (fromhost == NULL || prog_info == NULL || time_str == NULL) return;

  for (i = start; i < nlines; i++) {
	if (ps_list[i].sflag) continue;
	if (ppid == ps_list[i].ppid) {
	  /* clear memory areas */
	  memset(stat_line, '\0', MAX_STATUS_LINE_LEN);
	  memset(indent, '\0', MAX_SUB_LEVELS);
	  memset(pref_str, '\0', 20 + MAX_SUB_LEVELS);

	  ps_list[i].sflag = 1;
	  /* child is found */
	  for (j = 0; j < depth && j < MAX_SUB_LEVELS; j++) strcat(indent,"+");
	  /* replace '+' with '*' to indicate there are more subprocess(es) if
	   * appropriate. */
	  if (depth > MAX_SUB_LEVELS) *(indent+strlen(indent)-1) = '*'; 

	  sprintf(pref_str, "%s %s",indent, time_str); 
	  sprintf(stat_line, "%-25.25s %-10.10s %-10.10s %-7d %-7d %-50.50s\n", 
			  pref_str, fromhost, ps_list[i].user, ps_list[i].pid, 
			  ps_list[i].ppid, ps_list[i].cmd);

	  /* only keep up to max_data_len chars */
	  if ((strlen(prog_info) + strlen(stat_line)) < max_data_len)
		strcat(prog_info, stat_line);
	  else 
		strncat(prog_info, stat_line, max_data_len - strlen(prog_info) - 1);

	  extract_ps_info_of_descentors(fromhost, ps_list, nlines, ps_list[i].pid, prog_info,
								 0,  depth+1, max_data_len, time_str);
	}	  
  }
} /* extract_ps_info_of_descentors */

/***************************************************************************/
/*                                                                         */
/*                      get_proc_index                                     */
/*                                                                         */
/***************************************************************************/
int get_proc_index(Process ps_list[], int nlines, int pid)
{
  int i;

  for (i = 0; i < nlines; i++) {
	if (ps_list[i].pid == pid) 
	  return i;
  }
  return -1;
}

/***************************************************************************/
/*                                                                         */
/*                      append_ps_bufs                                     */
/*                                                                         */
/***************************************************************************/
void append_ps_bufs(struct ps_info_rec *ps_bufs, char *top_level_buf,
					char *all_levels_buf, int max_data_len)
{
  if (ps_bufs == NULL || top_level_buf == NULL || all_levels_buf == NULL) 
	return;
/*
printf("top_level_buf- <%s>\n", top_level_buf);
printf("all_level_buf- <%s>\n", all_levels_buf);
*/
  if ( ps_bufs->top_level && 
	  (strlen(ps_bufs->top_level) + strlen(top_level_buf)) <  max_data_len) {
	strcat(ps_bufs->top_level, top_level_buf);
  }
  else if (ps_bufs->top_level) {
	strncat(ps_bufs->top_level, top_level_buf, max_data_len -
			strlen(ps_bufs->top_level) -1 );
  }
		
  if (ps_bufs->all_levels &&
	  (strlen(ps_bufs->all_levels) + strlen(all_levels_buf)) < max_data_len) {
	strcat(ps_bufs->all_levels, all_levels_buf);
  }
  else if (ps_bufs->all_levels) {
	strncat(ps_bufs->all_levels, all_levels_buf, max_data_len -
			strlen(ps_bufs->all_levels) - 1);
  }

} /* append_ps_bufs */

/***************************************************************************/
/*                                                                         */
/*                      extract_ps_info_for_prog                           */
/*                                                                         */
/***************************************************************************/
void extract_ps_info_for_prog(char *fromhost,
	  Process ps_list[], int nlines, char *prog,  struct ps_info_rec *ps_bufs,
							  ps_type type, int max_data_len, char *time_str)
{
  int i;
  char stat_line1[MAX_STATUS_LINE_LEN];
  int ancestor;
  int ind;

  if (fromhost == NULL || prog == NULL || ps_bufs == NULL || 
	  strlen(fromhost) == 0 || strlen(prog) == 0 ||
	  time_str == NULL) return;

  for (i = 0; i < nlines; i++) {
	if (ps_list[i].sflag) continue;  /* already processed */


	/* keeps going since there may be more than one executation of program */
	/*
printf("cmd = <%s>, prog = <%s>\n", ps_list[i].cmd, prog);
*/
	if (!cmd_match_prog(ps_list[i].cmd, prog)) continue;

	find_top_ancestor(ps_list, nlines, ps_list[i].ppid, prog, &ancestor);


	if (ancestor == -1)
	  ancestor = ps_list[i].pid;								  

	ind = get_proc_index(ps_list, nlines, ancestor);

	if (ind == -1) continue;
	if (ps_list[ind].sflag) continue;
	memset(stat_line1, '\0', MAX_STATUS_LINE_LEN);
	sprintf(stat_line1, "%-25.25s %-10.10s %-10.10s %-7d %-7d %-50.50s\n", 
	     time_str, fromhost, ps_list[ind].user, ps_list[ind].pid, 
		 ps_list[ind].ppid, ps_list[ind].cmd);
	ps_list[ind].sflag = 1;
	append_ps_bufs(ps_bufs, stat_line1, stat_line1, max_data_len);
	extract_ps_info_of_descentors(fromhost, ps_list, nlines, ps_list[ind].pid, 
						  ps_bufs->all_levels, 0,1, max_data_len, time_str);

	append_ps_bufs(ps_bufs, EMPTY_STR, "\n", max_data_len); /* separator */

  }


} /* extract_ps_info_for_prog */

/***************************************************************************/
/*                                                                         */
/*                      sort_ps_info                                       */
/*                                                                         */
/***************************************************************************/
void sort_ps_info(Process ps_list[], int nlines)
{
  int i, j;
  Process ps_item;

  /* sort using pid */
  for (i = 0; i < nlines; i++) {
	for (j = i+1; j < nlines; j++) {
	  if (ps_list[i].pid > ps_list[j].pid) {
		/* swap location */
		memcpy(&ps_item, &ps_list[i], sizeof(Process));
		memcpy(&ps_list[i], &ps_list[j], sizeof(Process));
		memcpy(&ps_list[j], &ps_item, sizeof(Process));
	  }
	}
  }

} /* sort_ps_info */

/***************************************************************************/
/*                                                                         */
/*                      free_ps_bufs                                       */
/*                                                                         */
/***************************************************************************/
void free_ps_bufs(struct ps_info_rec *rec)
{
  if (rec == NULL) return;
  if (rec->top_level != NULL) 	free(rec->top_level);
  if (rec->all_levels != NULL) free(rec->all_levels);
  
}

/***************************************************************************/
/*                                                                         */
/*                      free_ps_info_rec                                   */
/*                                                                         */
/***************************************************************************/
void free_ps_info_rec(struct ps_info_rec *rec)
{

  if (rec == NULL) return;
  free_ps_bufs(rec);
  free(rec);
} /* free_ps_info_rec */

/***************************************************************************/
/*                                                                         */
/*                      init_ps_info_rec                                   */
/*                                                                         */
/***************************************************************************/
void init_ps_info_rec (struct ps_info_rec *rec,int data_len)
{
  if (rec == NULL || data_len < 0) return;

  rec->top_level = (char *) calloc(data_len, sizeof(char)); 
  if (rec->top_level == NULL) {
	perror("calloc ps buf");
	exit_system(NULL, -1);
  }
  rec->all_levels = (char *) calloc(data_len, sizeof(char)); 
  if (rec->all_levels == NULL) {
	perror("calloc ps buf");
	exit_system(NULL, -1);
  }
  memset(rec->top_level, '\0', data_len);
  memset(rec->all_levels, '\0', data_len);
}

/***************************************************************************/
/*                                                                         */
/*                      new_ps_info_rec                                    */
/*                                                                         */
/***************************************************************************/
struct ps_info_rec * new_ps_info_rec(int data_len)
{
  struct ps_info_rec *rec;

  rec = (struct ps_info_rec *) calloc(1, sizeof(struct ps_info_rec));
  if (rec == NULL) {
	perror("calloc ps rec");
	exit_system(NULL, -1);
  }
								 
  init_ps_info_rec(rec, data_len);
  return rec;
} /* new_ps_info_rec */

/***************************************************************************/
/*                                                                         */
/*                      format_ps_data                                     */
/*                                                                         */
/***************************************************************************/
void format_ps_data(char *fromhost, Process ps_list[], int nlines, 
					char *match_items[], int nitems, 
					struct ps_info_rec *ps_info, struct ps_info_rec *ps_bufs, 
					int max_data_len, ps_type type, char *time_str)
{
  int i;
  char *item;
  char tmpbuf[MAX_CMD_LEN];
  
  if (fromhost == NULL || ps_info == NULL || ps_info->top_level == NULL ||
	  ps_info->all_levels == NULL || ps_bufs == NULL || 
	  ps_bufs->top_level == NULL || ps_bufs->all_levels == NULL ||
	  time_str == NULL) return;

  if (type == PS_ALL_PROGS) nitems = nlines;
  for (i = 0; i < nitems; i++) {

	if (type == PS_ALL_PROGS) {
	  if (ps_list[i].sflag == 1 || ps_list[i].cmd == NULL ||
		  strlen(ps_list[i].cmd) == 0) continue;
	  memset(tmpbuf, '\0', MAX_CMD_LEN);
	  strcpy(tmpbuf, ps_list[i].cmd);
      if (strchr(tmpbuf, ' ') == NULL) /* cmd with no argument  */
		item = tmpbuf;
      else {
		/* get first string from the command line only */
		item = strchr(tmpbuf, ' ');
		*item = '\0';
		item = tmpbuf;
	  }
	}
	else {
	  if (match_items[i] == NULL) continue;
	  item = (char *) match_items[i];
	}

    if (item == NULL || strlen(item) == 0) continue;

    if (ps_bufs && ps_bufs->top_level)
	  memset(ps_bufs->top_level, '\0', max_data_len);
    if (ps_bufs->all_levels)
	  memset(ps_bufs->all_levels, '\0', max_data_len);
	/*
fprintf(stderr, "item: <%s>\n", item);
*/
	extract_ps_info_for_prog(fromhost, ps_list, nlines, item, 
							 ps_bufs, type, max_data_len, time_str);

	/* only keep max_data_len of info */

	append_ps_bufs(ps_info, ps_bufs->top_level,
				  ps_bufs->all_levels , max_data_len);

  }

} /* format_ps_data */

/***************************************************************************/
/*                                                                         */
/*                      remove_leading_blanks                              */
/*                                                                         */
/***************************************************************************/
char * remove_leading_blanks(char *s)
{
  /* remove leading blanks */
  while (s) {
	if (*s == ' ') s++; 
	break;
  }
  return s;
}

/***************************************************************************/
/*                                                                         */
/*                      remove_pathname                                    */
/*                                                                         */
/***************************************************************************/
char *remove_pathname(char *s)
{
  char *new_s;
  
  if (s == NULL) return NULL;

  if (strchr(s,'/') != NULL) 
	new_s = strrchr(s, '/') + 1; /* skip pathname */
  else
	new_s = s;                  /* no pathname to skip */

  return new_s;
}

/***************************************************************************/
/*                                                                         */
/*                      process_ps_info                                    */
/*                                                                         */
/***************************************************************************/
void process_ps_info(char *fromhost, char *msg, 
					 struct ps_info_rec *sp_ps_info,
					 struct ps_info_rec *ap_ps_info)
{

  int i;
  int n = 0;
  char *token;
  Process ps_list[MAX_PS_ITEMS];
  struct ps_info_rec sp_ps_bufs;   /* selectd progs */
  struct ps_info_rec ap_ps_bufs;   /* all progs */
  char *tmp[1];
  time_t curr_time;
  char *time_str;
  struct host_info_rec *host_info;

  if (fromhost == NULL || sp_ps_info == NULL || ap_ps_info == NULL ||
	  msg == NULL || strlen(msg) == 0) return;

  if (!host_exists(Gselected_hosts, fromhost)) return;

  if (strstr(msg, PS_INFO_KEY) == NULL) return;
  /* skip key word */
  msg = strstr(msg, PS_INFO_KEY) + strlen(PS_INFO_KEY);

  curr_time = get_curr_time();
  time_str = ctime(&curr_time);
  if (time_str == NULL) {
	fprintf(stderr, "unexepected NULL time_str\n");
	exit_system(NULL, -1);
  }
  /* skip day and remove trailing year from time_str */
  time_str += 4;
  *(time_str+16) = '\0';

  /*
printf("msg = <%s>\n", msg);
*/

  memset(ps_list, '\0', MAX_PS_ITEMS * sizeof(Process));

  /* ingest ps info into ps_list */
  token = strtok(msg, " ");

  strncat(ps_list[n].user, token, 8);

  while (token) {
	
	token = strtok(NULL, " "); if (token == NULL) break;
	ps_list[n].pid = atol(token);

	token = strtok(NULL, " "); if (token == NULL) break;
	ps_list[n].ppid = atol(token);

	token = strtok(NULL, "\n"); if (token == NULL) break;
	/* remove leading blanks  from command string */

	token = remove_leading_blanks(token);
	strncat(ps_list[n].cmd, token, MAX_CMD_LEN-1);

	n++; /* Another line? */
	token = strtok(NULL, " "); if (token == NULL) break;
	strncat(ps_list[n].user, token, 8);
  }
  /* sort ps_list in ascending order of pid.  */
  /* ps_list is modified by 'sort_ps_info' */
  sort_ps_info(ps_list, n); 
  init_ps_info_rec(&sp_ps_bufs, MAX_PS_BUF_LEN);

  /* format for selected programs */
  format_ps_data(fromhost, ps_list, n, Gselected_progs, MAX_PROGS, sp_ps_info,
				 &sp_ps_bufs, MAX_PS_BUF_LEN, PS_SELECTED_PROGS, time_str);
  free_ps_bufs(&sp_ps_bufs);

  /* reset ps list's flags */
  for (i = 0; i < n; i++) {
	ps_list[i].sflag = 0;
  }
  /* format for network top */
  tmp[0] = NULL;
  init_ps_info_rec(&ap_ps_bufs, MAX_PS_BUF_LEN);

  format_ps_data(fromhost, ps_list, n, tmp, 0,  ap_ps_info, &ap_ps_bufs, 
				MAX_PS_BUF_LEN, PS_ALL_PROGS, time_str);

  free_ps_bufs(&ap_ps_bufs);

  /* update host rec's  ps_data  */
  host_info = get_host_info(fromhost);
  if (host_info == NULL ) return;
  if (host_info->ps_info[SELECTED_PROGS_INDX]) {
	strcpy(host_info->ps_info[SELECTED_PROGS_INDX]->top_level, 
		   sp_ps_info->top_level);
	strcpy(host_info->ps_info[SELECTED_PROGS_INDX]->all_levels, 
		   sp_ps_info->all_levels);
  }
  if (host_info->ps_info[NETWORK_TOP_INDX]) {
	strcpy(host_info->ps_info[NETWORK_TOP_INDX]->top_level, 
		   ap_ps_info->top_level);
	strcpy(host_info->ps_info[NETWORK_TOP_INDX]->all_levels, 
		   ap_ps_info->all_levels);
  }

} /* process_ps_info */

/***************************************************************************/
/*                                                                         */
/*                      update_curr_file_status_report                     */
/*                                                                         */
/***************************************************************************/
void update_curr_file_status_report()
{

  extern char *curr_selected_prod_groups[];
  FD_logfile_form *form;


  form = job_grouping_form;
  if (form == NULL || form->browser == NULL || form->logfile_form == NULL) 
	return;
  if (!form->logfile_form->visible) return;
  fl_deactivate_object(form->browser);
  fl_freeze_form(form->logfile_form);

  update_file_status_report_form(form->logfile_form, form->browser, 
		  curr_selected_prod_groups, MAX_ITEMS, STAT_INFO_FILE_RNG);

  fl_unfreeze_form(form->logfile_form);
  fl_activate_object(form->browser);

} /* update_curr_file_status_report */


/***************************************************************************/
/*                                                                         */
/*                      update_prod_group_form                            */
/*                                                                         */
/***************************************************************************/
void update_prod_group_form(int reload_form)
{
  /* if reload_form is 1, reload form; otherwise, update form */


  if (product_grouping_form == NULL || product_grouping_form->browser == NULL ||
	  product_grouping_form->gms_form == NULL) return;
  if (!product_grouping_form->gms_form->visible) return;
  fl_deactivate_object(product_grouping_form->browser);
  fl_freeze_form(product_grouping_form->gms_form);

/*
  fl_deactivate_form(product_grouping_form->gms_form);
*/

  load_items_to_browser(product_grouping_form->gms_form,
						product_grouping_form->browser, CURR_PROD, 
						curr_selected_tape_group, CLEAR);

  fl_unfreeze_form(product_grouping_form->gms_form);
  fl_activate_object(product_grouping_form->browser);

/*
  fl_activate_form(product_grouping_form->gms_form);
*/
} /* update_prod_group_form */

/***************************************************************************/
/*                                                                         */
/*                     update_log_file_status_report                       */
/*                                                                         */
/***************************************************************************/
void update_log_file_status_report()
{
  extern char *log_selected_items[];
  FD_logfile_form *form;


  form = logfile_report_form;
  if (form == NULL || form->browser == NULL || form->logfile_form == NULL) 
	return;
  if (!form->logfile_form->visible) return;
  fl_deactivate_object(form->browser);
  fl_freeze_form(form->logfile_form);

/*
  fl_deactivate_form(form->logfile_form);
*/


  update_file_status_report_form(form->logfile_form,form->browser, 
		  log_selected_items, MAX_ITEMS, LOG_FILE_RNG);

  fl_unfreeze_form(form->logfile_form);
  fl_activate_object(form->browser);


/*
  fl_activate_form(form->logfile_form);
*/
} /* update_log_file_status_report */

/***************************************************************************/
/*                                                                         */
/*                     update_summary_report_form                             */
/*                                                                         */
/***************************************************************************/
void update_summary_report_form()
{
  
  char label[MAX_LABEL_LEN];

  if (summary_report_form == NULL || summary_report_form->report_form == NULL ||
	  summary_report_form->browser == NULL) return;
  if (!summary_report_form->report_form->visible) return;
  fl_deactivate_object(summary_report_form->browser);
/*
  fl_deactivate_form(summary_report_form->report_form);
*/
  fl_freeze_form(summary_report_form->report_form);
  memset(label, '\0', MAX_LABEL_LEN);

  if (selected_summary_level_type & PRODUCT_SUMMARY) {
	
	sprintf(label, "    Host    TapeID      Level     Product    Device     Date   Time    Runtime");
  }
  else if (selected_summary_level_type &  LEVEL_SUMMARY) {
	sprintf(label, "    TapeID     Level   Date     Time      Runtime");
  }
  else if (selected_summary_level_type & TAPE_SUMMARY) {
	sprintf(label, "    TapeID        Date     Time     Runtime");
  }
  fl_set_object_label(summary_report_form->label, label);
  /* freeze and clear form are done in when loading */
  load_items_to_browser(summary_report_form->report_form,
						summary_report_form->browser, SUMMARY_INFO, 
						(char *)  &selected_summary_level_type, CLEAR);

  fl_unfreeze_form(summary_report_form->report_form);
  fl_activate_object(summary_report_form->browser);

/*
  fl_activate_form(summary_report_form->report_form);
*/
} /* update_summary_report_form */


/***************************************************************************/
/*                                                                         */
/*                     update_gv_levels_form                            */
/*                                                                         */
/***************************************************************************/
void update_gv_levels_form()
{
  char label[MAX_LABEL_LEN], line[MAX_LINE_LEN];
  int ll, p;
  FL_OBJECT *browser;

  if (gv_levels_form == NULL || gv_levels_form->gv_levels_form == NULL ||
	  gv_levels_form->level_browser == NULL ||
	  gv_levels_form->available_progs_browser == NULL) return;
  if (!gv_levels_form->gv_levels_form->visible) return;

  browser = gv_levels_form->level_browser;
  fl_deactivate_object(browser);
  fl_freeze_form(gv_levels_form->gv_levels_form);
  memset(label, '\0', MAX_LABEL_LEN);
  strcpy(label, " Level     Program");
  fl_set_object_label(gv_levels_form->label, label);
  fl_clear_browser(browser);
  /* Load programs from all levels to level browser. */
  for (ll = 0; ll < MAX_GV_LEVELS_NUM; ll++) {
	for (p=0; p < MAX_PROGS_PER_LEVEL; p++) {
	  if (level_info_table[ll].prog_info[p].prog == NULL) continue;
	  memset(line, '\0', MAX_LINE_LEN);
	  sprintf(line, "  %d          %s", ll+1, level_info_table[ll].prog_info[p].prog);
	  fl_add_browser_line(browser, line);
	}
  }

  /* Load available programs to available progs browser. */
  load_items_to_browser(gv_levels_form->gv_levels_form,
						gv_levels_form->available_progs_browser, 
						AVAILABLE_PROGS, "", CLEAR);
  fl_unfreeze_form(gv_levels_form->gv_levels_form);
  fl_activate_object(gv_levels_form->level_browser);
} /* update_gv_levels_form */

/***************************************************************************/
/*                                                                         */
/*                     update_ps_form                                      */
/*                                                                         */
/***************************************************************************/
void update_ps_form()
{
  /* show ps_form */
  show_ps_form();

} /* update_ps_form */


/***************************************************************************/
/*                                                                         */
/*                     stat_is                                             */
/*                                                                         */
/***************************************************************************/
stat_type_t stat_is(char *stat_str)
{
  /* Convert status in string to stat_type_t. */

  if (stat_str == NULL || strlen(stat_str) == 0) return NOT_DEFINED;
  if (strstr(stat_str, SUCCESS_STR)) return SUCCESS;
  if (strstr(stat_str, FAILED_STR)) return FAILED;
  if (strstr(stat_str, ABORT_STR)) return ABORT;
  if (strstr(stat_str, WARNING_STR)) return WARNING;
  if (strstr(stat_str, RUNNING_STR)) return RUNNING;
  if (strstr(stat_str, FINISHED_STR)) return FINISHED;
  return NOT_DEFINED;
}

	
/***************************************************************************/
/*                                                                         */
/*                     logfile_rec *get_active_logfile                     */
/*                                                                         */
/***************************************************************************/
struct logfile_rec *get_active_logfile(struct graphic_status_rec *gstat, char *fname)
{
  /* returns logfile which is currently active having name as fname */
  struct logfile_rec *logfile;
  
  if (gstat == NULL || fname == NULL) return NULL;
  logfile = gstat->logfiles;
  while (logfile) {
	if (strcmp(logfile->name, fname) == 0 && logfile->active == 1) 
	  return logfile;
	logfile = logfile->next;
  }
  return NULL;
}

/***************************************************************************/
/*                                                                         */
/*                     show_logfile_list                                   */
/*                                                                         */
/***************************************************************************/
void show_logfile_list(struct logfile_rec *loglist)
{
  struct logfile_rec *log;

  if (loglist == NULL) return;
  log = loglist;
  printf("<<<\n");
  while (log) {
	fprintf(stderr, "logname = <%s>\n",log->name);

	printf("logname = <%s>, sdate <%s>, stime <%s> <%ld>, edate <%s>, etime <%s><%ld>\n",
		   log->name, log->sdate, log->stime, log->stime_item, log->edate, log->etime, log->etime_item);

	log = log->next;
  }
  fprintf(stderr, ">>>\n");
} /* show_logfile_list */

/***************************************************************************/
/*                                                                         */
/*                     show_logfile_list_backward                          */
/*                                                                         */
/***************************************************************************/
void show_logfile_list_backward(struct logfile_rec *loglist)
{
  struct logfile_rec *log;

  if (loglist == NULL) return;
  /* go to the end of list */
  log = loglist;
  while (log) {
	if (log->next == NULL) break;
	log = log->next;
  }
  /* walk backward and print it */
  printf("<<<\n");
  while (log) {
/*
	printf("logname = <%s>, sdate <%s>, stime <%s>, edate <%s>, etime <%s>\n",

		   log->name, log->sdate, log->stime, log->edate, log->etime);
*/
	printf("logname = <%s>\n",log->name);
	log = log->prev;
  }
  printf(">>>\n");
} /* show_logfile_list_backward */



/***************************************************************************/
/*                                                                         */
/*                     is_an_earlier_run                                   */
/*                                                                         */
/***************************************************************************/
int is_an_earlier_run(struct logfile_rec *log1, struct logfile_rec *log2)
{
  int rc;

  /* returns 1 if log1 has occured ealier or at the same time as log2. 
   * returns 0 otherwise.
   */
  if (log1 == NULL || log2 == NULL) return 0;

  /* use start date/time and end date/time to determine if jobs from log1 have
   * started earlier than jobs from log2
   */
  rc = difftime(log1->stime_item, log2->stime_item);
  /*
printf ("log1 sdate <%s>, stime <%s>, log2 sdate <%s>, stime <%s>\n",
		  log1->sdate, log1->stime, log2->sdate, log2->stime);
		  */
  if (rc < 0)
	return 1;
  if (rc > 0)
	return 0;
  /*
printf ("log1 edate <%s>, etime <%s>, log2 edate <%s>, etime <%s>\n",
		  log1->edate, log1->etime, log2->edate, log2->etime);
		  */
  rc = difftime(log1->etime_item, log2->etime_item);
  if (rc > 0) return 0;
  return 1;

} /* is_an_earlier_run */

/***************************************************************************/
/*                                                                         */
/*                     add_logfile_to_list                                 */
/*                                                                         */
/***************************************************************************/
int add_logfile_to_list(struct logfile_rec **logfile_list,
						 struct logfile_rec *logfile)
{
  /* returns 1 upon successful completion; -1 otherwise.
   */
  struct logfile_rec *log, *tmplog, *logfiles;
  int rc, rc1, rc2;


  /* add in the order of name, start date/time, and end date/time. */
  if (logfile == NULL) return -1;
/*
printf("enter add logfile to list log = <%s> \n", logfile->name);
*/

  logfiles = *logfile_list;
  if (logfiles == NULL){ 

	/* first elem. in the list */
	*logfile_list = logfile;
	(*logfile_list)->prev = NULL;
	(*logfile_list)->next = NULL;
/*
printf("exit add logfile to list \n");
*/
	return 1;
  }	
  log = logfiles;
  tmplog = NULL;


  while (log != NULL) {

	/* sort by name start date/time and end date/time */
	if ((rc = strcmp(log->fromhost, logfile->fromhost)) < 0 ||
		(rc == 0 && (rc1 = strcmp(log->tapeid, logfile->tapeid)) < 0) ||
		(rc1 == 0 && (rc2 = strcmp(log->prog, logfile->prog)) < 0) ||
		(rc2 == 0 && !is_an_earlier_run(log, logfile)) ){

	  tmplog = log;
	  log = log->next;
	}
	else
	  break;
  }
/*
printf("#tmplog name = <%s>, logfilename = <%s>\n", tmplog->name, logfile->name);
*/
  if (tmplog == NULL) {
	/* Add to the beginning of list */
	logfile->next = log;
	logfile->prev = NULL;
	log->prev = logfile;
	*logfile_list = logfile;
	return 1;
  }
	  
  tmplog->next = logfile;
  logfile->prev = tmplog;

  if (log != NULL) {
	/* logfile should be before log */
	logfile->next = log;
	log->prev = logfile;
  }
  else {
	logfile->next = NULL;
  }
/*
printf("exit add logfile to list \n");
*/
  return 1;

} /* add_logfile_to_list */

/***************************************************************************/
/*                                                                         */
/*                     create_logfile_name                                 */
/*                                                                         */
/***************************************************************************/
void create_logfile_name(char *host, char *tapeid, char *prod, char *filename)
{
  if (host == NULL || strlen(host) == 0 ||
	  tapeid == NULL || strlen(tapeid) == 0 ||
	  prod == NULL || strlen(prod) == 0 ||
	  filename == NULL) 
	return;
  sprintf(filename, "%s.%s.%s.log", host, tapeid, prod);

} /* create_logfile_name */


/***************************************************************************/
/*                                                                         */
/*                     process_del_job_msg                                 */
/*                                                                         */
/***************************************************************************/
int process_del_job_msg(char *buf)
{
  char logfile_name[MAX_FILENAME_LEN];
  char host[MAX_HOSTNAME_LEN];
  char tapeid[MAX_TAPEID_LEN];
  char stime_item_str[MAX_NAME_LEN];
  char prod[MAX_PRODNAME_LEN];
  char *selected_items[1];
  int removed = 0;
  time_t stime_item;
  double host_diff_time;
  
  if (buf == NULL || strlen(buf) == 0 || gstatus_info == NULL) return removed;
  
  /* clear buffers */
  memset(logfile_name, '\0', MAX_FILENAME_LEN);
  memset(host, '\0', MAX_HOSTNAME_LEN);
  memset(tapeid, '\0', MAX_TAPEID_LEN);
  memset(prod, '\0', MAX_PRODNAME_LEN);
  memset(stime_item_str, '\0', MAX_NAME_LEN);
  /*
fprintf(stderr, "buf: %s\n", buf);
*/
  /* buf contains:keyword del job host stime_secs etime_secs  tapeid device prod */
  if (sscanf(buf, "%*s %*s %*s %s %ld %*s %s %*s %s", host, &stime_item,
			 tapeid, prod) != 4) {
	fprintf(stderr, "WARNING: format of del job msg <%s> is unexpected. Ignore.\n", buf);
	return removed;
  }
  create_logfile_name(host, tapeid, prod, logfile_name);
  if (strlen(logfile_name) == 0) return removed;
  
  selected_items[0] = logfile_name;

  /* Adjust host's time to local host's clock */
  host_diff_time = get_host_diff_time(host);
  memset(stime_item_str, '\0', MAX_NAME_LEN);
  sprintf(stime_item_str, "%ld", (long) (stime_item - host_diff_time));
  /*
fprintf(stderr, "logfile_name: %s, ad time: %s, diff:%f, t: %ld\n", selected_items[0], stime_item_str, host_diff_time, stime_item);
*/
  stime_item -= (time_t) host_diff_time;

  /* remove info from list */
  removed = remove_info_for_selected_items(STAT_INFO, selected_items, 1, 
										   &stime_item, NULL);

  return removed;
  
} /* process_del_job_msg */


/***************************************************************************/
/*                                                                         */
/*                         new_tape_grouping_rec                               */
/*                                                                         */
/***************************************************************************/
struct tape_grouping_rec *new_tape_grouping_rec(char *tapeid, char *host, 
										struct logfile_rec *logfile)
{
  struct tape_grouping_rec *grp;

  grp = (struct tape_grouping_rec *) calloc(1, sizeof(struct tape_grouping_rec));
  if (grp == NULL) {
	perror("calloc struct tape_grouping_rec");
	exit_system(NULL, -1);
  }
  strcpy(grp->host, host);
  strcpy(grp->tapeid, tapeid);
  grp->last_logfile = logfile;
  grp->stat = logfile->stat;
  grp->finished = logfile->finished;
  grp->active = logfile->active;
  memcpy(&(grp->etime), &(logfile->etime_item), sizeof(time_t));
  memcpy(&(grp->stime),&(logfile->stime_item), sizeof(time_t));
  grp->next = NULL;
  return grp;
} /* new_tape_grouping_rec */


/***************************************************************************/
/*                                                                         */
/*                         get_first_updated_logfile                         */
/*                                                                         */
/***************************************************************************/
struct logfile_rec *get_first_updated_logfile(struct logfile_rec *logfiles,
											char *host, char *tapeid)
{
  /* Get first updated logfile for tape grouping.
   * 'logfiles' has logfiles grouped by host and tapeid.
   */
  struct logfile_rec *logfile, *first_logfile  = NULL;

  if (logfiles == NULL || host == NULL || tapeid == NULL)
	return NULL;

  logfile = logfiles;

  while (logfile) {
	if (strcmp(logfile->fromhost, host) == 0 && 
		strcmp(logfile->tapeid, tapeid) == 0) {

	  if (first_logfile == NULL || 
		  difftime(logfile->etime_item, first_logfile->etime_item) < 0)
		first_logfile = logfile;
	}
	else {
	  if (first_logfile)
		break;
	}
	logfile = logfile->next;
  }
  if (first_logfile)
	fprintf(stderr, "firstlogfile: %s\n", first_logfile->name);
  return first_logfile;
} /* get_first_updated_logfile */


/***************************************************************************/
/*                                                                         */
/*                         get_last_updated_logfile                         */
/*                                                                         */
/***************************************************************************/
struct logfile_rec *get_last_updated_logfile(struct logfile_rec *logfiles,
											char *host, char *tapeid)
{
  /* Get last updated logfile from tape grouping.
   * 'logfiles' has logfiles grouped by host and tapeid.
   */
  struct logfile_rec *logfile, *last_logfile  = NULL;

  if (logfiles == NULL || host == NULL || tapeid == NULL)
	return NULL;

  logfile = logfiles;

  while (logfile) {
	if (strcmp(logfile->fromhost, host) == 0 && 
		strcmp(logfile->tapeid, tapeid) == 0) {

	  if (last_logfile == NULL || 
		  difftime(logfile->etime_item, last_logfile->etime_item) > 0)
		last_logfile = logfile;
	}
	/*
	else {

	  if (last_logfile)
		break;
	}
	*/
	logfile = logfile->next;
  }

  return last_logfile;
} /* get_last_updated_logfile */


/***************************************************************************/
/*                                                                         */
/*                          update_tape_groups                              */
/*                                                                         */
/***************************************************************************/
void update_tape_groups(struct logfile_rec *logfiles,
					   struct tape_grouping_rec **tape_groups, 
					   struct logfile_rec *logfile, 
					   char *tapeid, char *host)
{

  struct tape_grouping_rec *grp, *last_grp=NULL;


  if (host == NULL || logfile == NULL || tapeid == NULL) return;

  /* Tape group doesnot contain empty logfile */
  if (logfile->nentries < 1) return;

  grp = *tape_groups;

  while (grp != NULL) {
	if (strcmp(host, grp->host) == 0 &&
		strcmp(tapeid, grp->tapeid) == 0) {
	  /*
 fprintf(stderr, "host = %s, tapeid = %s\n", grp->host, grp->tapeid);
 */
	  /* Update group's status */
	  get_tape_grouping_stat(grp->host, grp->tapeid, &(grp->stat),
							 &(grp->finished), &(grp->active));

	  /* Set global flag here.  Reset may be done when updating form */
	  if ((something_is_running == 0 && grp->stat == RUNNING) &&
		  grp->active && !(grp->stat & RUNNING_BUT_INACTIVE)) {
		something_is_running = 1;
		set_alarm_to_update_forms();	
	  }

	  /* change the last update logfile */
	  grp->last_logfile = logfile;
	  memcpy(&(grp->etime), &(logfile->etime_item), sizeof(time_t));
	  return;
	}
	last_grp = grp;
	grp = grp->next;
  }
  /* create and add new group to list */
  grp = new_tape_grouping_rec(tapeid, host, logfile);

  /* add grp to list */
  if (last_grp == NULL) {
	/* list was empty */
	*tape_groups = grp;
  }
  else {
	/* add at the end of list */
	last_grp->next = grp;
  }
} /* update_tape_groups */


/***************************************************************************/
/*                                                                         */
/*                             process_host_curr_time                      */
/*                                                                         */
/***************************************************************************/
void process_host_curr_time(char *fromhost, char *buf)
{
  char *nh_time_str;  /* neighbor host */
  time_t nh_time;
  time_t curr_time;
  struct host_info_rec *host_info;
  char msg[MAX_MSG_LEN];


  if (fromhost == NULL || strlen(fromhost) == 0 || 
	  buf == NULL || strlen(buf) == 0 || 
	  strstr(buf, HOST_CURR_TIME_KEY) == NULL) return;

  /* If msg came from the local host, ignore. */
  if (strcmp(fromhost, local_host) == 0) goto REQUEST;

  host_info = get_host_info(fromhost);
  if (host_info == NULL) {
	fprintf(stderr, "Error: There is no host info record for %s. Ignore set host curr time.\n", fromhost);
	return;
  }
  /* Skip key word */

  nh_time_str = buf + strlen(HOST_CURR_TIME_KEY);
  nh_time_str = remove_leading_blanks(nh_time_str);

  curr_time = local_curr_time;
  if (nh_time_str)
	nh_time = (time_t) atoi(nh_time_str);
  else nh_time = curr_time;

  host_info->diff_time = difftime(nh_time, curr_time);
  /*
fprintf(stderr, "buf: %s, host: %s\n", buf, fromhost);
fprintf(stderr, "nhtime_str: %s, nhtime: %ld, local_currtme: %ld, diffTime: %ld\n", nh_time_str, (long) nh_time, (long) curr_time, (long) host_info->diff_time);
*/
REQUEST:
  /* send msg to monitorps. This tells monitorps (neigbor) to start sending 
   * status messages to gms.
   */
  memset(msg, '\0', MAX_MSG_LEN);
  sprintf(msg, "%s %s.%d %s", RESET_STR, local_host, gms_portnum, fromhost); 
  if (send_msg(local_host, msg) < 0)
	exit_system(NULL, -1);

} /* process_host_curr_time */

/***************************************************************************/
/*                                                                         */
/*                     process_status_info                                 */
/*                                                                         */
/***************************************************************************/
int process_status_info(char *fromhost, char *buf)
{
  /* extract status info from buf and add it to gstatus_info->logfiles.
   * This routine ignores data from non-selected neighbors and of 
   * non-selected programs.
   *
   * buf is a list of the following messages:
   *     status: jobnum time tapeid  tape_device program infile status
   * Returns 0 if nothing from buf is kept; otherwise, 1.
   */
  char *prod,
       *status_str,
       *tapeid, *infile, date_str[MAX_STR_LEN], *time_str, *tape_dev, *jobnum,
       logfile_name[MAX_FILENAME_LEN];
  int slen;
  char tmp_time_str[MAX_STR_LEN];
  stat_type_t stat;
  struct logfile_rec *logfile;
  char *key;
  int rc = 0, kept = 0;
  int need_to_add_logfile_to_list;
  time_t time_item, save_time;
  double host_diff_time;

  if (fromhost == NULL || strlen(fromhost) == 0 || 
	  buf == NULL || (slen = strlen(buf) == 0) || gstatus_info == NULL) 
	return 0;

  /* only keep status info for local host or neighbors */
  if (!host_exists(Gselected_hosts, fromhost)) return 0;

/*
  oldmask = gms_block_ints();
 */
  /* extract jobnum time, tapeid, tapedevice, prod, infile, status */
  key = strtok(buf, ":");

  while (key) {
	need_to_add_logfile_to_list = 0;
	if (strncmp(key, STATUS_INFO_KEY, strlen(STATUS_INFO_KEY) - 1) != 0) 
	  return kept;
	/* init pointers */
	jobnum = NULL;
	time_str = NULL;
	tapeid = NULL;
	tape_dev = NULL;
	prod = NULL;
	infile = NULL;
	status_str = NULL;
	/* check for correct key word, only check up to ':', return if not 
	 * the right key.*/

	if ((jobnum = strtok(NULL, " ")) == NULL) break;
	if ((time_str = strtok(NULL, " ")) == NULL) break;
	if ((tapeid = strtok(NULL, " ")) == NULL) break;
	if ((tape_dev= strtok(NULL, " ")) == NULL) break;
	if ((prod = strtok(NULL, " ")) == NULL) break;
	if ((infile = strtok(NULL, " ")) == NULL) break;
	if ((status_str = strtok(NULL, "\n")) == NULL) break;
	/* check for invalid data--skip to the next one if invalid */
	if (strstr(jobnum, STATUS_INFO_KEY) ||
		strstr(time_str, STATUS_INFO_KEY) ||
		strstr(prod, STATUS_INFO_KEY) ||
		strstr(tapeid, STATUS_INFO_KEY) ||
		strstr(tape_dev, STATUS_INFO_KEY) ||
		strstr(infile, STATUS_INFO_KEY) ||
		strstr(status_str, STATUS_INFO_KEY)) 
	  goto READ_NEXT;  

	kept = 1;
	if (write_to_file) {
	  fprintf(test_file, "jobnum = <%s>, time = <%s>,prod = <%s>, tapeid - <%s>, infile = <%s>, statstr = <%s>\n",
			  jobnum, time_str,prod, tapeid, infile, status_str);
	  fflush(test_file);
	}
	/*
	fprintf(test_file, "jobnum = <%s>, time = <%s>,prod = <%s>, tapeid - <%s>, infile = <%s>, statstr = <%s>\n",
			  jobnum, time_str,prod, tapeid, infile, status_str);
			  */

	/* skip data message if its status is undefined--possible invalid format */
	stat = stat_is(status_str);
	if (stat == NOT_DEFINED)
	  goto READ_NEXT;

	/* add prog to available program list */
	if (add_new_prog(available_progs, prod) > 0) {

	  /* update affected forms */
	  update_forms_affected_by_prog_change(AVAILABLE_PROGS, 0);
	}


	/* only keep data for selected programs */
	if (!prog_exists(Gselected_progs, prod)) {
	  /* go to next line */
	  goto READ_NEXT;
	}

	/* truncate tapeid and prod to max size */
	if (strlen(tapeid) >= MAX_TAPEID_LEN)
	  *(tapeid+MAX_TAPEID_LEN-1) = '\0';
	if (strlen(prod) >= MAX_PRODNAME_LEN)
	  *(prod+MAX_PRODNAME_LEN-1) = '\0';

	
	gstatus_info->refresh_form = 1;

	/* add info to gstatus_info */
	memset(logfile_name, '\0', MAX_FILENAME_LEN);
	create_logfile_name(fromhost, tapeid, prod, logfile_name);	

	logfile = get_active_logfile(gstatus_info, logfile_name);

	if (logfile == NULL) {
	 /* logfile not exists, create new logfile and add to list. */
	  logfile = new_logfile_rec(logfile_name, fromhost, tapeid, prod);
	  if (logfile == NULL) { 
		fprintf(stderr, "logfile is NULL 1.\n");
		exit_system(NULL, -1);
	  }
	  strcpy(logfile->tape_dev, tape_dev);
	  /*
	  if (add_logfile_to_list(&(gstatus_info->logfiles), logfile))
		  gstatus_info->nlogfiles++;
		  */
	  /* status forms need to be reload */
	  load_status_forms = 1;  
	  need_to_add_logfile_to_list = 1;
	}

	/* if processing of this prod and tapeid is finished, set its status
	 * to finished.  
	 * Time from this message is used for logfile's end time .*/

	if (stat== FINISHED || stat == ABORT) { 
/* We assume jobs come in the right order.
   Set this logfile to not active, so no more items will be added to this
    logfile.

  logfile->active = 0; 
 */
	  if (stat == FINISHED)
		logfile->finished = 1;
	  logfile->active = 0;
	  logfile->stat |= stat;
	  logfile->need_refresh_chart = 1;
	}

	/* convert time in secs string to time_t */
	time_item = (time_t) atoi(time_str);
	save_time = time_item;
	/*
fprintf(stderr, "origtime:%ld, %s\n", (long) time, ctime(&time));
*/
	/* Adjust host's time to local host's clock */
	host_diff_time = get_host_diff_time(fromhost);
	time_item -= host_diff_time;
	memset(date_str, '\0', MAX_STR_LEN);
	memset(tmp_time_str, '\0', MAX_STR_LEN);

	/* convert time string to date and hhmmss */
	time_secs2strs(time_item, tmp_time_str, date_str);
	/*
	{
time_t curr_time;
curr_time = get_curr_time();
fprintf(stderr, "expected hdiff time: %ld, currtime-time: %ld,  adjtime_str:%s, date: %s, currtime: %s\n",
(long) host_diff_time, (long) difftime(save_time, curr_time), tmp_time_str, date_str, ctime(&curr_time));
	}
*/
	time_str = tmp_time_str;   /* hh:mm:ss */

	/* add file entry to logfile record. If status is 'finished', will only
	 * update end time 
	 */
	rc = add_entry_to_log_rec(logfile, jobnum, time_item, time_str, date_str,
							  infile, stat);

	if (need_to_add_logfile_to_list) {
	  /* add_logfile_to_list must be after add_entry_to_log_rec */
	  if (add_logfile_to_list(&(gstatus_info->logfiles), logfile))
		gstatus_info->nlogfiles++;
	}

	if (rc == 1) {
	  logfile->need_refresh_chart = 1;
	}
	  
	else if (rc == 2) {
	  /* Rerun has occurred.  create a 
	   * new logfile rec for its new execution and mark the last logfile 
	   * as no longer active.
	   */
	  logfile->active = 0;
	  logfile->need_refresh_chart = 1;

	  logfile = new_logfile_rec(logfile_name, fromhost, tapeid, prod);
	  if (logfile == NULL) {
		fprintf(stderr, "logfile is NULL 2.\n");
		exit_system(NULL, -1);
	  }

	  strcpy(logfile->tape_dev, tape_dev);

	  /* status forms need to be reloaded */
	  load_status_forms = 1;  

	  if ((rc = add_entry_to_log_rec(logfile, jobnum, time_item, time_str, date_str,
									 infile, stat)) == 1)
		logfile->need_refresh_chart = 1;

	  /* add_logfile_to_list must be after add_entry_to_log_rec */
	  if (add_logfile_to_list(&(gstatus_info->logfiles), logfile))
		  gstatus_info->nlogfiles++;

	}	  
	update_tape_groups(gstatus_info->logfiles, &(tape_groups), 
					  logfile, tapeid, fromhost);

	/* set last job status msg. */
	memset(last_job_status_msg, '\0', MAX_BROWSER_LINE_LEN);
	sprintf(last_job_status_msg, "%-9.9s %-8.8s %-8.8s %-17.17s %-10.10s %-16.16s   %-4.4s %-10.10s", 
			  fromhost, date_str, time_str, tapeid, tape_dev, prod, 
			  jobnum, status_str);


  READ_NEXT:
	/* go to the next line */
	key = strtok(NULL, ":");
  } /* while */
/*
  sigsetmask(oldmask);
*/
  return kept;
}/* process_status_info */




/***************************************************************************/
/*                                                                         */
/*                     clear_chart                                         */
/*                                                                         */
/***************************************************************************/
void clear_chart(struct chart_rec *chart_info)
{
  char *label = EMPTY_STR;

  if (chart_info == NULL) return;
  
  if (chart_info->chart) 
	fl_clear_chart(chart_info->chart);
  if (chart_info->llabel_obj) 
	fl_set_object_label(chart_info->llabel_obj, label);
  if (chart_info->blabel_obj) 
	fl_set_object_label(chart_info->blabel_obj, label);
  if (chart_info->log_num_obj)
	fl_set_object_label(chart_info->log_num_obj, label);
/*
  if (chart_info->chart)
	fl_set_object_label(chart_info->chart, label);
*/


} /* clear_gstatus */

/***************************************************************************/
/*                                                                         */
/*                     add_item_to_chart                                   */
/*                                                                         */
/***************************************************************************/
void add_item_to_chart(FL_OBJECT *chart, char *filename, stat_type_t stat)
{
  if (chart == NULL || filename == NULL) return;

  switch (stat) {
  case FAILED:
	fl_add_chart_value(chart, CHART_HEIGHT, filename, FAILURE_COLOR);
	break;
  case ABORT:
	fl_add_chart_value(chart, CHART_HEIGHT, filename, ABORT_COLOR);
	break;
  case WARNING:
	fl_add_chart_value(chart, CHART_HEIGHT, filename, WARNING_COLOR);
	break;
  case RUNNING: 
	fl_add_chart_value(chart, CHART_HEIGHT, filename, RUNNING_COLOR);
	break;
  case SUCCESS:
	fl_add_chart_value(chart, CHART_HEIGHT, filename, SUCCESS_COLOR);
	break;
  default:
	fl_add_chart_value(chart, CHART_HEIGHT, filename, DEFAULT_COLOR);
	break;
  }
} /* add_item_to_chart */

/***************************************************************************/
/*                                                                         */
/*                     replace_chart_item_value                            */
/*                                                                         */
/***************************************************************************/
void replace_chart_item_value(FL_OBJECT *chart, int index,
							  char *filename, stat_type_t stat)
{


  /* chart's index starts at  1 */
  if (chart == NULL || filename == NULL ||
	  index < 1 || index > MAX_CHART_ITEMS) return;

  switch (stat) {
  case FAILED:
	fl_replace_chart_value(chart, index, CHART_HEIGHT, filename, FAILURE_COLOR);
	break;
  case ABORT:
	fl_replace_chart_value(chart, index, CHART_HEIGHT, filename, ABORT_COLOR);
	break;
  case WARNING:
	fl_replace_chart_value(chart, index, CHART_HEIGHT, filename, WARNING_COLOR);
	break;
  case RUNNING:
	fl_replace_chart_value(chart, index, CHART_HEIGHT, filename, RUNNING_COLOR);
	break;
  case SUCCESS:
	fl_replace_chart_value(chart, index, CHART_HEIGHT, filename, SUCCESS_COLOR);
	break;
  default:
	fl_replace_chart_value(chart, index, CHART_HEIGHT, filename, DEFAULT_COLOR);
	break;
  }

} /**/

/***************************************************************************/
/*                                                                         */
/*                     logfile_entry *get_log_entry                        */
/*                                                                         */
/***************************************************************************/
struct logfile_entry *get_log_entry(struct logfile_rec *logfile, int fnum)
{
  struct logfile_entry *entry;

  if (logfile == NULL) return NULL;
  entry = logfile->data;
  while (entry) {
	if (entry->fnum == fnum)
	  return entry;
	entry = entry->next;
  }
  return NULL;
} /* get_log_entry */

/***************************************************************************/
/*                                                                         */
/*                     add_log_entry_to_list                               */
/*                                                                         */
/***************************************************************************/
void add_log_entry_to_list(struct logfile_rec *logfile_ptr,
						   struct logfile_entry *log_entry)
{
  /* add entry to list in the order of fnum if not previously added. 
   */
  struct logfile_entry *tmp, *last_entry;

  if (logfile_ptr == NULL || log_entry == NULL) return;

  /* only add if not in list */
  if (get_log_entry(logfile_ptr, log_entry->fnum) != NULL) return;

  /* Update last modified job */
  logfile_ptr->last_mod_job = log_entry;

  logfile_ptr->nentries++;



  /* Update last job */
  if (log_entry->fnum == logfile_ptr->nentries) {
	logfile_ptr->last_job = log_entry;
	/*
printf("name: %s, log_entry->fnum; %d, logfile_ptr->nentries: %d\n", logfile_ptr->name, log_entry->fnum,
logfile_ptr->nentries);
*/
  }

  if (logfile_ptr->data == NULL) {
	logfile_ptr->data = log_entry;
  }
  else {
	/* add in the order of fnum */
	tmp = logfile_ptr->data;
	if (tmp->fnum > log_entry->fnum) {
	  /* add at the beginning of list */
	  log_entry->next = logfile_ptr->data;
	  logfile_ptr->data = log_entry;
	  return;
	}
	while (tmp) {
	  if (tmp->fnum > log_entry->fnum) {
		/* add before tmp */
		last_entry->next = log_entry;
		log_entry->next = tmp;
		return;
	  }
	  last_entry = tmp;
	  tmp = tmp->next;
	}
	if (last_entry) {
	  /* add at end of list */
	  last_entry->next = log_entry;
	}
  }

} /* add_log_entry_to_list */


/***************************************************************************/
/*                                                                         */
/*                     logfile_entry * new_logfile_entry                   */
/*                                                                         */
/***************************************************************************/
struct logfile_entry * new_logfile_entry(char *infile, stat_type_t stat,
										 time_t time_item,
									char *time_str, char *date_str, int fnum,
  								    int need_redraw, double host_diff_time)
{   
  struct logfile_entry *entry;
  if (infile == NULL ||  date_str == NULL || time_str == NULL ||
	  fnum <= 0)
	return NULL;

  entry = (struct logfile_entry *) calloc(1, sizeof(struct logfile_entry));
  if (entry == NULL) {
	perror("calloc logfile entry");
	exit_system(NULL, -1);
  }

  strncpy(entry->infile, infile, MAX_FILENAME_LEN);
  entry->stat = stat;
  strcpy(entry->date, date_str);
  strcpy(entry->time_str, time_str);
  entry->runtime = -1;
  entry->time_item = time_item;

  entry->fnum = fnum;
  entry->draw_flag = ADD_ITEM;
  entry->next = NULL;
  return entry; 		
} /* new_logfile_entry */

/***************************************************************************/
/*                                                                         */
/*                     get_highest_file_num                                */
/*                                                                         */
/***************************************************************************/
int get_highest_file_num(struct logfile_rec *logfile_ptr)
{
  struct logfile_entry *entry;
  int last_fnum = 0;

  if (logfile_ptr == NULL) return last_fnum;
  entry = logfile_ptr->data;
  while (entry) {
	last_fnum = entry->fnum;
	entry = entry->next;
  }
  return last_fnum;
  
} /* get_highest_file_num */


/***************************************************************************/
/*                                                                         */
/*                                 get_host_diff_time                      */
/*                                                                         */
/***************************************************************************/
double get_host_diff_time(char *host)
{
  struct host_info_rec *host_info;

  if (host == NULL) return 0;

  host_info = get_host_info(host);
  if (host_info)
	return(host_info->diff_time);
  return 0;
} /* get_host_diff_time */

/***************************************************************************/
/*                                                                         */
/*                     add_entry_to_log_rec                                */
/*                                                                         */
/***************************************************************************/
int add_entry_to_log_rec(struct logfile_rec *logfile_ptr, 
						  char *jobnum, time_t time_item, char *time_str, 
						  char *date_str, char *infile, stat_type_t stat)
{
  /* add logfile data entry to log rec if entry has not be added or
   * Modify entry's status indicates not finished processing yet (i.e. running,
   * not_running, not_defined).
   * returns 1 if entry is added or modified; 2 if rerun occurs;
   * 0, otherwise.
   * Note: jobs coming in not in the right order.
   */
  struct logfile_entry *entry;
  int fnum, highest_fnum;
  int i;
  int update_stime = 0;
  int update_etime = 0;
  time_t entry_time_item;
  double diff_t1, diff_t2;
  double host_diff_time;


  if (jobnum == NULL || strlen(jobnum) == 0 || logfile_ptr == NULL || 
	  infile == NULL || strlen(infile) == 0 || date_str == NULL || 
	  time_str == NULL) 
	return 0;

  entry_time_item = time_item;

  
  /* check if status is 'finished', then update logfile's end date & time
   */
  if (stat == FINISHED) {
	update_etime = 1;
	goto UPDATE_END_TIME;
  }
  

  if (sscanf(jobnum, "%d", &fnum) != 1) return 0;


  entry = get_log_entry(logfile_ptr, fnum);

  if (entry != NULL) {
	/* update entry's data if it's status is different than the new stat 
	 * and is one of these: RUNNING, NOT_RUNNING, or NOT_DEFINED 
	 */
/*
	if (stat == entry->stat) {
printf("stat is the SAME******, fnum = %d, logfilename = %s\n", fnum, logfile_ptr->name);
	  return 2;
	}
*/

	if (entry->stat == RUNNING || entry->stat == NOT_RUNNING || 
		entry->stat == NOT_DEFINED) {

	  if (entry->stat == NOT_DEFINED)
		update_stime = 1;

	  /* changed last modified job */
	  logfile_ptr->last_mod_job = entry;

	  if (entry->stat == RUNNING) 
		entry->runtime = difftime(entry_time_item, entry->time_item);

	  entry->stat = stat;


	  strcpy(entry->date, date_str);
	  strcpy(entry->time_str, time_str);
	  memcpy(&(entry->time_item), &entry_time_item, sizeof(time_t));
	  if (entry->draw_flag == NOOPT)
		entry->draw_flag = UPDATE_ITEM;
	  goto UPDATE_TIME;

	}
	
	/* rerun occurs. this assumption is true only if all jobs are 
	 * received in the right order. Since this assumption is not always true,
	 * time boundary is checked. */
	if ((diff_t1 = difftime(logfile_ptr->etime_item,logfile_ptr->stime_item)) <
		(diff_t2 = difftime(entry_time_item, logfile_ptr->stime_item))) {
/*
 printf("fnum = %d, nentries = %d\n", fnum, logfile_ptr->nentries);
printf(" stime=<%s>,sdate=<%s>, etime=<%s>,edate=<%s>, time=<%s>,date=<%s>,diff t1 = %f, t2 =  %f\n", 
logfile_ptr->stime,  logfile_ptr->sdate, logfile_ptr->etime,  logfile_ptr->edate, time, date, diff_t1, diff_t2);

*/
	  return 2;
	}

	/* no modification is needed */
/*
	fprintf(stderr, "wARNING: duplicate job status received: name <%s>, jobnum <%d>\n", logfile_ptr->name, fnum);
*/
	if (write_to_file) {
	  fprintf(test_file, "WARNING: duplicate job status received: name <%s>, jobnum <%d>\n", logfile_ptr->name, fnum);
	}

	return 0;


  }
  /* entry is new, 
   * create (highest_file_num+1 to jobnum) dummy entries and add them to list
   * if fnum > highest_file_num.
   * Their info will be coming in later since data received from monitorps
   * may  be in any order. 
   */
  highest_fnum = get_highest_file_num(logfile_ptr);

  for (i = highest_fnum + 1; i < fnum; i++) {
	entry = new_logfile_entry(" ", NOT_DEFINED, 0, "", "", i, 1, host_diff_time);
	add_log_entry_to_list(logfile_ptr, entry);
  }

  /* create entry for this fnum and add to list
   */
  entry = new_logfile_entry(infile, stat, time_item, time_str, date_str, fnum, 1, host_diff_time);
  add_log_entry_to_list(logfile_ptr, entry);

 UPDATE_TIME:

  /* set logfile's start time based on the first job. Note job may not have
   * to start at 1.  Jobs may come in in any order.
   */
  if (strlen(logfile_ptr->sdate) == 0 || (update_stime == 1 && 
		   difftime(entry_time_item, logfile_ptr->stime_item) < 0)) {
	strcpy(logfile_ptr->sdate, date_str);
	strcpy(logfile_ptr->stime, time_str);
    memcpy(&(logfile_ptr->stime_item), &entry_time_item, sizeof(time_t));
  }

  /* update logfile's stat */
  logfile_ptr->stat = get_logfile_stat(logfile_ptr);


UPDATE_END_TIME:
/*
printf("fname =<%s>,fnum = %d,entry time: <%s>, stime = <%s>, etime = <%s>\n", logfile_ptr->name, fnum,  time_item, logfile_ptr->stime, logfile_ptr->etime);
*/
  if (update_etime || strlen(logfile_ptr->edate) == 0 || 
	  difftime(entry_time_item, logfile_ptr->etime_item) > 0) {

	/* update log's end date and time */
	strcpy(logfile_ptr->edate, date_str);
	strcpy(logfile_ptr->etime, time_str);
    memcpy(&(logfile_ptr->etime_item), &entry_time_item, sizeof(time_t));
/*
printf("*entry time: <%s>, stime = <%s>, etime = <%s>\n", time, logfile_ptr->stime, logfile_ptr->etime);
*/
  }

  return 1;

} /* add_entry_to_log_rec */

/***************************************************************************/
/*                                                                         */
/*                     logfile_rec * new_logfile_rec                       */
/*                                                                         */
/***************************************************************************/
struct logfile_rec * new_logfile_rec(char *fname, char *fromhost, char *tapeid,
									 char *prog)
{
  struct logfile_rec *log;

  if (fname == NULL || strlen(fname) == 0) return NULL;

  log = (struct logfile_rec *) calloc(1, sizeof(struct logfile_rec));
  if (log == NULL) {
	perror("calloc logfile rec");
	exit_system(NULL, -1);
  }

  memset(log->name, '\0', MAX_FILENAME_LEN);
  strcpy(log->name, fname);
  strcpy(log->fromhost, fromhost);
  strcpy(log->tapeid, tapeid);
  strcpy(log->prog, prog);
  log->finished  = 0;
  log->active  = 1;
  log->nentries = 0;
  log->data = NULL;
  log->last_mod_job = NULL;
  log->last_job = NULL;

  log->need_refresh_chart = 0;
  memset(log->last_status_sline, '\0', MAX_BROWSER_LINE_LEN);
  memset(log->last_log_sline, '\0', MAX_BROWSER_LINE_LEN);
  memset(&(log->stime_item), '\0', sizeof(time_t));
  memset(&(log->etime_item), '\0', sizeof(time_t));
  memset(log->sdate, '\0', MAX_NAME_LEN);
  memset(log->edate, '\0', MAX_NAME_LEN);
  memset(log->stime, '\0', MAX_NAME_LEN);
  memset(log->stime, '\0', MAX_NAME_LEN);
  log->next = NULL;
  log->prev = NULL;
  return log;
}

/***************************************************************************/
/*                                                                         */
/*                     generate_report_from_logdata                        */
/*                                                                         */
/***************************************************************************/
void generate_report_from_logdata(char *fromhost, 
								  struct tape_info_rec *tape_info, 
								  char *prod, char *buf)
{
  extern FILE *popen( const char *command, const char *type);
  extern int pclose( FILE *stream);

  /* invoke report generator program to generate a status report from
   * log data. The report should produce the file in the following format:
   *    jobnum date  time  tapeid tape_device  prod file   status  
   * This program assumes that the logfile contains info for one product only.
   * The report is stored in tape_info->logfiles[pindex].
   */
  FILE *fp;
  char cmd[MAX_DATA_LEN + MAX_CMD_LEN];
  char line[256];
  int pindex;
  char infile[MAX_FILENAME_LEN];
  char date_str[MAX_NAME_LEN];
  char jobnum[MAX_NAME_LEN];
  char time_str[MAX_NAME_LEN];
  time_t time_item;
  char tape_dev[MAX_NAME_LEN];
  char stat_str[MAX_STATUS_STR_LEN];
  struct logfile_rec *logfile_ptr;
  char logfile_name[MAX_FILENAME_LEN];
  int modified = 0;
  stat_type_t stat = NOT_DEFINED;
  double host_diff_time;

  if (fromhost == NULL || prod == NULL || buf == NULL || 
	  tape_info == NULL) return;

  memset(cmd, '\0', MAX_DATA_LEN+MAX_CMD_LEN);
  sprintf(cmd, "echo \"%s\" |rptgen", buf);

  if ((fp = popen(cmd, "r")) == NULL) {
	perror("open pipe");
	exit_system(NULL, -1);
  }
  pindex = get_prod_index(prod);
  if (pindex < 0 || pindex >= MAX_PRODS) return;
  memset(line, '\0', 256);

  /* create logfile for this product */
  memset(logfile_name, '\0', MAX_FILENAME_LEN);
  create_logfile_name(fromhost, tape_info->id, prod, logfile_name);
/*
printf("logfilename ===========<%s>\n", logfile_name);
*/
  logfile_ptr = new_logfile_rec(logfile_name, fromhost, tape_info->id, prod);
  /*
fprintf(stderr, "add lname = %s, %d\n", logfile_name, pindex);
*/
  if (logfile_ptr == NULL) {
	fprintf(stderr, "logfile_ptr is NULL.\n");
	exit_system(NULL, -1);
  }

  /* remove old logfile for this product */
/*
  if (tape_info->logfiles[pindex]) free_logfile_rec(tape_info->logfiles[pindex]);

  tape_info->logfiles[pindex] = logfile_ptr;
*/

/*
printf("generate rep: prod = %s, logfilename = %s\n", prod, logfile_ptr->name);
*/
  /* extract more detail from the first line */
  if (fgets(line, 255, fp)) {
	memset(jobnum, '\0', MAX_NAME_LEN);
	memset(time_str, '\0', MAX_NAME_LEN);
	memset(date_str, '\0', MAX_NAME_LEN);
	memset(tape_dev, '\0', MAX_NAME_LEN);
	memset(infile, '\0', MAX_FILENAME_LEN);
	memset(stat_str, '\0', MAX_STATUS_STR_LEN);
	if (sscanf(line, "%s %s %*s %s %*s %s %s", jobnum, time_str,tape_dev,
			 infile, stat_str) != 5) {
	  pclose(fp);
	  return;
	}
/*
printf(" date = <%s>, time = <%s>, tapedev = <%s>, infile = <%s>, stat=<%s>\n", date, time_item, tape_dev, infile, stat_str);
*/
	/* convert time string to date and hhmmss */

	/* Adjust host's time to local host's clock */
	host_diff_time = get_host_diff_time(fromhost);
	time_item = (time_t) atoi(time_str);
	time_item -= host_diff_time;

	time_secs2strs(time_item, time_str, date_str);

	stat = stat_is(stat_str);
	if (add_entry_to_log_rec(logfile_ptr, jobnum, time_item, time_str, date_str, 
									infile, stat) == 1)
	  modified = 1;
	/* set tapedevice  */
	strcpy(logfile_ptr->tape_dev, tape_dev);
	memset(line, '\0', 256);

  }

  while (fgets(line, 255, fp)) {
	/* add file and status info list */
	/* line format: infile status */
	*(line+strlen(line)-1) = '\0'; /* remove '\n' char */
	memset(jobnum, '\0', MAX_NAME_LEN);
	memset(date_str, '\0', MAX_NAME_LEN);
	memset(time_str, '\0', MAX_NAME_LEN);
	memset(tape_dev, '\0', MAX_NAME_LEN);
	memset(infile, '\0', MAX_FILENAME_LEN);
	memset(stat_str, '\0', MAX_STATUS_STR_LEN);
	if (sscanf(line, "%s %s %*s %*s %*s %s %s", jobnum, time_str,
			 infile, stat_str) != 4) continue;
/*
printf(" date = <%s>, time = <%s>, tapedev = <%s>, infile = <%s>, stat=<%s>\n", date, time, tape_dev, infile, stat_str);
*/
	/* set finished for logfile if it is done processing with this prod 
	 * and tapeid
	 */
	stat = stat_is(stat_str);
	if (stat == FINISHED || stat == ABORT) {
	  logfile_ptr->finished = 1; 
	  logfile_ptr->active = 0; 
	}
	time_item = (time_t) atoi(time_str);
	if (add_entry_to_log_rec(logfile_ptr, jobnum, time_item, time_str, date_str,  
							 infile, stat) == 1){
	  modified = 1;
	}

	memset(line, '\0', 256);
  }
  if (modified)
	logfile_ptr->need_refresh_chart = 1;
  pclose(fp);
  add_logfile_to_list(&(tape_info->logfiles[pindex]), logfile_ptr);

} /* generate_report_from_logdata */


/***************************************************************************/
/*                                                                         */
/*                     tapeid_exists                                       */
/*                                                                         */
/***************************************************************************/
int tapeid_exists(char *tapeid)
{
  int i;

  if (tapeid == NULL) return 0;

  for (i = 0; i < MAX_TAPES; i++) {
	if (strcmp(tapeids[i], tapeid) == 0)
	  return 1;
  }
  return 0;
}

/***************************************************************************/
/*                                                                         */
/*                     add_tapeid                                          */
/*                                                                         */
/***************************************************************************/
void add_tapeid(char *tapeid)
{
  int i;

  if (tapeid == NULL || strlen(tapeid) == 0) return;
  if (tapeid_exists(tapeid)) return;
  for (i = 0; i < MAX_TAPES; i ++) {
	if (strlen(tapeids[i]) == 0) {
	  strcpy(tapeids[i], tapeid);
	  return;
	}
  }
  fl_show_message("ERROR", "Number of tape ids exceeds the limit", "Ignore");
}

#if 0
/***************************************************************************/
/*                                                                         */
/*                      process_log_file                                   */
/*                                                                         */
/***************************************************************************/
void  process_log_file(char *fromhost, char *buf, char *tapeid, char *prod)
{
/* extract tapeid and product from buf.
 * Create tape info if not alread exists. Write data to disk.
 * Generate report from logfile -- the result is written to file (by the
 * script) and is kept in memory.
 */
  char *tmpname, *tmp;
  char *data;
  int nbytes;
  struct tape_info_rec *tape_info;


  if (fromhost == NULL || buf == NULL || tapeid == NULL || prod == NULL ||
	  strlen(buf) == 0) return;

  /* skip key word */
  if (strstr(buf, LOG_FILE_KEY) == NULL) return;
  buf = strstr(buf, LOG_FILE_KEY) + strlen(LOG_FILE_KEY);

  /* buf contains: log: logfile name (first string) and log data.
   * logfile name and data (status line only) received from monitorps 
   * are in the following formats: 
   *   logfilenaem: tapeid.product.log 
   *   status data: status: jobnum date time  tapeid tape_device infile status
   */

  /* remove leading spaces */
  buf = remove_leading_blanks(buf);
/*
  while (buf)  {
	if (*buf == ' ') buf++; 
	else break;
  }
*/
  data = strstr(buf, STATUS_INFO_KEY);  /* get actual data */

  if (strchr(buf, ' ') == NULL) goto DONE;
  tmpname = strchr(buf, ' ');
  *tmpname = '\0';
  tmpname = buf;
/*
printf("****tmpname = <%s>,data = <%s>\n", tmpname, data);
*/

  tmp = tmpname;
  nbytes = strlen(tmp) - strlen(strchr(tmp, '.'));
  memset(tapeid, '\0', MAX_TAPEID_LEN);
  strncpy(tapeid, tmp, nbytes);
  *(tapeid+nbytes) = '\0';
  tmp = strchr(tmp, '.') + 1;  /* skip tapeid */
  nbytes = strlen(tmp) - strlen(strchr(tmp, '.'));
  strncpy(prod, tmp, nbytes);
  *(prod+nbytes) = '\0';

  /* truncate tapeid and prod to max size */
  if (strlen(tapeid) >= MAX_TAPEID_LEN)
	*(tapeid+MAX_TAPEID_LEN-1) = '\0';
  if (strlen(prod) >= MAX_PRODNAME_LEN)
	*(prod+MAX_PRODNAME_LEN-1) = '\0';
/*
printf("**tapid = <%s>, prod = <%s>\n", tapeid, prod);
*/
  /* create tape info if not already exists and add to list */
  if ((tape_info = get_tape_info(tapeid)) == NULL) {
	if ((tape_info = new_tape_info(tapeid)) == NULL) goto DONE;
	add_tape_info(tape_info);
  }

  generate_report_from_logdata(fromhost, tape_info, prod, data);
 DONE:

  return;
} /* process_log_file */

#endif

/***************************************************************************/
/*                                                                         */
/*                     load_logfile_data_to_chart                          */
/*                                                                         */
/***************************************************************************/
void load_logfile_data_to_chart(FL_OBJECT *chart, struct logfile_rec *logfile)
{
  /* Reload logfile entries to chart (clear chart and add items to chart) 
   * Entry is represented by chart's item.
   * Item's color is coded as followed: yellow (warning), red (failure),
   * green (running), black (success), brown (undefined).
   */
  struct logfile_entry *dentry;
  int nitems = 0;

  if (chart == NULL || logfile == NULL)
	  return;
#define RELOAD 1   /* clear and add.  This cleans up the items' color correctly
                    * when there are so many items squeeze together
					*/
#if RELOAD
  fl_clear_chart(chart);
#endif

  dentry = logfile->data;

  while (dentry) {
	nitems++;

#if !RELOAD
	/* add item to chart or modify its value */
	if (dentry->draw_flag == ADD_ITEM)  {
	  /* add item to chart if max items not met*/
	  if (nitems > MAX_CHART_ITEMS) {
		fprintf(stderr, "WARNING: nitems, %d,  exceed MAX_CHART_ITEMS. Item is ignored\n", nitems);
	  }
	  else 
		add_item_to_chart(chart,  "", dentry->stat);

	  /* reset flags */
	  dentry->draw_flag = NOOPT;
	}
	else if (dentry->draw_flag == UPDATE_ITEM) {
	  /* update chart item's value for modified items */
	  replace_chart_item_value(chart, dentry->fnum, "", dentry->stat);
	  /* reset need redraw flag */
	  dentry->draw_flag = NOOPT;
	}
#else
		add_item_to_chart(chart,  "", dentry->stat);
#endif
	dentry = dentry->next;
  }

} /* load_logfile_data_to_chart */

/***************************************************************************/
/*                                                                         */
/*                     clear_all_charts                                    */
/*                                                                         */
/***************************************************************************/
void clear_all_charts(struct chart_rec *clist[], int nentries)
{
  int i;

  for (i = 0; i < nentries; i ++) 
	clear_chart(clist[i]);
} /* clear_all_charts */

/***************************************************************************/
/*                                                                         */
/*                     hide_chart                                          */
/*                                                                         */
/***************************************************************************/
void hide_chart(struct chart_rec *chart_rec)
{
  if (chart_rec == NULL) return;

  if (chart_rec->chart)
	fl_hide_object(chart_rec->chart);
  if (chart_rec->llabel_obj)
	fl_hide_object(chart_rec->llabel_obj);
  if (chart_rec->blabel_obj)
	fl_hide_object(chart_rec->blabel_obj);
  if (chart_rec->log_num_obj)
	fl_hide_object(chart_rec->log_num_obj);
}

/***************************************************************************/
/*                                                                         */
/*                     load_logfiles_data_to_charts                        */
/*                                                                         */
/***************************************************************************/
void load_logfiles_data_to_charts(struct graphic_status_rec *gstat)
{
  /* Assign logfile to specific chart on status form. Set chart's left side 
   * and below labels. Load logfile's entries to chart. It also hide chart
   * from form if there is no logfile associates with it.
   */
  int i;
  char label[MAX_LABEL_LEN];
  struct logfile_rec *logfile;
  int color_i = DEFAULT_COLOR;
  stat_type_t stat;
  char finished_char;


  /* load logfiles' data to charts. 
   */
  if (gstat == NULL) return;

  for (i = 0; i < MAX_CHARTS; i++) {
	if (gstat->chart_list[i] == NULL ||
		gstat->chart_list[i]->llabel_obj == NULL ||
		gstat->chart_list[i]->blabel_obj == NULL ||
		gstat->chart_list[i]->chart == NULL ||
		gstat->chart_list[i]->log_num_obj == NULL) {
	  fprintf(stderr, "ERROR: unexpected gstat->chart_list[%d]'s null \n", i);
	  exit_system(NULL, -1);
	}
	logfile = gstat->chart_list[i]->logfile;
	if (logfile == NULL) {
	  /* clear and hide  chart  since no logfile is associated with it. */

	  clear_chart(gstat->chart_list[i]);
	  hide_chart(gstat->chart_list[i]);

	  continue;
	}
	/* skip if  if logfile has not been modified since last load to chart. */
	if (logfile->need_refresh_chart == 0) continue;
	/* reset logfile need_refresh_chart flag */
	logfile->need_refresh_chart = 0;


	/* show chart & its label, load logfile data to chart  */

	/* maybe there is a bug in  form library, it doesnot clear the object's
	 * label area before it redisplay the new label value again between 
	 * freeze from and unfreezefrom.  This is a work around solution.
	 */

	/* clear chart's lelftside label */
/*
	fl_set_object_label(gstat->chart_list[i]->llabel_obj, label);
*/

	/* put '*' before label if logfile is FINISHED else
	 * put '-' before label if logifle is not active */
	if (logfile->finished)
	  finished_char = FINISHED_CHAR;
	else if (logfile->active == 0)
	  finished_char = NOT_ACTIVE_CHAR;
	else
	  finished_char = ' ';
	sprintf(label, "%c%s\n  %s\n  %s", finished_char, logfile->fromhost, 
			logfile->tapeid, logfile->prog);

	/* set label on the left side of chart */
	fl_set_object_label(gstat->chart_list[i]->llabel_obj, label);
	fl_show_object(gstat->chart_list[i]->chart);
	fl_show_object(gstat->chart_list[i]->llabel_obj);


	/* bottom label's color is associated with logfile's status */
	stat = logfile->stat;
	color_i = get_stat_color(stat);
  
	memset(label, '\0', MAX_LABEL_LEN);
	/* clear bottom chart's label */
/*
	fl_set_object_label(gstat->chart_list[i]->blabel_obj, label);
*/
	if (logfile->nentries > 1)
	  sprintf(label, "%d files", logfile->nentries);
	else 
	  sprintf(label, "%d file", logfile->nentries);
	fl_set_object_lcol(gstat->chart_list[i]->blabel_obj, color_i);
	fl_set_object_label(gstat->chart_list[i]->blabel_obj, label);
	fl_show_object(gstat->chart_list[i]->blabel_obj);
	
	/* load data to bar chart*/
	load_logfile_data_to_chart(gstat->chart_list[i]->chart, logfile);

  }

} /* load_logfiles_data_to_charts */

int get_nlogfiles_shown(struct logfile_rec **special_logfiles_array, 
						int nlogfiles)
{
  int num = 0;
  struct logfile_rec *logfile;

  if (special_logfiles_array == NULL) return num;
  

  while (num < nlogfiles) {
	logfile = special_logfiles_array[num];
	if (logfile == NULL) break;
	num++;
  }
  return num;
} /* get_nlogfiles_shown */


/****************************************************************************/
/*                                                                          */
/*                        update_labels_for_status_form                     */
/*                                                                          */
/****************************************************************************/

void update_labels_for_status_form(struct graphic_status_rec *gstat) 
{


  int log_num, i;
  char label[MAX_LABEL_LEN];
  char label1[MAX_LABEL_LEN];

  if (gstat == NULL || gstat->sform == NULL || 
	  gstat->sform->status_form == NULL || !gstat->sform->status_form->visible)
	return;

  /* update total number of logfiles if it's different */
  if (gstat->sform && gstat->sform->label) {


	memset(label, '\0', MAX_LABEL_LEN);
	sprintf(label, " %d", get_nlogfiles_shown(gstat->bar_sorted_logfiles_array,
											  gstat->nbar_array_items));
	if (strcmp(gstat->sform->label->label, label) != 0) {
	  /* Hide object, set its label, and show it again -- hide and show are
	   * required here since form just writes the new label over the old one 
	   * without clearing the old one first.
	   */
	  fl_hide_object(gstat->sform->label);
	  fl_set_object_label(gstat->sform->label, label);
	  fl_show_object(gstat->sform->label);
	}
  }


  /* find and set log number on the left side of form */

  for (i = 0; i < MAX_CHARTS; i++) {
	if (gstat->chart_list[i] && gstat->chart_list[i]->logfile) {
	  
	  log_num = get_nindexes_upward(gstat->bar_sorted_logfiles_array,
							 gstat->chart_list[i]->logfile, gstat->nbar_array_items);
	  memset(label1, '\0', MAX_LABEL_LEN);
	  sprintf(label1, "%4d", log_num);
	  fl_set_object_label(gstat->chart_list[i]->log_num_obj, label1);
	  fl_show_object(gstat->chart_list[i]->log_num_obj);
	}
  }

} /* update_labels_for_status_form */

/***************************************************************************/
/*                                                                         */
/*                     get_nlogfiles_downward                              */
/*                                                                         */
/***************************************************************************/
int get_nlogfiles_downward(struct logfile_rec *curr_logfile)		   
{
  /* only count logfile that has non-zero entry */
  int nfiles = 0;
  struct logfile_rec *logfile;

  if (curr_logfile == NULL) return 0;

  logfile = curr_logfile;
  if (logfile->nentries == 0) 
	logfile = get_next_logfile(logfile);
  while (logfile) {
	nfiles++;
	logfile = get_next_logfile(logfile);
  }
  return nfiles;
}

/***************************************************************************/
/*                                                                         */
/*                     get_nlogfiles_upward                                */
/*                                                                         */
/***************************************************************************/
int get_nlogfiles_upward(struct logfile_rec *curr_logfile)
{
  /* only count logfile that has non-zero entry */
  int nfiles = 0;
  struct logfile_rec *logfile;

  if (curr_logfile == NULL) return 0;

  logfile = curr_logfile;
  if (logfile->nentries == 0)
	logfile = get_previous_logfile(logfile);

  while (logfile) {
	nfiles++;
	logfile = get_previous_logfile(logfile);
  }

  return nfiles;
}

/***************************************************************************/
/*                                                                         */
/*                     logfile_rec * get_next_logfile                      */
/*                                                                         */
/***************************************************************************/
struct logfile_rec * get_next_logfile(struct logfile_rec *logfile)
{
  struct logfile_rec *tmplog;

  if (logfile == NULL) return NULL;

  /* returns next logfile that has entry */
  tmplog = logfile->next;
  while (tmplog && tmplog->nentries == 0)
	tmplog = tmplog->next;

  return tmplog;
} 

/***************************************************************************/
/*                                                                         */
/*                     init_to_load_new_logfile_to_chart                   */
/*                                                                         */
/***************************************************************************/
void init_to_load_new_logfile_to_chart(struct chart_rec *chart_rec)
{
  if (chart_rec == NULL || chart_rec->chart == NULL || chart_rec->logfile == NULL) return;

  /* clear chart */
  fl_clear_chart(chart_rec->chart);

  /* set need_add_to_chart flag for each item*/
  set_need_add_to_chart_flag(chart_rec->logfile->data);

  /* set need refresh chart flag */
  chart_rec->logfile->need_refresh_chart = 1;
  
} /* init_to_load_new_logfile_to_chart */

/***************************************************************************/
/*                                                                         */
/*                     set_logfiles_to_chart_list                          */
/*                                                                         */
/***************************************************************************/
void set_logfiles_to_chart_list(struct graphic_status_rec *gstat, 
							   int from_index, int to_index, 
								int top_logfile_index)
{
  /* Assign logfiles to bar charts */

  int i, k, j;
  struct logfile_rec *logfile;

  if (gstat == NULL || top_logfile_index < 0 || 
	  top_logfile_index >= gstat->nlogfiles)
	return;

  k = top_logfile_index;

  for (i = from_index; i < to_index && k < gstat->nlogfiles; i++, k++) {
	if (gstat->chart_list[i] == NULL) {
	  fprintf(stderr, "ERROR: gstat-chart_list[%d] is  NULL \n", i);
	  exit_system(NULL, -1);
	}

	logfile = gstat->bar_sorted_logfiles_array[k];
	/* if logfile is NULL, end of list occurs*/
	if (logfile == NULL) break;

	if (logfile == NULL) 
	  fprintf(stderr, "k == %d, logfile is NULL, i = %d\n", k, i);

	/* only assign logfile to chart if it's not already assigned */
	if (gstat->chart_list[i]->logfile == NULL ||
		(strcmp(gstat->chart_list[i]->logfile->name, logfile->name) != 0 ||
		 strcmp(gstat->chart_list[i]->logfile->sdate, logfile->sdate) != 0 ||
		 strcmp(gstat->chart_list[i]->logfile->stime, logfile->stime) != 0 ||
		 gstat->chart_list[i]->logfile->active != logfile->active) ) {

	  /* set new logfile to be displayed on the bar form */
	  gstat->chart_list[i]->logfile = logfile;
	  /* clear chart, set appropriate flags to have new logfile displayed */
	  init_to_load_new_logfile_to_chart(gstat->chart_list[i]);
	}
	
/*
printf("gstat->chart_list[%d]->logfile->name = %s\n", i, gstat->chart_list[i]->logfile->name);
*/

  }
  for (j = i; j < to_index; j++) {
	gstat->chart_list[j]->logfile = NULL;
  }
} /* set_logfiles_to_chart_list */

/***************************************************************************/
/*                                                                         */
/*                            get_previous_logfile_index                   */
/*                                                                         */
/***************************************************************************/
int get_previous_logfile_index(struct logfile_rec **special_logfiles_array,
							   struct logfile_rec *logfile, int nlogfiles) 
{
  int i=0;
  struct logfile_rec *log;

  if (special_logfiles_array == NULL || nlogfiles < 1 || logfile == NULL) 
	return -1;
  log = special_logfiles_array[i];

  while (log != NULL && i < nlogfiles) {
	if (memcmp(log, logfile, sizeof(struct logfile_rec)) == 0) 
	  return i-1;
	i++;
	log = special_logfiles_array[i];
  }
  return -1;
}
	
/***************************************************************************/
/*                                                                         */
/*                            get_nindexes_downward                        */
/*                                                                         */
/***************************************************************************/
int	get_nindexes_downward(struct logfile_rec **special_logfiles_array, 
						  struct logfile_rec *logfile, int nlogfiles) 
{
  /* Get the total number of indexes going downward including logfile */
  int i=0;
  struct logfile_rec *log;

  if (special_logfiles_array == NULL || nlogfiles < 1 || logfile == NULL) 
	return 0;
  log = special_logfiles_array[i];

  while (log != NULL && i < nlogfiles) {
	if (memcmp(log, logfile, sizeof(struct logfile_rec)) == 0) 
	  return (nlogfiles - i);
	i++;
	log = special_logfiles_array[i];
  }
  return 0;
}

/***************************************************************************/
/*                                                                         */
/*                            get_nindexes_upward                          */
/*                                                                         */
/***************************************************************************/
int	get_nindexes_upward(struct logfile_rec **special_logfiles_array, 
						struct logfile_rec *logfile, int nlogfiles) 
{
  /* Get the total number of indexes going upward including logfile */
  int i=0;
  struct logfile_rec *log;

  if (special_logfiles_array == NULL || nlogfiles < 1 || logfile == NULL) 
	return 0;

  log = special_logfiles_array[i];
  while (log != NULL && i < nlogfiles) {
	if (memcmp(log, logfile, sizeof(struct logfile_rec)) == 0) 
	  return (i+1);
	i++;
	log = special_logfiles_array[i];
  }
  return 0;
}

/***************************************************************************/
/*                                                                         */
/*                           get_next_logfile_index                        */
/*                                                                         */
/***************************************************************************/
int	get_next_logfile_index(struct logfile_rec **special_logfiles_array, 
						   struct logfile_rec *logfile, int nlogfiles) 
{
  int i=0;
  struct logfile_rec *log;

  if (special_logfiles_array == NULL || nlogfiles <= 0 ||
	  logfile == NULL) return -1;
  log = special_logfiles_array[i];

  while (log && i < nlogfiles) {

	if (memcmp(log, logfile, sizeof(struct logfile_rec)) == 0) {
	  if (i+1 < nlogfiles)
		return i+1;
	  else return -1;
	}
	i++;
	log = special_logfiles_array[i];
  }
  return -1;
}


/***************************************************************************/
/*                                                                         */
/*                     get_array_index_for_logfile                         */
/*                                                                         */
/***************************************************************************/
int get_array_index_for_logfile(struct logfile_rec **special_logfiles_array,
								struct logfile_rec *logfile, int nlogfiles) 
{
  int i=0;
  struct logfile_rec *log;

  if (special_logfiles_array == NULL || nlogfiles < 1 || logfile == NULL) 
	return -1;
  log = special_logfiles_array[i];

  while (log != NULL && i < nlogfiles) {
	if (memcmp(log, logfile, sizeof(struct logfile_rec)) == 0) 
	  return i;
	i++;
	log = special_logfiles_array[i];
  }
  return -1;
}

/***************************************************************************/
/*                                                                         */
/*                     logfile_rec * get_previous_logfile                  */
/*                                                                         */
/***************************************************************************/
struct logfile_rec * get_previous_logfile(struct logfile_rec *logfile)
{
  struct logfile_rec *tmplog;


  if (logfile == NULL) return NULL;
  /* returns previous logfile that has entry */
  tmplog = logfile->prev;
  while (tmplog && tmplog->nentries == 0) 
	tmplog = tmplog->prev;
  return tmplog;
} 

/***************************************************************************/
/*                                                                         */
/*                     logfile_rec * get_first_logfile                     */
/*                                                                         */
/***************************************************************************/
struct logfile_rec * get_first_logfile(struct graphic_status_rec *gstat)
{
  struct logfile_rec *tmplog;

  if (gstat == NULL) return NULL;

  tmplog = gstat->logfiles;
  while (tmplog && tmplog->nentries == 0) 
	tmplog = tmplog->next;
  return tmplog;
} 

/***************************************************************************/
/*                                                                         */
/*                     get_scroll_step                                     */
/*                                                                         */
/***************************************************************************/
int get_scroll_step(float new_per)
{
  return  ((new_per + .01) / SLIDER_STEP);
}


void set_need_add_to_chart_flag(struct logfile_entry *logdata)
{
  struct logfile_entry *entry;

  entry = logdata;
  while (entry) {
	entry->draw_flag = ADD_ITEM;
	entry = entry->next;
  }
}

/***************************************************************************/
/*                                                                         */
/*                     set_need_redraw_flag_for_logitems                   */
/*                                                                         */
/***************************************************************************/
void set_need_redraw_flag_for_logitems(struct logfile_entry *logdata)
{
  struct logfile_entry *entry;

  entry = logdata;
  while (entry) {
	entry->draw_flag = UPDATE_ITEM;
	entry = entry->next;
  }
}

/***************************************************************************/
/*                                                                         */
/*                     set_need_refresh_chart_flag                         */
/*                                                                         */
/***************************************************************************/
void set_need_refresh_chart_flag(struct graphic_status_rec *gstat, int value)
{
  int i;
  if (gstat == NULL) return;

  for (i = 0; i < MAX_CHARTS; i++) {
	if (gstat->chart_list[i] &&	gstat->chart_list[i]->logfile) {

	  gstat->chart_list[i]->logfile->need_refresh_chart = value;
	  set_need_redraw_flag_for_logitems(gstat->chart_list[i]->logfile->data);
	  /* set need refresh chart flag */
	  gstat->chart_list[i]->logfile->need_refresh_chart = 1;
	}
  }
}


/***************************************************************************/
/*                                                                         */
/*                                   get_logfile                           */
/*                                                                         */
/***************************************************************************/
struct logfile_rec *get_logfile(struct logfile_rec *logfiles,
								char *host, char *tapeid, char *sdate,
								char *stime, char *prog)
{
  /* Returns logfile with the specified hostt, tapeid, sdate, stime, & prog.
   */
  struct logfile_rec *log;

  if (logfiles == NULL || host == NULL || tapeid == NULL ||
	  sdate == NULL || stime == NULL || prog == NULL) return NULL;
  log = logfiles;

  while (log) {
	if (strcmp(host, log->fromhost) == 0 &&
		strcmp(tapeid, log->tapeid) == 0 &&
		strcmp(sdate, log->sdate) == 0   &&
		strcmp(stime, log->stime) == 0   &&		
		strcmp(prog, log->prog) == 0)
	  return log;
	log = log->next;
  }
  return NULL;
  
} /* get_logfile */



/***************************************************************************/
/*                                                                         */
/*                    get_logfile_index_at_scroll_percent                  */
/*                                                                         */
/***************************************************************************/
int get_logfile_index_at_scroll_percent(struct graphic_status_rec *gstat,
				  float scroll_per)
{
  /* scroll_per value returned value
   *    0.0              0
   *    x     (index = ((x+.05) * gstat->nbar_array_items)) if 
   *                   index < gstat->nbar_array_items  else
   *          (gstat->nbar_array_items - 1)
   */
  int index;

  if (gstat == NULL || scroll_per < 0.0 || scroll_per > 1.0) return -1;

  if (scroll_per == 0.0) return 0;
  /*
  if (scroll_per == 1.0) return (gstat->nbar_array_items - MAX_CHARTS);
  */
  /* scroll_per = 0.x */
  index = (int) ((scroll_per+0.05) * (float) gstat->nbar_array_items);
  /*
  if ((gstat->nbar_array_items - index) < MAX_CHARTS) 
	index = gstat->nbar_array_items - MAX_CHARTS;
	*/
  if (index >= gstat->nbar_array_items)
	index = gstat->nbar_array_items - 1;
  return index;

}  /* get_logfile_index_at_scroll_percent */


/***************************************************************************/
/*                                                                         */
/*                     update_charts                                       */
/*                                                                         */
/***************************************************************************/
int update_charts(struct graphic_status_rec *gstat,
				  float scroll_per)
{
  /* scroll_per is used to determine which logfiles in
   * gstat->bar_sorted_logfiles_array are going to be displayed on form.
   *  Value of scroll_per    Logfiles to be displayed (indices)
   *  -------------------    ------------------------------------
   *          0.0                     0 to MAX_CHARTS
   *          x                       k|n to m
   * if ((x+.05) * gstat->nbar_array_items) < gstat->nbar_array_items) then 
   * use k; otherwise, use n.
   *
   * where:
   * i = (gstat->nbar_array_items-MAX_CHARTS)
   * j = (gstat->nbar_array_items-1)
   * k = ((x+.05) * gstat->nbar_array_items) 
   * n = gstat->nbar_array_items-1
   * m = k|n + MAX_CHARTS  or gstat->nbar_array_items
   *
   * Returns 1 if any change occurs; 0, otherwise 
   */
  int changed = 0;
  int index;

  if (gstat == NULL || gstat->chart_list[0] == NULL) return changed;

  /* Get index of the first logfile to be shown.  */
  index = get_logfile_index_at_scroll_percent(gstat, scroll_per);
  /*
printf("updaet_charts:: per = %f, index = %d, nitems = %d\n", 
	   scroll_per, index, gstat->nbar_array_items);
	   */
  if (index >= 0 && index < gstat->nbar_array_items) {

	set_logfiles_to_chart_list(gstat, 0, MAX_CHARTS, index);
	changed = 1;
  }

  if (changed) {
	/*  load logfiles' data to charts area  */
	load_logfiles_data_to_charts(gstat);
	return 1;
  }
  return 0;
} /* update_charts */

/***************************************************************************/
/*                                                                         */
/*                              remove_tape_group_from_list                */
/*                                                                         */
/***************************************************************************/
int remove_tape_group_from_list(struct tape_grouping_rec **grp_list, 
								char  *s)
{
  /* Removes group from list that has the given tapeid and host from s.
   * s = "$hostname.$tapeid.$prodname.log".
   * Returns 1 if something was removed; 0 otherwise.
   */

  struct tape_grouping_rec *list, *last_grp, *grp, *removed_grp=NULL;
  char tapeid[MAX_TAPEID_LEN], host[MAX_HOSTNAME_LEN];

  list = *grp_list;
  if (list == NULL || s == NULL) return 0;
  memset(tapeid, '\0', MAX_TAPEID_LEN);
  memset(host, '\0', MAX_HOSTNAME_LEN);
  extract_info_from_filename(s, host, tapeid, NULL);
  grp = list;
  last_grp = NULL;
  while (grp) {
	if (strcmp(host, grp->host) == 0 && strcmp(tapeid, grp->tapeid) == 0) {
	  /* Found grp, remove it from list */
	  removed_grp = grp;

	  break;
	}
	last_grp = grp;
	grp = grp->next;
  }
  if (removed_grp == NULL) return 0;
  if (last_grp == NULL) {
	/* Removed group is the  first group on list */
	*grp_list = list->next;
  }
  else {
	last_grp->next = removed_grp->next;
  }
  removed_grp->next = NULL;
  free_tape_group(removed_grp);

  removed_grp = NULL;
  return 1;

} /* remove_tape_group_from_list */

/***************************************************************************/
/*                                                                         */
/*                    update_tape_grouping_form                             */
/*                                                                         */
/***************************************************************************/
void update_tape_grouping_form() 
{

   if (tape_grouping_form == NULL || tape_grouping_form->group_form == NULL ||
	   tape_grouping_form->browser == NULL || 
	   !tape_grouping_form->group_form->visible) 
	 return;

   fl_deactivate_object(tape_grouping_form->browser);
   fl_freeze_form(tape_grouping_form->group_form);
   load_items_to_browser(tape_grouping_form->group_form, 
						 tape_grouping_form->browser, TAPE_GROUP, "", CLEAR);

   /* Redraw on top of browser. */
   if (tape_grouping_form->divider_bar)
	 fl_show_object(tape_grouping_form->divider_bar);
   fl_unfreeze_form(tape_grouping_form->group_form);
   fl_activate_object(tape_grouping_form->browser);
} /* update_tape_grouping_form */

/***************************************************************************/
/*                                                                         */
/*                     update_status_form                                  */
/*                                                                         */
/***************************************************************************/
void update_status_form(float scroll_per, int reload)
{
  int i;
  char host[MAX_HOSTNAME_LEN], tapeid[MAX_TAPEID_LEN];
  /*
  char msg[MAX_LABEL_LEN];
  */
/*
printf("enter update status form\n");
*/
  /*
printf("***gst: sper = %f, scroll_per = %f\n", gstatus_info->scroll_per,
	   scroll_per);
	   */
  if (gstatus_info == NULL || gstatus_info->sform == NULL || 
	  gstatus_info->sform->slider == NULL) {
	fprintf(stderr, "ERROR: status info is NULL\n");
	exit_system(NULL, -1);
  }

  /* update status form if it is visible.
   */
  if ( gstatus_info->sform->status_form == NULL || 
	   !gstatus_info->sform->status_form->visible)
	goto DONE;
  fl_deactivate_object(gstatus_info->sform->slider);
  fl_freeze_form(gstatus_info->sform->status_form);

/*
printf("enter update status form \n");
*/

  if (gstatus_info->refresh_form || reload) {
	/* Clear and re-create sorted logfile list. */
	for (i=0; i<MAX_CHARTS; i++) {
	  if (gstatus_info->chart_list[i] == NULL) continue;
	  gstatus_info->chart_list[i]->logfile = NULL;
	}

	/* Create array of logfile pointers and keeps it arround until logfiles
	 * list has been modified.  This is used in order to call
	 * qsort() in make_special_logfile_list().  Leave this list around until
	 * form is closed or reload == 1 or refresh_form == 1.
	 */
	/* Free previous allocated list */
	if (gstatus_info->bar_sorted_logfiles_array) {
	  free(gstatus_info->bar_sorted_logfiles_array );
	  gstatus_info->bar_sorted_logfiles_array=NULL;
	  gstatus_info->nbar_array_items = 0;
	}

	gstatus_info->bar_sorted_logfiles_array = (struct logfile_rec **)
	  calloc(gstatus_info->nlogfiles, sizeof(struct logfile_rec *));

	if (gstatus_info->bar_sorted_logfiles_array == NULL) {
	  perror("calloc bar_sorted_logfiles_array1\n");
	  exit_system(NULL, -1);
	}


	memset(host, '\0', MAX_HOSTNAME_LEN);
	memset(tapeid, '\0', MAX_TAPEID_LEN);
	if (forms_view_options[STATUSF_VIEW_OPTION_IND] & 
		VIEW_ONE_TAPE_GRPING) {
	  /* Get host and tapeid from curr_selected_group */
	  extract_info_from_selected_grp_str(curr_selected_tape_group, host, tapeid);

	  if (strlen(host) == 0 || strlen(tapeid) ==  0) {
		/*
		sprintf(msg, "Select by clicking on an item on form:\n %s\n", TAPE_GROUPING_FORM_NAME);
		fl_show_message("WARNING!No tape grouping was selected.", msg);
		*/
		free(gstatus_info->bar_sorted_logfiles_array);
		gstatus_info->bar_sorted_logfiles_array=NULL;
		gstatus_info->nbar_array_items = 0;
		/* change state of view menu back */
		/*
		if (gstatus_info->sform && gstatus_info->sform->view_menu) {
		  forms_view_options[STATUSF_VIEW_OPTION_IND] ^= VIEW_ONE_TAPE_GRPING;
		  set_view_menu_options(gstatus_info->sform->view_menu, &(forms_view_options[STATUSF_VIEW_OPTION_IND]), STATUS_FORM_ID);
		}
		*/
		goto RESET_FORM;
	  }
	  
	}

	/* Create logfile list sorted either by end time or by host, tapeid, 
	 * and prog.
	 */
	make_special_logfile_list(gstatus_info->logfiles, gstatus_info->nlogfiles,
							  forms_view_options[STATUSF_VIEW_OPTION_IND],
							  host, tapeid,
							  gstatus_info->bar_sorted_logfiles_array,
							  &(gstatus_info->nbar_array_items));

RESET_FORM:
	clear_all_charts(gstatus_info->chart_list, MAX_CHARTS);
	/*
  fprintf(stderr, "host: %s, tapeid:%s\n", host, tapeid);
  */
	gstatus_info->refresh_form = 0; /* reset flag */
  }
  
  /* update charts on form */
  update_charts(gstatus_info, scroll_per);
  gstatus_info->scroll_per = scroll_per;

  /* update form's labels (number of bars, and bar #) */
  update_labels_for_status_form(gstatus_info);


  fl_unfreeze_form(gstatus_info->sform->status_form);
  fl_activate_object(gstatus_info->sform->slider);
 DONE:
/*
  printf("exit update status form \n");
*/
  return;

} /* update_status_form */





/***************************************************************************/
/*                                                                         */
/*                     get_prod_index                                      */
/*                                                                         */
/***************************************************************************/
int get_prod_index(char *prod)
{
  int i;

  if (prod == NULL || strlen(prod) == 0) return -1;

  for (i = 0; i < MAX_PRODS; i++) {
	if (strcmp(prod, prod_tbl[i]) == 0)
	  return i;
  }
  return -1;
}

/***************************************************************************/
/*                                                                         */
/*                     *get_prod_name                                      */
/*                                                                         */
/***************************************************************************/
char *get_prod_name(int pindex)
{

  if (pindex < 0 || pindex >= MAX_PRODS) return EMPTY_STR;
  return (prod_tbl[pindex]);
} 

/***************************************************************************/
/*                                                                         */
/*                               show_sys_msg                              */
/*                                                                         */
/***************************************************************************/
void show_sys_msg(char *msg)
{
  FL_Coord mx, my;
  unsigned int mask;
  
  if (msg == NULL || strlen(msg) == 0) return;

  /*Use mouse position */
  fl_get_mouse(&mx, &my, &mask);
  mx +=  10;
  my += 10;

  /*
  fl_set_oneliner_color(FL_SLATEBLUE, FL_WHEAT);
  */
  fl_show_oneliner(msg, mx, my);
} /* show_sys_msg */

/***************************************************************************/
/*                                                                         */
/*                               hide_sys_msg                              */
/*                                                                         */
/***************************************************************************/
void hide_sys_msg()
{
  fl_hide_oneliner();
}

/***************************************************************************/
/*                                                                         */
/*               set_pixmapbuttons_for_tape_grouping_form                  */
/*                                                                         */
/***************************************************************************/
void set_pixmapbuttons_for_tape_grouping_form()
{
  if (tape_grouping_form == NULL || tape_grouping_form->group_form == NULL ||
	  tape_grouping_form->del_job_button == NULL ||
	  tape_grouping_form->all_data_button == NULL ||
	  tape_grouping_form->progs_button == NULL ||
	  tape_grouping_form->neighbors_button == NULL ||
	  tape_grouping_form->report_button == NULL ||
	  tape_grouping_form->ps_button == NULL ||
	  tape_grouping_form->bar_button == NULL ||
	  tape_grouping_form->reset_button == NULL ||
	  tape_grouping_form->help_button == NULL ||
	  tape_grouping_form->save_button == NULL ||
	  tape_grouping_form->print_button == NULL ||
	  tape_grouping_form->exit_button == NULL ||
	  tape_grouping_form->exceeded_threshold_button == NULL)

	return;
  /* change to busy cursor */
  set_cursor_for_all_forms(XC_watch);

  show_sys_msg(WAIT_STR);

  fl_set_pixmapbutton_file(tape_grouping_form->bar_button, bars_icon);
  fl_set_pixmapbutton_file(tape_grouping_form->ps_button, ps_icon);

  fl_set_pixmapbutton_file(tape_grouping_form->report_button, report_icon);

  fl_set_pixmapbutton_file(tape_grouping_form->neighbors_button, neighbors_icon);
  fl_set_pixmapbutton_file(tape_grouping_form->progs_button, progs_icon);
  fl_set_pixmapbutton_file(tape_grouping_form->exceeded_threshold_button, exceeded_threshold_icon);

  fl_set_pixmapbutton_file(tape_grouping_form->exit_button, exit_icon);
  fl_set_pixmapbutton_file(tape_grouping_form->reset_button, reset_icon);
  fl_set_pixmapbutton_file(tape_grouping_form->print_button, print_icon);
  fl_set_pixmapbutton_file(tape_grouping_form->save_button, save_icon);
  fl_set_pixmapbutton_file(tape_grouping_form->help_button, help_icon);
  fl_set_pixmapbutton_file(tape_grouping_form->detail_button, show_detail_icon);
  fl_set_pixmapbutton_file(tape_grouping_form->all_data_button, show_all_data_icon);
  fl_set_pixmapbutton_file(tape_grouping_form->del_job_button, delete_job_icon);
  hide_sys_msg();
	/* change to normal cursor */
  reset_cursor_for_all_forms();

} /* set_pixmapbuttons_for_tape_grouping_form */


/***************************************************************************/
/*                                                                         */
/*                      set_pixmapbuttons_for_product_grouping_form        */
/*                                                                         */
/***************************************************************************/
void set_pixmapbuttons_for_product_grouping_form()
{
  if (product_grouping_form == NULL || product_grouping_form->gms_form == NULL ||
	  product_grouping_form->help_button == NULL ||
	  product_grouping_form->save_button == NULL ||
	  product_grouping_form->print_button == NULL ||
	  product_grouping_form->cancel_button == NULL ||
	  product_grouping_form->del_job_button == NULL)
	return;
  /* change to busy cursor */
  set_cursor_for_all_forms(XC_watch);
  show_sys_msg(WAIT_STR);
  fl_set_pixmapbutton_file(product_grouping_form->cancel_button, cancel_icon);
  fl_set_pixmapbutton_file(product_grouping_form->print_button, print_icon);
  fl_set_pixmapbutton_file(product_grouping_form->save_button, save_icon);
  fl_set_pixmapbutton_file(product_grouping_form->del_job_button, delete_job_icon);
  fl_set_pixmapbutton_file(product_grouping_form->help_button, help_icon);

  hide_sys_msg();
  /* change to normal cursor */
  reset_cursor_for_all_forms();
} /* set_pixmapbuttons_for_product_grouping_form */

/***************************************************************************/
/*                                                                         */
/*                    set_pixmapbuttons_for_summary_report_form               */
/*                                                                         */
/***************************************************************************/
void set_pixmapbuttons_for_summary_report_form()
{
  if (summary_report_form == NULL || 
	  summary_report_form->report_form == NULL ||
	   summary_report_form->cancel_button == NULL ||
	  summary_report_form->refresh_button == NULL ||
	  summary_report_form->save_button == NULL ||
	  summary_report_form->print_red_button == NULL ||
	  summary_report_form->print_yellow_button == NULL ||
	  summary_report_form->print_all_button == NULL)
	return;
  /* change to busy cursor */
  set_cursor_for_all_forms(XC_watch);

  show_sys_msg(WAIT_STR);
  fl_set_pixmapbutton_file(summary_report_form->cancel_button, cancel_icon);
  fl_set_pixmapbutton_file(summary_report_form->save_button, save_icon);
  fl_set_pixmapbutton_file(summary_report_form->del_job_button, delete_job_icon);
  fl_set_pixmapbutton_file(summary_report_form->print_red_button, print_red_icon);
  fl_set_pixmapbutton_file(summary_report_form->print_yellow_button, print_yellow_icon);
  fl_set_pixmapbutton_file(summary_report_form->print_all_button, print_all_icon);
  fl_set_pixmapbutton_file(summary_report_form->refresh_button, reset_icon);
  fl_set_pixmapbutton_file(summary_report_form->help_button, help_icon);

  hide_sys_msg();
  /* change to normal cursor */
  reset_cursor_for_all_forms();
} /* set_pixmapbuttons_for_summary_report_form */


/***************************************************************************/
/*                                                                         */
/*                         set_pixmapbuttons_for_logfile_form              */
/*                                                                         */
/***************************************************************************/
void set_pixmapbuttons_for_logfile_form(FD_logfile_form *form)
{
  if (form == NULL ||
	  form->cancel_button == NULL ||
	  form->save_button == NULL ||
	  form->print_red_button == NULL ||
	  form->print_yellow_button == NULL ||
	  form->print_all_button == NULL ||
	  form->del_job_button == NULL)
	return;
  /* change to busy cursor */
  set_cursor_for_all_forms(XC_watch);

  show_sys_msg(WAIT_STR);
  fl_set_pixmapbutton_file(form->cancel_button, cancel_icon);
  fl_set_pixmapbutton_file(form->save_button, save_icon);
  fl_set_pixmapbutton_file(form->print_red_button, print_red_icon);
  fl_set_pixmapbutton_file(form->print_yellow_button, print_yellow_icon);
  fl_set_pixmapbutton_file(form->print_all_button, print_all_icon);
  fl_set_pixmapbutton_file(form->del_job_button, delete_job_icon);
  fl_set_pixmapbutton_file(form->help_button, help_icon);
  hide_sys_msg();
  /* change to normal cursor */
  reset_cursor_for_all_forms();

} /* set_pixmapbuttons_for_logfile_form */


/***************************************************************************/
/*                                                                         */
/*               free_pixmapbuttons_for_tape_grouping_form                  */
/*                                                                         */
/***************************************************************************/
void free_pixmapbuttons_for_tape_grouping_form()
{
  if (tape_grouping_form || tape_grouping_form->group_form == NULL ||
	  tape_grouping_form->del_job_button == NULL ||
	  tape_grouping_form->all_data_button == NULL ||
	  tape_grouping_form->progs_button == NULL ||
	  tape_grouping_form->neighbors_button == NULL ||
	  tape_grouping_form->report_button == NULL ||
	  tape_grouping_form->ps_button == NULL ||
	  tape_grouping_form->bar_button == NULL ||
	  tape_grouping_form->reset_button == NULL ||
	  tape_grouping_form->help_button == NULL ||
	  tape_grouping_form->save_button == NULL ||
	  tape_grouping_form->print_button == NULL ||
	  tape_grouping_form->exit_button == NULL ||
	  tape_grouping_form->exceeded_threshold_button == NULL ||
	  tape_grouping_form->failure_light_button == NULL ||
	  tape_grouping_form->warning_light_button == NULL ||
	  tape_grouping_form->running_light_button == NULL ||
	  tape_grouping_form->unknown_light_button == NULL)
	return;

  fl_free_pixmapbutton_pixmap(tape_grouping_form->bar_button);
  fl_free_pixmapbutton_pixmap(tape_grouping_form->ps_button);

  fl_free_pixmapbutton_pixmap(tape_grouping_form->report_button);

  fl_free_pixmapbutton_pixmap(tape_grouping_form->neighbors_button);
  fl_free_pixmapbutton_pixmap(tape_grouping_form->progs_button);
  fl_free_pixmapbutton_pixmap(tape_grouping_form->exceeded_threshold_button);


  fl_free_pixmapbutton_pixmap(tape_grouping_form->exit_button);
  fl_free_pixmapbutton_pixmap(tape_grouping_form->reset_button);
  fl_free_pixmapbutton_pixmap(tape_grouping_form->print_button);
  fl_free_pixmapbutton_pixmap(tape_grouping_form->save_button);
  fl_free_pixmapbutton_pixmap(tape_grouping_form->help_button);
  fl_free_pixmapbutton_pixmap(tape_grouping_form->detail_button);
  fl_free_pixmapbutton_pixmap(tape_grouping_form->all_data_button);
  fl_free_pixmapbutton_pixmap(tape_grouping_form->del_job_button);
} /* free_pixmapbuttons_for_tape_grouping_form */


/***************************************************************************/
/*                                                                         */
/*                      free_pixmapbuttons_for_product_grouping_form        */
/*                                                                         */
/***************************************************************************/
void free_pixmapbuttons_for_product_grouping_form()
{
  if (product_grouping_form == NULL || product_grouping_form->gms_form == NULL ||
	  product_grouping_form->help_button == NULL ||
	  product_grouping_form->save_button == NULL ||
	  product_grouping_form->print_button == NULL ||
	  product_grouping_form->cancel_button == NULL ||
	  product_grouping_form->del_job_button == NULL)
	return;
  fl_free_pixmapbutton_pixmap(product_grouping_form->cancel_button);
  fl_free_pixmapbutton_pixmap(product_grouping_form->print_button);
  fl_free_pixmapbutton_pixmap(product_grouping_form->save_button);
  fl_free_pixmapbutton_pixmap(product_grouping_form->del_job_button);
  fl_free_pixmapbutton_pixmap(product_grouping_form->help_button);
} /* free_pixmapbuttons_for_product_grouping_form */

/***************************************************************************/
/*                                                                         */
/*                    free_pixmapbuttons_for_summary_report_form               */
/*                                                                         */
/***************************************************************************/
void free_pixmapbuttons_for_summary_report_form()
{
  if (summary_report_form == NULL || 
	  summary_report_form->report_form == NULL ||
	   summary_report_form->cancel_button == NULL ||
	  summary_report_form->refresh_button == NULL ||
	  summary_report_form->save_button == NULL ||
	  summary_report_form->print_red_button == NULL ||
	  summary_report_form->print_yellow_button == NULL ||
	  summary_report_form->print_all_button == NULL)
	return;

  fl_free_pixmapbutton_pixmap(summary_report_form->cancel_button);
  fl_free_pixmapbutton_pixmap(summary_report_form->save_button);
  fl_free_pixmapbutton_pixmap(summary_report_form->print_red_button);
  fl_free_pixmapbutton_pixmap(summary_report_form->print_yellow_button);
  fl_free_pixmapbutton_pixmap(summary_report_form->print_all_button);
  fl_free_pixmapbutton_pixmap(summary_report_form->refresh_button);
  fl_free_pixmapbutton_pixmap(summary_report_form->help_button);
} /* free_pixmapbuttons_for_summary_report_form */


/***************************************************************************/
/*                                                                         */
/*                         free_pixmapbuttons_for_logfile_form              */
/*                                                                         */
/***************************************************************************/
void free_pixmapbuttons_for_logfile_form(FD_logfile_form *form)
{
  if (form == NULL ||
	  form->cancel_button == NULL ||
	  form->save_button == NULL ||
	  form->print_red_button == NULL ||
	  form->print_yellow_button == NULL ||
	  form->print_all_button == NULL ||
	  form->del_job_button == NULL)
	return;
  fl_free_pixmapbutton_pixmap(form->cancel_button);
  fl_free_pixmapbutton_pixmap(form->save_button);
  fl_free_pixmapbutton_pixmap(form->print_red_button);
  fl_free_pixmapbutton_pixmap(form->print_yellow_button);
  fl_free_pixmapbutton_pixmap(form->print_all_button);
  fl_free_pixmapbutton_pixmap(form->del_job_button);
  fl_free_pixmapbutton_pixmap(form->help_button);
} /* free_pixmapbuttons_for_logfile_form */
