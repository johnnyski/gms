
#ifndef __GTS_MONITOR_H__
#define __GTS_MONITOR_H__ 1

#include <time.h>

#ifndef __GTS_MONITOR_FORM_H__
#define __GTS_MONITOR_FORM_H__ 1
#include "gts_monitor_form.h"
#endif
#include "gms_comm.h"

#define   VERSION_STR         PROG_VERSION

#define  WRITE_TO_FILE 1
#define   TEST 1

/* button's info */
#define MAX_BUTTONS                    100
/* Pixmap button's size */
#define PB_WIDTH                       32
#define PB_HEIGHT                      22
/* form names */
#define BARS_FORM_NAME                 "Information for Product Groupings (Bars)"
#define SUMMARY_REPORT_FORM_NAME       "Status Summary Report"
#define TEST_FORM_NAME                 "Test Window"
#define TEST_GET_INPUT_FORM_NAME       "Test Form"
#define FILE_REPORT_FORM_NAME          "File Status Report"
#define FILE_STAT_FORM_NAME            "Information for Job Groupings"
#define INPUT_GVS_FORM_NAME             "Input (Product Groupings To File)"
#define INPUT_FILE_STAT_FORM_NAME       "Input (Jobs To File)"
#define INPUT_SUMMARY_RPT_FORM_NAME     "Input (Summary Report To File)"
#define INPUT_FILE_RPT_FORM_NAME        "Input (Jobs(Report) To File)"
#define INPUT_RUN_GRP_FORM_NAME         "Input (Tape Groupings To File)"
#define INPUT_FILE_ALL_DATA_FORM_NAME   "Input(All Data to File)"
#define INPUT_FILE_EXCEEDED_THRESHOLD_FORM_NAME   "Input (Exceeded Threshold Jobs to  File)"
#define SYSTEM_MSG_FORM_NAME           "GMS System Messages"
#define ALL_DATA_FORM_NAME             "All Data (Sorted by time)"
#define NEIGHBOR_LIST_FORM_NAME        "Neighbor List"
#define SELECT_NEIGHBOR_LIST_FORM_NAME "Neighbor List"
#define PROGS_FORM_NAME                "Program Configuration"
#define ADMIN_FORM_NAME                "Network Configuration"
#define NEIGHBORS_FORM_NAME            "Neighbor Configuration"
#define PS_INFO_FORM_NAME              "PS Information"
#define GVS_MONITOR_FORM_NAME          "Information for Product Groupings"
#define TAPE_GROUPING_FORM_NAME        "GVS Monitoring System (Information for Host and TapeID Groupings)"
#define EXCEEDED_THRESHOLD_FORM_NAME   "Jobs Having Runtime Exceeded Thresdhold"
#define GV_LEVELS_FORM_NAME             "GV Levels Configuration"
#define STATUS_FIELDS_SELECTION_FORM_NAME "Fields Selection Form"

#define MAX_FORMS                      26

/* menu related definitions */
/* Note: the following indices correspond to the order listed when 
 * calling  fl_set_menu_item_mode()
 */
#define SHOW_TOOLBAR_MINDEX       1
#define SHOW_BUTTON_INFO_MINDEX   SHOW_TOOLBAR_MINDEX + 1
#define BLINK_BOXES_MINDEX        SHOW_BUTTON_INFO_MINDEX + 1


#define VIEW_ETIME_SORTED_MINDEX       1
#define VIEW_PROG_SORTED_MINDEX        VIEW_ETIME_SORTED_MINDEX + 1
#define VIEW_SEPARATOR1_MINDEX         VIEW_PROG_SORTED_MINDEX + 1
#define VIEW_ACTIVE_ONLY_MINDEX        VIEW_SEPARATOR1_MINDEX + 1
#define VIEW_INACTIVE_ONLY_MINDEX      VIEW_ACTIVE_ONLY_MINDEX + 1
#define VIEW_SEPARATOR2_MINDEX         VIEW_INACTIVE_ONLY_MINDEX + 1
#define VIEW_FAILURES_MINDEX           VIEW_SEPARATOR2_MINDEX + 1
#define VIEW_WARNINGS_MINDEX           VIEW_FAILURES_MINDEX + 1
#define VIEW_RUNNINGS_MINDEX           VIEW_WARNINGS_MINDEX + 1
#define VIEW_SUCCESSFULS_MINDEX        VIEW_RUNNINGS_MINDEX + 1
#define VIEW_UNKNOWNS_MINDEX           VIEW_SUCCESSFULS_MINDEX + 1
#define VIEW_ALL_MINDEX                VIEW_UNKNOWNS_MINDEX + 1
#define VIEW_SEPARATOR3_MINDEX         VIEW_ALL_MINDEX + 1
#define VIEW_ONE_TAPE_GRPING_MINDEX    VIEW_SEPARATOR3_MINDEX + 1


#define MENU_ITEM_MODE_MASK       FL_PUP_CHECK | FL_PUP_CHECK
#define MAX_WINDOWS_MENU_STR      340

/* time before refresh forms--must be non-zero */
#define UPDATE_FORMS_INTERVAL 3   /* seconds. This applies to all modified forms. */
#define BEFORE_SHOWING_TIME   1   /* sec-- this applies to button's info form.*/
#define SHOWING_TIME          2   /* sec-- this applies to button's info form.*/
#define UPDATE_RUNTIME_INTERVAL 15 /* seconds elapse before updating runtime */
#define UPDATE_BLINK_INTERVAL 1  /* sec. -- for blinking light buttons */
#define UPDATE_TRANS_STATUS_INTERVAL 1 /* sec. */
#define UPDATE_FORMS_DEFAULT  1   /* When starts up */

/* Font indexes*/
#define LABEL_FONT_STYLE      16
#define TEXT_FONT_STYLE       17
#define TEXT_SMALL_FONT_STYLE 18

#define MAX_GV_LEVELS_NUM     10    /* Actual is 3, but want to leave
									 * extra numbers for user defined.
									 */
#define MAX_LINE_LEN          256
#define   MAX_HOSTS           20
#define   MAX_PRODS           13
#define   MAX_HOSTNAME_LEN    40
#define   MAX_ITEMS           100
#define   MAX_ITEMNAME_LEN    256
#define   MAX_PROGS           MAX_ITEMS
#define   MAX_PROGS_PER_PROD  20
#define   MAX_TAPES           30

#define   CHART_HEIGHT        40

#define   MAX_CHARTS          5
#define   MAX_CHART_ITEMS     1000
#define   SLIDER_STEP         (float) (1.0 /MAX_CHARTS)
#define   MAX_PS_DATA_TYPES   2
#define   MAX_SYS_STATUS_MSGS 20
#define   MAX_SYS_STATUS_MSG_LEN MAX_BROWSER_LINE_LEN * 20

#ifndef MAX_NAME_LEN
#define MAX_NAME_LEN          40
#endif
#define   MAX_SUB_LEVELS      9

#define   MAX_CHARTNAME_LEN   6
#define   MAX_FILENAME_LEN    300
#define   MAX_STATUS_STR_LEN  10
#define   MAX_PROGNAME_LEN    22
#define   MAX_PRODNAME_LEN    MAX_PROGNAME_LEN
#define   MAX_ITEM_LEN        MAX_PROGNAME_LEN
#define   MAX_STATUS_LINE_LEN 500
#define   MAX_LABEL_LEN       400
#define   MAX_SMALL_LABEL_LEN       20
#define   MAX_MEDIUM_LABEL_LEN      50
#define   MAX_TAPEID_LEN      17
#define   MAX_TIME_STR_LEN    20

/*
#define   MAX_STR_LEN         250
*/
#define   MAX_STR_LEN         400
#define   MAX_BROWSER_LINE_LEN MAX_STR_LEN 
#define   MAX_RANGE_LEN       10
#define   MAX_PS_BUF_LEN      21000
#define   MAX_PS_ITEMS        1000
#define   SEPARATOR_STR       "======================================="
#define   LOCAL_LOGFILE_PATH  "./loginfo"
#define   TESTDATA_PATH       "./testdata"


/* ~/.gms file */
#define   GMS_INIT_FILE       ".gms"
/* GMS_NETWORK_HOSTS_FILENAME is defined in the Makefile. */
#ifdef GMS_NETWORK_HOSTS_FILENAME
   #define   NETWORK_HOSTS_FILENAME GMS_NETWORK_HOSTS_FILENAME
#else
   #define   NETWORK_HOSTS_FILENAME ""
#endif
#define   SELECTED_PROGS_COMMENT_LINE "# Monitored programs"
#define   SELECTED_HOSTS_COMMENT_LINE "# Monitored neighbors"
#define   OPTIONS_COMMENT_LINE        "# Options"
#define   LEVEL_INFO_COMMENT_LINE     "# GV Level Information (level# prog prog...)"
#define   SHOW_BUTTON_INFO_STR "Show_button_info: "
#define   SHOW_TOOLBAR_STR     "Show_toolbar: "
#define   BLINK_BOXES_STR      "Blink_boxes: "

/* status strings */
#define   SUCCESS_STR         "successful"
#define   FAILED_STR          "failed"
#define   ABORT_STR           "aborted"
#define   WARNING_STR         "warning"
#define   RUNNING_STR         "running"
#define   FINISHED_STR        "finished"
#define   UNKNOWN_STR         "unknown"
#define   EMPTY_STR            ""

/* Status abreviation */
#define   HALT_CHAR           ' '
#define   ACTIVE_NOT_RUNNING_CHAR           ' '
#define   RUNNING_CHAR        'R'
#define   EMPTY_CHAR          ' '

/* legend char */
#define   FINISHED_CHAR       '*'
#define   NOT_ACTIVE_CHAR     '-'

#define   RECEIVED_STR        "received"
#define   SENT_STR            "sent"
#define   NA_STR              "N/A"

/* button states */
#define   PUSHED             1
#define   RELEASED           0

/* browser */
#define   CLEAR              1
#define   UNCLEAR            0
#define   SHOW_ALL_LEVELS    "a"
#define   SHOW_TOP_LEVEL     "t"

/* color codes */
#define   FAILURE_COLOR      FL_RED
#define   WARNING_COLOR      FL_YELLOW
#define   RUNNING_COLOR      FL_GREEN
#define   SUCCESS_COLOR      FL_BLACK
#define   FINISHED_COLOR     FL_BLACK
#define   ABORT_COLOR        FL_DARKTOMATO
#define   SYS_STATUS_COLOR   FL_BLACK
#define   DEFAULT_COLOR      FL_CYAN
#define   UNKNOWN_COLOR      DEFAULT_COLOR

#define WAIT_STR             "Please wait..."
extern void hide_sys_msg();
extern void show_sys_msg(char *msg);
#define show_button_info_msg(m) show_sys_msg(m)
#define hide_button_info_msg() hide_sys_msg()

/* browser's text format */
/* Format strs are null since font size and style are defined during form 
 * creation.
 */
#define   TEXT_FORMAT_STR    ""
#define   FINISHED_FORMAT_STR ""
#define   NOT_ACTIVE_FORMAT_STR ""


#define PROGS            1
#define HOSTS            2
#define LOGFILE          3
#define CURR_FILE_STAT   4
#define ADMIN            5

#define SUMMARY_LEVEL    6
#define HOST_FOR_PS      7

/* view options type */
typedef enum view_option_type {
  SORT_BY_ETIME=1, SORT_BY_HOST_TAPE_PROG=2,  VIEW_SUCCESSFULS=4, 
  VIEW_ONE_TAPE_GRPING=8,
  VIEW_FAILURES=16, VIEW_WARNINGS=32, VIEW_RUNNINGS=64, VIEW_UNKNOWNS=128,
  VIEW_ALL=256, VIEW_ACTIVE_ONLY=512, VIEW_INACTIVE_ONLY=1024,
  SORT_BY_LEVEL = 2048, SORT_BY_TAPE=4096
} view_option_type_t;

#define ALL_VIEW_SELECTIONS  (VIEW_SUCCESSFULS | VIEW_ONE_TAPE_GRPING | \
						VIEW_FAILURES | VIEW_WARNINGS | VIEW_RUNNINGS | \
						VIEW_UNKNOWNS)

#define VIEW_DEFAULT (SORT_BY_ETIME | VIEW_ALL | VIEW_ACTIVE_ONLY | VIEW_INACTIVE_ONLY)

#define MAX_FORMS_WITH_VIEW_MENU 4
#define MAX_VIEW_OPTION_TYPES   2
typedef view_option_type_t view_options_t[MAX_FORMS_WITH_VIEW_MENU];

/* view options indexes */
#define STATUSF_VIEW_OPTION_IND 0
#define GMSF_VIEW_OPTION_IND    1
#define GROUPF_VIEW_OPTION_IND  2
#define SUMMARYF_VIEW_OPTION_IND 3

#define SORT_TYPE_IND           0
#define VIEW_TYPE_IND           1


typedef enum conf_type {
  ADD_PROG, ADD_HOST, DEL_PROG, DEL_HOST, ADD_NETWORK_HOST, DEL_NETWORK_HOST
} config_type_t;

typedef struct _form_info {
  char name[MAX_LABEL_LEN];
  FL_FORM *form;
  int min_width, min_height;
} form_info_t;


/* Don't change the order of form_id_type_t--Its value is hard coded in
 * forms via 'fdesign'.
 */
typedef enum form_id_type {
  LOGFILE_FORM_ID,   SUMMARY_REPORT_FORM_ID,     GMS_FORM_ID, 
  GROUP_FORM_ID,     STATUS_FORM_ID,             SYS_MSG_FORM_ID, 
  ALL_DATA_FORM_ID,  EXCEEDED_THRESHOLD_FORM_ID, LOGFILE_REPORT_FORM_ID, 
  JOB_GROUP_FORM_ID, ADD_DELETE_SHOW_FORM_ID,    GV_LEVELS_FORM_ID
} form_id_type_t;

typedef enum grouping_type {
  TAPE_GRPING, PROD_GRPING, JOB_GRPING
} grouping_type_t;

typedef enum item_type {
  SELECTED_HOSTS, AVAILABLE_HOSTS, SELECTED_PROGS, AVAILABLE_PROGS, TAPEID, 
  PROD, LOG_FILE_RNG, STAT_INFO_FILE_RNG,
  LOGFILE_NAME, PS, CURR_PROD, STAT_INFO, LOG_INFO, TAPE_GROUP, ALL_DATA, 
  EXCEEDED_THRESHOLD_JOBS, SELECTED_NETWORK_HOSTS, AVAILABLE_NETWORK_HOSTS, 
  ALL, SUMMARY_INFO, AVAILABLE_SUMMARY_LEVELS
} item_type_t;


typedef enum summary_type {
  TAPE_SUMMARY=1, LEVEL_SUMMARY=2, PRODUCT_SUMMARY=4
} summary_type_t;

#define TAPE_SUMMARY_STR    "Tape ID"
#define LEVEL_SUMMARY_STR   "GVS Level I,II,III"
#define PRODUCT_SUMMARY_STR "GVS Product"



typedef enum draw_type {
  ADD_ITEM, UPDATE_ITEM, NOOPT
} draw_type_t;

/* stat_type_t: status for jobs in the production
 */
typedef enum stat_type {
  SUCCESS=1, FAILED=2, WARNING=4, RUNNING=8, ABORT=16, NOT_RUNNING=32, 
  FINISHED=64,  SYS_STATUS=128,  NOT_DEFINED=256, NO_STATUS = 512, 
  RUNNING_BUT_INACTIVE = 1024,  ACTIVE= 2048, UNKNOWN=NOT_DEFINED
} stat_type_t;

typedef struct prog_stat {
  char * prog;
  stat_type_t stat;
} prog_stat_t;

#define MAX_PROGS_PER_LEVEL 20
typedef struct level_info {
  prog_stat_t prog_info[MAX_PROGS_PER_LEVEL]; /* Programs may not be in consectutive entries. */
  int          nprogs;                        /* The actual number of progs. */
} level_info_t;

typedef enum scroll_type {
  UP, DOWN
} scroll_type_t;

/* sys_stat_type_t: status of gms system 
 */
typedef enum sys_stat_type {
  RECEIVED_FROM_MONITORPS, RECEIVED_FROM_CHILD, SENT_TO_MONITORPS, 
  SENT_TO_PARENT
} sys_stat_type_t;


/* List of jobs for a product (program)*/
struct logfile_entry {
  char date[MAX_NAME_LEN];          /* start date and  time */
  char time_str[MAX_NAME_LEN];
  time_t time_item;                /* time of type time_t */
  double runtime;                   /* total runtime */
  char infile[MAX_FILENAME_LEN];
  int fnum;                         /* file number or jobnum */
  stat_type_t stat;
  draw_type_t draw_flag;            /* add item to chart or update its value */

  /* tape id is in logfilename */
  struct logfile_entry *next;
};

/* List of products from all tapes. */
struct logfile_rec {
  char name[MAX_FILENAME_LEN]; /* logfile name: host.tapeid.prod.log */
  char fromhost[MAX_HOSTNAME_LEN];
  char prog[MAX_PROGNAME_LEN];
  char tapeid[MAX_TAPEID_LEN];
  int  finished;               /* finished processing flag */
  int  active;                 /* currently running or not: this only matters
								* if rerun of job occurs */
  stat_type_t stat;
  struct logfile_entry * data; /* stores file entries */
  struct logfile_entry *last_mod_job;  /* Points to the last modified entry in 
									* the data list */
  struct logfile_entry *last_job;  /* Points to the last entry in 
									* the data list */
  int  nentries;               /* number of log entries--this is equivalent to
								* the last job or file num for this logfile. */
  char sdate[MAX_NAME_LEN];    /* start date and time */
  char stime[MAX_NAME_LEN];
  time_t stime_item;
  char edate[MAX_NAME_LEN];    /* end date and time */
  char etime[MAX_NAME_LEN];
  time_t etime_item;
  char tape_dev[MAX_NAME_LEN]; /* device */
  struct logfile_rec *next;    /* next & prev keep tracks of list sorted
								* by name, start date/time and end date/time.
								*/
  struct logfile_rec *prev;

  /* the following fields are used to indicate whether refresh form is needed
   * or not */
  int  need_refresh_chart;     /* if 1, redraw bar; reset after redraw */
  char last_status_sline[MAX_BROWSER_LINE_LEN]; /* status line appears on the main
											  * form -- product_grouping_form
											  */
  char last_log_sline[MAX_BROWSER_LINE_LEN]; /* status line appears on 
											  * summary_report_form
											  */

};


/* group by tapeid. List of tapes. */
struct tape_info_rec {
  char id[MAX_TAPEID_LEN];
  stat_type_t stat;                        /* status of tape */
  struct logfile_rec *logfiles[MAX_PRODS]; /* use the same index 
											* as the one to prod_tbl.
                                            */
  struct tape_info_rec *next;
};

struct chart_rec {
  struct logfile_rec *logfile;  /* logfile to be displayed. this is a pointer
								 * to logfile in graphic_status_rec's -- 
								 * manipulate logfile there not here. */
  FL_OBJECT *chart;           /* points to chart on the form */
  FL_OBJECT *llabel_obj;       /* points to left-chart label on the form */
  FL_OBJECT *blabel_obj;       /* points to below-chart label on the form */
  FL_OBJECT *log_num_obj;      /* points to the leftside label on the form --
								* show the log's number 
								*/
};

struct tape_grouping_rec {
  char tapeid[MAX_TAPEID_LEN];
  char host[MAX_HOSTNAME_LEN];
  struct logfile_rec *last_logfile;  /* Logfile last updated. This points
									  * to the logfile in graphic_status_rec's
									  */
#if 0
  struct logfile_rec  *logfiles;     /* List of logfiles belonging to this
									  * group.  These logfiles point to the
									  * actual allocated logfiles in 
									  * logfiles list residing
									  * in graphic_status_rec -- be careful
									  * when logfile is removed from the actual
									  * list.
									  * 
									  */
#endif
  stat_type_t stat;        /* status this job grouping by tape and host*/
  time_t stime;            /* start/end time of this job grouping */
  time_t etime;
  int finished;            /* for the whole group */
  int active;              /* for the whole group */
  struct tape_grouping_rec *next;
};


/* Main list */
struct graphic_status_rec {
  struct logfile_rec *logfiles; /* contains current stutus data and only 
								 * kept status info. for user-selected 
								 * neighbors, local host, & programs */
  int nlogfiles;         /* number of logfiles */
  struct chart_rec *chart_list[MAX_CHARTS];  /* list of bar charts that are 
											  * currently being displayed
											  * on sform */
  struct logfile_rec **gms_sorted_logfiles_array; /* Used for FD_gms_form */
  int ngms_array_items;          /* Num of items in gms_sorted_logfiles_array*/
  struct logfile_rec **bar_sorted_logfiles_array; /* Used for FD_status_form */
  int nbar_array_items;          /* Num of items in bar_sorted_logfiles_array*/
  float scroll_per;      /* scroll percentage in decimal (0.0 - 1.0) */
  FD_status_form *sform; /* logfile status form--contains bars */
  int refresh_form;      /* flag indicating if form needs to be reloaded. */
  time_t threshold_time; /* Threshold for job's runtime. */
};

#define   SELECTED_PROGS_INDX       0
#define   NETWORK_TOP_INDX          1
typedef enum {
  PS_SELECTED_PROGS = 1, PS_ALL_PROGS = 2, PS_TOP_LEVEL = 4, PS_ALL_LEVELS = 8
} ps_type;

struct ps_info_rec {
  char *top_level;     /* only first level */
  char *all_levels;    /* process and its descendents */
};


typedef struct raw_data_rec{
  struct logfile_entry *data;
  char *tapeid;
  char *host;
  char *prog;
  char *tape_dev;
} raw_data_t;


/* group by host */
struct host_info_rec {
  char name[MAX_HOSTNAME_LEN];
  struct ps_info_rec *ps_info[MAX_PS_DATA_TYPES];/* index: 0 for selected progs
									   *        1 for network top
									   */
  struct host_info_rec *next;
  double diff_time;                    /* Different time in sec. between
										* the local host and this host.
										*/
};


/**************8********* Function Prototypes *********************/
void free_array_of_str(char *array[],int nitems);
void do_toggle_toolbars(FL_OBJECT *obj);
void do_toggle_button_info(FL_OBJECT *menu);
void remove_name_from_windows_menu(FL_FORM *form);
void append_msg_to_sys_stat_msg(char *msg);
int get_nlogfiles_downward(struct logfile_rec *curr_logfile);
void process_status_msg(char *fromhost, char *msg, char *newmsg);
void load_file_to_browser(FL_OBJECT * browser, 
				 char *logfile);
struct logfile_rec * get_previous_logfile(struct logfile_rec *logfile);
struct logfile_rec * get_next_logfile(struct logfile_rec *logfile);
struct logfile_rec * get_first_logfile(struct graphic_status_rec *gstat);
void get_log_info(char *tapeid, char *prod, char *buf);

struct logfile_rec *get_active_logfile(struct graphic_status_rec *gstat, char *fname);
void show_logfile_list(struct logfile_rec *loglist);
struct logfile_rec *get_logfile(struct logfile_rec *logfiles,
								char *host, char *tapeid, char *sdate,
								char *stime, char *prog);
void show_logfile_list_backward(struct logfile_rec *loglist);
int update_status(char *get_status_cmd, char *logfile, int prod_index);
void update_forms_affected_by_logfile_change(int reload_form);
void update_forms_affected_by_status_info_change(int reload_form);
void  update_forms_affected_by_neighbor_change(item_type_t type, int reload_form);
int  check_statuses();

void initialize_gui();
void get_selected_items(FL_OBJECT *browser,	char **selected_items, 
						item_type_t type, int max_items, int max_item_len	);
int host_exists(char *list[], char *host);
int add_new_host(char *list[], char *host);

void add_selected_items(char *selected_items[], item_type_t itype);
void remove_selected_items(char *selected_items[], item_type_t itype, int max_items);
/*
struct prod_info_rec * update_status_info(char *host, char *prog, char *stat_str);
void extract_info_from_filename(char *fname, char *hostname, 
								char *tapeid, char *prod);
struct prod_info_rec * get_prod_info(char *prod);
*/
void update_curr_stat_color_code(char *prod);

struct host_info_rec *get_host_info(char *host);
struct tape_info_rec *get_tape_info(char *tapeid);
int get_prod_index(char *prod);
void clear_status_info(char *host, char *prog);
void update_ps_form();
void show_ps_form();
void add_host_to_internal_lists(char *host);
void process_incoming_data();
/*
void free_loginfo_form(FD_loginfo_form *form);
*/
void free_logfile_rec( struct logfile_rec *log);
struct logfile_entry *get_log_entry(struct logfile_rec *logfile, int fnum);
long gms_block_ints();
void gms_unblock_ints(long oldmask);
long block_all_signals();
struct tape_info_rec * new_tape_info(char *tapeid);
void add_tape_info(struct tape_info_rec *tape_info);
void add_browser(FL_OBJECT *browser, stat_type_t stat, char *str,  char *text_format);

void load_items_to_browser(FL_FORM *form, FL_OBJECT *browser, 
						   item_type_t type, void *anything, int clear);
void process_log_info(char *tapeid, char *prod, char *buf);
void show_form(long form_index);
void update_status_form(float scroll_per, int reload);
void update_curr_file_status_report();
void update_tape_grouping_form();
void free_tape_group(struct tape_grouping_rec *grp);
int remove_tape_group_from_list(struct tape_grouping_rec **grp_list, char  *s);
void update_tape_group(struct logfile_rec *logfiles,
	struct tape_grouping_rec **tape_groups, struct logfile_rec *logfile, 
    char *tapeid, char *host);
struct logfile_rec *get_last_updated_logfile(struct logfile_rec *logfiles,
											char *host, char *tapeid);
void update_log_file_status_report();
void  process_log_file(char *fromhost, char *buf, char *tapeid,char *prod);
int process_del_job_msg(char *buf);
void create_logfile_name(char *host, char *tapeid, char *prod, char *filename);
void update_summary_report_form();
char *get_prod_name(int pindex);
struct logfile_rec * get_next_logfile(struct logfile_rec *logfile);
void make_special_logfile_list(struct logfile_rec *logfiles, 
   int nlogfiles, view_option_type_t list_type, char *host, char *tapeid,
   struct logfile_rec **special_logfiles_array, int *narray_items);
int update_file_status_report_form(FL_FORM *form,
	 FL_OBJECT *browser, char *selected_items[], int nitems, int type);

void free_ps_info_rec(struct ps_info_rec *rec);
struct ps_info_rec * new_ps_info_rec(int data_len);

void close_form(FL_FORM *form);
FD_get_input_form *new_get_input_form();
void extract_info_from_filename(char *fname, char *hostname,    char *tapeid, char *prod);
int add_new_prog(char *list[], char *prog);
void update_forms_affected_by_prog_change(item_type_t type, int prog_removed);
int prog_exists( char *list[], char *prog);
stat_type_t get_logfile_stat(struct logfile_rec *logfile);
int get_stat_color(stat_type_t stat);
void set_need_add_to_chart_flag(struct logfile_entry *logdata);
void set_need_refresh_chart_flag(struct graphic_status_rec *gstat, int value);
void clear_chart(struct chart_rec *chart_info);
void update_prod_group_form(int reload_form);
void get_tape_grouping_stat(char *host, char *tapeid, stat_type_t *stat,
							int *finished, int *active);
void update_gms_init_file();
int process_status_info(char *fromhost, char *buf);
void reload_data_to_status_form();
	

/* callbacks */
/**************************************************************************/
/*                                                                        */
/*                        descriptive text box routines                   */
/*                                                                        */
/**************************************************************************/
void update_exceeded_threshold_form(FL_OBJECT *button, long val);
int do_button_info(FL_OBJECT *obj, int event,FL_Coord mx, FL_Coord my,
                                int key, void *raw_event);
int do_pointermotion(FL_OBJECT *button);
void do_select_tape_ids(FL_OBJECT *button , long val);
void process_ps_request(FL_OBJECT *button , long val);


void update_monitoring_config(FL_OBJECT *button, long type);
void popup_neighbor_ps_form(FL_OBJECT *button, long val);
void popup_product_grouping_form(FL_OBJECT *button, long val);
void popup_log_info_form(FL_OBJECT *button, long val);

void popup_loginfo_form(FL_OBJECT *button, long val);
void popup_file_status_report_form (FL_OBJECT *button, long val);
void read_gms_init_file();
void do_print_failures_from_report(FL_OBJECT *button, long level);
void do_print_warnings_from_report(FL_OBJECT *button, long level);
void do_print_all_from_report(FL_OBJECT *button, long level);
void write_browser_to_file(FL_OBJECT *button, long val);
int remove_info_for_selected_items(item_type_t type, char *selected_items[], 
								   int max_items, void *data1, char *data2);
void update_curr_file_status_report();
void do_del_job_button(FL_OBJECT *ob, long form_id);
void set_view_menu_options(FL_OBJECT *menu, view_option_type_t *view_opts,
						   int form_id);
int is_an_earlier_run(struct logfile_rec *log1, struct logfile_rec *log2);
void extract_info_from_selected_grp_str(char *grp_str, char *host, 
										char *tapeid);
void blink_stat_light_buttons();
time_t get_curr_time();
void set_alarm_to_update_forms();
double get_host_diff_time(char *host);
void set_pixmapbuttons_for_summary_report_form();
void set_pixmapbuttons_for_logfile_form(FD_logfile_form *form);
void set_pixmapbuttons_for_tape_grouping_form();
void set_pixmapbuttons_for_product_grouping_form();
void free_pixmapbuttons_for_logfile_form(FD_logfile_form *form);
void free_pixmapbuttons_for_tape_grouping_form();
void free_pixmapbuttons_for_product_grouping_form();
void free_pixmapbuttons_for_summary_report_form();
void end_xform();
void  select_summary_level(FL_OBJECT *button, long val);
void update_gv_levels_form();
void update_status_fields_selection_form();
void update_active_fields();
char * remove_leading_blanks(char *s);
void send_del_job_to_monitorps(struct logfile_rec *logfile);

#endif









