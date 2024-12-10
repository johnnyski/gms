
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <math.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>

#include <forms.h>
#include "gts_monitor_form.h"
#include "gts_monitor.h"
#include "gms_comm.h"
#include "gms_time.h"


/*************************** external data ****************************/

extern level_info_t level_info_table[MAX_GV_LEVELS_NUM]; /* Store programs in the GV level order. Used
								* to create summary reports.
								*/

extern summary_type_t selected_summary_level_type;
extern double threshold_time;

extern char last_job_status_msg[MAX_BROWSER_LINE_LEN];
extern int something_is_running;

/* view options for gts_monitor_form and status_form */
extern view_options_t forms_view_options;

extern char windows_menu_items[MAX_WINDOWS_MENU_STR];
extern char *button_info_tbl[];
extern int write_to_file;
extern char *curr_selected_prod_groups[MAX_ITEMS];
extern char *log_selected_items[MAX_ITEMS];
extern char curr_selected_tape_group[MAX_BROWSER_LINE_LEN];

extern char thost[MAX_HOSTNAME_LEN];
extern char tprog[MAX_PROGNAME_LEN];
extern char tstatus[MAX_STATUS_STR_LEN];
extern pid_t tpid;
extern pid_t tppid;
extern pid_t gms_ppid;         /* parent pid */
extern char tinfile[MAX_FILENAME_LEN];
extern int cpid;
extern unsigned short gms_portnum;
extern int form_atclose(struct forms_ *form, void *value);
extern FD_status_fields_selection_form *status_fields_selection_form;
extern FD_gv_levels_form *gv_levels_form;
extern FD_msg_form *system_status_form;
extern FD_ps_info_form *ps_form;
extern FD_status_form *stat_form;
extern struct host_info_rec *host_info_list;
extern FD_gms_form *product_grouping_form;
extern FD_logfile_form *job_grouping_form;
extern FD_group_form *tape_grouping_form;
extern FD_add_del_show_form * neighbors_form;
extern FD_add_del_show_form * admin_form;
extern FD_add_del_show_form * progs_form;
extern FD_report_form *summary_report_form;
extern FD_logfile_form *logfile_report_form;

extern FD_select_item_form *select_host_for_ps_form;


extern FD_msg_form *all_data_form;  /* show data of every job received from monitorps */
extern FD_msg_form *exceeded_threshold_form; 

/*
extern FD_get_input_form *host_select_form;

extern FD_test_form *test_form;

extern int TEST;
extern int TEST_clear_msg;
extern int TEST_msg_count;
*/

extern FD_test_form *test_form;

/* stores logfile status info for displaying graphically. */
extern struct graphic_status_rec *gstatus_info;
extern struct tape_grouping_rec *tape_groups;  

extern char *Gselected_progs[MAX_PROGS];  /* user's selected programs */
extern char *available_progs[MAX_PROGS]; /* all available programs */

/* list of hosts to monitor */
extern char *Gselected_hosts[MAX_HOSTS]; /* user's selected */
extern char *available_hosts[MAX_HOSTS]; /* all available */
/* List of hosts which are available in the network.  These hosts may not
 * already be communicating with each other via monitorps.
 */
extern char *Gavailable_network_hosts[MAX_HOSTS];


extern char tapeids[MAX_TAPES][MAX_TAPEID_LEN];
extern struct tape_info_rec *tape_info_list;

extern char local_host[MAX_HOSTNAME_LEN]; 
extern char display_host[MAX_HOSTNAME_LEN]; 

extern char *prod_tbl[];
extern char *prog_tbl[MAX_PRODS][MAX_PROGS_PER_PROD];
extern int show_button_info_flag;
extern int show_toolbar_flag;
extern FL_OBJECT *gms_save_button;
extern int form_count;

/* form and its name */
extern form_info_t gms_form_list[MAX_FORMS];

extern stat_type_t all_grps_stat;

static FL_OBJECT *current_print_browser = NULL;
static char *current_print_match_str = NULL;

/******************************* function prototypes *********************/
stat_type_t get_logfile_stat(struct logfile_rec *logfile);
char *get_stat_str_from_browser_line(char *line);
extern void  remove_logfile_from_chart_list(struct graphic_status_rec *gstat,
									 struct logfile_rec *rem_logfile,
										struct logfile_rec *next_logfile);
void free_get_input_form(FD_get_input_form *form);
void free_array_of_str(char *array[],int nitems);
void clean_all_data();
void popup_test_form(FL_OBJECT *button, long val);
void seconds_to_hhmmss_str(double secs, char *time_str);



/****************************** functions ********************************/
int get_level_for_prod(char *prod, char *level_str)
{
  /* Return product's level: I, II, III and set it in level_str if not NULL.
   * 0 for not defined level.
   */
  int ll, p, level_num = 0;

  if (prod == NULL) return 0;
  
  for (ll = 0; ll < MAX_GV_LEVELS_NUM; ll++) {
	for (p = 0; p < MAX_PROGS_PER_LEVEL; p++) {
	  if (level_info_table[ll].prog_info[p].prog == NULL) continue;
	  if (strcmp(level_info_table[ll].prog_info[p].prog, prod) == 0) {
		level_num = ll+1;
	  }
	}
  }
  if (level_str) {
	if (level_num > 0)
	  sprintf(level_str, "%d", level_num);
	else 
	  sprintf(level_str, "?");
  }
  return level_num;

}
/***************************************************************************/
/*                                                                         */
/*                              set_finished_indicators                    */
/*                                                                         */
/***************************************************************************/
void set_finished_indicators(int finished, int active,
							 char *finished_char, char **finished_format_str)
{
	if (finished) {
	  *finished_char = FINISHED_CHAR;
	  *finished_format_str = FINISHED_FORMAT_STR;
	}
	else if (active == 0) {
	  *finished_char = NOT_ACTIVE_CHAR;
	  *finished_format_str = NOT_ACTIVE_FORMAT_STR;
	}
	else {
	  *finished_char = EMPTY_CHAR;
	  *finished_format_str = TEXT_FORMAT_STR;
	}
}
/***************************************************************************/
/*                                                                         */
/*                               compute_runtime                           */
/*                                                                         */
/***************************************************************************/
void compute_runtime(int active, time_t stime, time_t etime, 
							 double *runtime_sec, char *runtime_str)
{
  /* Compute runtime. Store runtime in seconds to runtime_sec or
   * in string (hhmmss) to runtime_str if they are not NULL.
   * Use current time instead of etime if active is 1.
   */
  double time_sec;
  time_t curr_time;

  if (active) {
	curr_time = get_curr_time();
	time_sec = difftime(curr_time, stime);
  }
  else
	time_sec = difftime(etime, stime);

  if (time_sec == 0) time_sec = -1; /* unknown runtime */
  if (runtime_sec != NULL)
	*runtime_sec = time_sec;
  if (runtime_str != NULL) 
	seconds_to_hhmmss_str(time_sec, runtime_str);
} /*compute_runtime*/

/***************************************************************************/
/*                                                                         */
/*                               get_curr_time                             */
/*                                                                         */
/***************************************************************************/
time_t get_curr_time()
{
  struct timeval tv;
  struct timezone tz;

  tz.tz_minuteswest = 0;
  memset(&tv, '\0', sizeof(struct timeval));
  gettimeofday(&tv, &tz);
  /*
fprintf(stderr, "curr time =====%s, %d\n", ctime(&(tv.tv_sec)), tv.tv_sec); 
*/
  return (time_t) tv.tv_sec;
}

/***************************************************************************/
/*                                                                         */
/*                                get_host_info                            */
/*                                                                         */
/***************************************************************************/
struct host_info_rec *get_host_info(char *host)
{
  struct host_info_rec *host_info;

  if (host == NULL || strlen(host) == 0) return NULL;

  host_info = host_info_list;

  while (host_info != NULL) {
	if (strcmp(host_info->name, host) == 0) 
	  return host_info;
	host_info = host_info->next;
  }
  return NULL;
}


/***************************************************************************/
/*                                                                         */
/*                               get_tape_info                             */
/*                                                                         */
/***************************************************************************/
struct tape_info_rec *get_tape_info(char *tapeid)
{
  struct tape_info_rec *tape_info;

  if (tapeid == NULL || strlen(tapeid) == 0) return NULL;

  tape_info = tape_info_list;
  while (tape_info) {

	if (strcmp(tape_info->id, tapeid) == 0 ||
		(strlen(tapeid) == MAX_TAPEID_LEN-1 && 
		  strstr(tape_info->id, tapeid) != NULL))
	  return tape_info;
	tape_info = tape_info->next;
  }
  return NULL;
}

/***************************************************************************/
/*                                                                         */
/*                           remove_name_from_windows_menu                 */
/*                                                                         */
/***************************************************************************/  
void remove_name_from_windows_menu(FL_FORM *form)
{
  char *tmp_str, *tmp_str1, save_str[MAX_WINDOWS_MENU_STR];
  char win_name[MAX_FILENAME_LEN];
  char menu_items[MAX_WINDOWS_MENU_STR+20];

  tmp_str = strchr(form->label, ':');
  if (tmp_str == NULL) {
	fl_show_message("System Error", "form's title expects 'host:name'", "");
	return;
  }
  strcpy(win_name, tmp_str+2); /* copy just win name (skip first ' ') */
  strcpy(save_str, windows_menu_items);
  tmp_str = strstr(save_str, win_name); /* pts to win name*/
  
  if (tmp_str != NULL) {
	tmp_str1 = tmp_str + strlen(win_name) + 1; /* points to next item */
	*tmp_str = '\0';
	tmp_str = save_str;
	/* construct new string without form's label */
	sprintf(windows_menu_items, "%s%s", tmp_str, tmp_str1);
	fl_clear_menu(tape_grouping_form->windows_menu);
	memset(menu_items, '\0', MAX_WINDOWS_MENU_STR+20);
	sprintf(menu_items, "Close all others|----------------------------|%s", windows_menu_items);
	
	fl_set_menu(tape_grouping_form->windows_menu, menu_items); 
	fl_set_menu_item_mode(tape_grouping_form->windows_menu, 2, FL_PUP_GRAY);
  }
}

/***************************************************************************/
/*                                                                         */
/*                              close_form                                 */
/*                                                                         */
/***************************************************************************/
void close_form(FL_FORM *form)
{
  if (form == NULL) return;

  if (form->visible) {
	fl_hide_form((FL_FORM *)form);

	/* remove form name from gms_monitor_form's windows menu */
	remove_name_from_windows_menu(form);
  }
}


/***************************************************************************/
/*                                                                         */
/*                           do_close_form                                 */
/*                                                                         */
/***************************************************************************/
void do_close_form(FL_OBJECT *button, long form_id)
{
  /* form_id = 4: for FD_status_form
   */
  FL_FORM *form;
  long old_mask;

  old_mask = gms_block_ints();
  form = (FL_FORM *) button->u_vdata;
  switch (form_id) {
  case STATUS_FORM_ID:
	/* Free space */
	if (gstatus_info && gstatus_info->bar_sorted_logfiles_array) {
	  free(gstatus_info->bar_sorted_logfiles_array);
	  gstatus_info->bar_sorted_logfiles_array = NULL;
	  gstatus_info->nbar_array_items = 0;
	}
	break;
  case LOGFILE_REPORT_FORM_ID:
	free_pixmapbuttons_for_logfile_form(logfile_report_form);
	break;
  case JOB_GROUP_FORM_ID:
	free_pixmapbuttons_for_logfile_form(job_grouping_form);
	break;
  case SUMMARY_REPORT_FORM_ID:
	free_pixmapbuttons_for_summary_report_form();
	break;
  case GMS_FORM_ID:
	free_pixmapbuttons_for_product_grouping_form();
	break;
  default:

	break;
  }
  close_form(form);

  gms_unblock_ints(old_mask);
}

/***************************************************************************/
/*                                                                         */
/*                                free_report_form                         */
/*                                                                         */
/***************************************************************************/
void  free_report_form(FD_report_form *form)
{
  if (form == NULL) return;
  if (form->vdata)
	free_get_input_form(form->vdata);

  if (form->report_form) {
	if (form->report_form->visible)
	  fl_hide_form(form->report_form);
	fl_free_form(form->report_form);
  }
  free(form);

}


/***************************************************************************/
/*                                                                         */
/*                            free_logfile_entry                           */
/*                                                                         */
/***************************************************************************/ 
  
void free_logfile_entry(struct logfile_entry *entry)
{

  if (entry != NULL) {
	entry->next = NULL;
	free(entry);
  }
  entry = NULL;
}

/***************************************************************************/
/*                                                                         */
/*                           free_logfile_entries                          */
/*                                                                         */
/***************************************************************************/
void free_logfile_entries(struct logfile_entry *entry_list)
{
  struct logfile_entry *entry;

  if (entry_list == NULL) return;
  while (entry_list) {
	entry = entry_list;
	entry_list = entry_list->next;
	entry->next = NULL;

	free_logfile_entry(entry);
	entry = NULL;
  }
  entry_list = NULL;
}

/***************************************************************************/
/*                                                                         */
/*                           free_logfile_rec                              */
/*                                                                         */
/***************************************************************************/
void free_logfile_rec( struct logfile_rec *log)
{

  if (log == NULL) return;
  free_logfile_entries(log->data);

  log->next = NULL;
  log->prev = NULL;
  free(log);
  log = NULL;
}

/***************************************************************************/
/*                                                                         */
/*                              free_tape_info                             */
/*                                                                         */
/***************************************************************************/
void free_tape_info(struct tape_info_rec *tape_info)
{
  int i;

  if (tape_info == NULL) return;
  for (i = 0; i < MAX_PRODS; i++) {
	if (tape_info->logfiles[i]) {
	  free_logfile_rec(tape_info->logfiles[i]);
	}
  }

  tape_info->next = NULL;
  free(tape_info);
}


/***************************************************************************/
/*                                                                         */
/*                           popup_system_status_msg_form                  */
/*                                                                         */
/***************************************************************************/
void popup_system_status_msg_form(FL_OBJECT *button, long val)
{
  int old_mask;

  old_mask = gms_block_ints();
  if (show_button_info_flag)
	hide_button_info_msg();
  if (button == NULL) {
	gms_unblock_ints(old_mask);
	return;
  }

  /* popup system status form */
  if (system_status_form) {
	show_form(system_status_form->ldata);
  }
  gms_unblock_ints(old_mask);
} /* popup_tape_lists_form */

/***************************************************************************/
/*                                                                         */
/*                         do_last_night_request                           */
/*                                                                         */
/***************************************************************************/
void do_last_night_request(FL_OBJECT *button, long val)
{
  char msg[MAX_MSG_LEN];
  long old_mask;

  old_mask = gms_block_ints();
  if (show_button_info_flag)
	hide_button_info_msg();
  if (button == NULL) {
	gms_unblock_ints(old_mask);
	return ;
  }
  fl_deactivate_object(button);
  /* request monitor ps for logfiles from all neighbor and local hosts */
  memset (msg, '\0', MAX_MSG_LEN);  
  sprintf(msg, "get log files");
  if (send_msg(local_host, msg) < 0) 
	exit_system(NULL, -1);

#if 0
  curr_time = get_curr_time();
  /* add message to system_status_form */
  add_msg_to_sys_stat_browser(SENT_STR, local_host, GET_LOG_FILES,  
							  strlen(msg), (long) curr_time );
#endif
  fl_activate_object(button);
  gms_unblock_ints(old_mask);
} /* do_last_night_request */
  

/***************************************************************************/
/*                                                                         */
/*                        get_log_entry_stat                               */
/*                                                                         */
/***************************************************************************/

stat_type_t get_log_entry_stat(struct logfile_entry *entry)
{
  if (entry)
	return entry->stat;
  return NOT_DEFINED;
}

/***************************************************************************/
/*                                                                         */
/*                              get_logfile_stat                           */
/*                                                                         */
/***************************************************************************/
stat_type_t get_logfile_stat(struct logfile_rec *logfile)
{
  struct logfile_entry * entry;
  stat_type_t stat = 0;

  if (logfile == NULL) return NOT_DEFINED;
  if (logfile->nentries < 1 || logfile->data == NULL) return logfile->stat;
  entry = logfile->data;
  while (entry) {
	stat |= get_log_entry_stat(entry);
	entry = entry->next;
  }
  return stat;
  
} /* get_logfile_stat */

stat_type_t get_level_stat(level_info_t *level_info_table, int level_num)
{
  int p;
  int finished = 0, active = 0; /* Represent the whole level */
  int nprogs;
  stat_type_t stat = 0;
  int failures = 0;
  int warnings = 0;
  int runnings = 0;
  int success = 0;
  int abort = 0;
  int unknowns = 0;

  if (level_info_table == NULL) return NOT_DEFINED;

  nprogs = level_info_table[level_num-1].nprogs;
  for (p = 0; p < MAX_PROGS_PER_LEVEL; p++) {
	if (level_info_table[level_num-1].prog_info[p].prog == NULL) continue;
		stat = level_info_table[level_num-1].prog_info[p].stat;
		/*
fprintf(stderr, "stat:::%d, %s\n", stat, level_info_table[level_num-1].prog_info[p].prog);
*/
		/* The order is relevant */
		if (stat & FAILED) failures++; 
		else if (stat & WARNING) warnings++; 
		else if (stat & RUNNING) runnings++; 
		else if (stat & ABORT) abort++;
		else if (stat & UNKNOWN) unknowns++;
		else if (stat & SUCCESS) success++; 

		if (stat & FINISHED) finished++;
		if (stat & ACTIVE)
		  active = 1;

  }
  stat = NOT_DEFINED;
  /* The order is relevant */
  /* Set status for level */
  /* Something in this level is ... */
  if (failures) stat = FAILED;
  else if (warnings) stat = WARNING;   
  else if (runnings) stat = RUNNING;
  else if (unknowns) stat = UNKNOWN;
  /* This level is... */
  else if (abort == nprogs) stat = ABORT;
  else if (success == nprogs) stat = SUCCESS;

  if (finished == nprogs) 
	stat |= FINISHED;
  else
	stat |= ACTIVE;
  /*
fprintf(stderr, "finished: %d SSTAT: %d\n", finished, stat);
*/
  return stat;

} /* get_level_stat */



/***************************************************************************/
/*                                                                         */
/*                              get_tape_stat                              */
/*                                                                         */
/***************************************************************************/
stat_type_t get_tape_stat(char *tapeid)
{
  int i;
  int failures = 0;
  int warnings = 0;
  int runnings = 0;
  int success = 0;
  int abort = 0;
  struct tape_info_rec *tape_info;
  stat_type_t stat;

  if (tapeid == NULL || strlen(tapeid) == 0) return NOT_DEFINED;

  tape_info = get_tape_info(tapeid);
  if (tape_info == NULL) return NOT_DEFINED;
  
  for (i = 0; i < MAX_PRODS; i++) {
	if (tape_info->logfiles[i] == NULL) continue;
	stat = tape_info->logfiles[i]->stat;
	if (stat & FAILED) failures++; 
	else if (stat & WARNING) warnings++; 
	else if (stat & RUNNING) runnings++; 
	else if (stat & ABORT) abort++;
	else if (stat & SUCCESS) success++; 

  }
  if (failures) return FAILED;
  if (warnings) return WARNING;
  if (runnings) return RUNNING;
  if (abort) return ABORT;
  if (success) return SUCCESS;

  return NOT_DEFINED;	  
} /* get_tape_stat */

/***************************************************************************/
/*                                                                         */
/*                         get_last_job_status                             */
/*                                                                         */
/***************************************************************************/ 
stat_type_t get_last_job_status(struct logfile_rec *logfile)
{
  /* returns status of the last log's entry */

  struct logfile_entry *entry, *save_entry;
  stat_type_t stat = NOT_DEFINED;

  if (logfile ==  NULL) return stat;

  entry = logfile->data;
  save_entry = entry;
  while (entry) {
	save_entry = entry;
	entry = entry->next;
  }
	
  stat = get_log_entry_stat(save_entry);
  return stat;
  
} /* get_last_job_status */

/***************************************************************************/
/*                                                                         */
/*                              stat_to_str                                */
/*                                                                         */
/***************************************************************************/
char *stat_to_str(stat_type_t stat)
{
  
  /* The order is relevant. */
  if (stat & SUCCESS) return SUCCESS_STR; 
  else if (stat & FAILED) return FAILED_STR;
  else if (stat & WARNING) return WARNING_STR;
  else if (stat & RUNNING) return RUNNING_STR;
  else if (stat & ABORT) return ABORT_STR;
  else if (stat & SYS_STATUS) return EMPTY_STR;
  else return UNKNOWN_STR;

}

/***************************************************************************/
/*                                                                         */
/*                        extract_info_from_filename                       */
/*                                                                         */
/***************************************************************************/  
void extract_info_from_filename(char *fname, char *hostname, 
								char *tapeid, char *prod)
{
  /* filename format: host.tapeid.product.log.
   * extract  hostname if host != NULL
   * extract tapeid if tapeid != NULL
   * extract prod if prod != NULL.
   */
  char *tmp;
  int rc = 0;

  if (fname == NULL || strlen(fname) == 0 ) {
	return;
  }
  tmp = fname;                  /* no pathname to skip */
#if 0
  if (strchr(fname,'/') != NULL) 
	tmp = strrchr(fname, '/') + 1; /* skip pathname */
  else
	tmp = fname;                  /* no pathname to skip */
#endif
  if (strchr(tmp, '.')) {
	if (hostname != NULL) 
	  /* get hostname */
	  strncpy(hostname, tmp, strlen(tmp) - strlen(strchr(tmp, '.')));

	tmp = strchr(tmp, '.') + 1;      /* skip host */
	if (tmp && strchr(tmp, '.')) {
	  if (tapeid != NULL)
		/* get tapeid */
		strncpy(tapeid, tmp, strlen(tmp) - strlen(strchr(tmp, '.')));
	  tmp = strchr(tmp, '.') + 1;      /* skip tapeid */
	  if (tmp && strchr(tmp, '.')) {
		if (prod != NULL)
		  /* get prodname */
		  strncpy(prod, tmp, strlen(tmp) - strlen(strchr(tmp, '.')));
		return;
	  }
	  else  rc = -1;
	}
	else  rc = -1;
  }
  else  rc = -1;


}

/***************************************************************************/
/*                                                                         */
/*                            get_stat_color                               */
/*                                                                         */
/***************************************************************************/
int get_stat_color(stat_type_t stat)
{
  int color_i;

  /* The order is relevant. */
  color_i = DEFAULT_COLOR; 
  if (stat & FAILED)  color_i = FAILURE_COLOR; 
  else if (stat & WARNING) color_i = WARNING_COLOR; 
  else if (stat & RUNNING) color_i = RUNNING_COLOR; 
  else if (stat & ABORT)  color_i = ABORT_COLOR; 
  else if (stat & NOT_DEFINED) color_i = UNKNOWN_COLOR;
  else if (stat & SUCCESS)  color_i = SUCCESS_COLOR; 
  else if (stat & FINISHED) color_i = FINISHED_COLOR;
  else if (stat & SYS_STATUS) color_i = SYS_STATUS_COLOR;

  return color_i;
}

/***************************************************************************/
/*                                                                         */
/*                           add_browser                                   */
/*                                                                         */
/***************************************************************************/
void add_browser(FL_OBJECT *browser, stat_type_t stat, char *str, 
				 char *text_format) 
{
  /* add str to browser */

  int color_i;
  char *stat_str;
  char buf[MAX_BROWSER_LINE_LEN];

  if (browser == NULL || str == NULL || text_format == NULL) return;

  memset(buf, '\0', MAX_BROWSER_LINE_LEN);

  color_i = get_stat_color(stat);
  if (stat & NO_STATUS) {
	/* textformat + color msg  status */
	sprintf(buf, "%s@C%d %s ", text_format, color_i, str);
  }
  else {

	/* status is spelled out. */
	stat_str = stat_to_str(stat);		  

	/* textformat + color msg  status */
	sprintf(buf, "%s@C%d %s %-10.10s", text_format, color_i, str, stat_str);
  }

  if (strlen(buf) >= MAX_BROWSER_LINE_LEN) 
	buf[MAX_BROWSER_LINE_LEN-1] = '\0';

  fl_add_browser_line(browser, buf);	

}

/***************************************************************************/
/*                                                                         */
/*                          add_tape_info_to_browser                       */
/*                                                                         */
/***************************************************************************/
void add_tape_info_to_browser(struct tape_info_rec *tape_info,
							  FL_OBJECT *browser,
							  item_type_t type)
{
  int i;
  char buf[MAX_STR_LEN];
  char prod[MAX_PROGNAME_LEN];
  stat_type_t stat;
  struct logfile_rec *logfile;
  char *sdate, *stime, *edate, *etime, *tape_dev;
  char finished_char;
  char *text_format;


  if (tape_info == NULL || browser == NULL) return;


  for (i = 0; i < MAX_PRODS; i++) {

	/* only add info. of the product which is in the selected programs list */
	memset(prod, '\0', MAX_PRODNAME_LEN);
	strcpy(prod, get_prod_name(i));
	if (strlen(prod)  == 0) {
	  fprintf(stderr, "Error: can't get prod name from index, %d\n", i);
	  continue;
	}
	/* not add to browser if prod is not in user's selected list */
	if (!prog_exists(Gselected_progs, prod)) continue;

	/* skip if this product with index i does not have a logfile */
	if (tape_info->logfiles[i] == NULL) continue;

	logfile = tape_info->logfiles[i];

	/* if logfile has not entry, continue */
	if (logfile->nentries == 0) continue;

	/* only add info. which has come from selected neighbors or local host. */

	if (strlen(logfile->fromhost) == 0 || 
		!host_exists(Gselected_hosts, logfile->fromhost)) continue;

	sdate = logfile->sdate;
	stime = logfile->stime;
	edate = logfile->edate;
	etime = logfile->etime;
	tape_dev = logfile->tape_dev;

	/* clear buffers */
	memset(buf, '\0', MAX_STR_LEN);

	switch (type) {
	case TAPEID:
	  stat = get_tape_stat(tape_info->id);
	  /* status is spelled out */
	  sprintf(buf, "%s %s     %s %s     %s     %s      ", 
			  sdate, stime, edate, etime, tape_info->id, tape_dev);
/*
printf("buf = <%s>\n", buf);
*/
		/* reload data to browser */
	  buf[MAX_STR_LEN-1] = '\0';
	  add_browser(browser, stat|NO_STATUS, buf, TEXT_FORMAT_STR);	  
	  break;
	case PROD:
	  stat = logfile->stat;
/*
printf("logfile = %s, stat = %d\n", tape_info->logfiles[i], stat);
*/

	  set_finished_indicators(tape_info->logfiles[i]->finished, 1,
							  &finished_char, &text_format);
	  sprintf(buf, "%c%-8.8s %-8.8s %-8.8s %-8.8s  %-19.19s   %-10.10s %-19.19s  ", 
			  finished_char, sdate, stime,
			  edate, etime, tape_info->id, tape_dev, prod);
	  buf[MAX_STR_LEN-1] = '\0';
	  /*
printf("i = %d, buf == <%s>\n", i, buf);
*/
	  add_browser(browser, stat|NO_STATUS, buf, text_format);
	  break;
	default: break;
	}

  }
} /* add_tape_info_to_browser */


/***************************************************************************/
/*                                                                         */
/*                         add_file_range_stat_to_browser                  */
/*                                                                         */
/***************************************************************************/
void add_file_range_stat_to_browser(struct logfile_rec *logfile, 
									FL_OBJECT *browser)
{  
  char buf[MAX_STR_LEN];
  stat_type_t stat;
  stat_type_t  last_stat=-1;
  struct logfile_entry *log_entry, *last_log_entry, *first_log_entry;
  int curr_file_num = 1, from_file_num = 1, to_file_num = 1;
  char *time_str, *date, *tape_dev;
  char from_str[MAX_NAME_LEN], to_str[MAX_NAME_LEN];
  char finished_char, *text_format;

  if (logfile == NULL || browser == NULL) return;
  log_entry = logfile->data;

  /* return if file has no entry */
  if (log_entry == NULL) 
	return;


  first_log_entry = log_entry;

  finished_char = ' ';

  tape_dev = logfile->tape_dev;
  last_stat = get_log_entry_stat(log_entry);
  time_str = log_entry->time_str;
  date = log_entry->date;
  text_format = TEXT_FORMAT_STR;

  while (log_entry) {

	stat = get_log_entry_stat(log_entry);
	if (curr_file_num > 1 && stat != last_stat) {
	  /* clear buffers */
	  memset(buf, '\0', MAX_STR_LEN);
	  memset(from_str, '\0', MAX_NAME_LEN);
	  memset(to_str, '\0', MAX_NAME_LEN);

	  /* end of range. add this info to browser */
	  to_file_num = curr_file_num - 1;
	  if (from_file_num == to_file_num) {
		sprintf(buf, "%c%-8.8s %-8.8s %-19.19s %-10.10s %-21.21s   %-3d   ", 
			  finished_char, date, time_str, logfile->tapeid, tape_dev, logfile->prog, from_file_num);
	  }
	  else
		sprintf(buf, "%c%-8.8s %-8.8s %-19.19s %-10.10s %-21.21s %3d-%-3d ", 
				finished_char, date, time_str, logfile->tapeid, tape_dev, logfile->prog, 
				from_file_num, to_file_num);

	  add_browser(browser, last_stat|NO_STATUS, buf, text_format);	
	  /* reset current state */
	  last_stat = stat;
	  from_file_num = curr_file_num;

	}
	time_str = log_entry->time_str;
	date = log_entry->date;

	curr_file_num++;
	last_log_entry = log_entry;
	log_entry = log_entry->next;
  }

  /* clear buffers */
  memset(buf, '\0', MAX_STR_LEN);
  to_file_num = curr_file_num -1;

  log_entry = last_log_entry;
  if (log_entry == NULL) return;
  stat = get_log_entry_stat(log_entry);
  time_str = logfile->etime;
  date = logfile->edate;
  set_finished_indicators(logfile->finished, logfile->active, &finished_char,
						&text_format);
  if (from_file_num == to_file_num) 
	sprintf(buf, "%c%-8.8s %-8.8s %-19.19s %-10.10s %-21.21s   %-3d   ", 
			finished_char, date, time_str, logfile->tapeid, tape_dev, logfile->prog, from_file_num);
  else
	sprintf(buf, "%c%-8.8s %-8.8s %-19.19s %-10.10s %-21.21s %3d-%-3d ", 
			finished_char, date, time_str, logfile->tapeid, tape_dev, logfile->prog, from_file_num,
			to_file_num);

  /* end of range. add this info to browser */
  add_browser(browser, stat|NO_STATUS, buf, text_format);

  /* add newline to separate individual logfile */
  fl_add_browser_line(browser,"\n");
} /* add_file_range_stat_to_browser */



void get_tape_grouping_stat(char *host, char *tapeid, stat_type_t *stat,
							int *finished, int *active)
{
  struct logfile_rec *logfile;
  int unset_inactive_flag = 0;

  if (gstatus_info == NULL || gstatus_info->logfiles == NULL) return;

  *stat = 0;
  *finished = 1;
  *active = 0;

  logfile = gstatus_info->logfiles;
  while (logfile) {

	if ((strcmp(logfile->tapeid, tapeid) == 0 ||
		 (strlen(tapeid) == MAX_TAPEID_LEN-1 && 
		  strstr(logfile->tapeid, tapeid) != NULL))
		&&
		strcmp(logfile->fromhost, host) == 0) {
	  if (logfile->stat & RUNNING) {
		  if (logfile->finished || !logfile->active)
			*stat |= RUNNING_BUT_INACTIVE;
		  else unset_inactive_flag = 1;
	  }


	  *stat |= logfile->stat;
	  if (!logfile->finished && logfile->active) {
		*finished = 0;
		*active = 1;
	  }

	}
	logfile = logfile->next;
  }

 if (unset_inactive_flag && (*stat & RUNNING_BUT_INACTIVE))
   *stat ^= RUNNING_BUT_INACTIVE;  /* Unset */
} /* get_tape_grouping_stat */

/***************************************************************************/
/*                                                                         */
/*                        qsort_etime_compare_logfiles                     */
/*                                                                         */
/***************************************************************************/

int qsort_etime_compare_logfiles(struct logfile_rec **log1, struct logfile_rec **log2)
{
  /* Compare end time. Return -1, 0, 1 for <, =, > 
   * comparison. */


  if (*log1 == NULL) return 1;
  if (*log2 == NULL) return -1;

  if ((*log1)->etime_item < (*log2)->etime_item)
	return -1;
  else return 1;

} /* qsort_etime_compare_logfiles */

/***************************************************************************/
/*                                                                         */
/*                       qsort_host_tape_prog_compare_logfiles             */
/*                                                                         */
/***************************************************************************/
int qsort_host_tape_prog_compare_logfiles(struct logfile_rec **log1,  
					   struct logfile_rec **log2)
{
  /* Compare fromhost, tapeid, and prog. Return -1, 0, 1 for <, =, > 
   * comparison. */

  int rc, rc1;

  if (*log1 == NULL) return 1;
  if (*log2 == NULL) return -1;

  if ((rc = strcmp((*log1)->fromhost, (*log2)->fromhost)) < 0)
	return -1;
  else if (rc == 0 && (rc1 = strcmp((*log1)->tapeid, (*log2)->tapeid)) < 0)
	return -1;
  else if (rc1 == 0 && strcmp((*log1)->prog, (*log2)->prog) < 0)
	return -1;
  else return 1;

} /* qsort_host_tape_prog_compare_logfiles */

/***************************************************************************/
/*                                                                         */
/*                       qsort_level_compare_logfiles             */
/*                                                                         */
/***************************************************************************/
int qsort_level_compare_logfiles(struct logfile_rec **log1,  
					   struct logfile_rec **log2)
{
  /* Compare tape and levels. Return -1, 0, 1 for <, =, > 
   * comparison. */

  int rc;
  int log1_level, log2_level;

  if (*log1 == NULL) return 1;
  if (*log2 == NULL) return -1;

  log1_level = get_level_for_prod((*log1)->prog, NULL);
  log2_level = get_level_for_prod((*log2)->prog, NULL);
  if ((rc = strcmp((*log1)->tapeid, (*log2)->tapeid)) < 0)
	return -1;
  else if ((rc == 0) && (log1_level < log2_level))
	return -1;
  else return 1;

} /* qsort_level_compare_logfiles */


/***************************************************************************/
/*                                                                         */
/*                       qsort_tape_compare_logfiles             */
/*                                                                         */
/***************************************************************************/
int qsort_tape_compare_logfiles(struct logfile_rec **log1,  
					   struct logfile_rec **log2)
{
  /* Compare tapeid. Return -1, 0, 1 for <, =, > 
   * comparison. */

  if (*log1 == NULL) return 1;
  if (*log2 == NULL) return -1;

  if (strcmp((*log1)->tapeid, (*log2)->tapeid) < 0)
	return -1;
  else return 1;

} /* qsort_tape_compare_logfiles */



/***************************************************************************/
/*                                                                         */
/*                            qsort_host_tape_prog_compare_groups          */
/*                                                                         */
/***************************************************************************/  
int qsort_host_tape_prog_compare_groups(struct tape_grouping_rec **grp1,
							  struct tape_grouping_rec **grp2)
{
  int rc;

  if (grp1 == NULL) return 1;
  if (grp2 == NULL) return -1;
  
  if ((rc = strcmp((*grp1)->host, (*grp2)->host)) < 0)
	return -1;
  else if (rc == 0 && strcmp((*grp1)->tapeid, (*grp2)->tapeid) < 0)
	return -1;
  else	return 1;
	
} /* qsort_host_tape_prog_compare_groups */

/***************************************************************************/
/*                                                                         */
/*                            qsort_tape_compare_groups          */
/*                                                                         */
/***************************************************************************/  
int qsort_tape_compare_groups(struct tape_grouping_rec **grp1,
							  struct tape_grouping_rec **grp2)
{
  if (grp1 == NULL) return 1;
  if (grp2 == NULL) return -1;
  
  if (strcmp((*grp1)->tapeid, (*grp2)->tapeid) < 0)
	return -1;
  else	return 1;
	
} /* qsort_tape_compare_groups */

/***************************************************************************/
/*                                                                         */
/*                          qsort_etime_compare_groups                     */
/*                                                                         */
/***************************************************************************/ 
int qsort_etime_compare_groups(struct tape_grouping_rec **grp1,
							  struct tape_grouping_rec **grp2)
{
  if (grp1 == NULL) return 1;
  if (grp2 == NULL) return -1;

  if ((*grp1)->etime < (*grp2)->etime)
	return -1;
  else return 1;
} /* qsort_etime_compare_groups */

/***************************************************************************/
/*                                                                         */
/*                        qsort_etime_compare_raw_data                     */
/*                                                                         */
/***************************************************************************/ 
int qsort_etime_compare_raw_data(raw_data_t *data1, raw_data_t *data2)
{
  if (data1 == NULL || data1->data == NULL) return 1;
  if (data2 == NULL || data2->data == NULL) return -1;

  if (data1->data->time_item < data2->data->time_item) return -1;
  return 1;
} /* qsort_etime_compare_raw_data */

/***************************************************************************/
/*                                                                         */
/*                         make_special_grp_list                           */
/*                                                                         */
/***************************************************************************/ 
void make_special_grp_list(struct tape_grouping_rec *grps, int ngroups, 
						   view_option_type_t list_type,
						   struct tape_grouping_rec **special_groups_array,
						   int *narray_items)
{
  /* Create a special list depending on list_type.
   * The special list contain groups sorted by either end time or by 
   * host and tapeid.
   * The groups in special_groups will be pointed to the groups in grps.
   */
  struct tape_grouping_rec *grp;
  int i = 0, nitems;

  if (grps == NULL || special_groups_array == NULL) return;

  /* fill groups array */
  grp = grps;
  while (grp && i < ngroups) {
	if (((list_type & VIEW_SUCCESSFULS) && ((grp->stat & SUCCESS) &&
											 !(grp->stat & WARNING) &&
											 !(grp->stat & FAILED) &&
											 !(grp->stat & NOT_DEFINED) &&
											 !(grp->stat & ABORT) &&
											 !(grp->stat & RUNNING))) ||
		((list_type & VIEW_FAILURES) && (grp->stat & FAILED)) ||
		((list_type & VIEW_WARNINGS) && (grp->stat & WARNING)) ||
		((list_type & VIEW_UNKNOWNS) && (grp->stat & NOT_DEFINED)) ||
		((list_type & VIEW_RUNNINGS) && (grp->stat & RUNNING)) ||
		(list_type & VIEW_ALL)) {

	  if (((list_type & VIEW_ACTIVE_ONLY) && grp->active) ||
		  ((list_type & VIEW_INACTIVE_ONLY) && !grp->active)) {
		special_groups_array[i] = grp;
		i++;
	  }
	}

	grp = grp->next;
  }

  nitems = i;
  if (narray_items != NULL)
	*narray_items = nitems;
  while (i < ngroups) {
	special_groups_array[i] = NULL;
	i++;
  }

  if (list_type & SORT_BY_HOST_TAPE_PROG) {
	qsort((void *) special_groups_array,  (size_t) nitems,
		  (size_t) sizeof(struct tape_grouping_rec*), (int (*)(const void *,
		  const void *))qsort_host_tape_prog_compare_groups);
  }
  else if (list_type & SORT_BY_ETIME) {

	qsort((void *) special_groups_array,  (size_t) nitems,
		  (size_t) sizeof(struct tape_grouping_rec*), (int (*)(const void *,
		  const void *))qsort_etime_compare_groups);
  }
  else if (list_type & SORT_BY_TAPE) {

	qsort((void *) special_groups_array,  (size_t) nitems,
		  (size_t) sizeof(struct tape_grouping_rec*), (int (*)(const void *,
		  const void *))qsort_tape_compare_groups);
  }
} /* make_special_grp_list */


/***************************************************************************/
/*                                                                         */
/*                        make_special_logfile_list                        */
/*                                                                         */
/***************************************************************************/
void make_special_logfile_list(struct logfile_rec *logfiles, 
							   int nlogfiles, view_option_type_t list_type,
							   char *host, char *tapeid,
							   struct logfile_rec **special_logfiles_array,
							   int *narray_items)
{
  /* Create a special list depending on list_type.
   * The special list containing logfiles is sorted by either end time or by 
   * host, tapeid, and program.  
   * The logfiles in special_logfiles will be pointed to the logfiles in
   * logfiles. 
   */
  struct logfile_rec *logfile;
  int i = 0;
  int select_this_one = 0;

  if (logfiles == NULL || host == NULL || tapeid == NULL ||
	  special_logfiles_array == NULL || narray_items == NULL) {
	return;
  }

  /* Fill logfiles array.  Ignore empty logfile. */

  logfile = logfiles;
  while (logfile && i < nlogfiles) {
	if (logfile->nentries < 1) goto NEXT;

	select_this_one = 0;
	if (list_type & VIEW_ALL) {
	  /* View all */
	  select_this_one = 1;
	}
	else if (( (list_type & VIEW_SUCCESSFULS) && 
				   ((logfile->stat & SUCCESS) &&
					!(logfile->stat & WARNING) &&
					!(logfile->stat & FAILED) &&
					!(logfile->stat & ABORT) &&
					!(logfile->stat & NOT_DEFINED) &&
					!(logfile->stat & RUNNING))) ||
			 ( (list_type & VIEW_FAILURES) && (logfile->stat & FAILED)) ||
			 ( (list_type & VIEW_WARNINGS) && (logfile->stat & WARNING)) ||
			 ( (list_type & VIEW_UNKNOWNS) && (logfile->stat & NOT_DEFINED)) ||
			 ( (list_type & VIEW_RUNNINGS) && (logfile->stat & RUNNING)) ) {

	  if (!(list_type & VIEW_ONE_TAPE_GRPING)) {
		/* View selected statuses only */
		select_this_one = 1;
	  }
	  else if ((list_type & VIEW_ONE_TAPE_GRPING) &&
			   ((strlen(host) == 0  || strcmp(host, logfile->fromhost) == 0) &&
				(strlen(tapeid) == 0 || (strcmp(tapeid, logfile->tapeid) == 0 ||
										 (strlen(tapeid) == MAX_TAPEID_LEN-1 && 
										 strstr(logfile->tapeid, tapeid) != NULL))
				 ))){
		/* View one_tape with specified statuses  */
		select_this_one = 1;
	  }

	}
	else if (((list_type & VIEW_ONE_TAPE_GRPING) &&
			  ((strlen(host) == 0  || strcmp(host, logfile->fromhost) == 0) &&
			   (strlen(tapeid) == 0 || (strcmp(tapeid, logfile->tapeid) == 0 ||
									   (strlen(tapeid) == MAX_TAPEID_LEN-1 && 
										strstr(logfile->tapeid, tapeid) != NULL))
)))&&
			 (!(list_type & VIEW_SUCCESSFULS) &&
			  !(list_type & VIEW_FAILURES) && 
			  !(list_type & VIEW_WARNINGS) &&
			  !(list_type & VIEW_UNKNOWNS) && 
			  !(list_type & VIEW_RUNNINGS))) {

	  /* View only one_tape */
	  select_this_one = 1;  
	}

	if (select_this_one) {
	  if (((list_type & VIEW_ACTIVE_ONLY) && logfile->active) ||
		  ((list_type & VIEW_INACTIVE_ONLY) && !logfile->active)) {

		special_logfiles_array[i] = logfile;
		i++;
	  }


	  /*
fprintf(stderr, "list type = %d, logfile: %s stat = %d i = %d\n", list_type, logfile->name, logfile->stat, i);
*/
	  /*
fprintf(stderr, "added: %s %s %s %d\n", logfile->fromhost, logfile->tapeid, logfile->prog, i);
*/
	}
  NEXT:
	logfile = logfile->next;
  } 
  *narray_items = i;

  /* Fill the rest with NULL */
  while (i < nlogfiles) {
	special_logfiles_array[i] = NULL;
	i++;
  }
  if (list_type & SORT_BY_HOST_TAPE_PROG) {
	qsort((void *)special_logfiles_array, (size_t) *narray_items, 
		  (size_t) sizeof(struct logfile_rec *), (int (*)(const void *,
		  const void *))qsort_host_tape_prog_compare_logfiles);
  }

  else if (list_type & SORT_BY_ETIME) {
	qsort((void *)special_logfiles_array, (size_t) *narray_items, 
		  (size_t)sizeof(struct logfile_rec *), (int (*)(const void *, 
          const void *)) qsort_etime_compare_logfiles);
  }

  else if (list_type & SORT_BY_LEVEL) {
	qsort((void *)special_logfiles_array, (size_t) *narray_items, 
		  (size_t) sizeof(struct logfile_rec *), (int (*)(const void *,
		  const void *))qsort_level_compare_logfiles);
  }
  else if (list_type & SORT_BY_TAPE) {
	qsort((void *)special_logfiles_array, (size_t) *narray_items, 
		  (size_t) sizeof(struct logfile_rec *), (int (*)(const void *,
		  const void *))qsort_tape_compare_logfiles);
  }
} /* make_special_logfile_list */

/***************************************************************************/
/*                                                                         */
/*                           extract_info_from_selected_grp_str            */
/*                                                                         */
/***************************************************************************/
void extract_info_from_selected_grp_str(char *grp_str, char *host, 
										char *tapeid)
{   
  /* grp_str's format:
   * form_code [*]host tapeid grp_runtime R|H product prod_runtime R|H prod_status last_job_runtime R|H
   */

  char fromhost[MAX_HOSTNAME_LEN+1];
  char *tmphost;

  if (grp_str == NULL || host == NULL || tapeid == NULL) return;

  if (sscanf(grp_str, "%*s %s %s %*s %*c %*s %*s %*c %*s %*s %*c", 
			 fromhost, tapeid) != 2) return;
  /* remove finished char, '*' or '-' if any from host str. */
  tmphost = fromhost;
  if (*tmphost == FINISHED_CHAR)
	tmphost++;
  strcpy(host, tmphost);
}

/***************************************************************************/
/*                                                                         */
/*                               get_group_count                           */
/*                                                                         */
/***************************************************************************/
int get_group_count(struct tape_grouping_rec *grps)
{
  int count = 0;
  struct tape_grouping_rec *grp;

  if (grps == NULL) return 0;
  
  grp = grps;
  while (grp) {
	count++;
	grp = grp->next;
  }
  return count;
} /* get_group_count */

/***************************************************************************/
/*                                                                         */
/*                        seconds_to_hhmmss                                */
/*                                                                         */
/***************************************************************************/      

void seconds_to_hhmmss(double total_seconds,
					   int *day, int *hr, int *min, int *sec)
{
  if (day == NULL || sec == NULL || min == NULL || hr == NULL) return;

  *sec = (int) total_seconds % 60;
  *hr = total_seconds / 3600;
  *min = (int) (total_seconds / 60) - (*hr * 60);
  *day = *hr / 24;
  *hr %= 24;
  /*
fprintf(stderr, "total: %f, h: %d, m: %d, s:%d\n", total_seconds, *hr, *min, *sec);
*/
}

/***************************************************************************/
/*                                                                         */
/*                        get_last_logfile_entry                           */
/*                                                                         */
/***************************************************************************/ 
struct logfile_entry *get_last_logfile_entry(struct logfile_rec *logfile)
{
  struct logfile_entry *entry = NULL, *save_entry;

  if (logfile == NULL) return entry;

  entry = logfile->data;
  save_entry = entry;

  while (entry) {
	save_entry = entry;
	entry = entry->next;
  }
  return save_entry;
} /* get_last_logfile_entry */

/***************************************************************************/
/*                                                                         */
/*                           load_logfile_entries_to_browser               */
/*                                                                         */
/***************************************************************************/ 
void load_logfile_entries_to_browser(FL_OBJECT *browser, 
									 struct logfile_entry *entries,
									 char *tapeid, char *tape_device,
									 char *prog)
{
  struct logfile_entry *entry;
  char buf[MAX_STR_LEN];

  if (browser == NULL) return;

  entry = entries;
  while (entry) {
	memset(buf, '\0', MAX_STR_LEN);
	sprintf(buf, "%-8.8s %-8.8s %-19.19s %-10.10s %-19.19s   %-3d   %-70s", 
			entry->date, entry->time_str, tapeid, tape_device, prog,
			entry->fnum, entry->infile);
	add_browser(browser, (entry->stat)|NO_STATUS, buf, TEXT_FORMAT_STR);
	entry = entry->next;
  }
} /* load_logfile_entries_to_browser */

/***************************************************************************/
/*                                                                         */
/*                         load_logfile_entries_to_array                   */
/*                                                                         */
/***************************************************************************/
void load_logfile_entries_to_array(raw_data_t *all_data_array, 
								   int start_index, int max_items,
								   struct logfile_entry *entries,
								   char *host, char *tapeid, char *tape_dev,
								   char *prog)
{
  int i;
  struct logfile_entry *entry;

  if (all_data_array == NULL) return;
  i = start_index;
  entry = entries;
  while (i < max_items && entry) {
	all_data_array[i].data = entry;
	all_data_array[i].host = host;
	all_data_array[i].tapeid = tapeid;
	all_data_array[i].prog = prog;
	all_data_array[i].tape_dev = tape_dev;
	entry = entry->next;
	i++;
  }
} /* load_logfile_entries_to_array */

/***************************************************************************/
/*                                                                         */
/*                        load_raw_data_array_to_browser                   */
/*                                                                         */
/***************************************************************************/ 
void load_raw_data_array_to_browser(raw_data_t *all_data_array, int max_items,
									FL_OBJECT *browser)
{
  int i = 0;
  char buf[MAX_STR_LEN];
  struct logfile_entry *entry;

  if (browser == NULL || all_data_array == NULL) return;

  while (i < max_items) {
	entry = all_data_array[i].data;
	memset(buf, '\0', MAX_STR_LEN);
	
	sprintf(buf, "%-8.8s %-8.8s %-19.19s %-10.10s %-19.19s   %-3d   %-20s", 
			entry->date, entry->time_str, all_data_array[i].tapeid, 
			all_data_array[i].tape_dev, all_data_array[i].prog,
			entry->fnum, entry->infile);
	add_browser(browser, entry->stat|NO_STATUS, buf, TEXT_FORMAT_STR);
	i++;
  }
} /* load_raw_data_array_to_browser */

/***************************************************************************/
/*                                                                         */
/*                        compute_total_jobs                               */
/*                                                                         */
/***************************************************************************/
int compute_total_jobs(struct logfile_rec *logfiles)
{
  int num = 0;
  struct logfile_rec *logfile;

  logfile = logfiles;
  while (logfile) {
	num += logfile->nentries;
	logfile = logfile->next;
  }
  return num;
} /* compute_total_jobs */


/***************************************************************************/
/*                                                                         */
/*                        update_light_buttons                             */
/*                                                                         */
/***************************************************************************/
void update_light_buttons(stat_type_t stat) 
{
  /* Update light buttons on tape_grouping_form */

  if (!(tape_grouping_form == NULL || tape_grouping_form->group_form == NULL ||
		!tape_grouping_form->group_form->visible ||
		tape_grouping_form->failure_light_button == NULL ||
		tape_grouping_form->warning_light_button == NULL ||
		tape_grouping_form->running_light_button == NULL ||
		tape_grouping_form->unknown_light_button == NULL  )){
	if (stat & FAILED) 
	  fl_set_button(tape_grouping_form->failure_light_button, PUSHED);
	else 
	  fl_set_button(tape_grouping_form->failure_light_button, RELEASED);
	if (stat & WARNING)
	  fl_set_button(tape_grouping_form->warning_light_button, PUSHED);
	else 
	  fl_set_button(tape_grouping_form->warning_light_button, RELEASED);
	if (stat & RUNNING)
	  fl_set_button(tape_grouping_form->running_light_button, PUSHED);
	else 
	  fl_set_button(tape_grouping_form->running_light_button, RELEASED);
	if (stat & UNKNOWN)
	  fl_set_button(tape_grouping_form->unknown_light_button, PUSHED);
	else 
	  fl_set_button(tape_grouping_form->unknown_light_button, RELEASED);
  }
} /* update_light_buttons */


/***************************************************************************/
/*                                                                         */
/*                               get_prev_log_entry                        */
/*                                                                         */
/***************************************************************************/
struct logfile_entry *get_prev_log_entry(struct logfile_rec *logfile, int fnum)
{
  /* Get the closest previous log entry that has non-unknown status.
   */
  struct logfile_entry *entry;
  int i=1;

  entry = get_log_entry(logfile, fnum-i);
  while (entry) {
	if (entry->stat == UNKNOWN) {
	  i++;
	  entry = get_log_entry(logfile, fnum-i);
	}
	break;
  }


  return entry;
} /* get_prev_log_entry */


/***************************************************************************/
/*                                                                         */
/*                               get_job_runtime                           */
/*                                                                         */
/***************************************************************************/
double get_job_runtime(struct logfile_rec *logfile, struct logfile_entry *job)
{
  double total_job_time = -1.0;
  struct logfile_entry *prev_job;
  time_t curr_time;


  if (job == NULL) return -1.0;
  

  if ((get_log_entry_stat(job) & RUNNING) && (logfile->active)) {
	  
	curr_time = get_curr_time();
	total_job_time = difftime(curr_time, job->time_item);
	return total_job_time;
  }
  
  /* Job is not running */
  if (job->runtime > 0.0)
	total_job_time = job->runtime;
  else {
	/* This occurs when no status msg (RUNNING) has been received for 
	 * job.
	 * Get prev job--use difftime of last_job and prev_job (with known status)
	 * as total_job_time.  This assumes no missing gap exists between jobs 
	 */
	prev_job = get_prev_log_entry(logfile, job->fnum);
	if (prev_job == NULL)
	  ;
	
	else { 
	  /*
{
extern FILE *test_file;
fprintf(test_file, "currfnum: %d, prev_job:%s %s, %d  time %ld\n", 
job->fnum, logfile->tapeid,
logfile->prog, prev_job->fnum, prev_job->time_item);
}
*/
      if (prev_job->time_item == 0) 
		/* prev_job's stat is probably unknown */
		;
	  
	  else 
		total_job_time = difftime(job->time_item, prev_job->time_item);

	}

  }
  return total_job_time;
  
} /* get_job_runtime */
 
/***************************************************************************/
/*                                                                         */
/*                        load_tape_groupings_to_browser                   */
/*                                                                         */
/***************************************************************************/
void load_tape_groupings_to_browser(FL_OBJECT *browser)
{

  /* Load tape groupings' info to browser depending on form's view options. 
   * Sort tape grouping by either end time or host and tapeid 
   * Create array of logfile pointers.  This is used in order to call
   * qsort() in make_special_logfile_list().
   */


  char buf[MAX_STR_LEN];
  int color_i;
  char finished_char;
  struct tape_grouping_rec **special_groups_array;
  struct tape_grouping_rec *grp;
  char prod_stat_char, group_stat_char,job_stat_char;
  char group_time_str[MAX_TIME_STR_LEN], product_time_str[MAX_TIME_STR_LEN],
	job_time_str[MAX_TIME_STR_LEN];
  double total_product_time, total_group_time, total_job_time;
  int ngroups, i;
  int nrunning_groups;
  struct logfile_rec *logfile;
  char *finished_format_str;
  time_t curr_time;
  stat_type_t prod_stat, job_stat, group_stat;
  struct logfile_entry *last_job;
  int narray_items=0;  

  if (browser == NULL) return;

  grp = tape_groups;
  ngroups = get_group_count(grp);

  special_groups_array = (struct tape_grouping_rec **)
	calloc(ngroups, sizeof(struct tape_grouping_rec *));
  if (special_groups_array == NULL) {
	perror("calloc special_groups_array");
	exit_system(NULL, -1);
  }
  /* Create group list sorted either by end time or by host and tapeid, 
   */
  make_special_grp_list(grp, ngroups, 
						forms_view_options[GROUPF_VIEW_OPTION_IND],
						special_groups_array, &narray_items);
  i = 0;
  nrunning_groups = 0;
  all_grps_stat = 0;
  while (i < narray_items) {
	grp = special_groups_array[i];
	i++;
	if (grp == NULL) continue;
	logfile = grp->last_logfile;
	  
	if (logfile == NULL) continue;
	/* clear buffers  */
	memset(buf, '\0', MAX_STR_LEN);
	/*
	 * get_tape_grouping_stat(grp->host, grp->tapeid, &group_stat, &finished,
	 * &active);
	 */
	group_stat = grp->stat;
	all_grps_stat |= group_stat;
	
	
	color_i = get_stat_color(group_stat);
	
	/* put '*' in front if finished processing for this tape */
	set_finished_indicators(grp->finished, 1, &finished_char, &finished_format_str);
	prod_stat = logfile->stat;

	curr_time = get_curr_time();

	job_stat_char = HALT_CHAR;
	if (logfile->active) 
	  prod_stat_char = RUNNING_CHAR;
	else
	  prod_stat_char = HALT_CHAR;

	/* Compute product's runtime */
	compute_runtime(logfile->active, logfile->stime_item, logfile->etime_item,
					&total_product_time, product_time_str);

	{
extern FILE *test_file;
if (write_to_file) {
fprintf(test_file, "prod:%s, %s:stime: %ld, curtime: %ld, etime: %ld %s\n", 
		logfile->tapeid, logfile->prog, logfile->stime_item, curr_time, logfile->etime_item, product_time_str);

fflush(test_file);
}
	  }


	/* Compute group's runtime */
	compute_runtime(grp->active, grp->stime, grp->etime, &total_group_time,
					group_time_str);
	if (grp->active) {
	  group_stat_char = RUNNING_CHAR;
	  nrunning_groups++;
	}
	else
	  group_stat_char = HALT_CHAR;

	  {
extern FILE *test_file;
char curr_time_str[30];
if (write_to_file) {
fprintf(test_file, "grp:%s: stime: %ld, curtime: %ld, etime: %ld %s\n", 
		grp->tapeid, grp->stime, curr_time, grp->etime, group_time_str);
strcpy(curr_time_str, ctime(&curr_time));
fprintf(test_file, "currtimec: %s, date:time: %s:%s stimec: %s, runtime: %s\n", curr_time_str, logfile->sdate, logfile->stime,
ctime(&(grp->stime)), group_time_str);
fflush(test_file);
}
	  }



	/* compute last modified job's runtime */
	total_job_time = 0;
	last_job = logfile->last_job;
	if (last_job == NULL) {
	  total_job_time = -1;
	  goto SKIP_LASTJOB;
	}
{
extern FILE *test_file;
fprintf(test_file, "log: %s, lastjob not NULLLLLLLLLLLLLLLLLL\n", logfile->name);	
	}
	if (((job_stat = get_log_entry_stat(last_job)) & RUNNING) && 
		logfile->active) {
	  job_stat_char = RUNNING_CHAR;
	}
	/*
	{
extern FILE *test_file;
fprintf(test_file, "last_job:%s,%s: fnum - %d, runtime: %d\n", logfile->tapeid,
logfile->prog, last_job->fnum, (int)last_job->runtime);
}
*/
	total_job_time = get_job_runtime(logfile, last_job);
	/* Set its runtime--so won't need to do this again. */
	logfile->last_job->runtime = total_job_time;
  SKIP_LASTJOB:

	/*
fprintf(stderr, "currtime = %ld, etime = %ld, etimes: %s, stimes: %s, difftime = %d,name - %s, \n", curr_time, logfile->etime_item, logfile->etime, logfile->stime, (int) total_job_time, logfile->name);   
*/

	seconds_to_hhmmss_str(total_job_time, job_time_str);
	

/*
  {
extern FILE *test_file;
if (prev_job)
  fprintf(test_file, "%s, %s:stat: %d, lastjob %d:time: %ld, curtime: %ld, tlastjob %d: time: %ld runtime: %d, %2d:%2d:%2d\n", logfile->tapeid, logfile->prog,job_stat, last_job->fnum, last_job->time_item, curr_time,prev_job->fnum, prev_job->time_item, (int)total_job_time,job_hr, job_min, job_sec);
else
  fprintf(test_file, "%s, %s:stat: %d:lastjob %d:time: %ld, curtime: %ld,  runtime: %d, %2d:%2d:%2d\n", logfile->tapeid, logfile->prog, job_stat, last_job->fnum, last_job->time_item, curr_time, (int)total_job_time, job_hr, job_min, job_sec);
fflush(test_file);
prev_job = NULL;
}
	  */

	/* buf's format:
	 * form_code [*]host tapeid grp_runtime R|H product prod_runtime R|H prod_status last_job_runtime R|H
	 * There should be only one space in between form_code and host.
	 *
	 * Modify extract_info_from_selected_grp_str() when the format gets 
	 * modified.
	 */
	/*
	sprintf(buf, "%s@C%d %c%-9.9s %-19.19s %s %c   %-19.19s %s %c  %-10.10s    %s %c\n", 
	*/
	/*
fprintf(stderr, "prodstat: %d, %s, grpstat: %d, tapeid: %s\n", prod_stat, 
		stat_to_str(prod_stat), group_stat, grp->tapeid);
*/
	sprintf(buf, "%s@C%d %c%-9.9s %-16.16s %12.12s %c  %-16.16s %12.12s %c  %-10.10s %12.12s %c", 
			finished_format_str, color_i, finished_char,
			logfile->fromhost, logfile->tapeid, group_time_str,
			group_stat_char, logfile->prog, product_time_str, 
			prod_stat_char, stat_to_str(prod_stat), 
			job_time_str, job_stat_char);
	
	buf[MAX_STR_LEN-1] = '\0';
	/*
fprintf(stderr, "buf: %s\n", buf);
*/

	fl_add_browser_line(browser, buf);

  }

  /* Update light buttons' state */
  update_light_buttons(all_grps_stat);
  
  free(special_groups_array);
  
  if (nrunning_groups > 0) {
	something_is_running = 1;
	set_alarm_to_update_forms();
  }
  else 
	something_is_running = 0;

} /* load_tape_groupings_to_browser */


/****************************************************************************/
/*                                                                          */
/*                    load_product_groupings_to_browser                     */
/*                                                                          */
/****************************************************************************/
void load_product_groupings_to_browser(FL_OBJECT *browser, 
									   char *tape_grouping_str)
{
  /* Load product groupings to browser.
   */

  double total_product_time;
  int i;
  char buf[MAX_STR_LEN];
  char finished_char;
  stat_type_t stat;
  int color_i;
  char prod_stat_char;
  char tapeid[MAX_TAPEID_LEN];
  char fromhost[MAX_HOSTNAME_LEN];
  char product_time_str[MAX_TIME_STR_LEN];
  struct logfile_rec *logfile;
  char *finished_format_str;


  if (browser == NULL) return;
  if (gstatus_info == NULL) return;

  memset(fromhost, '\0', MAX_HOSTNAME_LEN);
  memset(tapeid, '\0', MAX_TAPEID_LEN);

  if (forms_view_options[GMSF_VIEW_OPTION_IND] & VIEW_ONE_TAPE_GRPING) {
	
	if (tape_grouping_str != NULL) {
	  /* extract fromhost and  tapeid
	   * from: code host tapeid runtime prog jobtime jobstatus */
	  extract_info_from_selected_grp_str(tape_grouping_str, fromhost, tapeid);
	}
  }

  /* Create array of logfile pointers.  This is used in order to call
   * qsort() in make_special_logfile_list().
   */
  /* Remove previous list */
  if (gstatus_info->gms_sorted_logfiles_array) {
	free(gstatus_info->gms_sorted_logfiles_array);
	gstatus_info->gms_sorted_logfiles_array = NULL;
	gstatus_info->ngms_array_items = 0;
  }
  gstatus_info->gms_sorted_logfiles_array = (struct logfile_rec **)
	calloc(gstatus_info->nlogfiles, sizeof(struct logfile_rec *));
  if (gstatus_info->gms_sorted_logfiles_array == NULL) {
	perror("calloc gstatus_info->gms_sorted_logfiles_array\n");
	exit_system(NULL, -1);
  }

  /* Create logfile list sorted either by end time or by host, tapeid, 
   * and prog.  List contains only logfile matches host and/or tapeid or
   * all if host and tapeid are "".  Leave this list around--will use it when
   * deleting job.
   */
  make_special_logfile_list(gstatus_info->logfiles, gstatus_info->nlogfiles,
							forms_view_options[GMSF_VIEW_OPTION_IND],
							fromhost, tapeid,
							gstatus_info->gms_sorted_logfiles_array, 
							&(gstatus_info->ngms_array_items));

  i = 0;
  while (i < gstatus_info->ngms_array_items) {
	logfile = gstatus_info->gms_sorted_logfiles_array[i];
	
	/* end of list */
	if (logfile == NULL) break;
	i++;
	
	/* clear buffers  */
	memset(buf, '\0', MAX_STR_LEN);
	
	stat = logfile->stat;
	color_i = get_stat_color(stat);
	
	/* put '*' in front if finished processing for this product and tape */
	set_finished_indicators(logfile->finished, logfile->active, &finished_char,
						&finished_format_str);

	/* form_code [*]host tapeid product device sdate stime runtime */
	/* Note: if form_code is "", modify code where browser line is parsed 
	 * in do_del_job_button() from product_grouping_form_cd.c.
	 * There should be only one space in between form_code and host.
	 */
	
	if (logfile->active) {
	  prod_stat_char = RUNNING_CHAR;
	}
	else
	  prod_stat_char = HALT_CHAR;

	/* Compute product's runtime */	
	compute_runtime(logfile->active, logfile->stime_item, logfile->etime_item,
					&total_product_time, product_time_str);
	sprintf(buf, "%s@C%d %c%-9.9s %-19.19s %-21.21s %-10.10s %-8.8s %-8.8s %11.11s %c  %3d", 
			finished_format_str, color_i, finished_char, logfile->fromhost,
			logfile->tapeid, logfile->prog, logfile->tape_dev,
			logfile->sdate, logfile->stime,  
			product_time_str, prod_stat_char, logfile->nentries);
	  
	buf[MAX_STR_LEN-1] = '\0';
	  
	/* reload data to browser */
	fl_add_browser_line(browser, buf);  /* Add to the MAIN browser. */
	/* reset last status sline */
	memset(logfile->last_status_sline, '\0', MAX_BROWSER_LINE_LEN);
	strcpy(logfile->last_status_sline, buf);
	
  }
  

} /* load_product_groupings_to_browser */

/***************************************************************************/
/*                                                                         */
/*                         load_job_groupings_to_browser                   */
/*                                                                         */
/***************************************************************************/ 
void load_job_groupings_to_browser(FL_OBJECT *browser, 
								   char *product_grouping_str)
{
  /* Load current status info file range (product's jobs) to browser.
   */
  char prod[MAX_PRODNAME_LEN];
  char tapeid[MAX_TAPEID_LEN];
  char fromhost[MAX_HOSTNAME_LEN];
  char sdate[MAX_NAME_LEN];
  char stime[MAX_NAME_LEN];
  char *tmphost = "";
  struct logfile_rec *logfile;

  if (browser == NULL || product_grouping_str == NULL) return;
  if (gstatus_info == NULL) return;

  memset(prod, '\0', MAX_PRODNAME_LEN);
  memset(fromhost, '\0', MAX_HOSTNAME_LEN);
  memset(tapeid, '\0', MAX_TAPEID_LEN);
  memset(sdate, '\0', MAX_NAME_LEN);
  memset(stime, '\0', MAX_NAME_LEN);

  /* extract fromhost, sdate, stime, tapeid, and prod 
   * from: code host tapeid product device sdate stime runtime total-jobs 
   */
  if (sscanf(product_grouping_str, "%*s %s %s %s %*s %s %s %*s %*s", 
			 fromhost, tapeid, prod, sdate, stime) != 5) return;
  /* remove finished char, '*' or '-' if any from host str. */
  tmphost = fromhost;
  if (*tmphost == FINISHED_CHAR || *tmphost == NOT_ACTIVE_CHAR)
	tmphost++;

	
  /*
printf("load job: tapeid = %s, prod = <%s>\n", tapeid, prod);
*/
  /* load tapeid, products, file ranges, and their status to browser area */

  logfile = get_first_logfile(gstatus_info);
  while (logfile) {
	if ( strcmp(tmphost, logfile->fromhost) == 0 &&
		 (strcmp(tapeid, logfile->tapeid) == 0 ||
		  (strlen(tapeid) == MAX_TAPEID_LEN-1 && 
		   strstr(logfile->tapeid, tapeid) != NULL))
		 &&
		 strcmp(prod, logfile->prog) == 0 && 
		 strcmp(sdate, logfile->sdate) == 0 &&
		 strcmp(stime, logfile->stime) == 0 ) {
	  add_file_range_stat_to_browser(logfile,  browser);
	  break;
	}
	logfile = get_next_logfile(logfile);
  }
} /* load_job_groupings_to_browser */

/***************************************************************************/
/*                                                                         */
/*                       seconds_to_hhmmss_str                             */
/*                                                                         */
/***************************************************************************/
void seconds_to_hhmmss_str(double secs, char *time_str)
{
  int hr, min, sec, day=0;
  char buf[MAX_TIME_STR_LEN];
  

  if (time_str == NULL) return;
  
  memset(time_str, '\0', MAX_TIME_STR_LEN);
  if (secs < 0) {
	sprintf(time_str, "unknown");
	return;
	
  }
  
  seconds_to_hhmmss(secs, &day, &hr, &min, &sec);
  memset(buf, '\0', MAX_TIME_STR_LEN);
  sprintf(buf, "%-2.2d:%-2.2d:%-2.2d", hr, min, sec);
  if (day > 0)
	sprintf(time_str, "%d.%s", day, buf);
  else
	sprintf(time_str, "%s",buf);
  
  
} /* seconds_to_hhmmss_str */

/***************************************************************************/
/*                                                                         */
/*          load_exceeded_threshold_jobs_from_logfile_to_browser           */
/*                                                                         */
/***************************************************************************/
void load_exceeded_threshold_jobs_from_logfile_to_browser(
					  FL_OBJECT *browser,
					  struct logfile_rec *logfile, double threshold_sec)
{
  struct logfile_entry *entry;
  char runtime_str[MAX_TIME_STR_LEN];
  char buf[MAX_STR_LEN];
  double runtime;
  

  if (logfile == NULL || logfile->nentries < 1 || browser == NULL) return;
  
  entry = logfile->data;
  while (entry) {
	runtime = get_job_runtime(logfile, entry);
	/* Set its runtime--so won't need to do this again. */
	entry->runtime = runtime;

	/* If threshold_sec is negative, show everything; otherwise, show only
	 * if its runtime exceeds threshold.
	 */
	if (runtime > threshold_sec || threshold_sec < 0) {
	  /* entry's runtime exceeds threshold */
	  memset(buf, '\0', MAX_STR_LEN);
	  seconds_to_hhmmss_str(runtime, runtime_str);
	  sprintf(buf, "%-8.8s %-8.8s %12.12s %-16.16s %-10.10s %-16.16s %-3d %-70s",
			  entry->date, entry->time_str, runtime_str, logfile->tapeid,
			  logfile->tape_dev, logfile->prog, entry->fnum, entry->infile);
	  add_browser(browser, entry->stat|NO_STATUS, buf, TEXT_FORMAT_STR);
	  
	}

	/*
	else 
{	  seconds_to_hhmmss_str(runtime, runtime_str);
	  fprintf(stderr, "%-8.8s %-8.8s %-8.8s %-19.19s %-10.10s %-19.19s %-3d %-70s\n",
			  entry->date, entry->time, runtime_str, logfile->tapeid,
			  logfile->tape_dev, logfile->prog, entry->fnum, entry->infile);
}
*/
	entry = entry->next;
	
  }
  
} /* load_exceeded_threshold_jobs_from_logfile_to_browser */

/***************************************************************************/
/*                                                                         */
/*                                hours2secs                               */
/*                                                                         */
/***************************************************************************/
double hours2secs(double hours)
{
  
  return (hours * 3600.0);
  
} /* hours2secs */



/***************************************************************************/
/*                                                                         */
/*                          load_product_summary_to_browser                */
/*                                                                         */
/***************************************************************************/
void load_product_summary_to_browser(FL_OBJECT *browser)
{
  struct logfile_rec **sorted_logfiles_array, *logfile;
  int nsorted_items = 0, i;
  char buf[MAX_STR_LEN];
  char level_str[MAX_NAME_LEN];
  char finished_char = ' ', *finished_format_str;
  char prod_stat_char;
  int color_i;
  double total_product_time;
  char product_time_str[MAX_TIME_STR_LEN];

  if (browser == NULL) return;

  if (gstatus_info == NULL) return;
  /* Create logfile list sorted by tape and level
   * List contains all logfiles.
   */
  sorted_logfiles_array = (struct logfile_rec **)
	calloc(gstatus_info->nlogfiles, sizeof(struct logfile_rec *));
  if (sorted_logfiles_array == NULL) {
	perror("calloc sorted_logfiles_array\n");
    fl_show_message("calloc soreted_logfiles_array failed.", "Ignore loading product summary info.", "");
	return;
  }
  make_special_logfile_list(gstatus_info->logfiles, gstatus_info->nlogfiles,
							SORT_BY_LEVEL | forms_view_options[SUMMARYF_VIEW_OPTION_IND],
							"", "",
							sorted_logfiles_array, 
							&nsorted_items);
  for (i = 0; i < nsorted_items; i++) {
	/* Show status of each product. */
	logfile = sorted_logfiles_array[i];
	if (logfile == NULL) break;

	color_i = get_stat_color(logfile->stat);
	/* put '*' in front if finished processing for this product and tape */
	set_finished_indicators(logfile->finished, logfile->active, &finished_char,
						&finished_format_str);


	/* form_code [*]host tapeid level product device sdate stime runtime */
	/* Note: if form_code is "", modify code where browser line is parsed 
	 * in do_del_job_button() from product_grouping_form_cd.c.
	 * There should be only one space in between form_code and host.
	 */
	
	if (logfile->active) {
	  prod_stat_char = RUNNING_CHAR;
	}
	else
	  prod_stat_char = HALT_CHAR;

	/* Compute product's runtime */	
	compute_runtime(logfile->active, logfile->stime_item, logfile->etime_item,
					&total_product_time, product_time_str);
	/* clear buffers  */
	memset(buf, '\0', MAX_STR_LEN);
	memset(level_str, '\0', MAX_NAME_LEN);
	get_level_for_prod(logfile->prog, level_str);
	sprintf(buf, "%s@C%d %c%-9.9s %-19.19s %-5.5s %-21.21s %-10.10s %-8.8s %-8.8s %11.11s %c",
			finished_format_str, color_i, finished_char, logfile->fromhost,
			logfile->tapeid, level_str, logfile->prog, logfile->tape_dev,
			logfile->sdate, logfile->stime,  
			product_time_str, prod_stat_char);
	  
	buf[MAX_STR_LEN-1] = '\0';
	/* reload data to browser */
	fl_add_browser_line(browser, buf);  
  }
  free(sorted_logfiles_array);
} /* load_product_summary_to_browser */



/***************************************************************************/
/*                                                                         */
/*                         set_status_for_prog_in_level_table              */
/*                                                                         */
/***************************************************************************/
void set_status_for_prog_in_level_table(level_info_t *level_info_table,
										int level_num,
										char *prog, stat_type_t  stat)
{
  /* Set the status for the program in particular level num in the level table. 
   * If prog == "", set all programs with the specified stat.
   */
  int p, ll;

  if (level_info_table == NULL || prog == NULL) return;

  
  ll = level_num-1;
  if (ll < 0 || ll >= MAX_GV_LEVELS_NUM) return;
  for (p = 0; p < MAX_PROGS_PER_LEVEL; p++) {
	if (level_info_table[ll].prog_info[p].prog == NULL) continue;
	if (strlen(prog) == 0) 
	  level_info_table[ll].prog_info[p].stat = stat;
	else if (strcmp(level_info_table[ll].prog_info[p].prog, prog) == 0) {
	  level_info_table[ll].prog_info[p].stat = stat;
	  break;
	}
  }
												  
} /* set_status_for_prog_in_level_table */



/***************************************************************************/
/*                                                                         */
/*                             view_this_entry                             */
/*                                                                         */
/***************************************************************************/
int view_this_entry(stat_type_t entry_stat, int active,
					view_option_type_t view_type)
{
	int select_this_one = 0;

	if (view_type & VIEW_ALL) {
	  /* View all */
	  select_this_one = 1;
	}
	else if (( (view_type & VIEW_SUCCESSFULS) && 
				   ((entry_stat & SUCCESS) &&
					!(entry_stat & WARNING) &&
					!(entry_stat & FAILED) &&
					!(entry_stat & ABORT) &&
					!(entry_stat & NOT_DEFINED) &&
					!(entry_stat & RUNNING))) ||
			 ( (view_type & VIEW_FAILURES) && (entry_stat & FAILED)) ||
			 ( (view_type & VIEW_WARNINGS) && (entry_stat & WARNING)) ||
			 ( (view_type & VIEW_UNKNOWNS) && (entry_stat & NOT_DEFINED)) ||
			 ( (view_type & VIEW_RUNNINGS) && (entry_stat & RUNNING)) ) {


	  select_this_one = 1;

	}

	if (select_this_one) {
	  if (((view_type & VIEW_ACTIVE_ONLY) && active == 1) ||
		  ((view_type & VIEW_INACTIVE_ONLY) && active == 0)) {

		return 1;
	  }


	  /*
fprintf(stderr, "list type = %d, logfile: %s stat = %d i = %d\n", view_type, logfile->name, entry_stat, i);
*/
	  /*
fprintf(stderr, "added: %s %s %s %d\n", logfile->fromhost, logfile->tapeid, logfile->prog, i);
*/
	}
	return 0;
} /* view_this_entry */

/***************************************************************************/
/*                                                                         */
/*                            load_level_summary_to_browser                */
/*                                                                         */
/***************************************************************************/
void load_level_summary_to_browser(FL_OBJECT *browser)
{
  /* Load status info on GV level I-III */
  struct logfile_rec **sorted_logfiles_array, *logfile;
  stat_type_t stat;
  int nsorted_items = 0, i;
  char buf[MAX_STR_LEN];
  char curr_level_str[MAX_NAME_LEN];
  char finished_char = ' ', *finished_format_str;
  int color_i;
  double total_level_time;
  char level_time_str[MAX_TIME_STR_LEN];
  int curr_level, level, next_level;
  int finished;
  char curr_tapeid[MAX_TAPEID_LEN];
  time_t curr_stime_item, curr_etime_item;
  int done;
  char *curr_stime, *curr_sdate;

  if (browser == NULL) return;

  if (gstatus_info == NULL) return;
  /* Create logfile list sorted by tapeid and prog.  
   * List contains all logfiles.
   */
  sorted_logfiles_array = (struct logfile_rec **)
	calloc(gstatus_info->nlogfiles, sizeof(struct logfile_rec *));
  if (sorted_logfiles_array == NULL) {
	perror("calloc sorted_logfiles_array\n");
    fl_show_message("calloc soreted_logfiles_array failed.", "Ignore loading level summary info.", "");
	return;
  }
  /* Sort by program and tape only since we consider programs from all hosts. */
  make_special_logfile_list(gstatus_info->logfiles, gstatus_info->nlogfiles,
							SORT_BY_LEVEL | VIEW_ALL | VIEW_ACTIVE_ONLY | 
							VIEW_INACTIVE_ONLY,
							"", "",
							sorted_logfiles_array, 
							&nsorted_items);

  done = 0;
  

  for (i = 0; i < nsorted_items; i++) {
	/* Show status of each level for each tape. One level may contain more
	 * than one program.  
	 *   i.e.: level      Programs
	 *           I          level_1
	 *           II         2A-52, 2A-53, 2A-54, 2A-55
	 *           III        3A-53, 3A-54, 3A-55
	 *
	 * List is sorted by tape and prog.
	 */
	logfile = sorted_logfiles_array[i];
	if (logfile == NULL) break;

	memset(curr_level_str, '\0', MAX_NAME_LEN);
	level = get_level_for_prod(logfile->prog, curr_level_str);

	finished = 0;
	curr_level = level; 
	next_level = level;
	curr_stime_item = logfile->stime_item;
	curr_etime_item = logfile->etime_item;
	curr_stime = logfile->stime;
	curr_sdate = logfile->sdate;
	strcpy(curr_tapeid, logfile->tapeid);
	/* Initialize */
	set_status_for_prog_in_level_table(level_info_table, curr_level, "", 0);

	/*
fprintf(stderr, "#######\n");
*/
    while ((next_level == curr_level && 
			strcmp(curr_tapeid, logfile->tapeid) == 0)){
	  /* Process the current level */

	  /*
fprintf(stderr, "host: %s tapeid: %s, prog: %s\n", logfile->fromhost, logfile->tapeid, logfile->prog);
*/

	  stat = logfile->stat;
	  /* Change level's start and end time. */
	  if (curr_stime_item > logfile->stime_item) {
		curr_stime_item = logfile->stime_item;
		curr_stime = logfile->stime;
		curr_sdate = logfile->sdate;
	  }
	  if (curr_etime_item < logfile->etime_item)
		curr_etime_item = logfile->etime_item;

	  i++;

	  if (logfile->active)
		stat |= ACTIVE;
	  if (logfile->finished)
		stat |= FINISHED;
	  set_status_for_prog_in_level_table(level_info_table, curr_level, 
										 logfile->prog, stat);
	  if (i >= nsorted_items) {
		done = 1;
		break;
	  }

	  /* Go to the next logfile in the list. */
	  logfile = sorted_logfiles_array[i];
      if (logfile == NULL) {
		done = 1;
		break;
	  }

	  next_level = get_level_for_prod(logfile->prog, NULL);
	} /* while */

	i--;  /* Go back to the previous logfile */

	/* Only show level 1+ */
	if (curr_level < 1) continue;

	/*
fprintf(stderr, "<< %s\n", logfile->tapeid);
*/

	/*
fprintf(stderr, " %s, %d>>\n", logfile->tapeid, stat&FINISHED);
*/
	stat = get_level_stat(level_info_table, curr_level);
	color_i = get_stat_color(stat);

	if (stat & FINISHED) finished = 1;

	/* Show this entry only if its status matches the user spec.
	 */
	if (!view_this_entry(stat,  !finished,
						 forms_view_options[SUMMARYF_VIEW_OPTION_IND]))
	  continue;
	/* Show the status of the current level */

	/* put '*' in front if finished processing for this product and tape */
	set_finished_indicators(finished, !finished, &finished_char,
						&finished_format_str);

	/* form_code [*]tapeid level sdate stime runtime */
	/* Note: if form_code is "", modify code where browser line is parsed 
	 * in do_del_job_button() from product_grouping_form_cd.c.
	 * There should be only one space in between form_code and host.
	 */
	
	total_level_time = 0;
	strcpy(level_time_str, "");
	/* Compute level's runtime */	
	compute_runtime(!finished, curr_stime_item, curr_etime_item,
					&total_level_time, level_time_str);
	/* clear buffers  */
	memset(buf, '\0', MAX_STR_LEN);

	sprintf(buf, "%s@C%d %c%-19.19s %-5.5s  %-8.8s   %-8.8s   %11.11s",
			finished_format_str, color_i, finished_char, 
			curr_tapeid, curr_level_str, 
			curr_sdate, curr_stime,  
			level_time_str);
	  
	buf[MAX_STR_LEN-1] = '\0';
	
	/* reload data to browser */
	fl_add_browser_line(browser, buf);  
	if (done)
	  goto DONE;

  } /* for */
  DONE:
  free(sorted_logfiles_array);

} /* load_level_summary_to_browser */



/***************************************************************************/
/*                                                                         */
/*                           load_tape_summary_to_browser                  */
/*                                                                         */
/***************************************************************************/
void load_tape_summary_to_browser(FL_OBJECT *browser)
{
  /* Load status info on tapes */
  struct tape_grouping_rec **special_groups_array;
  stat_type_t stat;
  char buf[MAX_STR_LEN];
  char finished_char = ' ', *finished_format_str;
  int color_i;
  double total_tape_time;
  char tape_time_str[MAX_TIME_STR_LEN];
  int finished = 0;
  char curr_tapeid[MAX_TAPEID_LEN];
  time_t curr_stime_item, curr_etime_item;
  int done;
  char curr_stime[MAX_NAME_LEN], curr_sdate[MAX_NAME_LEN];
  int failures = 0;
  int warnings = 0;
  int runnings = 0;
  int success = 0;
  int abort = 0;
  int unknowns = 0;
  int nitems, ngroups, i;
  struct tape_grouping_rec *grp;
  int narray_items = 0;  
  
  if (browser == NULL) return;
  grp = tape_groups;
  ngroups = get_group_count(grp);

  special_groups_array = (struct tape_grouping_rec **)
	calloc(ngroups, sizeof(struct tape_grouping_rec *));
  if (special_groups_array == NULL) {
	perror("calloc special_groups_array");
	exit_system(NULL, -1);
  }
  /* Create group list sorted either by end time or by host and tapeid, 
   */
  make_special_grp_list(grp, ngroups, 
						SORT_BY_TAPE | VIEW_ALL | VIEW_ACTIVE_ONLY| 
						VIEW_INACTIVE_ONLY,
						special_groups_array, &narray_items);
  done = 0;
  i = 0;
  while (i < narray_items) {
	/* Show status of each tape. 
	 * List is sorted by tape.  Same tape ID may have different hosts.
	 */
	grp = special_groups_array[i];
	if (grp == NULL) {
	  goto NEXT;
	}
	finished = 0;
	failures = 0;
	warnings = 0;
	runnings = 0;
	success = 0;
	unknowns = 0;
	abort = 0;
	curr_stime_item = grp->stime;
	curr_etime_item = grp->etime;
	strcpy(curr_tapeid, grp->tapeid);
	nitems = 0;

    while (strcmp(curr_tapeid, grp->tapeid) == 0){
	  /* Process the current tape. */
	  /*
fprintf(stderr, "host: %s tapeid: %s\n", grp->host, grp->tapeid);
*/

	  nitems++;

	  /* Change current tape's start and end time. */
	  if (curr_stime_item > grp->stime) {
		curr_stime_item = grp->stime;
	  }
	  if (curr_etime_item < grp->etime)
		curr_etime_item = grp->etime;

	  i++;
	  stat = grp->stat;
	  /* THe order is relevant */
	  if (stat & FAILED) failures++; 
	  else if (stat & WARNING) warnings++; 
	  else if (stat & RUNNING) runnings++; 
	  else if (stat & ABORT) abort++;
	  else if (stat & UNKNOWN) unknowns++; 
	  else if (stat & SUCCESS) success++; 


	  if (grp->finished) finished++;

	  if (i >= narray_items) {
		done = 1;
		break;
	  }

	  /* Go to the next tape group in the list. */
	  grp = special_groups_array[i];
      if (grp == NULL) {
		break;
	  }

	} /* while */

	i--;  /* Go back to the previous grp. */

	/* Show the status of the current tape  */
	stat = UNKNOWN;
	/* THe order is relevant */
	/* Something in this tape is ... */
	if (failures) stat = FAILED;
	else if (warnings) stat = WARNING;   
	else if (runnings) stat = RUNNING;
	else if (unknowns) stat = UNKNOWN;
	/* This tape is... */
	else if (abort == nitems) stat = ABORT;
	else if (success == nitems) stat = SUCCESS;

	/* Show this entry only if its status matches the user spec.
	 */
	if (!view_this_entry(stat,  !finished,
						 forms_view_options[SUMMARYF_VIEW_OPTION_IND]))
	  goto NEXT;

	color_i = get_stat_color(stat);

	if (finished == nitems) finished = 1;
	else finished = 0;

	/* put '*' in front if finished processing for this product and tape */
	set_finished_indicators(finished, !finished, &finished_char,
						&finished_format_str);

	/* form_code [*]tapeid sdate stime runtime */
	/* Note: if form_code is "", modify code where browser line is parsed 
	 * in do_del_job_button() from product_grouping_form_cd.c.
	 * There should be only one space in between form_code and host.
	 */
	
	total_tape_time = 0;
	strcpy(tape_time_str, "");
	/* Compute tape's runtime */	
	compute_runtime(!finished, curr_stime_item, curr_etime_item,
					&total_tape_time, tape_time_str);
	/* clear buffers  */
	memset(buf, '\0', MAX_STR_LEN);

    memset(curr_sdate, '\0', MAX_NAME_LEN);
    memset(curr_stime, '\0', MAX_NAME_LEN);
	
	time_secs2strs(curr_stime_item, curr_stime, curr_sdate);
	sprintf(buf, "%s@C%d %c%-19.19s %-8.8s   %-8.8s   %11.11s",
			finished_format_str, color_i, finished_char, 
			curr_tapeid,  
			curr_sdate, curr_stime,  
			tape_time_str);
	  
	buf[MAX_STR_LEN-1] = '\0';

	/* reload data to browser */
	fl_add_browser_line(browser, buf);  

	if (done)
	  goto DONE;
  NEXT:
	i++;

  } /* while */
  DONE:
  free(special_groups_array);

  
} /* load_tape_summary_to_browser */


/***************************************************************************/
/*                                                                         */
/*                            load_summary_info_to_browser                 */
/*                                                                         */
/***************************************************************************/
void load_summary_info_to_browser(FL_OBJECT *browser, summary_type_t type)
{
  if (browser == NULL) return;
  if ((type & (TAPE_SUMMARY |  LEVEL_SUMMARY | PRODUCT_SUMMARY)) ==
	  (TAPE_SUMMARY |  LEVEL_SUMMARY | PRODUCT_SUMMARY) ||
	  (type & PRODUCT_SUMMARY)) {
	
	/* Load summary of tapes, level I-III, and products */
	load_product_summary_to_browser(browser);
  }
  else if ((type & (TAPE_SUMMARY |  LEVEL_SUMMARY)) ==
		   (TAPE_SUMMARY |  LEVEL_SUMMARY) ||
		   (type &  LEVEL_SUMMARY)) {

	/* Load summary of level */
	load_level_summary_to_browser(browser);

  }
  else if (type & TAPE_SUMMARY) {
	/* Load summary of tape */
	load_tape_summary_to_browser(browser);
  }
} /* load_summary_info_to_browser */

/***************************************************************************/
/*                                                                         */
/*                   load_exceeded_threshold_jobs_to_browser               */
/*                                                                         */
/***************************************************************************/
void load_exceeded_threshold_jobs_to_browser(FL_OBJECT *browser, double threshold_value)
{
  
  double threshold_secs;
  struct logfile_rec *logfile;
  

  if (browser == NULL) return;
  if (gstatus_info == NULL) return;
  threshold_secs = hours2secs(threshold_value);
  /*
printf("thresholdsec: %f, value: %f\n", threshold_secs, threshold_value);
*/
  
  logfile = get_first_logfile(gstatus_info);
  while (logfile) {
	load_exceeded_threshold_jobs_from_logfile_to_browser(browser, logfile, 
														 threshold_secs);
	logfile = get_next_logfile(logfile);
  }
  
} /* load_exceeded_threshold_jobs_to_browser */

/***************************************************************************/
/*                                                                         */
/*                         load_items_to_browser                           */
/*                                                                         */
/***************************************************************************/ 
void load_items_to_browser(FL_FORM *form, FL_OBJECT *browser, 
						   item_type_t type, void *anything, int clear)
{
  int i;
  struct tape_info_rec *tape_info;
  struct logfile_rec *logfile;
  int  max_browser_line = 0;
  int  load = 0;
  struct ps_info_rec *ps_bufs;
  struct host_info_rec *host_info;
  raw_data_t *all_data_array;
  int total_jobs;
  int save_topline;
  char line[MAX_BROWSER_LINE_LEN];
  char *color_code;

  if (form == NULL || browser == NULL) return;

  /* only load data to browser if form is visible */
  if (!form->visible) return;
/*
  if (!form->frozen)
*/
  fl_freeze_form(form);

  save_topline = fl_get_browser_topline(browser);
  if (clear == 1)
	fl_clear_browser(browser);
  
  max_browser_line = fl_get_browser_maxline(browser);
  if (clear || max_browser_line == 0)
	  load = 1;

  switch (type) {
  case ALL_DATA:
	/* load every single job from gstatus_info's logfiles to browser. Jobs
	 * are sorted by time. 
	 */
	if (gstatus_info == NULL) break;
	total_jobs = compute_total_jobs(gstatus_info->logfiles);
	all_data_array = (raw_data_t *) calloc(total_jobs, sizeof(raw_data_t));
	if (all_data_array == NULL) {
	  perror("calloc all_data_array ");
	  break;
	}
	i = 0;

	logfile =  gstatus_info->logfiles;
	while (logfile) {
	  load_logfile_entries_to_array(all_data_array, i, total_jobs, logfile->data,
									logfile->fromhost, logfile->tapeid, 
									logfile->tape_dev, logfile->prog);
      i += logfile->nentries;
	  logfile = logfile->next;
	}
	qsort((void *) all_data_array, (size_t) total_jobs,
		  (size_t) sizeof(raw_data_t), (int (*)(const void *,
		  const void *))qsort_etime_compare_raw_data);

	load_raw_data_array_to_browser(all_data_array, total_jobs, browser);
	free(all_data_array);

    break;
  case SELECTED_PROGS:
	/* load selected programs to browser area */
	for (i = 0; i < MAX_PROGS; i++) {
	  if (Gselected_progs[i] == NULL || strlen(Gselected_progs[i]) == 0)
		continue;
	  fl_add_browser_line(browser, Gselected_progs[i]);
	}
	break;
  case AVAILABLE_PROGS:
	/* load selected programs to browser area */
	for (i = 0; i < MAX_PROGS; i++) {
	  if (available_progs[i] == NULL || strlen(available_progs[i]) == 0) 
		continue;
	  if (prog_exists(Gselected_progs,available_progs[i]))
		color_code = "@C17";
	  else 
		color_code = "";
	  sprintf(line, "%s %s", color_code, available_progs[i]);
	  fl_add_browser_line(browser, line);
	}
	break;
  case SELECTED_HOSTS:
	/* load neighbors to browser area */
	for (i = 0; i < MAX_HOSTS; i++) {
	  if (Gselected_hosts[i] && strlen(Gselected_hosts[i]) > 0) 
		fl_add_browser_line(browser, Gselected_hosts[i]);
	}
	if ((char *)anything && strlen((char *)anything) > 0) 
		fl_add_browser_line(browser, (char *)anything);

	break;
  case AVAILABLE_HOSTS:
	/* load neighbors to browser area */
	for (i = 0; i < MAX_HOSTS; i++) {
	  if (available_hosts[i] && strlen(available_hosts[i]) > 0) {
		if (host_exists(Gselected_hosts, available_hosts[i]))
		  color_code = "@C17";
		else 
		  color_code = "";
		sprintf(line, "%s %s", color_code, available_hosts[i]);
		fl_add_browser_line(browser, line);
	  }
	}
	break;
  case AVAILABLE_NETWORK_HOSTS:
	/* Load available network hosts to the browser */

	for (i = 0; i < MAX_HOSTS; i++) {
	  if (Gavailable_network_hosts[i] && 
		  strlen(Gavailable_network_hosts[i]) > 0) {
		if (host_exists(available_hosts, Gavailable_network_hosts[i])) 
		  color_code = "@C17";
		else 
		  color_code = "";
		sprintf(line, "%s %s", color_code, Gavailable_network_hosts[i]);
		fl_add_browser_line(browser, line);
	  }
	}
	break;
  case SELECTED_NETWORK_HOSTS:
	/* load selected network neighbors to browser area */
	for (i = 0; i < MAX_HOSTS; i++) {
	  if (available_hosts[i] && strlen(available_hosts[i]) > 0) {
		fl_add_browser_line(browser, available_hosts[i]);
	  }
	}
	break;
  case TAPEID:
	/* load tape ids and their status to browser area */
fprintf(stderr, "LOAD TAPEID >>>>>>\n");
	tape_info = tape_info_list;
	while (tape_info) {
	  /* add tapeid and its status to browser */
	  add_tape_info_to_browser( tape_info, browser, TAPEID);
	  tape_info = tape_info->next;
	}
	break;
#if 0
  case PROD:
	/* load tapeids, products ,and their status of tape ran last night and 
	   currently running to browser area */
	tape_info = tape_info_list;
	while (tape_info) {
	  add_tape_info_to_browser(tape_info, browser, PROD);
	  tape_info = tape_info->next;
	}
	break;
#endif
  case CURR_PROD:
	load_product_groupings_to_browser(browser, (char *)anything);

	break;
#if 0
  case LOG_FILE_RNG:
	/* load file range (detailed data) for log data -- received from monitorps
	 * as log data 
	 */

	if ((char *)anything == NULL) break;

	/* clear buffers */
	memset(prod, '\0', MAX_PRODNAME_LEN);
	memset(tapeid, '\0', MAX_TAPEID_LEN);
	
	/* extract tapeid and prod from:
	 * code sdate stime edate etime tapeid tape_device product */
	sscanf((char *)anything, "%*s %*s %*s %*s %*s %s %*s %s",  tapeid, prod);
	/*
printf("#tapeid = %s, prod = <%s>\n", tapeid, prod);
*/
	/* load tapeid, products, file ranges, and their status to browser area */
	tape_info = get_tape_info(tapeid);

	if (tape_info == NULL || strlen(prod) == 0) break;

	pindex = get_prod_index(prod);

	if (pindex < 0 || pindex >= MAX_PRODS) break;

	logfile = tape_info->logfiles[pindex];
	add_file_range_stat_to_browser(logfile,  browser);
	break;
#endif
  case STAT_INFO_FILE_RNG:
	load_job_groupings_to_browser(browser, (char *)anything);
	break;

  case PS:
	/* load ps info */
	if ((char *)anything == NULL) break;

	host_info = host_info_list;
	while (host_info) {
	  if (!host_exists(Gselected_hosts, host_info->name)) goto NEXT;
	  if (*((char *)anything) & PS_SELECTED_PROGS)
		ps_bufs = host_info->ps_info[SELECTED_PROGS_INDX];
	  else if (*((char *)anything) & PS_ALL_PROGS)
		ps_bufs = host_info->ps_info[NETWORK_TOP_INDX];
	  
	  if (ps_bufs == NULL) goto NEXT;

	  if (*((char *)anything) & PS_TOP_LEVEL) {
		if (ps_bufs->top_level) 
		  fl_add_browser_line(browser, ps_bufs->top_level);
	  }
	  else if (*((char *)anything) & PS_ALL_LEVELS) {
		if (ps_bufs->all_levels)
		  fl_add_browser_line(browser, ps_bufs->all_levels);
	  }
NEXT:
	  host_info = host_info->next;
	}
	break;
  case TAPE_GROUP:
	load_tape_groupings_to_browser(browser);
	break;
  case EXCEEDED_THRESHOLD_JOBS:
	
	load_exceeded_threshold_jobs_to_browser(browser, threshold_time);
	break;
  case SUMMARY_INFO:
	/* Load info to the summary form */
	load_summary_info_to_browser(browser, *((summary_type_t *)anything));
	break;
  case AVAILABLE_SUMMARY_LEVELS:
	sprintf(line, "%s\n%s\n%s", TAPE_SUMMARY_STR, LEVEL_SUMMARY_STR, 
			PRODUCT_SUMMARY_STR);
	fl_add_browser_line(browser, line);
	break;
  default: break;
  }

/*
  if (form->frozen)
*/
	fl_set_browser_topline(browser, save_topline);
	fl_unfreeze_form(form);

} /* load_items_to_browser */

/***************************************************************************/
/*                                                                         */
/*                         popup_select_host_for_ps_form                   */
/*                                                                         */
/***************************************************************************/
void popup_select_host_for_ps_form(FL_OBJECT *button, long val)
{
  
  long old_mask;

  old_mask = gms_block_ints();
  if (show_button_info_flag)
	hide_button_info_msg();
  if (select_host_for_ps_form == NULL ||
	  select_host_for_ps_form->select_item_form == NULL || 
	  select_host_for_ps_form->browser == NULL ) {
	gms_unblock_ints(old_mask);
	return;
  }

  fl_deactivate_object(select_host_for_ps_form->browser);
  show_form(select_host_for_ps_form->ldata);
  load_items_to_browser(select_host_for_ps_form->select_item_form, 
						select_host_for_ps_form->browser, SELECTED_HOSTS, "", 
						CLEAR);
  fl_activate_object(select_host_for_ps_form->browser);

  gms_unblock_ints(old_mask);
}


/***************************************************************************/
/*                                                                         */
/*                        send_ps_request_for_selected_items               */
/*                                                                         */
/***************************************************************************/
void send_ps_request_for_selected_items(char *host_list[], int max_hosts)
{
  char msg[MAX_MSG_LEN];
  int i;

  for (i = 0; i < max_hosts; i++) {
	if (host_list[i] == NULL) continue;
	if (strlen(host_list[i]) == 0) continue;

	memset(msg, '\0', MAX_MSG_LEN);
	sprintf(msg, "%s %s.%d", ADD_PSGUI_GUI_STR, local_host, gms_portnum);

	if (send_msg(host_list[i], msg) < 0)
	  exit_system(NULL, -1);
  }

} /* send_ps_request_for_selected_items */

/***************************************************************************/
/*                                                                         */
/*                         process_ps_request                              */
/*                                                                         */
/***************************************************************************/
void process_ps_request(FL_OBJECT *button, long val)
{
  FD_select_item_form *form;
  char *selected_hosts[MAX_HOSTS];
  long oldmask;


  oldmask = gms_block_ints();

  if (button == NULL || button->u_vdata == NULL) {
	gms_unblock_ints(oldmask);
	return;
  }
  fl_deactivate_object(button);
  form = (FD_select_item_form *) button->u_vdata;
  get_selected_items(form->browser, selected_hosts, SELECTED_HOSTS, 
					 MAX_HOSTS, MAX_HOSTNAME_LEN);
  
  send_ps_request_for_selected_items(selected_hosts, MAX_HOSTS);

  fl_activate_object(button);
  do_close_form(form->cancel_button, 0);
  free_array_of_str(selected_hosts, MAX_HOSTS);

  gms_unblock_ints(oldmask);
} /* process_ps_request */


/***************************************************************************/
/*                                                                         */
/*                                do_nothing                               */
/*                                                                         */
/***************************************************************************/

void do_nothing(FL_OBJECT *button, long val)
{

}

/***************************************************************************/
/*                                                                         */
/*                           clear_ps_bufs_for_host                        */
/*                                                                         */
/***************************************************************************/

void clear_ps_bufs_for_host(struct host_info_rec *host_info)
{
  struct ps_info_rec *ps_bufs;
  int i;

  if (host_info == NULL) return;
  
  for (i = 0 ; i < MAX_PS_DATA_TYPES; i++) {
	if (host_info->ps_info[i] == NULL) continue;
	ps_bufs = host_info->ps_info[i];

	if (ps_bufs->top_level) memset(ps_bufs->top_level, '\0', MAX_PS_BUF_LEN);
	if (ps_bufs->all_levels) memset(ps_bufs->all_levels, '\0', MAX_PS_BUF_LEN);
  }
}

/***************************************************************************/
/*                                                                         */
/*                           clear_ps_bufs_for_all_hosts                   */
/*                                                                         */
/***************************************************************************/
void clear_ps_bufs_for_all_hosts() 
{

  struct host_info_rec *host_info;

  host_info = host_info_list;
  while (host_info) {
	clear_ps_bufs_for_host(host_info);
	host_info = host_info->next;
  }

}
	  
/***************************************************************************/
/*                                                                         */
/*                           popup_ps_form                                 */
/*                                                                         */
/***************************************************************************/

void popup_ps_form(FL_OBJECT *button, long prod_index)
{
  long oldmask;

  oldmask = gms_block_ints();
  if (show_button_info_flag)
	hide_button_info_msg();
  if (button == NULL || ps_form == NULL || ps_form->ps_info_form == NULL) {
	gms_unblock_ints(oldmask);
	return;
  }

  clear_ps_bufs_for_all_hosts();
  show_form(ps_form->ldata);
  show_ps_form();

  gms_unblock_ints(oldmask);

} /* popup_ps_form( */

/***************************************************************************/
/*                                                                         */
/*                          add_tape_info                                  */
/*                                                                         */
/***************************************************************************/

void add_tape_info(struct tape_info_rec *tape_info)
{
  if (tape_info == NULL) return;

  /* add new item to top of list */
  tape_info->next = tape_info_list;
  tape_info_list = &(*tape_info);
}

/***************************************************************************/
/*                                                                         */
/*                                remove_tape_info                         */
/*                                                                         */
/***************************************************************************/
void remove_tape_info(char *tapeid)
{
  struct tape_info_rec *tape_info, *tmp=NULL;

  if (tapeid == NULL || strlen(tapeid) == 0 || tape_info_list == NULL) return;
  tape_info = tape_info_list;

  if ((strcmp(tape_info->id, tapeid) == 0 ||
	   (strlen(tapeid) == MAX_TAPEID_LEN-1 && 
		strstr(tape_info->id, tapeid) != NULL))) {
	tape_info_list = tape_info->next;
	free_tape_info(tape_info);
	tape_info = NULL;
	return;
  }
  while (tape_info) {
	if (strcmp(tape_info->id, tapeid) == 0 ||
		 (strlen(tapeid) == MAX_TAPEID_LEN-1 &&
		  strstr(tape_info->id, tapeid) != NULL)) {
	  tmp->next = tape_info->next;
	  free_tape_info(tape_info);
	  tape_info = NULL;
	  return;
	}
	tmp = tape_info;
	tape_info = tape_info->next;
  }
	  
}

/***************************************************************************/
/*                                                                         */
/*                             new_tape_info                               */
/*                                                                         */
/***************************************************************************/ 
struct tape_info_rec * new_tape_info(char *tapeid)
{
  struct tape_info_rec *tape_info;
  int i;

  if (tapeid == NULL || strlen(tapeid) == 0) return NULL;
 
 /* create a new tape info and add to list. */
  tape_info = (struct tape_info_rec *) calloc(1, sizeof(struct tape_info_rec));
  if (tape_info == NULL) {
	perror("calloc tape info");
	exit_system(NULL, -1);
  }
  strncpy(tape_info->id, tapeid, MAX_TAPEID_LEN);
  /* teminate the string -- incase tapeid is longer */
  tape_info->id[MAX_TAPEID_LEN -1] = '\0';

  /* initialze logfiles */
  for (i = 0; i < MAX_PRODS; i++) 
	tape_info->logfiles[i] = NULL;

  return tape_info;
}  /* new_tape_info */


/***************************************************************************/
/*                                                                         */
/*                            raise_form                                   */
/*                                                                         */
/***************************************************************************/
void raise_form(FL_FORM *form)
{
  /* raise form */
  if (form == NULL) return;
  fl_raise_form(form);
  /* XMapWindow is required to deiconify the window if it is iconified*/
  XMapWindow(fl_get_display(), form->window);
}

/***************************************************************************/
/*                                                                         */
/*                              show_form                                  */
/*                                                                         */
/***************************************************************************/
void show_form(long form_index)
{
  /* Show form if it is invisible; raise it otherwise.
   * add form's name to product_grouping_form's windows menu.
   */
  char label[MAX_LABEL_LEN];
  FL_Coord w, h;
  char *name; 
  FL_FORM *form;

  if (form_index < 0 || form_index >= MAX_FORMS) return;

  form = gms_form_list[form_index].form;
  name = gms_form_list[form_index].name;
  if (form == NULL || name == NULL) return;

  if (form->visible) { 
	raise_form(form);
	return;
  }
  memset(label, '\0', MAX_LABEL_LEN);
  /* need a space after ':'; otherwise modify do_close_form()  */

  sprintf(label, "%s: %s", local_host, name);


  /* call set minsize routine once for each form just right before 
   * calling show form routine in order for it to affect the form. (why ?) 
   */

  w = gms_form_list[form_index].min_width;
  h = gms_form_list[form_index].min_height;
  fl_set_form_minsize(form, w, h);

  fl_show_form(form, FL_PLACE_FREE,  FL_FULLBORDER, label);


  /* Add form name to tape_grouping_form's  windows menu */
  fl_addto_menu(tape_grouping_form->windows_menu, name);

  /* Add window name to windows_menu_items -- will remove from menu later */
  /* No space in between '|'; otherwise modify do_close_form()  */
  strcat(windows_menu_items, name);
  strcat(windows_menu_items, "|");

} /* show_form */

/***************************************************************************/
/*                                                                         */
/*                        popup_status_form                                */
/*                                                                         */
/***************************************************************************/ 
void popup_status_form(FL_OBJECT *button, long val)
{
  FL_Coord w, h;
  long oldmask;


  oldmask = gms_block_ints();
  if (show_button_info_flag)
	hide_button_info_msg();
  if (button == NULL || gstatus_info == NULL || gstatus_info->sform == NULL ||
	  gstatus_info->sform->status_form == NULL) {
	gms_unblock_ints(oldmask);
	return;
  }

  w = gstatus_info->sform->status_form->w;
  h = gstatus_info->sform->status_form->h;
  fl_set_form_maxsize(gstatus_info->sform->status_form, w, h);

  /* shows form before updates it since update_status_form() only updates
   * form if form is being mapped.
   */

  show_form(gstatus_info->sform->ldata);

  /* Use default view options */
  forms_view_options[STATUSF_VIEW_OPTION_IND] = VIEW_DEFAULT;
  set_view_menu_options(gstatus_info->sform->view_menu, 
						&(forms_view_options[STATUSF_VIEW_OPTION_IND]), 
						STATUS_FORM_ID);

  /* Reload data to form containing bars */
  update_status_form(gstatus_info->scroll_per, 1);

  gms_unblock_ints(oldmask);

} /* popup_status_form */

/***************************************************************************/
/*                                                                         */
/*                            file_to_browser                              */
/*                                                                         */
/***************************************************************************/
void file_to_browser(FL_OBJECT * browser, 
				   char *logfile)
{
  if (browser == NULL || logfile == NULL) return;

  if (!fl_load_browser(browser, logfile))
	fl_add_browser_line(browser,"Log file not available!");
} /* load_file_to_browser */

/***************************************************************************/
/*                                                                         */
/*                            free_ps_form                                 */
/*                                                                         */
/***************************************************************************/
void free_ps_form( FD_ps_info_form * form)
{
  if (form == NULL) return;
  if (form->ps_info_form ) {
	if (form->ps_info_form->visible)
	  fl_hide_form(form->ps_info_form);
	fl_free_form(form->ps_info_form);
  }
  free(form);
}
/***************************************************************************/
/*                                                                         */
/*                            free_select_item_form                        */
/*                                                                         */
/***************************************************************************/
void free_select_item_form(FD_select_item_form *form)
{
  if (form == NULL) return;
  if (form->select_item_form) {
	if (form->select_item_form->visible)
	  fl_hide_form(form->select_item_form);
	fl_free_form(form->select_item_form);
  }
  free(form);

}


/***************************************************************************/
/*                                                                         */
/*                            free_product_grouping_form                              */
/*                                                                         */
/***************************************************************************/
void free_product_grouping_form(FD_gms_form *form)
{
  if (form == NULL) return;
  if (form->gms_form)  {

	if (form->gms_form->visible)
	  fl_hide_form(form->gms_form);
	fl_free_form(form->gms_form);

  }

  free(form);

}
/***************************************************************************/
/*                                                                         */
/*                            free_get_input_form                              */
/*                                                                         */
/***************************************************************************/
void free_get_input_form(FD_get_input_form *form)
{
  if (form == NULL) return;
  if (form->get_input_form) {
	if (form->get_input_form->visible)
	  fl_hide_form(form->get_input_form);
	fl_free_form(form->get_input_form);
  }
  free(form);
}
/***************************************************************************/
/*                                                                         */
/*                            free_test_form                              */
/*                                                                         */
/***************************************************************************/
void free_test_form(FD_test_form *form)
{
  if (form == NULL) return;
  if (form->test_form) {
	if (form->test_form->visible)
	  fl_hide_form(form->test_form);
	fl_free_form(form->test_form);
  }
  free(form);
}
/***************************************************************************/
/*                                                                         */
/*                            free_logfile_form                              */
/*                                                                         */
/***************************************************************************/
void free_logfile_form(FD_logfile_form *form)
{
  if (form == NULL) return;
  if (form->vdata)
	free_get_input_form(form->vdata);

  if (form->logfile_form) {
	if (form->logfile_form->visible)
	  fl_hide_form(form->logfile_form);
	fl_free_form(form->logfile_form);
  }
  free(form);
}


/***************************************************************************/
/*                                                                         */
/*                            free_msg_form                              */
/*                                                                         */
/***************************************************************************/
void free_msg_form(FD_msg_form *form)
{
  if (form == NULL) return;
  if (form->msg_form) {
	if (form->msg_form->visible)
	  fl_hide_form(form->msg_form);
	fl_free_form(form->msg_form);
  }
  free(form);
}


/***************************************************************************/
/*                                                                         */
/*                            free_host_info                              */
/*                                                                         */
/***************************************************************************/
void free_host_info(struct host_info_rec *info) 
{
  int i;

  if (info == NULL) return;
  for (i = 0; i < MAX_PS_DATA_TYPES;i++) {
	free_ps_info_rec(info->ps_info[i]);
  }
  info->next = NULL;
  free(info);
}
/***************************************************************************/
/*                                                                         */
/*                            free_chart_rec                               */
/*                                                                         */
/***************************************************************************/
void free_chart_rec (struct chart_rec *chart_item)
{
  /* this routine does not free logfile list since logfile is pointed
   * to item from other list.  chart, llabel_obj, blabel_obj, and log_num_obj
   * are freed when freeing form.
   */

  if (chart_item == NULL) return;
  free(chart_item);
  
} /* free_chart_rec */
/***************************************************************************/
/*                                                                         */
/*                            free_status_form                              */
/*                                                                         */
/***************************************************************************/
void free_status_form(FD_status_form *form)
{

  if (form == NULL) return;
  if (form->status_form) {
	if (form->status_form->visible)
	  fl_hide_form(form->status_form);
	fl_free_form(form->status_form);
  }
  free(form);

} /* free_status_form */
/***************************************************************************/
/*                                                                         */
/*                            free_gstatus_rec                              */
/*                                                                         */
/***************************************************************************/
void free_gstatus_rec(struct graphic_status_rec *gstat)
{
  /* free all member
   */
  int i;
  struct logfile_rec *logfile, *tmp_logfile;

  if (gstat == NULL) return;
  free_status_form(gstat->sform);
  for (i = 0; i < MAX_CHARTS; i++) {
	free_chart_rec(gstat->chart_list[i]);
  }
  logfile = gstat->logfiles;
  while (logfile) {
	tmp_logfile = logfile;
	logfile = logfile->next;
	free_logfile_rec(tmp_logfile);
  }
  if (gstat->gms_sorted_logfiles_array) free(gstat->gms_sorted_logfiles_array);
  if (gstat->bar_sorted_logfiles_array)  free(gstat->bar_sorted_logfiles_array);

  free(gstat);
}
/***************************************************************************/
/*                                                                         */
/*                            free_array_of_str                              */
/*                                                                         */
/***************************************************************************/
void free_array_of_str(char *array[],int nitems) 
{
  int i = 0;

  while (i < nitems && array[i]) {
	free(array[i]);
	array[i] = NULL;
	i++;
  }

}
/***************************************************************************/
/*                                                                         */
/*                                free_group_form                          */
/*                                                                         */
/***************************************************************************/
void free_group_form(FD_group_form *form)
{
  if (form == NULL) return;
  if (form->group_form) {
	if (form->group_form->visible)
	  fl_hide_form(form->group_form);
	fl_free_form(form->group_form);;
  }
  free(form);
} /* free_group_form */


void free_add_del_show_form(FD_add_del_show_form *form)
{
  if (form == NULL) return;
  if (form->add_del_show_form) {
	if (form->add_del_show_form->visible)
	  fl_hide_form(form->add_del_show_form);
	fl_free_form(form->add_del_show_form);
  }
  free(form);
} 

/***************************************************************************/
/*                                                                         */
/*                            exit_system                              */
/*                                                                         */
/***************************************************************************/
void exit_system(FL_OBJECT *obj, long value)
{
  /* Cleanup and exit.
   * Special cases: value: == -2: clean up memory, close forms, end xform
   *                otherwise send del_gui msg to monitorps and 
   *                call exit_system(NULL, -2);
   */
/*
  extern int ps_info_pipe[];
*/
  extern FILE *test_file;
  char msg[MAX_MSG_LEN];
  long old_mask;
  static int cleaned_forms = 0;
  static int told_monitorps = 0;
  
  old_mask = gms_block_ints();
  /*
fprintf(stderr, "exitsystem\n");
*/

  /* Ignore child's termnination */
  signal(SIGCLD, SIG_IGN);
  if (value != -2) {
	if (told_monitorps == 0) {
	  memset(msg, '\0', MAX_MSG_LEN);
	  /* send del gui to remove it from monitorps' list */
	  sprintf(msg, "%s %s.%d", DEL_GUI_STR, local_host, gms_portnum);
	  if (send_msg(local_host, msg) < 0)
		fprintf(stderr, "Error: Can't tell monitorps <%s>\n", msg);
	  told_monitorps = 1;
	}

	/* tell child to exit */

	kill(cpid, SIGTERM);
	exit_system(NULL, -2);
  }
  else 	{
	/* value == -2 */
	if (cleaned_forms == 0) {
	

	  /* free all forms: can only free forms once */
	  cleaned_forms = 1;
#if 0
	  free_ps_form(ps_form);
	  
	  free_report_form(summary_report_form);
	
	  free_logfile_form(logfile_report_form);
	  free_logfile_form(job_grouping_form);
	  free_msg_form(system_status_form);
	  free_msg_form(all_data_form);
	  free_msg_form(exceeded_threshold_form);
	  free_test_form(test_form);
	  free_product_grouping_form(product_grouping_form);
	  free_group_form(tape_grouping_form);
	  free_add_del_show_form(neighbors_form);
	  free_add_del_show_form(admin_form);
	  free_add_del_show_form(progs_form);
	  free_select_item_form(select_host_for_ps_form);
	  clean_all_data();
#endif	  

	  if (write_to_file ) {
		/* close test file */
		fclose(test_file);
	  }

	  end_xform();
	}
	kill(0, SIGTERM);  /* Kill other child process -- created when sending
					   * command to monitorps */
  }
  exit(value);
} /* exit_system */

/***************************************************************************/
/*                                                                         */
/*                            do_clear_input                              */
/*                                                                         */
/***************************************************************************/
void do_clear_input(FL_OBJECT *button, long val)
{
  long old_mask;

  old_mask = gms_block_ints();

  if (button == NULL || button->u_vdata == NULL) {
	gms_unblock_ints(old_mask);
	return;
  }
  fl_set_input(button->u_vdata, "");
  gms_unblock_ints(old_mask);
}

/***************************************************************************/
/*                                                                         */
/*                            do_select_item_from_browser                              */
/*                                                                         */
/***************************************************************************/
void do_select_item_from_browser(FL_OBJECT *browser, long form_id)
{
  long old_mask;
  FD_add_del_show_form *form1;
  FD_gv_levels_form *form2;
  char *item;

  old_mask = gms_block_ints();

  if (browser == NULL || browser->u_vdata == NULL) goto DONE;

  if ((item = (char *)fl_get_browser_line(browser, fl_get_browser(browser))) == NULL) 
	goto DONE;

  /* Remove color code */
  if (*item == '@' && strchr(item, ' ') != NULL) {
	item = strchr(item, ' ');
	item++;
  }
  switch (form_id) {
  case ADD_DELETE_SHOW_FORM_ID:

	form1 = browser->u_vdata;
	if (form1->input == NULL) goto DONE;

	fl_set_input(form1->input, item);
	break;
  case GV_LEVELS_FORM_ID:
	form2 = browser->u_vdata;
	if (form2->program_input == NULL) goto DONE;
	/* Just show program if it's 'level program'. */
	if (strrchr(item, ' ') != NULL)
	  item = strrchr(item, ' ') + 1;
	fl_set_input(form2->program_input, item);
	break;
  }
  DONE:
  gms_unblock_ints(old_mask);

}

/***************************************************************************/
/*                                                                         */
/*                            do_clear_selected_items                              */
/*                                                                         */
/***************************************************************************/
void do_clear_selected_items(FL_OBJECT *button, long form_id)
{
  /* form_id = 0: any type
   */

  /* unselect items from browser */
  long old_mask;
  FL_OBJECT *browser;

  old_mask = gms_block_ints();
  
  if (button == NULL || button->u_vdata == NULL) {
	goto DONE;
  }

  switch (form_id) {
  default:
	browser = button->u_vdata;
	break;
  }
  fl_deselect_browser(browser);
 DONE:
  gms_unblock_ints(old_mask);
}
/***************************************************************************/
/*                                                                         */
/*                          get_chart_index                                */
/*                                                                         */
/***************************************************************************/
int get_chart_index (struct graphic_status_rec *gstat,
									 struct logfile_rec *logfile)
{
  int i;

  if (gstat == NULL || logfile == NULL) return -1;

  for (i = 0; i < MAX_CHARTS; i++) {
	if (gstat->chart_list[i] && gstat->chart_list[i]->logfile &&
		strcmp(gstat->chart_list[i]->logfile->name, logfile->name) == 0)
	  return i;
  }
  return -1;
}

/***************************************************************************/
/*                                                                         */
/*                             remove_logfile_from_chart_list               */
/*                                                                         */
/***************************************************************************/
void  remove_logfile_from_chart_list(struct graphic_status_rec *gstat,
									 struct logfile_rec *rem_logfile,
									 struct logfile_rec *next_logfile)
{
  /* If logfile is in the chart list (being displayed), remove it
   * from the chart list and refresh the window.  This routine assumes that
   * logfile hasnot been removed from the logfile list yet.
   */
  int cindex, i;
  struct logfile_rec *logfile, *tmplogfile;

  if (gstat == NULL || rem_logfile == NULL) return;
  cindex = get_chart_index(gstat, rem_logfile);

  /* if logfile not in chart list, return */
  if (cindex < 0 || cindex >= MAX_CHARTS) return;
  logfile = rem_logfile;
  tmplogfile = next_logfile;
  for (i = cindex; i < MAX_CHARTS; i++) {
	gstat->chart_list[i]->logfile = tmplogfile;
	logfile = tmplogfile;
	tmplogfile = get_next_logfile(logfile);
  }
} /* remove_logfile_from_chart_list */

/***************************************************************************/
/*                                                                         */
/*                            free_tape_group                              */
/*                                                                         */
/***************************************************************************/
void free_tape_group(struct tape_grouping_rec *grp)
{
  if (grp == NULL) 
	return;
  
  grp->last_logfile = NULL;
  grp->next = NULL;
  free(grp);
}
/***************************************************************************/
/*                                                                         */
/*                            free_all_tape_groups                          */
/*                                                                         */
/***************************************************************************/
void free_all_tape_groups()
{
  struct tape_grouping_rec *grp, *tmp_grp;
  
  grp = tape_groups;
  while (grp) {
	tmp_grp = grp;
	grp = grp->next;
	tmp_grp->next = NULL;
	free_tape_group(grp);
	grp = NULL;
  }
  tape_groups = NULL;

} /* free_all_tape_groups */

/****************************************************************************/
/*                                                                          */
/*                         reload_last_logfile_for_tape_groups               */
/*                                                                          */
/****************************************************************************/
void reload_last_logfile_for_tape_groups ()
{
  struct tape_grouping_rec *grp, *tmp_grp = NULL;
  struct logfile_rec *last_logfile;

  if (gstatus_info == NULL) return;

  if (gstatus_info->logfiles == NULL) 
	free_all_tape_groups();

  grp = tape_groups;
  while (grp) {
	/* update tape_groups list */
	last_logfile = get_last_updated_logfile(gstatus_info->logfiles,
											 grp->host, grp->tapeid);
	if (last_logfile == NULL) {
	  /* remove this group from list */
	  if (tmp_grp == NULL) {
		/*  remove the first group from list */
		tape_groups = grp->next;
		grp->next = NULL;
		free_tape_group(grp);
		grp = tape_groups;
		tmp_grp = NULL;
		continue;
	  }
	  else {
		tmp_grp->next = grp->next;
		grp->next = NULL;
		free_tape_group(grp);
		grp = tmp_grp->next;
		continue;
	  }
	}

	grp->last_logfile = last_logfile;
	tmp_grp = grp;
	grp = grp->next;
  }
  
} /* reload_last_logfile_for_tape_groups */

void update_tape_grouping_stat_affected_by_logfile_deletion()
{
  struct tape_grouping_rec *grp;
  grp = tape_groups;

  while (grp != NULL) {
    get_tape_grouping_stat(grp->host, grp->tapeid, &(grp->stat), 
								  &(grp->finished), &(grp->active));
	grp = grp->next;
  }

}/*	update_tape_grouping_stat_affected_by_logfile_deletion */

/***************************************************************************/
/*                                                                         */
/*                            remove_logfile_recs(                         */
/*                                                                         */
/***************************************************************************/
int remove_logfile_recs(struct logfile_rec **logfile_list, char *s, 
						time_t stime)
{
  /* remove logfile, from logfile list  that contains s in its file name and
   * has the same stime if stime is not 0.
   *
   * s = "$hostname.$tapeid.$prodname.log"|
   *     "$hostname" | "$tapeid" | "$prodname" 
   * Returns the number of record removed; 0 otherwise.
   */
  int removed_rec_count = 0;
  struct logfile_rec *logfile, *tmplog, *nextlog=NULL;
  char tmp_name[MAX_FILENAME_LEN];

  if (s == NULL || strlen(s) == 0) 
	return removed_rec_count;

  logfile = *logfile_list;
  while (logfile != NULL) {

	/* if logfile name contains string s, remove it from logfile list
	 * and remove it from chart list if needed. */

	memset(tmp_name, '\0', MAX_FILENAME_LEN);
	create_logfile_name(logfile->fromhost, logfile->tapeid, logfile->prog,
						tmp_name);
	/*
fprintf(stderr, "*stime: %d, ltime: %d\n", (int) stime, (int)logfile->stime_item);
*/

	if (strstr(tmp_name, s) != NULL &&
		(stime == (time_t) 0 || (stime > (time_t) 0 && 
								 logfile->stime_item == stime))) {
	  /*
fprintf(stderr, "stime: %d, ltime: %d\n", (int) stime, (int)logfile->stime_item);
*/
	  /* remove logfile from log list */
	  removed_rec_count++;

	  if (logfile->prev == NULL)  {
		*(logfile_list) = logfile->next;
		if (*logfile_list)
		  (*logfile_list)->prev = NULL;
		nextlog = *logfile_list;
	  }
	  else {
		(logfile->prev)->next = logfile->next;
		nextlog = logfile->next;
		if (logfile->next) 
		  (logfile->next)->prev = logfile->prev;
	  }


	  tmplog = logfile;
	  logfile = nextlog;
	  tmplog->next = NULL;
	  tmplog->prev = NULL;

	  free_logfile_rec(tmplog);
	  tmplog = NULL;

	  continue;
	}

	logfile = logfile->next;
  }

  if (removed_rec_count > 0) {
	reload_last_logfile_for_tape_groups ();
	update_tape_grouping_stat_affected_by_logfile_deletion();
  }
  return removed_rec_count;
} /* remove_logfile_recs */

/****************************************************************************/
/*                                                                          */
/*                         reload_data_to_status_form              */
/*                                                                          */
/****************************************************************************/
void reload_data_to_status_form()
{
  int i;

  if (gstatus_info == NULL || gstatus_info->sform == NULL) return;
  
  /* reset charts to point to NULL */
  for (i = 0; i < MAX_CHARTS; i++) {
	if (gstatus_info->chart_list[i] && gstatus_info->chart_list[i]->logfile) {
	  /* clear bar & labels on form */
	  clear_chart(gstatus_info->chart_list[i]);
	  gstatus_info->chart_list[i]->logfile = NULL;
	}
  }
  gstatus_info->refresh_form = 1;
  /* update  form keeping the same scroll position */
  update_status_form(gstatus_info->scroll_per, 1);

} /* reload_data_to_status_form */


/***************************************************************************/
/*                                                                         */
/*                               remove_log_info                           */
/*                                                                         */
/***************************************************************************/
int remove_log_info(struct tape_info_rec **tape_list, char *str,
					time_t stime)
{


  /* remove log info whose whole or sub name matches with str
   * from tape_list.  Match sdate and stime too if they are not NULL. 
   */
/*
 * str is '$hostname.$tapeid.$productname.log'|
 *         "$hostname" | "$tapeid" | "$prodname" 
 */
  int removed = 0;
  struct tape_info_rec *tmp_tape_info, *tape_info, *last_tape_info;
  int i;

  if (str == NULL || strlen(str) == 0) return 0;

  tape_info = *tape_list; 
  last_tape_info = NULL;
  while (tape_info) {
	/* remove tape info if str matched with tapeid */
	if (strcmp(str, tape_info->id) == 0 ||
		(strlen(str) == MAX_TAPEID_LEN-1 && 
		 strstr(tape_info->id, str) != NULL)) {
	  tmp_tape_info = tape_info->next;
	  tape_info->next = NULL;
	  free_tape_info(tape_info);
	  tape_info = tmp_tape_info;
	  if (last_tape_info == NULL)
		*tape_list = tmp_tape_info;
	  removed = 1;
	  goto NEXT;
	}

	/* remove logfile recs where str != "$tapeid" */
	for (i = 0; i < MAX_PRODS; i++) {
	  if (tape_info->logfiles[i] == NULL) continue;
	  if (remove_logfile_recs(&(tape_info->logfiles[i]), str,
							  stime) > 0) {
		removed = 1;
	  }
	}
  NEXT:
	last_tape_info = tape_info;
	tape_info = tape_info->next;
  }
  return removed;
} /* remove_log_info */

/****************************************************************************/
/*                                                                          */
/*                              logfile_exist_for_tape                       */
/*                                                                          */
/****************************************************************************/
int logfile_exist_for_tape(struct logfile_rec *logfiles,
										   char *s)
{
  /* Returns 1 if there is any logfile having the specified tapeid and 
   * host from s; 0 otherwise.
   *    s = "$hostname.$tapeid.$prodname.log".
   */
  char tapeid[MAX_TAPEID_LEN], host[MAX_HOSTNAME_LEN];
  struct logfile_rec *log;

  if (logfiles == NULL) return 0;
  memset(tapeid, '\0', MAX_TAPEID_LEN);
  memset(host, '\0', MAX_HOSTNAME_LEN);
  extract_info_from_filename(s, host, tapeid, NULL);

  log = logfiles;

  while (log) {
	
	if ((strcmp(tapeid, log->tapeid) == 0 ||
		 (strlen(tapeid) == MAX_TAPEID_LEN-1 && 
		  strstr(log->tapeid, tapeid) != NULL))
		&& 
		strcmp(host, log->fromhost) == 0 && log->nentries > 0) {
	  return 1;
	}
	log = log->next;
	  
  }
  return 0;

} /* logfile_exist_for_tape */

/****************************************************************************/
/*                                                                          */
/*                             remove_info_for_selected_items               */
/*                                                                          */
/****************************************************************************/
int remove_info_for_selected_items(item_type_t type, char *selected_items[], 
								   int max_items, void *data1, char *data2)
{
  /* If type is not TAPE_GROUP, remove status info whose whole or sub name
   * matches with selected_items 
   * from gstatus_info's logfile list.  Match sdate and stime too if they 
   * are not NULL. Remove tape grouping  too if it becomes empty.
   * 
   * If type is TAPE_GROUP, remove tape grouping from tape_groupings list.
   * Remove all of its logfiles from gstatus_info's logfile list too.
   */
/*
 * Each 'selected_items[]' is '$hostname.$tapeid.$productname.log' |
 *                            "$hostname" | "$tapeid" | "$prodname" 
 */
  int i;
  int removed_rec_count = 0;
  time_t *stime, dtime = (time_t) 0;

  if (gstatus_info == NULL) return 0;
  if (data1 == NULL) 
	stime  = &dtime;
  else {
	stime = (time_t *) data1;
  }
  for (i = 0; i < max_items; i++) {
	  if (selected_items[i] == NULL) break;
	switch (type) {
	case TAPE_GROUP:

	  /* Remove logfiles for this tape group from gstatus_info's logfile list
	   */


	  if ((removed_rec_count = remove_logfile_recs(&(gstatus_info->logfiles), 
												   selected_items[i], *stime)) > 0) {
		gstatus_info->nlogfiles -= removed_rec_count;
		gstatus_info->refresh_form = 1;

		if (gstatus_info->nlogfiles == 0) {
		  gstatus_info->logfiles = NULL;
		  goto DONE;
		}
	  }
	  break;
	case STAT_INFO:

	  if ((removed_rec_count = remove_logfile_recs(&(gstatus_info->logfiles), 
												   selected_items[i], *stime))) {
		gstatus_info->nlogfiles -= removed_rec_count;
		gstatus_info->refresh_form = 1;

		if (gstatus_info->nlogfiles == 0) {
		  gstatus_info->logfiles= NULL;
		  /* No more logfile for this tape group--remove it from list */
		  remove_tape_group_from_list(&(tape_groups), selected_items[i]);
		  goto DONE;
		}
		else {
		  if (logfile_exist_for_tape(gstatus_info->logfiles,
									 selected_items[i]) == 0) {
			/* No more logfile for this tape group--remove it from list */
			remove_tape_group_from_list(&(tape_groups), selected_items[i]);
		  }
		}
	  }
	  break;
	case LOG_INFO:
	  if ((removed_rec_count = remove_log_info(&tape_info_list, 
											   selected_items[i], *stime))) {
	  }
	  break;
	default:
	  break;
	}
  }
DONE:
  if (type == TAPE_GROUP) {
	/* Remove tape grouping from tape groupings list.
	 * Second arguments must contain host and tapeid.
	 # Assuming that selected_items belong to the same tape.
	 */
	remove_tape_group_from_list(&(tape_groups), selected_items[0]);
  }

  return removed_rec_count;

} /* remove_info_for_selected_items */



/****************************************************************************/
/*                                                                          */
/*                         get_selected_items              */
/*                                                                          */
/****************************************************************************/
void get_selected_items(FL_OBJECT *browser, char *selected_items[], 
						item_type_t type,
						int max_items, int max_item_len)
{
  /* read selected items from browser to selected_items[] */

  int i, j;
  char *item;
  char prod[MAX_PRODNAME_LEN];
  char tapeid[MAX_TAPEID_LEN];

  if (browser == NULL) return;

  /* initialize list */
  for (i = 0; i < max_items;i++) {
	selected_items[i] = (char *)NULL;
  }

  for (i = 1,j = 0; i <= fl_get_browser_maxline(browser); i++) {
	if (fl_isselected_browser_line(browser, i)) {

	  item = (char *) fl_get_browser_line(browser, i);
	  selected_items[j] = (char *) calloc(1, max_item_len);
	  if (selected_items[j] == NULL) {
		perror ("calloc item");
		exit_system(NULL, -1);
	  }
	  switch (type) {
	  case TAPEID:
		/* get tapeid from string:
		 *	  sdate.stime edate.etime tapeid  tape_dev 
		 **/
		sscanf(item, "%*s %*s %*s %s %*s", selected_items[j]);
		break;
	  case PROD:
		/* get product name from str:
		 *	  control_str sdate stime edate etime tapeid  tape_dev prod
		 */
		sscanf(item, "%*s %*s %*s %*s %*s %*s %*s %s", selected_items[j]);
		break;
	  case LOGFILE_NAME:
		/* get tapeid & prod from str:
		 *	  sdate stime edate etime tapeid  tape_dev prod
		 * and form logfile name 
		 */
		memset(tapeid, '\0', MAX_TAPEID_LEN);
		memset(prod, '\0', MAX_PRODNAME_LEN);
		sscanf(item, "%*s %*s %*s %*s %*s %s %*s %s", tapeid, prod);
		sprintf(selected_items[j], "%s.%s.log", tapeid, prod);
		break;
	  default:
		sprintf(selected_items[j],"%s",item);

		break;
	  }
	  selected_items[j][max_item_len-1] = '\0';
	  j++;
	}
  }
} /* get_selected_items */

/****************************************************************************/
/*                                                                          */
/*                               host_exists                                */
/*                                                                          */
/****************************************************************************/
int host_exists(char *list[MAX_HOSTS], char *host)
{
  /* return 1 if host exists in list */
  int i;
 
  if (host == NULL || strlen(host) == 0) return 0;
  for (i = 0; i < MAX_HOSTS; i++) {
	if (list[i] == NULL) continue;
	if (strncmp(list[i], host, MAX_HOSTNAME_LEN) == 0) {
	  return 1;
	}
  }
  return 0;
}

/****************************************************************************/
/*                                                                          */
/*                                 host_info_exists                         */
/*                                                                          */
/****************************************************************************/
int host_info_exists(char *host)
{
  struct host_info_rec *item;
  
  if (host == NULL || strlen(host) == 0) return 0;
  item = host_info_list;
  while(item) {
	if (strcmp(item->name, host) == 0)
	  return 1;
	item = item->next;
  }
  return 0;
}

/****************************************************************************/
/*                                                                          */
/*                                   new_host_info                          */
/*                                                                          */
/****************************************************************************/

struct host_info_rec *new_host_info( char *host)
{
  struct host_info_rec *item;
  int i;

  if (host == NULL || strlen(host) == 0) return NULL;

  /* create new host  info record */
  item = (struct host_info_rec *) calloc(1, sizeof(struct host_info_rec));
  if (item == NULL) {
	perror("calloc host info rec");
	exit_system(NULL, -1);
  }
  
  strncpy(item->name, host, MAX_HOSTNAME_LEN);
  item->name[MAX_HOSTNAME_LEN-1] = '\0';

  for (i = 0; i < MAX_PS_DATA_TYPES; i++) {
	item->ps_info[i] = new_ps_info_rec(MAX_PS_BUF_LEN);
	if (item->ps_info[i] == NULL) {
	  fprintf(stderr, "item->ps_info[%d] is nULL\n", i);
	  exit_system(NULL, -1);
	}
  }
  item->diff_time = (double) 0;
  return item;
} /* new_host_info */

/****************************************************************************/
/*                                                                          */
/*                         add_host_info              */
/*                                                                          */
/****************************************************************************/
void add_host_info(char *host)
{
  extern int form_atclose(struct forms_ *form, void *value);
  /* check if there is a form created for this host */
  struct host_info_rec *item;

  if (host == NULL || strlen(host) == 0) return;
  if (host_info_exists(host)) return;

  item = new_host_info(host);
	
  if (item == NULL) {
	fprintf(stderr, "new host info  is null\n");
	exit_system(NULL, -1);
  }

  /* add at the front of list */
  item->next = host_info_list;
  host_info_list = item;

}


/****************************************************************************/
/*                                                                          */
/*                         add_host_to_internal_lists              */
/*                                                                          */
/****************************************************************************/
void add_host_to_internal_lists(char *host)
{

  /* create host_info record and add to list for this new host */
  add_host_info(host);
}


/****************************************************************************/
/*                                                                          */
/*                                 add_new_host                             */
/*                                                                          */
/****************************************************************************/
int add_new_host(char *list[MAX_HOSTS], char *host)
{
  /* add host to list if it's not already in list */

  int i;

  if (host == NULL || strlen(host) == 0 || host_exists(list, host)) 
	return 0;
  for (i = 0; i < MAX_HOSTS; i++) {
	if (list[i] != NULL) continue;
	
	list[i] = (char *) calloc(1, MAX_HOSTNAME_LEN);
	if (list[i] == NULL) {
	  perror("calloc host");
	  exit_system(NULL, -1); 
	}
	strncpy(list[i], host, MAX_HOSTNAME_LEN - 1);
	break;
  }
  if (i >= MAX_HOSTNAME_LEN) {
	fprintf(stderr, "Error: too many host. host <%s> is discarded\n", host);
	return 0;
  }

  add_host_to_internal_lists(host);

  return 1;
}

/****************************************************************************/
/*                                                                          */
/*                                   add_new_prog                           */
/*                                                                          */
/****************************************************************************/
int add_new_prog(char *list[MAX_PROGS], char *prog)
{
  /* add prog to particular list, depending on type, if it's not already 
   * in list 
   */
  int i = 0;

  if (prog == NULL || strlen(prog) == 0 || 
	  prog_exists(list, prog)) return 0;

  for (i = 0; i < MAX_PROGS; i++) {
	if (list[i] != NULL) continue;
	  
	list[i] = (char *) calloc(1, MAX_PROGNAME_LEN);
	if (list[i] == NULL) {
	  perror("calloc prog");
	  exit_system(NULL, -1);
	}
	strncpy(list[i], prog, MAX_PROGNAME_LEN-1);
	break;
  }
  return 1;
}
/****************************************************************************/
/*                                                                          */
/*                                  prog_exists                             */
/*                                                                          */
/****************************************************************************/

int prog_exists( char *list[MAX_PROGS], char *prog)
{
  int i = 0;

  if (prog == NULL || strlen(prog) == 0) return 0;
  for (i = 0; i < MAX_PROGS; i++) {
	if (list[i] == NULL) continue;
	if (strncmp(list[i], prog, MAX_PROGNAME_LEN-1) == 0) return 1;
  }
	 
  return 0;
}



/****************************************************************************/
/*                                                                          */
/*                         remove_prog              */
/*                                                                          */
/****************************************************************************/
void remove_prog(char *prog)
{
  /* remove prog from user-selected list */
  int i;
  int removed_count;

  if (prog == NULL || strlen(prog) == 0) return;

  for (i = 0; i < MAX_PROGS; i++) {
	if (Gselected_progs[i] && strcmp(Gselected_progs[i], prog) == 0){
	  free(Gselected_progs[i]);
	  Gselected_progs[i] = NULL;

	  /* remove data for this prog from other lists */
	  if (gstatus_info) {
		removed_count = remove_logfile_recs(&(gstatus_info->logfiles), prog,0);
		if (removed_count > 0) {
		  gstatus_info->nlogfiles -= removed_count;
		  if (gstatus_info->nlogfiles == 0)
			gstatus_info->logfiles = NULL;

		  reload_data_to_status_form();
		  gstatus_info->refresh_form = 1;
		}
	  }
	  break;
	}
  }
}

/****************************************************************************/
/*                                                                          */
/*                          remove_host_info              */
/*                                                                          */
/****************************************************************************/
void  remove_host_info(char *host)
{
  struct host_info_rec *item, *tmpitem = NULL;

  if (host == NULL || strlen(host) == 0) return;

  item = host_info_list;
  if (item == NULL) return;

  if (strcmp(item->name, host) == 0) {
	host_info_list = item->next;
	free_host_info(item);
	return;
  }
  while (item) {
	if (strcmp(item->name, host) == 0) {
	  tmpitem->next = item->next;
	  free_host_info(item);
	  break;
	}
	else {
	  tmpitem = item;
	  item = item->next;
	}
  }
}


/****************************************************************************/
/*                                                                          */
/*                         remove_host              */
/*                                                                          */
/****************************************************************************/
void remove_host(char *host)
{
  /* remove host from user-selected list. Remove data belonging to this host 
   * from all forms and lists. 
   */
  int i, j;
  struct host_info_rec *host_info;
  int removed_count;

  if (host == NULL || strlen(host) == 0) return;  

  for (i = 0; i < MAX_HOSTS; i++) {
	if (Gselected_hosts[i] == NULL) continue;
	if (strcmp(Gselected_hosts[i], host) == 0){
	  free(Gselected_hosts[i]); /* only remove host from hosts list. This list
							 * is used to determine whether to display info
							 * coming from it or not. 
							 */
	  Gselected_hosts[i] = NULL;
	  host_info = get_host_info(host);
	  if (host_info != NULL) {
		/* clear ps_bufs */
		for (j = 0; j < MAX_PS_DATA_TYPES; j++) {
		  if (host_info->ps_info[j] == NULL) continue;
		  memset(host_info->ps_info[j]->top_level, '\0', MAX_PS_BUF_LEN);
		  memset(host_info->ps_info[j]->all_levels, '\0', MAX_PS_BUF_LEN);
		}
	  }

#if 0
	  curr_time = get_curr_time();
	  /* add message to system_status_form */
	  add_msg_to_sys_stat_browser(SENT_STR, host, DEL_NEIGHBOR,  
								  strlen(msg), (long) curr_time );
#endif
	  /* remove data from this host from other lists */
	  if (gstatus_info) {
		removed_count = remove_logfile_recs(&(gstatus_info->logfiles), host, 0);
		if (removed_count > 0) {
		  gstatus_info->nlogfiles -= removed_count;
		  if (gstatus_info->nlogfiles == 0) 
			gstatus_info->logfiles = NULL;
		  reload_data_to_status_form();
		  gstatus_info->refresh_form = 1;
		}
	  }
	  remove_host_info(host);
		
	  break;
	}
  }
}


/****************************************************************************/
/*                                                                          */
/*                          update_all_data_form              */
/*                                                                          */
/****************************************************************************/
void  update_all_data_form()
{

  if (all_data_form == NULL || all_data_form->msg_form == NULL ||
	  !all_data_form->msg_form->visible || all_data_form->browser == NULL) 
	return;
  fl_deactivate_object(all_data_form->browser);
  load_items_to_browser(all_data_form->msg_form, 
						all_data_form->browser, ALL_DATA, "", CLEAR);
  fl_activate_object(all_data_form->browser);

} /* update_all_data_form */


/****************************************************************************/
/*                                                                          */
/*                         update_forms_affected_by_neighbor_change              */
/*                                                                          */
/****************************************************************************/
void update_forms_affected_by_neighbor_change(item_type_t type, 
											  int neighbor_removed)
{
  int reload_form = 0;

  switch (type) {
  case SELECTED_HOSTS:
	if (neighbors_form) {
	  /* reflect the change in the neighbor form */
	  load_items_to_browser(neighbors_form->add_del_show_form, 
							neighbors_form->selected_browser, 
							SELECTED_HOSTS, "", CLEAR);
	  load_items_to_browser(neighbors_form->add_del_show_form, 
							neighbors_form->available_browser, 
							AVAILABLE_HOSTS, "", CLEAR);
	}
	if (neighbor_removed) {
	  reload_form = 1;
	  update_forms_affected_by_status_info_change(reload_form);
	  update_forms_affected_by_logfile_change(reload_form);
	}

	if (select_host_for_ps_form) {
	  /* reflect the change in the select host for ps form */
	  load_items_to_browser(select_host_for_ps_form->select_item_form, 
				select_host_for_ps_form->browser, SELECTED_HOSTS, "", CLEAR);
	}
	if (admin_form) {
	  /* reflect the change in the admin form */
	  load_items_to_browser(admin_form->add_del_show_form, 
							admin_form->available_browser, 
						    AVAILABLE_NETWORK_HOSTS, "", CLEAR);
	}
	break;
  case AVAILABLE_HOSTS:
	if (neighbors_form) {
	  load_items_to_browser(neighbors_form->add_del_show_form, 
							neighbors_form->available_browser, 
							AVAILABLE_HOSTS, "", CLEAR);
	}
	if (select_host_for_ps_form) {
	  /* reflect the change in the select host for ps form */
	  load_items_to_browser(select_host_for_ps_form->select_item_form, 
				select_host_for_ps_form->browser, SELECTED_HOSTS, "", CLEAR);
	}
	if (admin_form) {
	  /* reflect the change in the admin form */
	  load_items_to_browser(admin_form->add_del_show_form, 
							admin_form->selected_browser, 
						    SELECTED_NETWORK_HOSTS, "", CLEAR);
	  /* reflect the change in the admin form */
	  load_items_to_browser(admin_form->add_del_show_form, 
							admin_form->available_browser, 
						    AVAILABLE_NETWORK_HOSTS, "", CLEAR);

	}
	break;
  default:
	break;
  }

} /* update_forms_affected_by_neighbor_change */

/****************************************************************************/
/*                                                                          */
/*                    update_forms_affected_by_network_host_change          */
/*                                                                          */
/****************************************************************************/
void update_forms_affected_by_network_host_change()
{
  update_forms_affected_by_neighbor_change (AVAILABLE_HOSTS, 1);
#if 0
  if (admin_form) {
	/* reflect the change in the admin form */
	load_items_to_browser(admin_form->add_del_show_form, 
						  admin_form->selected_browser, 
						  AVAILABLE_NETWORK_HOSTS, "", CLEAR);
  }
#endif
} /* update_forms_affected_by_network_host_change */

/****************************************************************************/
/*                                                                          */
/*                         update_forms_affected_by_prog_change              */
/*                                                                          */
/****************************************************************************/
void update_forms_affected_by_prog_change(item_type_t type, int prog_removed)
{
  int reload_form = 0;

  switch (type) {
  case SELECTED_PROGS:
	if (progs_form) {
	  /* reflect the change in the prog form */
	  load_items_to_browser(progs_form->add_del_show_form, 
				progs_form->selected_browser, SELECTED_PROGS, "", CLEAR);

	  load_items_to_browser(progs_form->add_del_show_form, 
				progs_form->available_browser, AVAILABLE_PROGS, "", CLEAR);
	}

	if (prog_removed)  {
	  reload_form = 1;
	  update_forms_affected_by_status_info_change(reload_form);
	  update_forms_affected_by_logfile_change(reload_form);
	}
	break;
  case AVAILABLE_PROGS:
	if (progs_form) {
	  /* reflect the change in the prog form */
	  load_items_to_browser(progs_form->add_del_show_form, 
				progs_form->available_browser, AVAILABLE_PROGS, "", CLEAR);
	}

	break;
  default:
	break;
  }
  update_gv_levels_form();

} /* update_forms_affected_by_prog_change */

/****************************************************************************/
/*                                                                          */
/*                         update_forms_affected_by_status_info_change              */
/*                                                                          */
/****************************************************************************/
void update_forms_affected_by_status_info_change(int reload_form)
{ 
  extern char last_job_status_msg[MAX_BROWSER_LINE_LEN];



  /* Update status msg on the run grouping form if need to*/
  if (tape_grouping_form && tape_grouping_form->status_label &&
	  tape_grouping_form->status_label->label &&
	  strcmp(tape_grouping_form->status_label->label, last_job_status_msg) != 0){

	/* Hide object, set its label, and show it again -- hide and show are
	 * required here since form just writes the new label over the old one 
	 * without clearing the old one first.
	 */
	fl_hide_object(tape_grouping_form->status_label);
	fl_set_object_label(tape_grouping_form->status_label, last_job_status_msg);
	fl_show_object(tape_grouping_form->status_label);
  }

  /* on gts_monitor display */
  update_prod_group_form(reload_form);
  /* curr status' detailed form */
  update_curr_file_status_report();
  /* form contains bars */
  if (gstatus_info) {
	gstatus_info->refresh_form = 1;
	update_status_form(gstatus_info->scroll_per, reload_form);
  }
  /* form contains jobs groups status info */
  update_tape_grouping_form();

  /* form contains all data received from monitorps  */
  update_all_data_form();

  /* form contains jobs exceeded threshold */
  update_exceeded_threshold_form(NULL, 0);

  /* Form contains summary info. */
  update_summary_report_form();
} /* update_forms_affected_by_status_info_change */


/****************************************************************************/
/*                                                                          */
/*                         update_forms_affected_by_logfile_change              */
/*                                                                          */
/****************************************************************************/
void update_forms_affected_by_logfile_change(int reload_form)
{
  update_summary_report_form();
  /* detailed form */
  update_log_file_status_report();

} /*  update_forms_affected_by_logfile_change */

/****************************************************************************/
/*                                                                          */
/*                         remove_selected_items              */
/*                                                                          */
/****************************************************************************/
void remove_selected_items( 
						   char *selected_items[], 
						   item_type_t itype, int max_items)
{
  /* remove selected items from user selected host/prog list*/
  int i;

  switch (itype) {
  case SELECTED_HOSTS:
	for (i = 0; i < max_items; i++) {
	  if (selected_items[i] == NULL) continue;
	  remove_host(selected_items[i]);
	}

	break;
  case SELECTED_PROGS:
	for (i = 0; i < max_items; i++) {
	  if (selected_items[i] == NULL) continue;
	  remove_prog(selected_items[i]);
	}

	break;	
  default: 
	return;
  }
  
}


/****************************************************************************/
/*                                                                          */
/*                        delete_network_host                               */
/*                                                                          */
/****************************************************************************/
int delete_network_host(char *host)
{
  /* Removes host from available_hosts and sends delete neighbor message to
   * monitorps--even if host is not in the available_hosts list. 
   * Returns 1 if removed host from available_hosts successfully; 0, otherwise.
   */

  int i;
  int removed = 0;
  char msg[MAX_MSG_LEN];

  if (host == NULL) return 0;
  for (i = 0; i < MAX_HOSTS; i++) {
	if (available_hosts[i] == NULL) continue;
 	if (strcmp(host, available_hosts[i]) == 0) {
	  free(available_hosts[i]);
	  available_hosts[i] = NULL;
	  removed = 1;
	  break;
	}
  }
		
  /* Send msg to monitorps */
  memset(msg, '\0', MAX_MSG_LEN);
  sprintf(msg, "del neighbor %s",host);
  if (send_msg(local_host, msg) < 0)
	fprintf(stderr, "WARNING: Can't send monitorps msg: %s\n", msg);
  return removed;
} /* delete_network_host */

/****************************************************************************/
/*                                                                          */
/*                             add_network_host                             */
/*                                                                          */
/****************************************************************************/
int add_network_host(char *host) 
{
  /* Adds host to available_hosts and sends add neighbor message to 
   * monitorps.
   * Returns 1 if added to list; 0,otherwise.
   */
  int added = 0;
  char msg[MAX_MSG_LEN];

  if (host == NULL) return 0;
  if (add_new_host(available_hosts, host) > 0) added = 1;

  memset(msg, '\0', MAX_MSG_LEN);
  sprintf(msg, "add neighbor %s", host);
  if (send_msg(local_host, msg) < 0)
	fprintf(stderr, "WARNING: Can't send monitorps cmd: %s\n", msg);
  return added;
	
} /* add_network_host */


/****************************************************************************/
/*                                                                          */
/*                         update_monitoring_config                         */
/*                                                                          */
/****************************************************************************/
void update_monitoring_config(FL_OBJECT *button, long type)
{
  /* delete/add host/prog from user selected list. 
   * update forms affected by this operation.
   */
  char *item;
  FD_add_del_show_form *nform;
  long oldmask;

  oldmask = gms_block_ints();
  if (button == NULL || button->u_vdata == NULL) {
	goto DONE;
  }
  nform = (FD_add_del_show_form *) button->u_vdata;
  if (nform->input == NULL) 
	goto DONE;

  /* deactivate everything on form except cancel&clear buttons */
  if (nform->add_button)
	fl_deactivate_object(nform->add_button);
  if (nform->delete_button)
	fl_deactivate_object(nform->delete_button);
  if (nform->available_browser)
	fl_deactivate_object(nform->available_browser);
  if (nform->selected_browser) 
	fl_deactivate_object(nform->selected_browser);

  /* get item from user's input */
  item = (char *)fl_get_input(nform->input);
  /* Skip color code if any */
  if (strrchr(item, ' ') != NULL) {
	item = strrchr(item, ' ');
	item++;
  }
  
  switch ((int) type) {
  case ADD_HOST:
	if (add_new_host(Gselected_hosts, item) > 0) {
	}
	/* update other affected forms */
	update_forms_affected_by_neighbor_change(SELECTED_HOSTS, 0);
	/* write user's selected programs and neighbors to ~/.gms */
	update_gms_init_file();
	break;
  case DEL_HOST:

	/* remove host from user's selected neighbor list */
	remove_host(item);

	/* update other affected forms */
	update_forms_affected_by_neighbor_change(SELECTED_HOSTS, 1);
	/* write user's selected programs and neighbors to ~/.gms */
	update_gms_init_file();
	break;
  case ADD_PROG:
	add_new_prog(Gselected_progs, item);
	
	/* update other affected forms */
	update_forms_affected_by_prog_change(SELECTED_PROGS, 0);
	/* write user's selected programs and neighbors to ~/.gms */
	update_gms_init_file();
	break;
  case DEL_PROG:
	/* remove prog from user selected list */
	remove_prog(item);

	/* update other affected forms */
	update_forms_affected_by_prog_change(SELECTED_PROGS, 1);
	/* write user's selected programs and neighbors to ~/.gms */
	update_gms_init_file();
	break;
  case ADD_NETWORK_HOST:
#ifdef XFORMS_081
	if (fl_show_question("Warning!!!", 
						 "This request will change the network configuration.",
						 "Do you really want to continue?")) {
#else
	if (fl_show_question("Warning!!!\nThis request will change the network configuration.\nDo you really want to continue?", 0)) {
#endif
	  /* yes, continue. */
	  if (add_network_host(item) > 0) 
		update_forms_affected_by_network_host_change();
	}
	break;
  case DEL_NETWORK_HOST:
#ifdef XFORMS_081
	if (fl_show_question("Warning!!!", 
						 "This request will change the network configuration.",
						 "Do you really want to continue?")) {
#else
	if (fl_show_question("Warning!!!\nThis request will change the network configuration.\nDo you really want to continue?", 0)) {
#endif
	  /* yes, continue. */
	  if (delete_network_host(item) > 0) 
		update_forms_affected_by_network_host_change();
	}
	break;
  default:
	break;
  }

  /* clear input area */
  fl_set_input(nform->input, "");

  /* activate everything on form except cancel&clear buttons */
  if (nform->add_button)
	fl_activate_object(nform->add_button);
  if (nform->delete_button)
	fl_activate_object(nform->delete_button);
  if (nform->available_browser)
	fl_activate_object(nform->available_browser);
  if (nform->selected_browser) 
	fl_activate_object(nform->selected_browser);

 DONE:
  gms_unblock_ints(oldmask);

} /* update_monitoring_config */


/****************************************************************************/
/*                                                                          */
/*                         add_progs_to_list              */
/*                                                                          */
/****************************************************************************/
void add_progs_to_list(char *list[MAX_PROGS], char *progs[MAX_PROGS])
{
  int i;
  for (i = 0; i < MAX_PROGS; i++) {
	add_new_prog(list, progs[i]);
  }
}


/****************************************************************************/
/*                                                                          */
/*                         popup_summary_report_form              */
/*                                                                          */
/****************************************************************************/
void popup_summary_report_form(FL_OBJECT *button , long val)
{
  long oldmask;
  summary_type_t summary_type = 0;

  /* this is to prevent interrupts while  accessing internal lists */

  oldmask = gms_block_ints();
  if (show_button_info_flag)
	hide_button_info_msg();
  if (button == NULL) {
	gms_unblock_ints(oldmask);
	return;
  }

  fl_deactivate_object(summary_report_form->browser);
  if (summary_report_form == NULL || summary_report_form->report_form == NULL )  {
	fprintf(stderr, "ERROR: There is no tape report form.\n");
	exit_system(NULL, -1);
  }
  

  /* Only set pixmap if form was not visible */
  if (!summary_report_form->report_form->visible)
	set_pixmapbuttons_for_summary_report_form();
  show_form(summary_report_form->ldata);
  load_items_to_browser(summary_report_form->report_form, 
						summary_report_form->browser, SUMMARY_INFO,
						&summary_type, CLEAR);

  fl_activate_object(summary_report_form->browser);
  gms_unblock_ints(oldmask);

} /* popup_summary_report_form */

/****************************************************************************/
/*                                                                          */
/*                         popup_all_data_form              */
/*                                                                          */
/****************************************************************************/
void popup_all_data_form(FL_OBJECT *button , long val)
{
  long oldmask;
  /* this is to prevent interrupts while  accessing internal lists */

  oldmask = gms_block_ints();
  if (show_button_info_flag)
	hide_button_info_msg();  
  if (button == NULL) {
	gms_unblock_ints(oldmask);
	return;
  }
  show_form(all_data_form->ldata);
  update_all_data_form();
  gms_unblock_ints(oldmask);

} /* popup_all_data_form */

/****************************************************************************/
/*                                                                          */
/*                         popup_product_grouping_form              */
/*                                                                          */
/****************************************************************************/
void popup_product_grouping_form(FL_OBJECT *button , long val)
{
  /* val: 1 -- This routine was invoked by selecting item from browser 
   * on tape_grouping_form.
   */
  long oldmask;
  FD_group_form *grouping_form;
  FL_OBJECT *browser;
  char *selected_str = NULL;
  int item_i;

  /* this is to prevent interrupts while  accessing internal lists */

  oldmask = gms_block_ints();
  if (show_button_info_flag)
	hide_button_info_msg();  
  if (button == NULL) {
	gms_unblock_ints(oldmask);
	return;
  }


  if (product_grouping_form == NULL || product_grouping_form->gms_form == NULL )  {
	fprintf(stderr, "ERROR: There is no gts monitor form.\n");
	exit_system(NULL, -1);
  }
  /* Only set pixmaps if form is not visible */
  if (!product_grouping_form->gms_form->visible)
	set_pixmapbuttons_for_product_grouping_form();

  show_form(product_grouping_form->ldata);
  grouping_form = button->u_vdata;

  switch (val) {
  case 1:
	/* invoked from the group form's browser */
	if (grouping_form && grouping_form->group_form && grouping_form->browser) {
	  browser = grouping_form->browser;
	  fl_deactivate_object(browser);
	  fl_freeze_form(grouping_form->group_form);
	  item_i = fl_get_browser(browser);
	  strcpy(curr_selected_tape_group, "");
	  if (item_i > 0 && 
		  (selected_str = (char *) fl_get_browser_line(browser, item_i))!= NULL)
		strcpy(curr_selected_tape_group, selected_str);
	  

	  fl_deselect_browser(browser);

	  fl_unfreeze_form(grouping_form->group_form);
	  fl_activate_object(browser);

	  /* Redraw divider line */
	  if (grouping_form->divider_bar)
		fl_show_object(grouping_form->divider_bar);
	  
	  /* change view type automatically gms form. */
	  forms_view_options[GMSF_VIEW_OPTION_IND] |= VIEW_ONE_TAPE_GRPING;
	  forms_view_options[GMSF_VIEW_OPTION_IND] ^= VIEW_ALL;
	  
	  set_view_menu_options(product_grouping_form->view_menu, 
							&(forms_view_options[GMSF_VIEW_OPTION_IND]), 
							GMS_FORM_ID);
	  if (gstatus_info) {
		/* change view type and update status form automatically. */
		forms_view_options[STATUSF_VIEW_OPTION_IND] |= VIEW_ONE_TAPE_GRPING;
		forms_view_options[STATUSF_VIEW_OPTION_IND] ^= VIEW_ALL;
		set_view_menu_options(gstatus_info->sform->view_menu, 
							  &(forms_view_options[STATUSF_VIEW_OPTION_IND]),
							  STATUS_FORM_ID);

		gstatus_info->refresh_form = 1;
		update_status_form(gstatus_info->scroll_per, 1);
	  } 

	}
	break;
  default:
	  /* change view type to default setting automatically . */
	  forms_view_options[GMSF_VIEW_OPTION_IND] = VIEW_DEFAULT;
	  
	  set_view_menu_options(product_grouping_form->view_menu, 
							&(forms_view_options[GMSF_VIEW_OPTION_IND]), 
							GMS_FORM_ID);
	break;
  }

  load_items_to_browser(product_grouping_form->gms_form,
	product_grouping_form->browser, CURR_PROD, curr_selected_tape_group, CLEAR);


  gms_unblock_ints(oldmask);

} /* popup_product_grouping_form */

/****************************************************************************/
/*                                                                          */
/*                         do_select_tape_ids              */
/*                                                                          */
/****************************************************************************/
void do_select_tape_ids(FL_OBJECT *button , long val)
{

} /* do_select_tape_ids */

/****************************************************************************/
/*                                                                          */
/*                         show_ps_form              */
/*                                                                          */
/****************************************************************************/
void show_ps_form()
{
  extern FD_ps_info_form *ps_form;
  int all_levels, prog_only;
  char typemask;

  if (ps_form == NULL || ps_form->ps_info_form == NULL ||
	 ps_form->browser == NULL || ps_form->progs_only_button == NULL ||
	 ps_form->every_level_button  == NULL) return;

  /* don't update form if it's not mapped */
  if (!ps_form->ps_info_form->visible) return;

  fl_deactivate_object(ps_form->browser);
  fl_freeze_form(ps_form->ps_info_form);
  typemask = 0;
  all_levels = fl_get_button(ps_form->every_level_button);
  prog_only = fl_get_button(ps_form->progs_only_button);
  if (all_levels) 
	typemask |= PS_ALL_LEVELS;
  else
	typemask |= PS_TOP_LEVEL;
  if (prog_only)
	typemask |= PS_SELECTED_PROGS;
  else 
	typemask |= PS_ALL_PROGS;


  load_items_to_browser(ps_form->ps_info_form,	ps_form->browser, PS,
						&typemask, CLEAR);
  fl_unfreeze_form(ps_form->ps_info_form);
  fl_activate_object(ps_form->browser);

} /* show_ps_form */

/****************************************************************************/
/*                                                                          */
/*                         do_toggle_show_all_levels_button              */
/*                                                                          */
/****************************************************************************/
void do_toggle_show_all_levels_button(FL_OBJECT *button , long val)
{
  long old_mask;

  old_mask = gms_block_ints();
  if (button == NULL) {
	gms_unblock_ints(old_mask);
	return;
  }
  fl_deactivate_object(button);
  show_ps_form();

  fl_activate_object(button);
  gms_unblock_ints(old_mask);
} /* do_toggle_show_all_levels_button */

/****************************************************************************/
/*                                                                          */
/*                         do_toggle_show_network_top_button              */
/*                                                                          */
/****************************************************************************/
void do_toggle_show_network_top_button(FL_OBJECT *button , long val)
{
  long old_mask;

  old_mask = gms_block_ints();
  if (button == NULL) {
	gms_unblock_ints(old_mask);
	return;
  }
  fl_deactivate_object(button);
  
  show_ps_form();

  fl_activate_object(button);
  gms_unblock_ints(old_mask);
} /* do_toggle_show_all_levels_button */


/****************************************************************************/
/*                                                                          */
/*                         do_help              */
/*                                                                          */
/****************************************************************************/
void do_help(FL_OBJECT *button, long val)
{
  long old_mask;

  old_mask = gms_block_ints();
  if (button == NULL) {
	gms_unblock_ints(old_mask);
	return;
  }
  fl_deactivate_object(button);
  fl_show_message("Sorry!", "Help is currently  unavailable.", "");
  fl_activate_object(button);
  gms_unblock_ints(old_mask);
}
/*########################################################################*/
/*                                                                        */
/*                        descriptive text box routines                   */
/*                                                                        */
/*########################################################################*/

/**************************************************************************/
/*                                                                        */
/*                                point_outside_button                    */
/*                                                                        */
/**************************************************************************/
int point_outside_button(FL_OBJECT *button, FL_Coord x, FL_Coord y)
{
  /* returns 1 if mouse is outside of the button; 0 otherwise
   */


  if (button == NULL || button->form == NULL) return 0;

   if (x < button->x || x > (button->x + button->w) ||
	  y < button->y || y > (button->y + button->h)) 
	return 1;

  return 0;
}

/**************************************************************************/
/*                                                                        */
/*                                do_button_info                          */
/*                                                                        */
/**************************************************************************/
int do_button_info(FL_OBJECT *obj, int ev,
                                FL_Coord mx, FL_Coord my,
                                int key, void *raw_event)
{ 

  /* has to return !FL_PREEMPT to continue with the built-in callback handler*/
  /* move form with mouse */


  char *text;
  long old_mask;

  old_mask = gms_block_ints();

  /* if show button info or toolbar is not turned on, then do nothing */
  if (show_button_info_flag == 0 || show_toolbar_flag == 0) goto DONE;

  if (obj == NULL ) goto DONE;

  if (ev != FL_ENTER && ev != FL_LEAVE && ev != FL_MOTION)
	goto DONE;

  /* close form if mouse moved out of button's boundary */
  if (point_outside_button(obj, mx, my)) {
	hide_button_info_msg();
	goto DONE;
  }
  /* get button's info text from table */
  text = button_info_tbl[(int)obj->u_ldata];


  show_button_info_msg(text);


 DONE:

  gms_save_button = obj;
  gms_unblock_ints(old_mask);

  return !FL_PREEMPT;
  
} /* do_button_info */

int do_pointermotion(FL_OBJECT *button)
{
  FL_Coord mx, my;
  unsigned int mask;

  /* if show button info or toolbar is not turned on, then do nothing */
  if (show_button_info_flag == 0 || show_toolbar_flag == 0) goto DONE;

  if (button == NULL || button->form == NULL) goto DONE;

  fl_get_form_mouse(button->form, &mx, &my, &mask);
  if (point_outside_button(button, mx, my)) 
	hide_button_info_msg();

 DONE:
  return 0;
} /* do_pointermotion */
/*########################################################################*/
/*                                                                        */
/*                              menu related routines                     */
/*                                                                        */
/*########################################################################*/

/***************************************************************************/
/*                                                                         */
/*                             do_file_menu                                */
/*                                                                         */
/***************************************************************************/

void do_file_menu(FL_OBJECT *obj, long form_id)
{
   long old_mask;
   const char *selected_item;
   FL_FORM *form = NULL;
   FD_logfile_form *logfile_form;
   FD_report_form *report_form;
   FD_gms_form *gms_form;
   FD_msg_form *msg_form;

   old_mask = gms_block_ints();
   if (obj == NULL) {
	 gms_unblock_ints(old_mask);
	 return;
   }
   selected_item = fl_get_menu_text(obj);


   /* these 'Print **' must be checked before 'Print' */
   if (strstr(selected_item, "Print Failures") != NULL) {
	 do_print_failures_from_report(obj, form_id);
   }
   else if (strstr(selected_item, "Print Warnings") != NULL) {
	 do_print_warnings_from_report(obj, form_id);
   }
   else if (strstr(selected_item, "Print All") != NULL) {
	 do_print_all_from_browser(obj, form_id);
   }
   else if (strstr(selected_item, "Close") != NULL) {
	 switch (form_id) {
	 case LOGFILE_FORM_ID:
	   logfile_form = obj->u_vdata;
	   if (logfile_form)
		 form = logfile_form->logfile_form;
	   break;
	 case SUMMARY_REPORT_FORM_ID:
	   report_form = obj->u_vdata;
	   if (report_form)
		 form = report_form->report_form;
	   break;
	 case GMS_FORM_ID:
	   gms_form = obj->u_vdata;
	   if (gms_form) 
		 form = gms_form->gms_form;
	   break;
	 case SYS_MSG_FORM_ID:
	 case ALL_DATA_FORM_ID:
	 case EXCEEDED_THRESHOLD_FORM_ID:
	   msg_form = obj->u_vdata;
	   if (msg_form)
		 form = msg_form->msg_form;
	   break;
	 default:
	   break;
	 }
	 close_form(form);
   }

   else if (strstr(selected_item, "Save") != NULL) {
	 do_write_browser_to_file(obj, form_id);
   }
   else if (strstr(selected_item, "Print") != NULL) {
	 do_print_all_from_browser(obj, form_id);
   }
   
   else if (strstr(selected_item, "Exit") != NULL) {
	 exit_system(obj, 0);
   }

   gms_unblock_ints(old_mask);
}
/***************************************************************************/
/*                                                                         */
/*                             do_view_menu                                */
/*                                                                         */
/***************************************************************************/

void do_view_menu(FL_OBJECT *menu, long form_id)
{
  /* form_id: 2 - FD_gms_form
   *      3 - FD_group_from
   *      4 - FD_status_form
   */
  long old_mask;
  const char *selected_item;
  FD_gms_form *gm_form;
  FD_group_form *grp_form;
  int index;
  char msg[MAX_LABEL_LEN];
  view_option_type_t view_opts = 0, keep_opts = 0;

  old_mask = gms_block_ints();
  if (menu == NULL || menu->u_vdata == NULL) {
	gms_unblock_ints(old_mask);
	return;
  }

  switch (form_id) {
  case GMS_FORM_ID:
	index = GMSF_VIEW_OPTION_IND;
	break;
  case STATUS_FORM_ID:
	index = STATUSF_VIEW_OPTION_IND;
	break;
  case GROUP_FORM_ID:
	index = GROUPF_VIEW_OPTION_IND;
	break;
  case SUMMARY_REPORT_FORM_ID:
	index = SUMMARYF_VIEW_OPTION_IND;
	break;
  default:
	break;
  }


  selected_item = fl_get_menu_text(menu);
  /* Toggle selected item */
  if (strstr(selected_item, "All")  != NULL) {
	view_opts = VIEW_ALL;
  }
  else  if (strstr(selected_item, "One tape grouping")  != NULL) {
	view_opts = VIEW_ONE_TAPE_GRPING;
  }
  else  if (strstr(selected_item, "failed") != NULL) {
	view_opts = VIEW_FAILURES;
  }
  else  if (strstr(selected_item, "running") != NULL) {
	view_opts = VIEW_RUNNINGS;
  }

  else  if (strstr(selected_item, "warning") != NULL) {
	view_opts = VIEW_WARNINGS;
  }
  else  if (strstr(selected_item, "successful") != NULL) {
	view_opts = VIEW_SUCCESSFULS;
  }

  else  if (strstr(selected_item, "unknown") != NULL) {
	view_opts = VIEW_UNKNOWNS;
  }

  else  if (strstr(selected_item, "Active groups only") != NULL) {
	view_opts = VIEW_ACTIVE_ONLY;
  }
  else  if (strstr(selected_item, "Inactive groups only") != NULL) {
	view_opts = VIEW_INACTIVE_ONLY;
  }
  else if (strstr(selected_item, "Sort by host, tapeid, and program") != NULL ||
		   strstr(selected_item, "Sort by host and tapeid") != NULL) {
	/* toggle on off by using exclusive OR */
	view_opts = SORT_BY_HOST_TAPE_PROG|SORT_BY_ETIME;
  }
  else if (strstr(selected_item, "Sort by end time")  != NULL ||
		   strstr(selected_item, "Sort by group's end time")  != NULL)  {
	/* toggle on off by using exclusive OR */
	view_opts = SORT_BY_ETIME|SORT_BY_HOST_TAPE_PROG;
  }

  if (view_opts == VIEW_ALL) {
	/* Select only view_all and view_one_tape_grping if applied. 
	 * Clear the other options */
	keep_opts = forms_view_options[index] & 
	  (SORT_BY_HOST_TAPE_PROG|SORT_BY_ETIME|VIEW_ACTIVE_ONLY|VIEW_INACTIVE_ONLY);
	forms_view_options[index] = view_opts | keep_opts;
  }
  else if (view_opts != 0) {
	/* Toggle menu item */
	/*
printf("viewopt: %d, forms view opt: %d\n", view_opts, forms_view_options[index]);
*/
	forms_view_options[index] ^= view_opts;
	/*
printf("*viewopt: %d, forms view opt: %d\n", view_opts, forms_view_options[index]);
*/
  }

  set_view_menu_options(menu, &(forms_view_options[index]), form_id);

  switch (form_id) {
  case GMS_FORM_ID:

	/* set the view menu items appropriately */
	gm_form = (FD_gms_form *) menu->u_vdata;

	if (forms_view_options[GMSF_VIEW_OPTION_IND] & VIEW_ONE_TAPE_GRPING) {
	  if (strlen(curr_selected_tape_group) == 0) {
		sprintf(msg, "Select by clicking on an item on form: %s\n", 
				TAPE_GROUPING_FORM_NAME);
		fl_show_message("WARNING", "No tape grouping was selected.", msg);
		/* change state of view menu back */
		if (gm_form && gm_form->view_menu) {
		  forms_view_options[GMSF_VIEW_OPTION_IND] ^= VIEW_ONE_TAPE_GRPING;
		  set_view_menu_options(gm_form->view_menu, 
								&(forms_view_options[GMSF_VIEW_OPTION_IND]),
								GMS_FORM_ID);
		}
		goto DONE;
	  }
	}

	/* reload data to form */
	load_items_to_browser(gm_form->gms_form, gm_form->browser, 
						  CURR_PROD, curr_selected_tape_group, CLEAR);

	break;
  case STATUS_FORM_ID:
	if (gstatus_info) {
	  gstatus_info->refresh_form = 1;
	  /* reload data to form */
	  update_status_form(gstatus_info->scroll_per, 1);
	}
	break;
  case GROUP_FORM_ID:
	grp_form = (FD_group_form *) menu->u_vdata;
	load_items_to_browser(grp_form->group_form, grp_form->browser,
						  TAPE_GROUP, "", CLEAR);
	/* Redraw line so it appears on top of browser. */
	if (grp_form->divider_bar && grp_form->group_form->visible)
	  fl_show_object(grp_form->divider_bar);

	break;
  case SUMMARY_REPORT_FORM_ID:
	update_summary_report_form();
	break;
  default:
	break;
  }
  DONE:
  gms_unblock_ints(old_mask);

} /* do_view_menu */

/***************************************************************************/
/*                                                                         */
/*                             do_status_menu                                */
/*                                                                         */
/***************************************************************************/

void do_status_menu (FL_OBJECT *obj, long val)
{
  long old_mask;
  const char *selected_item;

  old_mask = gms_block_ints();
  if (obj == NULL) {
	gms_unblock_ints(old_mask);
	return;
  }
  selected_item = fl_get_menu_text(obj);
   if (strstr(selected_item, "Show product groupings (bars)") != NULL)
	 popup_status_form(obj, val);
  else if (strstr(selected_item, "Show PS Info") != NULL)
	popup_ps_form(obj, val);
  else if (strstr(selected_item, "Show System Messages") != NULL)
	popup_system_status_msg_form(obj, val);
  else if (strstr(selected_item, "Show product groupings (text)") != NULL)
	popup_product_grouping_form(obj, val);
  else if (strstr(selected_item, "Show all jobs sorted by time") != NULL)
	popup_all_data_form(obj, val);
  else if (strstr(selected_item, "Show jobs running for too long") != NULL)
	popup_exceeded_threshold_form(obj, val);
  gms_unblock_ints(old_mask);
}
/****************************************************************************/
/*                                                                          */
/*                         do_report_menu              */
/*                                                                          */
/****************************************************************************/
void do_report_menu(FL_OBJECT *obj, long val)
{
  long old_mask;
  const char *selected_item;

  old_mask = gms_block_ints();
  if (obj == NULL) {
	gms_unblock_ints(old_mask);
	return;
  }
  selected_item = fl_get_menu_text(obj);
  if (strstr(selected_item, "Request Report") != NULL) 
	 do_last_night_request(obj, val);
  else if (strstr(selected_item, "Show Status Summary Report") != NULL)
	popup_summary_report_form(obj, val);
  gms_unblock_ints(old_mask);
}


/****************************************************************************/
/*                                                                          */
/*                     do_clear_status_fields_form                          */
/*                                                                          */
/****************************************************************************/
void do_clear_status_fields_form(FL_OBJECT *button, long val)
{
  long oldmask;
  FD_status_fields_selection_form *form;

  /* this is to prevent interrupts while  accessing internal lists */

  oldmask = gms_block_ints();

  if (button == NULL || button->u_vdata == NULL) goto DONE;
  form = button->u_vdata;
  
  fl_set_input(form->tapeid_input, "");
  fl_set_input(form->host_input, "");
  fl_set_input(form->product_input, "");
  fl_set_input(form->level_input, "");
  fl_set_input(form->sdate_input, "");
DONE:
  gms_unblock_ints(oldmask);
	
} /* do_clear_status_fields_form */


/****************************************************************************/
/*                                                                          */
/*                      do_delete_summary_info                              */
/*                                                                          */
/****************************************************************************/
void do_delete_summary_info(FL_OBJECT *button, long val)
{
  long oldmask;
  item_type_t remove_type;
  time_t stime=0;
  FD_status_fields_selection_form *form;
  char **selected_items;
  struct logfile_rec *logfile = NULL;
  int level = 0, logfile_level;
  char *tapeid, *host, *level_str, *product, *sdate_str;
	
  int i,j;
  int yr, mon, day, hr, min, sec;

  /* this is to prevent interrupts while  accessing internal lists */

  oldmask = gms_block_ints();

  if (button == NULL || button->u_vdata == NULL) goto DONE;

  form = button->u_vdata;

  /* Get fields from input areas on form */
  tapeid = (char *) strdup(fl_get_input(form->tapeid_input));
  host = (char *) strdup(fl_get_input(form->host_input));
  level_str = (char *) strdup(fl_get_input(form->level_input));
  product = (char *) strdup(fl_get_input(form->product_input));
  /* date_str's format: mm/dd/yy [hh:mm:ss]  */
  sdate_str = (char *) strdup(fl_get_input(form->sdate_input));

  /* Validating specified fields. */
  /* Need host, tapeid, prod for product summary */
  if (!((selected_summary_level_type & PRODUCT_SUMMARY &&
		 strlen(tapeid) > 0 && strlen(host) > 0 &&  
		 strlen(product) > 0) || 
		/* Only need tapeid and level for level summary */
		(selected_summary_level_type & LEVEL_SUMMARY &&
		 strlen(host) == 0 && strlen(product) == 0 && 
		 strlen(tapeid) > 0 && strlen(level_str) > 0) ||
		/* Only need tapeid for TAPE SUMMARY */
		(selected_summary_level_type & TAPE_SUMMARY &&
		 strlen(host) == 0 && strlen(product) == 0 && 
		 strlen(tapeid) > 0 && strlen(level_str) == 0))) {
	fl_show_message("Warning:", "Not all required fields were filled in.", 
					"Please try again.");
	goto DONE;
  }

  if (strlen(sdate_str) > 0) {
	/* Validate and convert date_str: mm/dd/yy [hh:mm:ss] to seconds.
	 */
	yr = 0; mon = 0; day = 0; hr = 0; min = 0; sec = 0;
	if (sscanf(sdate_str, "%2d/%2d/%2d %2d:%2d:%2d", &mon, &day, &yr, &hr,
			   &min, &sec) != 6) {
	  if (sscanf(sdate_str, "%2d/%2d/%2d", &mon, &day, &yr) != 3 ||
		  strlen(sdate_str) > 8) {
		fl_show_message("Warning:", "Start date/time format is invalid.\n", 
						"Please try again.");
		goto DONE;
	  }
	}
	stime = construct_time(yr, mon, day, hr, min, sec);  
fprintf(stderr, "yr: %d/%d/%d %d:%d:%d, %ld\n", mon, day, yr, hr, min, sec, stime);

  }


#ifdef XFORMS_081
  if (!fl_show_question("",
					   "Do you really want to delete?",
					   "")) {
#else
  if (!fl_show_question("Do you really want to delete?", 0)) {
#endif
	goto DONE; /* Cancel */
  }
  selected_items = (char **) calloc(MAX_ITEMS, sizeof(char *));
  if (selected_items == NULL) {
	perror("calloc selected_item");
	goto DONE;
  }
  for (i = 0; i < MAX_ITEMS; i++)
	selected_items[i] = NULL;
  
  j = 0;

  if (strlen(level_str) > 0) 
	level = atoi(level_str);

  remove_type = STAT_INFO;  
  logfile = gstatus_info->logfiles;
  /* Delete status info for either a tape, level, or product. */
  while (logfile) {
	if (strlen(tapeid) > 0 && strcmp(tapeid, logfile->tapeid) == 0 &&
		strlen(host) > 0 && strcmp(host, logfile->fromhost) == 0 &&
		strlen(product) > 0 && strcmp(product, logfile->prog) == 0 &&
		(stime == 0 || (stime > 0 && stime == logfile->stime_item))) {


	  goto REMOVE; /* Delete this logfile matched the specified product. */
	}

	else if (strlen(host) == 0 &&
			 strlen(product) == 0 &&
			 strlen(tapeid) > 0 && 
			 (strcmp(logfile->tapeid, tapeid) == 0 ||
			  (strlen(tapeid) == MAX_TAPEID_LEN-1 && 
			   strstr(logfile->tapeid, tapeid) != NULL))) { 

	  if (strlen(level_str) > 0) {
		logfile_level = get_level_for_prod(logfile->prog, NULL);
		if (level == logfile_level) {
		  goto REMOVE; /* Delete this logfile matched the specified  level. */
		}
		else
		  goto NEXT;   /* Not the same level */
	  }

	}
	else
	  goto NEXT;
  REMOVE:
fprintf(stderr, "LOGF: %s %s\n", logfile->sdate, logfile->stime);
	/* Send del job msg to monitorps for this logfile.
	 */
	send_del_job_to_monitorps(logfile);

	/*
	 * Construct $hostname.$tapeid.$product.log
	 */
	selected_items[j] = (char *)calloc(MAX_FILENAME_LEN, sizeof(char));
	create_logfile_name(logfile->fromhost, 
						logfile->tapeid, logfile->prog, selected_items[j]);
	j++;
  NEXT:
	logfile = logfile->next;
  } /** WHile logfile */

  /* REmove all reference information for this tape grouping */
  remove_info_for_selected_items(remove_type, selected_items, j, 
								 NULL, NULL);
  free_array_of_str(selected_items, j);
  free(selected_items);
  
  /* Update forms affected by this deletion. */
  update_forms_affected_by_status_info_change(1);

DONE:
  if (tapeid) free(tapeid);
  if (host) free(host);
  if (product) free(product);
  if (level_str) free(level_str);
  if (sdate_str) free(sdate_str);


  gms_unblock_ints(oldmask);
	
} /* do_delete_summary_info */

/****************************************************************************/
/*                                                                          */
/*                         update_active_fields                             */
/*                                                                          */
/****************************************************************************/
void update_active_fields()
{
  FD_status_fields_selection_form *form;

  if (status_fields_selection_form == NULL || 
	  status_fields_selection_form->status_fields_selection_form == NULL) 
	return;
  form = status_fields_selection_form;
  if (selected_summary_level_type & PRODUCT_SUMMARY) {
	/* Activate */
	fl_deactivate_object(form->level_input);
	fl_activate_object(form->product_input);
	fl_activate_object(form->host_input);
	fl_activate_object(form->sdate_input);
	fl_activate_object(form->tapeid_input);
  }
  else if (selected_summary_level_type &  LEVEL_SUMMARY) {
	fl_activate_object(form->level_input);
	fl_deactivate_object(form->product_input);
	fl_deactivate_object(form->host_input);
	fl_deactivate_object(form->sdate_input);
	fl_activate_object(form->tapeid_input);
  }
  else if (selected_summary_level_type & TAPE_SUMMARY) {
	fl_deactivate_object(form->level_input);
	fl_deactivate_object(form->product_input);
	fl_deactivate_object(form->host_input);
	fl_deactivate_object(form->sdate_input);
	fl_activate_object(form->tapeid_input);
  }
} /* update_active_fields() */
/****************************************************************************/
/*                                                                          */
/*                      popup_status_fields_selection_form                  */
/*                                                                          */
/****************************************************************************/
void popup_status_fields_selection_form(FL_OBJECT *obj, long form_id)
{
  long oldmask;

  /* this is to prevent interrupts while  accessing internal lists */
  oldmask = gms_block_ints();

  show_form(status_fields_selection_form->ldata);
  /* Disable non relevant fields */
  update_active_fields();
  gms_unblock_ints(oldmask);
	
} /* popup_status_fields_selection_form */


/****************************************************************************/
/*                                                                          */
/*                         popup_add_del_show_form              */
/*                                                                          */
/****************************************************************************/
void popup_add_del_show_form(FL_OBJECT *obj, long type)
{
  long oldmask;
  
  /* this is to prevent interrupts while  accessing internal lists */

  oldmask = gms_block_ints();
  
  if (show_button_info_flag)
	hide_button_info_msg();

  switch ((int)type) {
  case PROGS:

	if (progs_form == NULL || progs_form->add_del_show_form == NULL )  {
	  fprintf(stderr, "ERROR: There is no progs form.\n");
	  exit_system(NULL, -1);
	}
	show_form(progs_form->ldata);
	load_items_to_browser(progs_form->add_del_show_form,
						  progs_form->selected_browser, SELECTED_PROGS, "", CLEAR);
	load_items_to_browser(progs_form->add_del_show_form,
						  progs_form->available_browser, AVAILABLE_PROGS, "", CLEAR);

	break;
  case HOSTS:
	if (neighbors_form == NULL || neighbors_form->add_del_show_form == NULL ) {
	  fprintf(stderr, "ERROR: There is no neighbors form.\n");
	  exit_system(NULL, -1);
	}
	show_form(neighbors_form->ldata);
	load_items_to_browser(neighbors_form->add_del_show_form,
						  neighbors_form->selected_browser, SELECTED_HOSTS, 
						  "", CLEAR);
	load_items_to_browser(neighbors_form->add_del_show_form,
						  neighbors_form->available_browser, AVAILABLE_HOSTS, 
						  "", CLEAR);

	break;
  case ADMIN:
	if (admin_form == NULL || admin_form->add_del_show_form == NULL) {
	  fprintf(stderr, "ERROR: There is no admin form.\n");
	  exit_system(NULL, -1);
	}
	show_form(admin_form->ldata);
	load_items_to_browser(admin_form->add_del_show_form,
						  admin_form->selected_browser, SELECTED_NETWORK_HOSTS,
						  "", CLEAR);
	load_items_to_browser(admin_form->add_del_show_form,
						  admin_form->available_browser, AVAILABLE_NETWORK_HOSTS,
						  "", CLEAR);
  default:
	break;
  }
  gms_unblock_ints(oldmask);
	
}

/****************************************************************************/
/*                                                                          */
/*                         popup_gv_levels_form                             */
/*                                                                          */
/****************************************************************************/
void popup_gv_levels_form()
{
  show_form(gv_levels_form->ldata);
  update_gv_levels_form();
} /* popup_gv_levels_form */



/****************************************************************************/
/*                                                                          */
/*                         do_config_menu               */
/*                                                                          */
/****************************************************************************/
void do_config_menu (FL_OBJECT *obj, long val)
{
  long old_mask;
  const char *selected_item;

  old_mask = gms_block_ints();
  if (obj == NULL) {
	gms_unblock_ints(old_mask);
	return;
  }
  selected_item = fl_get_menu_text(obj);

  if (selected_item == NULL)
	goto DONE;

  /* from gms_form */
  if (strstr(selected_item, "Add/Delete Programs") != NULL)
	popup_add_del_show_form(obj, PROGS);
  else if (strstr(selected_item, "Add/Delete Neighbors") != NULL)
	popup_add_del_show_form(obj, HOSTS);
  else if (strstr(selected_item, "Administrative Functions") != NULL)
	popup_add_del_show_form(obj, ADMIN);

  /* from ps_form */
  else if (strstr(selected_item, "Select Host(s)") != NULL)
	popup_select_host_for_ps_form(NULL, 0);

  else if (strstr(selected_item, "Define GV Levels") != NULL)
	popup_gv_levels_form();
DONE:
  gms_unblock_ints(old_mask);
}

/****************************************************************************/
/*                                                                          */
/*                         do_toggle_toolbars              */
/*                                                                          */
/****************************************************************************/
void do_toggle_toolbars(FL_OBJECT *menu)
{
  /* show/unshow toolbars for all forms
   */
  unsigned mode;


  mode = fl_get_menu_item_mode(menu, SHOW_TOOLBAR_MINDEX);
  if (mode == FL_PUP_BOX) {
	/* hide it */
	if (tape_grouping_form && tape_grouping_form->toolbar_grp) 
	  fl_hide_object(tape_grouping_form->toolbar_grp);
	if (product_grouping_form && product_grouping_form->toolbar_grp) 
	  fl_hide_object(product_grouping_form->toolbar_grp);
	if (summary_report_form && summary_report_form->toolbar_grp)
	  fl_hide_object(summary_report_form->toolbar_grp);
	if (logfile_report_form && logfile_report_form->toolbar_grp)
	  fl_hide_object(logfile_report_form->toolbar_grp);
	if (job_grouping_form && 
		job_grouping_form->toolbar_grp)
	  fl_hide_object(job_grouping_form->toolbar_grp);

	hide_button_info_msg();
	show_toolbar_flag = 0;
  }
  else {
	/* show it */
	if (tape_grouping_form && tape_grouping_form->toolbar_grp) {
	  fl_show_object(tape_grouping_form->toolbar_grp);
	  /* redraw is needed to have pixmap repainted */
	  fl_redraw_object(tape_grouping_form->toolbar_grp);
	}
	if (product_grouping_form && product_grouping_form->toolbar_grp) {
	  fl_show_object(product_grouping_form->toolbar_grp);
	  /* redraw is needed to have pixmap repainted */
	  fl_redraw_object(product_grouping_form->toolbar_grp);
	}
	if (summary_report_form && summary_report_form->toolbar_grp) {
	  fl_show_object(summary_report_form->toolbar_grp);
	  fl_redraw_object(summary_report_form->toolbar_grp);
	}
	if (logfile_report_form && logfile_report_form->toolbar_grp) {
	  fl_show_object(logfile_report_form->toolbar_grp);
	  fl_redraw_object(logfile_report_form->toolbar_grp);
	}
	if (job_grouping_form && 
		job_grouping_form->toolbar_grp) {
	  fl_show_object(job_grouping_form->toolbar_grp);
	  fl_redraw_object(job_grouping_form->toolbar_grp);
	}
	
	show_toolbar_flag = 1;
  }

} /* do_hide_toolbars */


/****************************************************************************/
/*                                                                          */
/*                         do_toggle_button_info              */
/*                                                                          */
/****************************************************************************/
void do_toggle_button_info(FL_OBJECT *menu)
{
  /* show/unshow button's description
   */
  unsigned mode;

  mode = fl_get_menu_item_mode(menu, SHOW_BUTTON_INFO_MINDEX);
  /* if this item is not checked, close button info form and turn flag off. */
  if (mode == FL_PUP_BOX) {
	hide_button_info_msg();
	show_button_info_flag = 0;
  }
  else 
	show_button_info_flag = 1; /* turn flag on */
}

/****************************************************************************/
/*                                                                          */
/*                         do_toggle_blink_opt                              */
/*                                                                          */
/****************************************************************************/
void do_toggle_blink_opt(FL_OBJECT *menu)
{
  extern int blink_boxes_flag;

  unsigned mode;
  mode = fl_get_menu_item_mode(menu, BLINK_BOXES_MINDEX);
  /* if this item is not checked, turn blinking off. */
  if (mode == FL_PUP_BOX) 
	blink_boxes_flag = 0;
  else {
	blink_boxes_flag = 1;
	blink_stat_light_buttons();	
	set_alarm_to_update_forms();	
  }
} 

/****************************************************************************/
/*                                                                          */
/*                         do_options_menu              */
/*                                                                          */
/****************************************************************************/
void do_options_menu(FL_OBJECT *obj, long val)
{

  long old_mask;
  const char *selected_item;

  old_mask = gms_block_ints();
  if (obj == NULL) {
	gms_unblock_ints(old_mask);
	return;
  }
  selected_item = fl_get_menu_text(obj);
  if (strstr(selected_item, "Show Toolbar") != NULL) 
	do_toggle_toolbars(obj);

  else if (strstr(selected_item, "Show Button Description") != NULL) 
	do_toggle_button_info(obj);

  else if (strstr(selected_item, "Blink Boxes") != NULL) 
	do_toggle_blink_opt(obj);
  else if (strstr(selected_item, "Refresh data") != NULL)
	do_reset(obj, val);
  else if (strstr(selected_item, "Test") != NULL)
	popup_test_form(obj, val);

  /* write user's selected programs and neighbors to ~/.gms */
  update_gms_init_file();

  gms_unblock_ints(old_mask);
}

/****************************************************************************/
/*                                                                          */
/*                         do_action_menu              */
/*                                                                          */
/****************************************************************************/
void do_action_menu(FL_OBJECT *menu, long form_id)
{
  /* form_id = 1: for FD_report_form
   *           0: for FD_logfile_form
   *           4: for FD_status_form
   */
  long old_mask;
  const char *selected_item;
  FD_logfile_form *lform;
  FD_gms_form *gmsform;
  FD_group_form *gform;
  FL_OBJECT *del_button;

  old_mask = gms_block_ints();
  if (menu == NULL || menu->u_vdata == NULL) goto DONE;

  if ((selected_item = fl_get_menu_text(menu)) == NULL) goto DONE;

  if (strstr(selected_item, "Close") != NULL)
	do_close_form(menu, form_id);

  else if (strstr(selected_item, "Remove") != NULL &&
		   strstr(selected_item, "groupings") != NULL) {
	/* Remove [tape|product|job] groupings */
	switch (form_id) {
	case LOGFILE_FORM_ID:
	  lform = (FD_logfile_form *) menu->u_vdata;
	  del_button = lform->del_job_button;
	  break;
	case GROUP_FORM_ID:
	  gform = (FD_group_form *) menu->u_vdata;
	  del_button = gform->del_job_button;
	  break;
	case GMS_FORM_ID:
	  gmsform = (FD_gms_form *) menu->u_vdata;
	  del_button = gmsform->del_job_button;
	  break;
	default:
	  goto DONE;
	}

	do_del_job_button(del_button, menu->u_ldata);

  }

 DONE:
  gms_unblock_ints(old_mask);
}

/****************************************************************************/
/*                                                                          */
/*                         do_windows_menu              */
/*                                                                          */
/****************************************************************************/
void do_windows_menu(FL_OBJECT *obj, long val)
{
  char *selected_item;
  long old_mask;
  int i;

  old_mask = gms_block_ints();
  if (obj == NULL) goto DONE;

  if ((selected_item = (char *) fl_get_menu_text(obj)) == NULL) goto DONE;


  if (strcmp(selected_item, "Close all others") == 0) {
	/* close all forms except the main one */
	for (i = 0; i < form_count; i++) {
	  if (strcmp(gms_form_list[i].name, TAPE_GROUPING_FORM_NAME) == 0) continue;
	  close_form(gms_form_list[i].form);
	}
  }
  else {
	/* raise selected form */
	for (i = 0; i < form_count; i++) {
	  if (strcmp(gms_form_list[i].name, selected_item) == 0) {
		/* raise form if it was selected */
		raise_form(gms_form_list[i].form);
		break;
	  }
	}
  }

DONE:
  gms_unblock_ints(old_mask);
}

/****************************************************************************/
/*                                                                          */
/*                         do_help_menu               */
/*                                                                          */
/****************************************************************************/
void do_help_menu (FL_OBJECT *obj, long val)
{
  long old_mask;
  const char *selected_item;

  old_mask = gms_block_ints();
  if (obj == NULL) {
	gms_unblock_ints(old_mask);
	return;
  }
  selected_item = fl_get_menu_text(obj);
  if (strstr(selected_item,"About gms") != NULL)
	fl_show_message("GVS Monitoring System", VERSION_STR, "Copyright @ 1996-1998 NASA/TRMM Office");
  else if (strstr(selected_item,"Contexts") != NULL)
	fl_show_message("Sorry!", "Help is currently unavailable.", "");
  gms_unblock_ints(old_mask);

}


/****************************************************************************/
/*                                                                          */
/*                         send_items_from_browser_to_file_or_printer              */
/*                                                                          */
/****************************************************************************/
void send_items_from_browser_to_file_or_printer(FL_OBJECT *browser, 
												long form_id,
												char *match_str, 
												char *print_cmd, char *fname)
{
  /* Send browser line that has string matched with match_str to either
   * a file or a printer.
   * if match_str is "", send everything from browser
   */
  extern char *tempnam(const char *dir, const char *pfx);
  int i, line_num = 0;
  char buf[MAX_DATA_LEN];
  char *item;
  char *stat_str = UNKNOWN_STR;
  char cmd[MAX_CMD_LEN];
  char *tmp_fname = NULL;
  FILE *fp;
  char label[MAX_LABEL_LEN], *line_num_str = "     ";

  if (browser == NULL || match_str == NULL ||
	  (print_cmd == NULL && fname == NULL)) return;

  memset(buf, '\0', MAX_DATA_LEN);
  if (fname == NULL || strlen(fname) == 0) {
	/* Store the data from browser in a tmp file and send that file to the 
	 * printer 
	 */
	tmp_fname = tempnam("/tmp", "gms_");
	if (tmp_fname == NULL) 
	  tmp_fname = "gms.tmp";
  }
  else
	tmp_fname = fname;

  fp = fopen(tmp_fname, "w");
  if (fp == NULL) {
	perror(tmp_fname);
	fl_show_message("Error", "Can't open file.", "Abort.");
	return;
  }
  memset(label, '\0', MAX_LABEL_LEN);
  switch(form_id) {
  case LOGFILE_FORM_ID: 
	/* current file status report and report's file status report forms */
	fprintf(fp, "%s    Date     Time     Tape-ID             Device     Product              Job-num    Status", line_num_str);
	fprintf(fp, "\n%s===========================================================================================\n", line_num_str);
	break;
  case SUMMARY_REPORT_FORM_ID: 
	/* summary form */
	if (selected_summary_level_type & PRODUCT_SUMMARY) {

	  fprintf(fp, "%s    Host        TapeID         Level        Product          Device       Date    Time       Runtime       Status", line_num_str);
	  fprintf(fp, "\n%s=======================================================================================================================\n", line_num_str);
	}
	else if (selected_summary_level_type &  LEVEL_SUMMARY) {
	  fprintf(fp, "%s    TapeID           Level      Date       Time       Runtime     Status", line_num_str);
	  fprintf(fp, "\n%s========================================================================\n", line_num_str);
	}
	else if (selected_summary_level_type & TAPE_SUMMARY) {
	  fprintf(fp, "%s    TapeID               Date      Time         Runtime     Status", line_num_str);
	  fprintf(fp, "\n%s==================================================================\n", line_num_str);
	}

	break;
  case GMS_FORM_ID:
	fprintf(fp, "%s    Host     Tape ID            Product                Device      Date     Time      Runtime   TotalJobs   Status", line_num_str);
	fprintf(fp, "\n%s==================================================================================================================\n", line_num_str);
	break;
  case GROUP_FORM_ID:  
	fprintf(fp, "%s                                                Current           Product      Product         Last        Group\n%s     Host    Group ID             Runtime       Product           Runtime       Status      Job  Runtime   Status", line_num_str, line_num_str);
	fprintf(fp, "\n%s=====================================================================================================================\n", line_num_str);
	break;
  case ALL_DATA_FORM_ID:
	fprintf(fp, "%s   Date     Time     Tape-ID            Device       Product         Job-num    Filename             Status", line_num_str);
	fprintf(fp, "\n%s===========================================================================================================\n", line_num_str);
	break;
  case EXCEEDED_THRESHOLD_FORM_ID:
	fprintf(fp, "%s   Date     Time        Runtime   Tape ID        Device       Product     Job-num Filename             Status", line_num_str);
	fprintf(fp, "\n%s=============================================================================================================\n", line_num_str);
	break;	
  default:
	break;
  }



  for (i = 1; i <= fl_get_browser_maxline(browser); i++) {
	item = (char *) fl_get_browser_line(browser, i);

	/* remove leading spaces */
	while (item) {
	  if (*item == ' ') item++;
	  else break;
	}

	if (item == NULL || strchr(item, ' ') == NULL) 
	  continue;

	line_num++;
	sprintf(buf, "%s%5d ", buf, line_num);
	stat_str = get_stat_str_from_browser_line(item);
	if (stat_str && strstr(stat_str, match_str) != NULL) {
	  /* skip code string */
	  if (strchr(item, ' ')){
		if (strlen(buf) + strlen(item) - 1 + strlen(stat_str) +1 > MAX_DATA_LEN) {
		  /* the buffer is full, print it and reuse buffer. */
		  fprintf(fp, "%s", buf);
		  memset(buf, '\0', MAX_DATA_LEN);
		}
		/* not copy '\n' */
		strncat(buf, strchr(item, ' '), strlen(item)-1);
		/* append status str at end of line*/
		strcat(buf, "  ");
		strcat(buf, stat_str);
	  }
	  strcat(buf, "\n");

	}
  }
  if (strlen(buf) == 0 && i == 1) {
	fl_show_message("Warning", "Nothing is in the browser.", "Ignore.");
	return;
  }

  fprintf(fp, "%s", buf);
  fprintf(fp, "\nLegend:\n");
  fprintf(fp, "         '*'  Finished      '-' Stopped\n");
  fclose(fp);

  if (print_cmd != NULL && strlen(print_cmd) != 0) {
	/* Send to a printer */
	sprintf(cmd, "%s %s; rm %s", print_cmd, tmp_fname, tmp_fname);

	/*
  sprintf(cmd, "%s %s", print_cmd, tmp_fname);
  */
	fl_exe_command(cmd, 0); /* Exec command without holding */
	fl_show_message("", "Printing Job is submitted.", cmd);
#if 0
	fl_show_command_log(1); /* Show message log */
#endif
  }

} /* send_items_from_browser_to_file_or_printer */


/****************************************************************************/
/*                                                                          */
/*                         do_get_scroll_percent              */
/*                                                                          */
/****************************************************************************/
void do_get_scroll_percent(FL_OBJECT *slider, long val)
{
  float per;
  long oldmask;

  /* get scroll percent and update status form */

  oldmask = gms_block_ints();
/*
printf("enter do get scroll percent\n");
*/
  if (gstatus_info == NULL || gstatus_info->sform == NULL ||
	  gstatus_info->sform->status_form == NULL || slider == NULL || 

	  !slider->active) {
/*
printf("exit do get scroll percent\n");
*/
	gms_unblock_ints(oldmask);
	return;
  }
  fl_freeze_form(gstatus_info->sform->status_form);
  fl_deactivate_object(slider);
  per = fl_get_slider_value(slider);
  update_status_form(per, 0);
  fl_activate_object(slider);
  fl_unfreeze_form(gstatus_info->sform->status_form);
/*
printf("exit do get scroll percent\n");
*/
  gms_unblock_ints(oldmask);
}


/****************************************************************************/
/*                                                                          */
/*                           do_print_items_from_browser                    */
/*                                                                          */
/****************************************************************************/
void do_print_items_from_browser(FL_OBJECT *button, long form_id)
{
  long oldmask;
  FD_get_input_form *form;
  char *print_cmd;
  FL_OBJECT *browser;
  FD_report_form *rpt_form;
  FD_logfile_form *log_form;
  FD_group_form *group_form;
  FD_msg_form *msg_form;
  FD_gms_form *gm_form;

  oldmask = gms_block_ints();

  if (button == NULL || button->u_vdata == NULL) {
	gms_unblock_ints(oldmask);
	return;
  }
  fl_deactivate_object(button);

  form = button->u_vdata; /* get print cmd form (input form) */

  switch (form_id) {
  case LOGFILE_FORM_ID: 
	/* current file status report and report's file status report forms */
	log_form = (FD_logfile_form *) form->vdata;
	if (log_form == NULL || log_form->browser == NULL) goto DONE;
	browser = log_form->browser;
	break;
  case SUMMARY_REPORT_FORM_ID: 
	/* report form */
	rpt_form = (FD_report_form *)form->vdata;
	if (rpt_form == NULL || rpt_form->browser == NULL) goto DONE;
	browser = rpt_form->browser;
	break;

  case GMS_FORM_ID:
	gm_form = (FD_gms_form *)form->vdata;
	if (gm_form == NULL || gm_form->browser == NULL) goto DONE;
	browser = gm_form->browser;
	break;

  case GROUP_FORM_ID:
	group_form = (FD_group_form *)form->vdata;
	if (group_form == NULL || group_form->browser == NULL) goto DONE;
	browser = group_form->browser;
	break;
  case ALL_DATA_FORM_ID:
	msg_form = (FD_msg_form  *) form->vdata;
	if (msg_form == NULL || msg_form->browser == NULL) goto DONE;
	browser = msg_form->browser;
	break;

  case EXCEEDED_THRESHOLD_FORM_ID:
	msg_form = (FD_msg_form  *) form->vdata;
	if (msg_form == NULL || msg_form->browser == NULL) goto DONE;
	browser = msg_form->browser;
	break;	
  default:
	goto DONE;
  }
	
  if (form->input == NULL) goto DONE;
  print_cmd = (char *)fl_get_input(form->input);
  if (print_cmd == NULL || strlen(print_cmd) == 0) {
	fl_show_message("Error", "Print command is not specified.", "Try again.");
	goto DONE;
  }
  send_items_from_browser_to_file_or_printer(browser, form_id,
										 current_print_match_str, print_cmd, 
											 NULL);

  current_print_browser = NULL;
  current_print_match_str = NULL;
  do_close_form(form->cancel_button, 0);
  /* clear input */
  fl_set_input(form->input, "");
DONE:
  fl_activate_object(button);

  gms_unblock_ints(oldmask);

} /* do_print_items_from_browser */


/****************************************************************************/
/*                                                                          */
/*                         do_print_failures_from_report              */
/*                                                                          */
/****************************************************************************/
void do_print_failures_from_report(FL_OBJECT *button, long form_id)
{
  /* form_id = 1: for FD_report_form
   *           0: for FD_logfile_form
   */
  FD_report_form *report_form;
  FD_logfile_form *logfile_form;
  long old_mask;
  FL_OBJECT *browser = NULL;
  FD_get_input_form *get_input_form = NULL;

  old_mask = gms_block_ints();
  if (show_button_info_flag)
	hide_button_info_msg();
  if (button == NULL || button->u_vdata == NULL) {
	gms_unblock_ints(old_mask);
	return;
  }
  switch(form_id) {
  case LOGFILE_FORM_ID:
	logfile_form = (FD_logfile_form *) button->u_vdata;
	browser = logfile_form->browser;
	get_input_form = logfile_form->vdata;
	break;
  case SUMMARY_REPORT_FORM_ID:
	report_form = (FD_report_form *) button->u_vdata;
	browser = report_form->browser;
	get_input_form = report_form->vdata;
	break;
  default:
	break;
  }

  if (get_input_form == NULL ||
	  get_input_form->ok_button == NULL || get_input_form->label == NULL ||
	  get_input_form->get_input_form == NULL)
	goto DONE;
  current_print_browser = browser;
  current_print_match_str = FAILED_STR;
  fl_set_object_callback(get_input_form->ok_button, 
						 do_print_items_from_browser, form_id);
  fl_set_object_label(get_input_form->label,"Enter Print Command:");

  
  show_form(get_input_form->ldata);
 DONE:
  gms_unblock_ints(old_mask);
}

/****************************************************************************/
/*                                                                          */
/*                         do_print_warnings_from_report              */
/*                                                                          */
/****************************************************************************/
void do_print_warnings_from_report(FL_OBJECT *button, long form_id)
{
  /* form_id = 1: for FD_report_form
   *           0: for FD_logfile_ form
   */
  FD_report_form *report_form;
  FD_logfile_form *logfile_form;
  long old_mask;
  FL_OBJECT *browser = NULL;
  FD_get_input_form *get_input_form = NULL;

  old_mask = gms_block_ints();
  if (show_button_info_flag)
	hide_button_info_msg();
  if (button == NULL || button->u_vdata == NULL) {
	gms_unblock_ints(old_mask);
	return;
  }
  switch (form_id) {
  case LOGFILE_FORM_ID:
	logfile_form = (FD_logfile_form *) button->u_vdata;
	browser = logfile_form->browser;
	get_input_form = logfile_form->vdata;
	break;
  case SUMMARY_REPORT_FORM_ID:
	report_form = (FD_report_form *) button->u_vdata;
	browser = report_form->browser;
	get_input_form = report_form->vdata;
	break;
  default:
	break;
  }
  if (get_input_form == NULL ||
	  get_input_form->ok_button == NULL || get_input_form->label == NULL ||
	  get_input_form->get_input_form == NULL)
	goto DONE;
  current_print_browser = browser;
  current_print_match_str = WARNING_STR;
  fl_set_object_callback(get_input_form->ok_button, 
						 do_print_items_from_browser, form_id);
  fl_set_object_label(get_input_form->label,"Enter Print Command:");
  
  show_form(get_input_form->ldata);
 DONE:
  gms_unblock_ints(old_mask);
}

/****************************************************************************/
/*                                                                          */
/*                         do_print_all_from_browser              */
/*                                                                          */
/****************************************************************************/
void do_print_all_from_browser(FL_OBJECT *button, long form_id)
{
  /* form_id =   0: for FD_logfile_ form
   *             1: for FD_report_form
   *             2: for FD_gms_form
   *             3: for FD_group_form
   *             5: for FD_msg_form
   */
  FD_report_form *report_form;
  FD_logfile_form *logfile_form;
  FD_gms_form *gms_form;
  FD_group_form *group_form;
  FD_msg_form *msg_form;
  long old_mask;
  FL_OBJECT *browser = NULL;
  FD_get_input_form *get_input_form = NULL;

  old_mask = gms_block_ints();
  if (show_button_info_flag)
	hide_button_info_msg();
  if (button == NULL || button->u_vdata == NULL) {
	gms_unblock_ints(old_mask);
	return;
  }
  switch ((int)form_id) {
  case LOGFILE_FORM_ID:
	logfile_form = (FD_logfile_form *) button->u_vdata;
	browser = logfile_form->browser;
	get_input_form = logfile_form->vdata;
	break;
  case SUMMARY_REPORT_FORM_ID:
	report_form = (FD_report_form *)button->u_vdata;
	browser = report_form->browser;
	get_input_form = report_form->vdata;
	break;
  case GMS_FORM_ID:
	gms_form = (FD_gms_form *) button->u_vdata;
	browser = gms_form->browser;
	get_input_form = gms_form->vdata;
	break;
  case GROUP_FORM_ID:
	group_form = (FD_group_form *) button->u_vdata;
	browser = group_form->browser;
	get_input_form = group_form->vdata;
	break;
  case SYS_MSG_FORM_ID:
  case ALL_DATA_FORM_ID:
  case EXCEEDED_THRESHOLD_FORM_ID:
	msg_form = (FD_msg_form *) button->u_vdata;
	browser = msg_form->browser;
	get_input_form = msg_form->vdata;
	break;
  default:
	fprintf(stderr, "do_print_all_from_browser: invalide formid, %d\n", (int)form_id);
	goto DONE;
  }

  if (get_input_form == NULL ||
	  get_input_form->ok_button == NULL || get_input_form->label == NULL ||
	  get_input_form->get_input_form == NULL)
	goto DONE;
  current_print_browser = browser;
  current_print_match_str = EMPTY_STR;
  fl_set_object_callback(get_input_form->ok_button, 
						 do_print_items_from_browser, form_id);
  fl_set_object_label(get_input_form->label,"Enter Print Command:");

  
  show_form(get_input_form->ldata);
 DONE:
  gms_unblock_ints(old_mask);

} /* do_print_all_from_browser */



int update_file_status_report_form(FL_FORM *form,
	 FL_OBJECT *browser, char *selected_items[], int nitems, int type)
{
  int i;
  int update = 0;

  if (form == NULL || !form->visible || browser == NULL) return update;
  fl_freeze_form(form);

  fl_clear_browser(browser);
  for (i = 0; i < nitems; i++) {
	if (selected_items[i] == NULL) continue;
	update = 1;
	load_items_to_browser(form, browser, (int)type, selected_items[i], CLEAR);
  }  
  fl_unfreeze_form(form);

  return update;
}

/****************************************************************************/
/*                                                                          */
/*                         popup_file_status_report_form               */
/*                                                                          */
/****************************************************************************/
void popup_file_status_report_form (FL_OBJECT *button, long type)
{
  FD_logfile_form *form;
  char title[MAX_LABEL_LEN];
  FL_OBJECT *browser = NULL;
  FD_gms_form *gms_form;
  long oldmask;
  int update;

  oldmask = gms_block_ints();
  if (show_button_info_flag)
	hide_button_info_msg();
  if (button == NULL) {
	gms_unblock_ints(oldmask);
	return;
  }

  memset(title, '\0', MAX_LABEL_LEN);

  switch (type) {
#if 0
  case  LOG_FILE_RNG:
	form = logfile_report_form;
	if (form == NULL || form->logfile_form == NULL) 
	  break;
	/* Only set pixmaps if form is not visible */
	if (!form->logfile_form->visible)
	  set_pixmapbuttons_for_logfile_form(form);
	/* show form before load data to form */
	show_form(form->ldata);

	report_form = button->u_vdata;
	if (report_form == NULL || report_form->browser == NULL || 
		report_form->report_form == NULL)
	  break;

	fl_deactivate_object(report_form->browser);

	browser = report_form->browser;
	
	fl_freeze_form(report_form->report_form);
	free_array_of_str(log_selected_items, MAX_ITEMS);
	get_selected_items(browser, log_selected_items, 
					   (int)type, MAX_ITEMS,  MAX_ITEMNAME_LEN);

	fl_deselect_browser(browser);

	fl_unfreeze_form(report_form->report_form);
	fl_activate_object(report_form->browser);

	fl_deactivate_object(form->browser);
	update = update_file_status_report_form(form->logfile_form, form->browser, 
								   log_selected_items, MAX_ITEMS, type); 
	fl_activate_object(form->browser);
	break;
#endif
  case STAT_INFO_FILE_RNG:
	/* not report type */
	form = job_grouping_form;
	if (form == NULL || form->logfile_form == NULL || form->browser == NULL)
	  break;
	/* Only set pixmap if form is not visible */
	if (!form->logfile_form->visible)
	  set_pixmapbuttons_for_logfile_form(form);
	/* show form before load data to form */
	show_form(form->ldata);

	
	gms_form = button->u_vdata;
	if (gms_form == NULL || gms_form->gms_form == NULL ||
		gms_form->browser == NULL)
	  break;

	fl_deactivate_object(gms_form->browser);
	browser = gms_form->browser;

	fl_freeze_form(gms_form->gms_form);
	free_array_of_str(curr_selected_prod_groups, MAX_ITEMS);
	get_selected_items(browser, curr_selected_prod_groups, 
					   (int)type, MAX_ITEMS,  MAX_ITEMNAME_LEN);

	fl_deselect_browser(browser);

	fl_unfreeze_form(gms_form->gms_form);
	fl_activate_object(gms_form->browser);

	fl_deactivate_object(form->browser);
	update = update_file_status_report_form(form->logfile_form,form->browser, 
								   curr_selected_prod_groups, MAX_ITEMS, type); 
	fl_activate_object(form->browser);

	break;
  }

  fl_activate_object(button);

  gms_unblock_ints(oldmask);
}

/****************************************************************************/
/*                                                                          */
/*                         clean_all_data              */
/*                                                                          */
/****************************************************************************/
void clean_all_data()
{
  struct tape_info_rec *tape_info;
  struct host_info_rec *host_info;
  int i;

    /* free host info list */
  while(host_info_list) {
	host_info = host_info_list;	
	host_info_list = host_info_list->next;
	free_host_info(host_info);
  }
  host_info_list = NULL;

  /* free tape info list */
  while (tape_info_list) {
	tape_info = tape_info_list;
	tape_info_list = tape_info_list->next;
	free_tape_info(tape_info);
  }
  tape_info_list = NULL;

  /* free graphic status rec */
  free_gstatus_rec(gstatus_info);

  /* free prog lists */
  for (i = 0; i <MAX_PROGS; i++) {
	if (Gselected_progs[i]) {
	  free(Gselected_progs[i]);
	  Gselected_progs[i] = NULL;
	}
	if (available_progs[i]) {
	  free(available_progs[i]);
	  available_progs[i] = NULL;
	}
  }

  /* free host lists */
  for (i = 0; i <MAX_HOSTS; i++) {
	if (Gselected_hosts[i]) {
	  free(Gselected_hosts[i]);
	  Gselected_hosts[i] = NULL;
	}
	if (available_hosts[i]) {
	  free(available_hosts[i]);
	  available_hosts[i] = NULL;
	}
	if (Gavailable_network_hosts[i]) {
	  free(Gavailable_network_hosts[i]);
	  Gavailable_network_hosts[i] = NULL;
	}
  }

  free_array_of_str(curr_selected_prod_groups, MAX_ITEMS);
  free_array_of_str(log_selected_items, MAX_ITEMS);

} /* clean_all_data */

/****************************************************************************/
/*                                                                          */
/*                         clear_report_form              */
/*                                                                          */
/****************************************************************************/
void clear_report_form(FD_report_form *form)
{
  if (form == NULL || form->report_form == NULL ||
	  !form->report_form->visible || form->browser == NULL) return;
  
  fl_clear_browser(form->browser);

} /* clear_report_form */

/****************************************************************************/
/*                                                                          */
/*                         clear_logfile_form              */
/*                                                                          */
/****************************************************************************/
void clear_logfile_form(FD_logfile_form *form)
{
  if (form == NULL || form->logfile_form == NULL ||
	  !form->logfile_form->visible || form->browser == NULL) return;
  
  fl_clear_browser(form->browser);
} /* clear_logfile_form */

/****************************************************************************/
/*                                                                          */
/*                         clear_msg_form              */
/*                                                                          */
/****************************************************************************/
void clear_msg_form(FD_msg_form *form)
{
  if (form == NULL || form->msg_form == NULL ||
	  !form->msg_form->visible || form->browser == NULL) return;
  
  fl_clear_browser(form->browser);
} /* clear_msg_form */

/****************************************************************************/
/*                                                                          */
/*                         clear_product_grouping_form              */
/*                                                                          */
/****************************************************************************/
void clear_product_grouping_form(FD_gms_form *form)
{
  int index;

  if (form == NULL || form->gms_form == NULL ||
	  !form->gms_form->visible || form->browser == NULL) return;

  fl_freeze_form(form->gms_form);
  fl_deactivate_object(form->browser);
  fl_clear_browser(form->browser);
  
  /* reset view type */
  index = GMSF_VIEW_OPTION_IND;
  forms_view_options[index] = VIEW_DEFAULT;
  set_view_menu_options(form->view_menu, &(forms_view_options[index]), GMS_FORM_ID);

  strcpy(last_job_status_msg, "");
  fl_activate_object(form->browser);
  fl_unfreeze_form(form->gms_form);
} /* clear_product_grouping_form */

/****************************************************************************/
/*                                                                          */
/*                         clear_group_form              */
/*                                                                          */
/****************************************************************************/
void clear_group_form(FD_group_form *form)
{
  if (form == NULL || form->group_form == NULL ||
	  !form->group_form->visible || form->browser == NULL) return;
  
  fl_clear_browser(form->browser);
  fl_set_object_label(form->status_label, EMPTY_STR);

  /* Turn light buttons off */
  fl_set_button(tape_grouping_form->failure_light_button, RELEASED);
  fl_set_button(tape_grouping_form->running_light_button, RELEASED);
  fl_set_button(tape_grouping_form->warning_light_button, RELEASED);
  fl_set_button(tape_grouping_form->unknown_light_button, RELEASED);

  strcpy(last_job_status_msg, "");
} /* clear_group_form */


/****************************************************************************/
/*                                                                          */
/*                         clear_status_form              */
/*                                                                          */
/****************************************************************************/
void clear_status_form(struct graphic_status_rec *gstat)
{
  FD_status_form *form;
  int i;
  char label[MAX_LABEL_LEN];
  int index;

  if (gstat == NULL || gstat->sform == NULL ||
	  gstat->sform->status_form  == NULL || 
	  !gstat->sform->status_form->visible ||
	  gstat->sform->slider == NULL ||
	  gstat->sform->label == NULL)
	
	return;

  form = gstat->sform;
  fl_deactivate_object(form->slider);
  fl_freeze_form(form->status_form);

  /* update form's label   */
  memset(label, '\0', MAX_LABEL_LEN);
  sprintf(label, " %d", 
		  get_nlogfiles_downward(gstat->logfiles));
  fl_hide_object(gstat->sform->label);
  fl_set_object_label(gstat->sform->label, label);
  fl_show_object(gstat->sform->label);

  for (i = 0; i < MAX_CHARTS; i++) {
	if (gstat->chart_list[i] == NULL) continue;
	clear_chart(gstat->chart_list[i]);
	gstat->chart_list[i]->logfile = NULL;
  }
  /* reset view type */
  if (form->view_menu) {
	index = STATUSF_VIEW_OPTION_IND;
	forms_view_options[index] = VIEW_DEFAULT;
	set_view_menu_options(form->view_menu, &(forms_view_options[index]), 
						  STATUS_FORM_ID);
  }
  fl_unfreeze_form(form->status_form);
  fl_activate_object(form->slider);

} /* clear_status_form */

/****************************************************************************/
/*                                                                          */
/*                         clear_gstatus_rec              */
/*                                                                          */
/****************************************************************************/
void clear_gstatus_rec(struct graphic_status_rec *gstat)
{
  /* free all member except its form and charts. 
   */
  struct logfile_rec *logfile, *tmp_logfile;

  if (gstat == NULL) return;
  logfile = gstat->logfiles;

  while (logfile) {
	tmp_logfile = logfile;
	logfile = logfile->next;
	free_logfile_rec(tmp_logfile);
	
  }
  if (gstat->gms_sorted_logfiles_array){
	free(gstat->gms_sorted_logfiles_array);
	gstat->gms_sorted_logfiles_array = NULL;
	gstat->ngms_array_items = 0;
  }

  if (gstat->bar_sorted_logfiles_array) {
	free(gstat->bar_sorted_logfiles_array);
	gstat->bar_sorted_logfiles_array = NULL;
	gstat->nbar_array_items = 0;
  }

  gstat->logfiles = NULL;
  gstat->nlogfiles = 0;
  clear_status_form(gstat);

} /* clear_gstatus_rec */

/****************************************************************************/
/*                                                                          */
/*                         reset_forms              */
/*                                                                          */
/****************************************************************************/
void reset_forms()
{

  extern FILE *test_file;
  extern char test_fname[];

  if (write_to_file) {
	/* reset test file */
	fclose(test_file);
	if ((test_file = fopen(test_fname, "w+")) == NULL) {
	  fprintf(stderr, "Failed to reopen %s\n", test_fname);
	  exit_system(NULL, -1);
	}
	
  }
  clear_report_form(summary_report_form);
  
  clear_logfile_form(logfile_report_form);
  clear_logfile_form(job_grouping_form);
  clear_product_grouping_form(product_grouping_form);
  clear_group_form(tape_grouping_form);
  clear_msg_form(all_data_form);
  clear_msg_form(exceeded_threshold_form);

} /* reset_forms */

/****************************************************************************/
/*                                                                          */
/*                         reset_data              */
/*                                                                          */
/****************************************************************************/
void reset_data()
{
  struct tape_info_rec *tape_info;

  /* free tape info list */

  while (tape_info_list) {
	tape_info = tape_info_list;
	tape_info_list = tape_info_list->next;
	free_tape_info(tape_info);
  }
  tape_info_list = NULL;

#if 0
  /* free available and selected prog list */
  for (i = 0; i <MAX_PROGS; i++) {
	if (available_progs[i])  {
	  free(available_progs[i]);
	  available_progs[i] = NULL;
	}
	if (Gselected_progs[i])  {
	  free(Gselected_progs[i]);
	  Gselected_progs[i] = NULL;
	}
  }
#endif
  free_array_of_str(curr_selected_prod_groups, MAX_ITEMS);
  free_array_of_str(log_selected_items, MAX_ITEMS);
  strcpy(curr_selected_tape_group, "");
  free_all_tape_groups();
  clear_gstatus_rec(gstatus_info);

  reset_forms();

  all_grps_stat = 0;
#if 0
  /* Clear available neighbor list */
  free_array_of_str(available_hosts, MAX_HOSTS);
  update_forms_affected_by_neighbor_change(AVAILABLE_HOSTS, 1);
#endif
#if 0
  /* read progs & neighbors from file, .gms */
  read_gms_init_file();


  update_forms_affected_by_prog_change(SELECTED_PROGS, 1);
  update_forms_affected_by_prog_change(AVAILABLE_PROGS, 1);

  update_forms_affected_by_neighbor_change(SELECTED_HOSTS, 1);
  update_forms_affected_by_neighbor_change(AVAILABLE_HOSTS, 1);
#endif
} /* reset_data */

/****************************************************************************/
/*                                                                          */
/*                         do_reset              */
/*                                                                          */
/****************************************************************************/
void do_reset(FL_OBJECT *button, long val)
{
  /* send reset msg to monitorps. clear old status info. Not reread data
   * from ~/.gms.
   */
  char msg[MAX_MSG_LEN];
  long old_mask;

  old_mask = gms_block_ints();
  if (show_button_info_flag)
	hide_button_info_msg();
  if (button == NULL) 
	goto DONE;

  fl_deactivate_object(button);

  reset_data();

  /* send msg to monitorps */
  memset(msg, '\0', MAX_MSG_LEN);
  sprintf(msg, "%s %s.%d", RESET_STR, local_host,gms_portnum); 
  if (send_msg(local_host, msg) < 0)
	exit_system(NULL, -1);

  /* reset alarm for updating forms*/
  alarm(0);
  alarm(UPDATE_FORMS_DEFAULT);

  fl_activate_object(button);
  DONE:
  gms_unblock_ints(old_mask);

} /* do_reset */


/****************************************************************************/
/*                                                                          */
/*                         do_write_browser_to_file              */
/*                                                                          */
/****************************************************************************/
void do_write_browser_to_file(FL_OBJECT *button, long form_id)
{
  FD_report_form *rpt_form;
  FD_logfile_form *log_form;
  FD_get_input_form *get_input_form;
  long old_mask;
  FD_gms_form *gm_form;
  FD_group_form *group_form;
  FD_msg_form *msg_form;

  old_mask = gms_block_ints();
  if (show_button_info_flag)
	hide_button_info_msg();
  if (button == NULL || button->u_vdata == NULL) {
	gms_unblock_ints(old_mask);
	return;
  }

  switch (form_id) {
  case LOGFILE_FORM_ID: /* logfile */
	log_form = button->u_vdata;
	get_input_form = log_form->vdata;
	if (get_input_form == NULL) goto DONE;
	break;

  case SUMMARY_REPORT_FORM_ID:  /* report form */
	rpt_form = button->u_vdata;
	get_input_form = rpt_form->vdata;
	if (get_input_form == NULL) goto DONE;
	break;
  case GMS_FORM_ID:
	gm_form = button->u_vdata;
	get_input_form = gm_form->vdata;
	if (get_input_form == NULL) goto DONE;
	break;
  case GROUP_FORM_ID:
	group_form = button->u_vdata;
	get_input_form = group_form->vdata;
	if (get_input_form == NULL) goto DONE;
	break;
  case ALL_DATA_FORM_ID:
  case SYS_MSG_FORM_ID:
  case EXCEEDED_THRESHOLD_FORM_ID:
	msg_form = button->u_vdata;
	get_input_form = msg_form->vdata;
	if (get_input_form == NULL) goto DONE;
	break;
  default:
	goto DONE;
  }
  if (get_input_form->ok_button == NULL || get_input_form->label == NULL ||
	  get_input_form->get_input_form == NULL)
	goto DONE;

  fl_set_object_callback(get_input_form->ok_button, 
						 write_browser_to_file, form_id);
  fl_set_object_label(get_input_form->label,"Enter filename:");

  
  show_form(get_input_form->ldata);
 DONE:
  gms_unblock_ints(old_mask);
}

/****************************************************************************/
/*                                                                          */
/*                               color_to_stat_str                          */
/*                                                                          */
/****************************************************************************/
char *color_to_stat_str(int color_id)
{
  char *stat_str = UNKNOWN_STR;

  switch (color_id) {
  case FAILURE_COLOR: stat_str = FAILED_STR; break;
  case WARNING_COLOR: stat_str = WARNING_STR; break;
  case RUNNING_COLOR: stat_str = RUNNING_STR; break;
  case ABORT_COLOR: stat_str = ABORT_STR; break;
  case SUCCESS_COLOR: stat_str = SUCCESS_STR; break; 
  case DEFAULT_COLOR: stat_str = UNKNOWN_STR; break;
  default: stat_str = UNKNOWN_STR; break;
  }
  return stat_str;
}

/****************************************************************************/
/*                                                                          */
/*                          get_stat_str_from_browser_line                  */
/*                                                                          */
/****************************************************************************/
char *get_stat_str_from_browser_line(char *line)
{
  /* get color id from line: %s@C%d (text_format, color_id) */
  char *stat_str = UNKNOWN_STR;
  char code_str[MAX_STR_LEN], *tmp_str;
  int color_i;

  if (line == NULL || strchr(line, ' ') == NULL) return stat_str;

  /* get color id from code: %s@C%d (text_format, color_id) */
  memset(code_str, '\0', MAX_STR_LEN);
  strncpy(code_str, line, strlen(line) - strlen(strchr(line, ' ')));
  tmp_str = strstr(code_str, "@C");
  if (tmp_str) {
	/* skip to color id */
	tmp_str += 2;
	color_i = atoi(tmp_str);
	stat_str = color_to_stat_str(color_i);
  }
  return stat_str;
}

/****************************************************************************/
/*                                                                          */
/*                         write_browser_to_file              */
/*                                                                          */
/****************************************************************************/
void write_browser_to_file(FL_OBJECT *button, long form_id)
{

  char *filename;
  FD_get_input_form *form;
  FD_gms_form *gm_form;
  long oldmask;
  FL_OBJECT *browser, *save_button;
  FD_report_form *rpt_form;
  FD_logfile_form *log_form;
  FD_group_form *group_form;
  FD_msg_form *msg_form;


  /* this is to prevent interrupts while  accessing internal lists */

  oldmask = gms_block_ints();

  if (button == NULL || button->u_vdata == NULL) {
	gms_unblock_ints(oldmask);
	return;
  }
  fl_deactivate_object(button);

  form = button->u_vdata; /* get input form */
  switch (form_id) {
  case LOGFILE_FORM_ID: 
	/* current file status report and report's file status report forms */
	log_form = (FD_logfile_form *) form->vdata;
	if (log_form == NULL || log_form->browser == NULL || 
		log_form->save_button == NULL)
	  goto DONE;
	browser = log_form->browser;
	save_button = log_form->save_button;
	break;
  case SUMMARY_REPORT_FORM_ID: 
	/* report form */
	rpt_form = (FD_report_form *)form->vdata;
	if (rpt_form == NULL || rpt_form->browser == NULL || 
		rpt_form->save_button == NULL) goto DONE;
	browser = rpt_form->browser;
	save_button = rpt_form->save_button;
	break;

  case GMS_FORM_ID:
	gm_form = (FD_gms_form *)form->vdata;
	if (gm_form == NULL || gm_form->browser == NULL || 
		gm_form->save_button == NULL) goto DONE;
	browser = gm_form->browser;
	save_button = gm_form->save_button;
	break;

  case GROUP_FORM_ID:
	group_form = (FD_group_form *)form->vdata;
	if (group_form == NULL || group_form->browser == NULL || 
		group_form->save_button == NULL) goto DONE;
	browser = group_form->browser;
	save_button = group_form->save_button;
	break;
  case ALL_DATA_FORM_ID:
	msg_form = (FD_msg_form  *) form->vdata;
	if (msg_form == NULL || msg_form->browser == NULL) goto DONE;
	browser = msg_form->browser;
	save_button = NULL;
	break;

  case EXCEEDED_THRESHOLD_FORM_ID:
	msg_form = (FD_msg_form  *) form->vdata;
	if (msg_form == NULL || msg_form->browser == NULL) goto DONE;
	browser = msg_form->browser;
	save_button = NULL;
	break;	
  default:
	goto DONE;
  }
	
  if (form->input == NULL) goto DONE;
  filename = (char *)fl_get_input(form->input);
  if (filename == NULL || strlen(filename) == 0) {
	fl_activate_object(button);
	goto DONE;
  }

  /* Send to file */
  send_items_from_browser_to_file_or_printer(browser, form_id,
											 EMPTY_STR, NULL,
											 filename);

  do_clear_input(form->clear_button, 0);
  fl_activate_object(button);
  if (save_button)
	fl_activate_object(save_button);
  do_close_form(form->cancel_button, 0);
DONE:

  gms_unblock_ints(oldmask);

} /* write_browser_to_file */
/****************************************************************************/
/*                                                                          */
/*                         update_exceeded_threshold_form                   */
/*                                                                          */
/****************************************************************************/
void update_exceeded_threshold_form(FL_OBJECT *button, long val)
{
  long old_mask;
  old_mask = gms_block_ints();
  if (exceeded_threshold_form == NULL) goto DONE;
  
  threshold_time = fl_get_counter_value(exceeded_threshold_form->threshold_counter);
  
  load_items_to_browser(exceeded_threshold_form->msg_form, 
						exceeded_threshold_form->browser, 
						EXCEEDED_THRESHOLD_JOBS, "", CLEAR);
DONE:
  gms_unblock_ints(old_mask);
} /* update_exceeded_threshold_form */

/****************************************************************************/
/*                                                                          */
/*                         popup_exceeded_threshold_form                    */
/*                                                                          */
/****************************************************************************/
void popup_exceeded_threshold_form(FL_OBJECT *button, long val)
{
  long old_mask;

  old_mask = gms_block_ints();
  if (show_button_info_flag)
	hide_button_info_msg();
  if (exceeded_threshold_form == NULL || exceeded_threshold_form->msg_form == NULL) {
	fprintf(stderr, "ERROR: There is no exceeded runtime form.\n");
	goto DONE;
  }
  show_form(exceeded_threshold_form->ldata);

  load_items_to_browser(exceeded_threshold_form->msg_form, 
						exceeded_threshold_form->browser, 
						EXCEEDED_THRESHOLD_JOBS, "", CLEAR);

  DONE:
  gms_unblock_ints(old_mask);
} /* popup_exceeded_threshold_form */

/****************************************************************************/
/*                                                                          */
/*                         show_tape_info_list                              */
/*                                                                          */
/****************************************************************************/
void show_tape_info_list ()
{
  struct tape_info_rec *tape_info;
  int i;

  tape_info = tape_info_list;
  while (tape_info) {
	if (!tape_info) continue;
	
	fprintf(stderr, "tape info list:\n");

	for (i = 0; i < MAX_PRODS; i++) {
	  fprintf(stderr, "i = %d\n", i);
	  show_logfile_list(tape_info->logfiles[i]);
	}
	tape_info = tape_info->next;
  }
}

/****************************************************************************/
/*                                                                          */
/*                         do_refresh_summary_form                           */
/*                                                                          */
/****************************************************************************/
void do_refresh_summary_form(FL_OBJECT *button, long val)
{
  update_summary_report_form();
} /* do_refresh_summary_form */


/****************************************************************************/
/*                                                                          */
/*                         prog_exists_in_level                             */
/*                                                                          */
/****************************************************************************/
int prog_exists_in_level(int level, char *prog)
{
  /* Return 1 if prog exists in the specified level; 0, otherwise.  */
  int ll, p;

  if (prog == NULL || level < 1 || level > MAX_GV_LEVELS_NUM) return 0;
  ll = level -1;
  for (p=0; p < MAX_PROGS_PER_LEVEL; p++) {
	if (level_info_table[ll].prog_info[p].prog == NULL) continue;
	if (strcmp(level_info_table[ll].prog_info[p].prog, prog) == 0) 
	  return 1;
  }

  return 0;
  
} /* prog_exists_in_level */

/****************************************************************************/
/*                                                                          */
/*                        do_modify_gv_levels_table                         */
/*                                                                          */
/****************************************************************************/
void do_modify_gv_levels_table(FL_OBJECT *button, long modify_type)
{
  long oldmask;
  FD_gv_levels_form *form;
  int level,ll, p;
  char * program, *level_str, max_levels_str[MAX_NAME_LEN];

  oldmask = gms_block_ints();

  if (button == NULL || button->u_vdata == NULL) {
	gms_unblock_ints(oldmask);
	return;
  }

  fl_deactivate_object(button);
  form = (FD_gv_levels_form *) button->u_vdata;
  /* Get level #. */
  level_str = (char *) fl_get_input(form->level_input);
  if (level_str == NULL) goto DONE;
  level = atoi(level_str);

  if (level < 1 || level > MAX_GV_LEVELS_NUM) {
	sprintf(max_levels_str, "%d", MAX_GV_LEVELS_NUM);
	fl_show_message("WARNING: Level is out of range.", "It must be from 0 to", 
					max_levels_str);
	goto DONE;
  }

  /* Get program */
  program = (char *) fl_get_input(form->program_input);
  if (program == NULL || strlen(program) < 1) goto DONE;

  switch((int) modify_type) {
  case 1: /* add */
	ll = level - 1;
	/* Don't want to add duplicates */
	if (prog_exists_in_level(level, program)) goto DONE;
	for (p = 0; p < MAX_PROGS_PER_LEVEL; p++) {
	  if (level_info_table[ll].prog_info[p].prog == NULL) {
		/* Add to this empty entry */
		level_info_table[ll].prog_info[p].prog = (char *) strdup(program);
		level_info_table[ll].nprogs++;
		goto UPDATE;
	  }
	}
	/* List is overflow */
	fl_show_message("Warning:", "Number of programs exceeds the", "limit for each level. Ignore.");
	break;
  case 2:
	/* Delete */
	ll = level - 1;
	if (!prog_exists_in_level(level, program)) goto DONE; /* Doesnot exist.*/
	for (p = 0; p < MAX_PROGS_PER_LEVEL; p++) {
	  if (level_info_table[ll].prog_info[p].prog == NULL) continue;
	  /* Set its entry to NULL--*/
	  if (strcmp(level_info_table[ll].prog_info[p].prog, program) == 0) {
		free(level_info_table[ll].prog_info[p].prog);
		level_info_table[ll].prog_info[p].prog = NULL;
		level_info_table[ll].nprogs--;
		goto UPDATE;
	  }
	}
	break;
  default:
	break;
  }
UPDATE:
  update_gv_levels_form();
  update_summary_report_form();
  update_gms_init_file();

DONE:
  fl_activate_object(button);
  gms_unblock_ints(oldmask);
} /*  do_modify_gv_levels_table */


/****************************************************************************/
/*                                                                          */
/*                        select_and_show_summary_report                    */
/*                                                                          */
/****************************************************************************/
void select_and_show_summary_report(FL_OBJECT *button, long summary_type)
{
  /* Select level type and update summary report form */
  long oldmask;
  FD_report_form *form;

  oldmask = gms_block_ints();

  if (button == NULL || button->u_vdata == NULL) {
	gms_unblock_ints(oldmask);
	return;
  }
  fl_deactivate_object(button);
  form = (FD_report_form *) button->u_vdata;
  /* Unselect the other buttons */
  if (form->tape_button != button)
	fl_set_button(form->tape_button, 0);
  if (form->level_button != button)
	fl_set_button(form->level_button, 0);
  if (form->product_button != button)
	fl_set_button(form->product_button, 0);

  fl_set_button(button, 1);

  selected_summary_level_type = (summary_type_t) summary_type;
  update_summary_report_form();
  update_active_fields();

  fl_activate_object(button);
  gms_unblock_ints(oldmask);
} /* select_and_show_summary_report */




/* code for testing -- simulate the monitor daemon sending msg to GUI. */
#if 1
extern FD_test_form *test_form;


/****************************************************************************/
/*                                                                          */
/*                         popup_test_form              */
/*                                                                          */
/****************************************************************************/
void popup_test_form(FL_OBJECT *button, long val)
{
  long old_mask;

  old_mask = gms_block_ints();
  if (show_button_info_flag)
	hide_button_info_msg();
  if (button == NULL || test_form->test_form == NULL) {
	gms_unblock_ints(old_mask);
	return;
  }
 
  show_form(test_form->ldata);
  gms_unblock_ints(old_mask);
}
#include <dirent.h>
/****************************************************************************/
/*                                                                          */
/*                          read_file_to_buf              */
/*                                                                          */
/****************************************************************************/
void  read_file_to_buf(char *filename, char *bufptr)
{
  FILE *fp;
  int m = MAX_DATA_LEN;
  char *buf, data[MAX_DATA_LEN];

  if (filename == NULL || bufptr == NULL)
	return;

  memset(data, '\0', MAX_DATA_LEN);
  buf = data;

  /* read data from file */
  if ((fp = fopen(filename, "r")) == NULL) {
	perror("open test logfile");
	fclose(fp);
	return;
  }
  while (m > 0 && fread(buf, sizeof(char), m, fp)) {
	m -= strlen(buf);
	buf += strlen(buf);
  }
  *buf = '\0';
  strcpy(&(*bufptr), data);
/*
printf("read fiel: bufptr = <%s>\n", bufptr);  
*/
  fclose(fp);  
}


/****************************************************************************/
/*                                                                          */
/*                         do_read_logfiles              */
/*                                                                          */
/****************************************************************************/
void do_read_logfiles(FL_OBJECT *button, long val)
{
#if 0
  /* read logfiles from test dir */
  char fname [MAX_FILENAME_LEN];
  DIR *dfd;
  struct dirent *dp;
  char buf[MAX_DATA_LEN], tmpbuf[MAX_DATA_LEN];
  char *dir = TESTDATA_PATH;
  char prod[MAX_PRODNAME_LEN];
  char tapeid[MAX_TAPEID_LEN];
  long old_mask;

  old_mask = gms_block_ints();
  if (button == NULL) {
	gms_unblock_ints(old_mask);
	return;
  }
  /*
  do_close_test(button, val);
  */
  /* read logfile from test dir */
  if ((dfd = opendir(dir)) == NULL) {
	perror ("opendir");
	gms_unblock_ints(old_mask);
	return;
  }

  while ((dp = readdir(dfd)) != NULL) {
	if ((strcmp(dp->d_name, ".") == 0) ||
		(strcmp(dp->d_name, "..") == 0) || 
		(strstr(dp->d_name, ".log") == NULL) ) continue;
	
	memset(buf, '\0', MAX_DATA_LEN);
	memset(tmpbuf, '\0', MAX_DATA_LEN);
	memset(fname, '\0', MAX_FILENAME_LEN);

	/* put logfile name and log info into buffer */
	sprintf(fname, "%s/%s", dir,dp->d_name);
	read_file_to_buf(fname, tmpbuf);
	sprintf(buf, "%s %s %s", LOG_FILE_KEY,dp->d_name, tmpbuf);
/*
   printf("read: buf ===== <%s>\n", buf);
*/

	process_log_file(local_host, buf, tapeid, prod);

  }
  closedir(dfd);
  update_forms_affected_by_logfile_change(1);
  gms_unblock_ints(old_mask);
#endif
}

/****************************************************************************/
/*                                                                          */
/*                         do_flush              */
/*                                                                          */
/****************************************************************************/
void do_flush(FL_OBJECT *button, long val)
{
  long old_mask;

  old_mask = gms_block_ints();
  fflush(stdout);
  fflush(stderr);
  gms_unblock_ints(old_mask);
} /* do_flush */

/****************************************************************************/
/*                                                                          */
/*                         show_tape_group_list              */
/*                                                                          */
/****************************************************************************/
void show_tape_group_list()
{
  struct tape_grouping_rec *grp;

  if (tape_groups == NULL) return;

  fprintf(stderr, "<<\n");
  grp = tape_groups;
  while (grp) {
	if (grp->last_logfile)
	  fprintf(stderr, "grp: %s.%s, %s\n", grp->host, grp->tapeid, grp->last_logfile->name);
	else 
	  fprintf(stderr, "grp: %s.%s.\n", grp->host, grp->tapeid);
	grp = grp->next;
  }
  fprintf(stderr, ">>\n");
}

/****************************************************************************/
/*                                                                          */
/*                         do_show_logfile_list              */
/*                                                                          */
/****************************************************************************/
void do_show_logfile_list(FL_OBJECT *button, long val)
{
  extern void show_logfile_list(struct logfile_rec *);
  long old_mask;

  old_mask = gms_block_ints();
  if (gstatus_info){
	printf("gstatus_info list forward--nlogs: %d:\n", gstatus_info->nlogfiles);
	show_logfile_list(gstatus_info->logfiles);
/*
	printf("backward:\n");
	show_logfile_list_backward(gstatus_info->logfiles);
*/
  }
  show_tape_group_list();
  show_tape_info_list ();

  gms_unblock_ints(old_mask);

}

/****************************************************************************/
/*                                                                          */
/*                         do_test_update_history              */
/*                                                                          */
/****************************************************************************/
void do_test_update_history(FL_OBJECT *button, long val)
{
  long old_mask;

  old_mask = gms_block_ints();
#if 0
  do_read_logfiles(button, val);

  char fname [MAX_FILENAME_LEN];
  DIR *dfd;
  struct dirent *dp;
  char *dir = LOCAL_LOGFILE_PATH;
  char *tname;

  char prod[MAX_PRODNAME_LEN];
  char tapeid[MAX_TAPEID_LEN];
  char fromhost[MAX_HOSTNAME_LEN];
  struct tape_info_rec *tape_info;
  char logfile[MAX_FILENAME_LEN];
  /*
  do_close_test(button, val);
  */
  /* read logfile from dir */
  if ((dfd = opendir(dir)) == NULL) {
	perror ("opendir");
	return;
  }

  while ((dp = readdir(dfd)) != NULL) {
	if ((strcmp(dp->d_name, ".") == 0) ||
	  (strcmp(dp->d_name, "..") == 0)) continue;

	memset(prod, '\0', MAX_PRODNAME_LEN);
	memset(fromhost, '\0', MAX_HOSTNAME_LEN);
	memset(tapeid, '\0', MAX_TAPEID_LEN);

	tname = strchr(dp->d_name, '.');
	strncpy(fromhost, dp->d_name, strlen(dp->d_name) - strlen(tname));
	*(fromhost+strlen(fromhost)) = '\0';
	tname++;
	strncpy(tapeid, tname, strlen(tname) - strlen(strchr(tname, '.')));
	*(tapeid+strlen(tapeid)) = '\0';
	tname = strchr(tname, '.')+1;
	strncpy(prod, tname, strlen(tname) - strlen(strchr(tname, '.')));
	*(prod + strlen(prod)) = '\0';
/*
printf("from host = %s, tapeid = %s, prod = %s\n", fromhost, tapeid, prod);
*/
	/* create tape info if not already exists and add to list */
	if ((tape_info = get_tape_info(tapeid)) == NULL) {
	  if ((tape_info = new_tape_info(tapeid)) == NULL) return;
	  add_tape_info(tape_info);
	}
	sprintf(logfile, "%s/%s", dir, dp->d_name);
	generate_report_from_logfile(fromhost, tape_info, prod, );
  }
  closedir(dfd);
  update_summary_report_form();
  update_log_file_status_report();
#endif
  gms_unblock_ints(old_mask);
} /* do_test_update_history*/


/****************************************************************************/
/*                                                                          */
/*                         do_test_get_sinfo              */
/*                                                                          */
/****************************************************************************/
void do_test_get_sinfo(FL_OBJECT *button, long val)
{
  FD_get_input_form *form;
  char *filename, fname[MAX_FILENAME_LEN];
  char *buf, data[MAX_DATA_LEN];
  char *fromhost;
  char *dir = TESTDATA_PATH;
  long old_mask;

  old_mask = gms_block_ints();
 
  if (button == NULL || button->u_vdata == NULL)
	goto DONE;

  fl_deactivate_object(button);
  form = button->u_vdata;
  if (form->input == NULL) 
	goto DONE;

  filename = (char *)fl_get_input(form->input);
  if (filename == NULL || strlen(filename) == 0) {
	fl_activate_object(button);
	gms_unblock_ints(old_mask);
	return;
  }
  memset(data, '\0', MAX_DATA_LEN);  
  buf = data;
  sprintf(fname, "%s/%s",dir , filename);

  read_file_to_buf(fname, data);
/*
  printf("get sinfo: buf ===== <%s>\n", data);
*/
  fromhost = filename;
  if (strchr(fromhost, '.'))
	*(fromhost+strlen(strchr(fromhost, '.'))-1) = '\0';
  
  process_status_info(fromhost, data);
  update_status_form(gstatus_info->scroll_per, 0);
  update_summary_report_form();
  update_log_file_status_report();

  update_prod_group_form(1);
  update_curr_file_status_report();


  do_clear_input(form->clear_button, 0);
  fl_activate_object(button);
  do_close_form(form->cancel_button, 0);
 DONE:
  gms_unblock_ints(old_mask);
}

FD_get_input_form *test_get_input_form;
/****************************************************************************/
/*                                                                          */
/*                         do_test_get_stat_info              */

/****************************************************************************/
void do_test_get_stat_info(FL_OBJECT *button, long val)
{
  FD_test_form *form;
  long old_mask;

  old_mask = gms_block_ints();

  if (button == NULL || button->u_vdata == NULL ) {
	gms_unblock_ints(old_mask);
	return;
  }
  fl_deactivate_object(button);
  form = button->u_vdata;

  if (test_get_input_form == NULL) 
	test_get_input_form = new_get_input_form();
  if (test_get_input_form == NULL ||test_get_input_form->get_input_form == NULL ||
	  test_get_input_form->label == NULL) {
	gms_unblock_ints(old_mask);
	return;
  }
  test_get_input_form->get_input_form->u_vdata = form->get_stat_info_button;
  fl_set_object_label(test_get_input_form->label, "Logfile name:");
  fl_set_object_callback(test_get_input_form->ok_button, 
						 do_test_get_sinfo , 0);
  show_form(test_get_input_form->ldata);

  gms_unblock_ints(old_mask);
} /* do_test_get_stat_info_form */


/****************************************************************************/
/*                                                                          */
/*                         do_close_test              */
/*                                                                          */
/****************************************************************************/
void do_close_test(FL_OBJECT *button, long val)
{
  FD_test_form *form;
  long old_mask;

  old_mask = gms_block_ints();
  if (button == NULL || button->u_vdata == NULL ) {
	gms_unblock_ints(old_mask);
	return;
  }
  form = button->u_vdata;
  if (form->test_form)
	fl_hide_form(form->test_form);
  if (form->vdata)
	fl_activate_object(form->vdata);
  gms_unblock_ints(old_mask);
}
#endif



