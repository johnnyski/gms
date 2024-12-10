/* 
 * gts_monitor_form_cb.c
 */
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "forms.h"
#include "gts_monitor_form.h"
#include "gts_monitor.h"
#include "gms_comm.h"


extern struct tape_grouping_rec *tape_groups;  
extern FD_gms_form *gts_monitor_form;
extern char local_host[];
extern struct graphic_status_rec *gstatus_info;
extern int load_status_forms;
extern char curr_selected_tape_group[];
extern char *curr_selected_prod_groups[MAX_ITEMS];
char *remove_coding_chars(char * linestr)
{
  /* Remove form_code and '*' | '-', if any, from linestr.
   */
  char *tmpstr;

  if (linestr == NULL) return NULL;

  if ((tmpstr = strchr(linestr, ' ')) != NULL) 
	linestr = tmpstr+1;   /* skip to [*|-]host */
	
  /* remove finished char, '*' or '-' if any from linestr. */
  if (*linestr == FINISHED_CHAR || *linestr == NOT_ACTIVE_CHAR)
	linestr++;
  return linestr;
}


/***************************************************************************/
/*                                                                         */
/*                             get_selected_line_from_browser              */
/*                                                                         */
/***************************************************************************/
char * get_selected_line_from_browser(FL_OBJECT *browser)
{
  /* Get selected line for deletion.  Returns line with form_code and '*'|'-'
   * removed.
   */
  char *linestr = NULL;
  int linenum;


  if (browser == NULL) return NULL;
#ifdef XFORMS_081
  if (fl_show_question("",
					   "Do you really want to delete displayed job grouping from the log?",
					   "")) {
#else
  if (fl_show_question("Do you really want to delete displayed job grouping from the log?", 0)) {
#endif

	/* Yes delete. */
	
	/* Grab the selection. */
	linenum = fl_get_browser(browser);
	if (linenum == 0) {
	  fl_show_message("",
					  "Your selection has been overwritten by an update.",
					  "Please make your selection again."
					  );
	  return NULL;
	}

	linestr = (char *)fl_get_browser_line(browser, linenum);
	 /* linestr: form_code hostname tapeid product dev date time runtime totaljobs (gms_form)
	 * Remove form_code and '*' | '-', if any, from linestr.
	 */
	linestr = remove_coding_chars(linestr);

  }
  return linestr;
} /* get_selected_line_from_browser */

/***************************************************************************/
/*                                                                         */
/*                               send_del_job_to_monitorps                 */
/*                                                                         */
/***************************************************************************/
void send_del_job_to_monitorps(struct logfile_rec *logfile)
{
  char cmd[MAX_MSG_LEN];
  double host_diff_time;
  time_t stime, etime;
  char tmp_prog[MAX_PROGNAME_LEN+1];
  char tmp_tapeid[MAX_TAPEID_LEN+1];

  if (logfile == NULL) return;

  memset(cmd, '\0', MAX_MSG_LEN);

  /* Need to adjust time for particular host appropriately since the 
   * stored time was adjusted according to the local host's clock.
   */
  host_diff_time = get_host_diff_time(logfile->fromhost);
  stime = logfile->stime_item + host_diff_time;
  etime = logfile->etime_item + host_diff_time;

  strcpy(tmp_prog, logfile->prog);
  if (strlen(logfile->prog) == MAX_PROGNAME_LEN-1) {
	/* Append '*' to prog--it might have been truncated. */
	strcat(tmp_prog, "*");
  }
  strcpy(tmp_tapeid, logfile->tapeid);
  if (strlen(logfile->tapeid) == MAX_TAPEID_LEN-1) {
	/* Append '*' to tapeid--it might have been truncated. */
	strcat(tmp_tapeid, "*");
  }
  /* cmd:   del job host stime_secs etime_sec tapeid  device prog
   */
  sprintf(cmd,"%s%s %ld %ld %s %s %s", DEL_JOB_STR, logfile->fromhost,
		  stime,  etime, tmp_tapeid, logfile->tape_dev, tmp_prog);
  /*
fprintf(stderr, "tapeid: %s, %d, %s\n", logfile->tapeid, strlen(logfile->tapeid),cmd);
*/


  send_msg(local_host, cmd);

} /* send_del_job_to_monitorps */

/***************************************************************************/
/*                                                                         */
/*                          remove_job_grpings                             */
/*                                                                         */
/***************************************************************************/  
void remove_job_grpings(FL_OBJECT *browser)
{
  int linenum;
  char *linestr;
  char tapeid[MAX_TAPEID_LEN];
  char sdate[MAX_NAME_LEN], stime[MAX_NAME_LEN];
  char *selected_item[1], prog[MAX_PROGNAME_LEN];
  char host[MAX_HOSTNAME_LEN];
  struct logfile_rec *logfile = NULL;
  char tmp_str[MAX_NAME_LEN];

  if (browser == NULL) return;
  /* 1. Pop up a confirmation panel.
   * 2. Perform function of deleting a job entry by sending
   *    a message to monitorps.
   */

  linestr = get_selected_line_from_browser(browser);
  if (linestr == NULL) return;
  /*
   * Color characters and '*'|'-' finished marker are deleted. 
   * linestr: hostname tapeid product dev date time runtime totaljobs
   */
  memset(host, '\0', MAX_HOSTNAME_LEN);
  memset(tapeid, '\0', MAX_TAPEID_LEN);
  memset(prog, '\0', MAX_PROGNAME_LEN);
  memset(sdate, '\0', MAX_NAME_LEN);
  memset(stime, '\0', MAX_NAME_LEN);
  if (sscanf(linestr, "%s %s %s %*s %s %s %*s", host, tapeid, 
			 prog, sdate, stime) != 5) {
	fprintf(stderr, "linestr is obsolete. Abort del job\n");
	return;
  }

  logfile = get_logfile(gstatus_info->logfiles, host, tapeid, sdate, 
						stime, prog);

  if (logfile == NULL) {
  
	fprintf(stderr, "logfile is NULL for: <%s>. Ignore del job.\n", linestr);
	return;
  }
  send_del_job_to_monitorps(logfile);
  fl_delete_browser_line(browser, linenum);

  /*
   * We must remove all references to the entry deleted from the browser.  
   * Construct $hostname.$tapeid.$product.log
   */
  selected_item[0] = (char *)calloc(MAX_FILENAME_LEN, sizeof(char));
  if (selected_item[0] == NULL) {
	perror("calloc selected_item");
	return;  /* Ignore */
  }
  create_logfile_name(logfile->fromhost, 
					  logfile->tapeid, logfile->prog, selected_item[0]);

  memset(tmp_str, '\0', MAX_NAME_LEN);
  sprintf(tmp_str, "%ld", logfile->stime_item);
  remove_info_for_selected_items(STAT_INFO, selected_item, 1, 
								 &(logfile->stime_item), NULL);
  free(selected_item[0]);
  free_array_of_str(curr_selected_prod_groups, MAX_ITEMS);
} /* remove_job_grpings */

struct logfile_rec *get_logfile_from_prod_grp_line(char *linestr)
{
  char tapeid[MAX_TAPEID_LEN];
  char sdate[MAX_NAME_LEN], stime[MAX_NAME_LEN];
  char prog[MAX_PROGNAME_LEN];
  char host[MAX_HOSTNAME_LEN];
  struct logfile_rec *logfile;

  if (linestr == NULL) return NULL;

  /*
   * Color characters and '*'|'-' finished marker were deleted. 
   * linestr: hostname tapeid product dev date time runtime  totaljobs
   */
  memset(host, '\0', MAX_HOSTNAME_LEN);
  memset(tapeid, '\0', MAX_TAPEID_LEN);
  memset(prog, '\0', MAX_PROGNAME_LEN);
  memset(sdate, '\0', MAX_NAME_LEN);
  memset(stime, '\0', MAX_NAME_LEN);
  if (sscanf(linestr, "%s %s %s %*s %s %s %*s", host, tapeid, 
			 prog, sdate, stime) != 5) {
	fprintf(stderr, "linestr is obsolete. Abort del job\n");
	return NULL;
  }

  logfile = get_logfile(gstatus_info->logfiles, host, tapeid, sdate, 
						stime, prog);
  return logfile;

} /* get_logfile_from_prod_grp_line */


/***************************************************************************/
/*                                                                         */
/*                          remove_prod_grpings                            */
/*                                                                         */
/***************************************************************************/  
void remove_prod_grpings(FL_OBJECT *browser)
{
  /* Remove product groupings from internal lists if user confirmed it. */

  int maxline;
  char *linestr;
  char **selected_items;
  int i, j=0;
  struct logfile_rec *logfile=NULL;

  if (browser == NULL) goto IGNORE;
  /* 1. Pop up a confirmation panel.
   * 2. Perform function of deleting product groupings by sending
   *    a message for each group to monitorps.
   */
#ifdef XFORMS_081
  if (!fl_show_question("",
						"Do you really want to delete displayed product groupings from the log?",
						"")) 
#else
  if (!fl_show_question("Do you really want to delete displayed product groupings from the log?", 0)) 
#endif
	return;  /* No, cancel */

  /* Yes, delete all product groups */
  maxline = fl_get_browser_maxline(browser);

  selected_items = (char **) calloc(maxline, sizeof(char *));
  if (selected_items == NULL) {
	perror("calloc selected_item");
	goto IGNORE;
  }
  for (i = 0; i < maxline; i++)
	selected_items[i] = NULL;

  for (i = 1; i <= maxline; i++) {
	linestr = (char *) fl_get_browser_line(browser, i);
	linestr = remove_coding_chars(linestr);
	if (linestr == NULL) 
	  goto IGNORE;

	logfile = get_logfile_from_prod_grp_line(linestr);
	if (logfile == NULL) continue;
	send_del_job_to_monitorps(logfile);

	/*
	 * Construct $hostname.$tapeid.$product.log
	 */
	selected_items[j] = (char *)calloc(MAX_FILENAME_LEN, sizeof(char));
	if (selected_items[j] == NULL) {
	  perror("calloc selected_items[j]");
	  goto IGNORE;
	}
	create_logfile_name(logfile->fromhost, 
						logfile->tapeid, logfile->prog, selected_items[j]);
	j++;
  }
  if (j > 0) {
	 /* We must remove all references to the entries deleted from the browser.
	  */
	remove_info_for_selected_items(TAPE_GROUP, selected_items, j, 
								 NULL, NULL);
  }
  free_array_of_str(selected_items, j);
  free(selected_items);
  strcpy(curr_selected_tape_group, "");
  free_array_of_str(curr_selected_prod_groups, MAX_ITEMS);

  return;

IGNORE:
	fprintf(stderr, "Can't delete product groupings. Ignore request.\n");
	return;
} /* remove_prod_grpings */

/***************************************************************************/
/*                                                                         */
/*                            remove_tape_grping                           */
/*                                                                         */
/***************************************************************************/
void remove_tape_grping(char *linestr)
{
  /* linestr contains: 
   * host tapeid runtime curr_product prod_runtime prod_status last_job_runtime
   *
   * Remove tape grouping. Send del job for each logfile from this group to
   * monitorps. Remove all refrerences.
   */
  char *host;
  char *tapeid;
  struct logfile_rec *logfile;
  int j=0, i;
  char **selected_items;

  if (linestr == NULL) return;
  if (gstatus_info == NULL) return;  

  host = strtok(linestr, (const char *) " ");
  tapeid = strtok(NULL, (const char *) " ");
  if (host == NULL || tapeid == NULL) {
	fprintf(stderr, "Can't get host and tapeid from tape grouping line.\n");
	return;
  }
  

  selected_items = (char **) calloc(MAX_ITEMS, sizeof(char *));
  if (selected_items == NULL) {
	perror("calloc selected_item");
    return;
  }
  for (i = 0; i < MAX_ITEMS; i++)
	selected_items[i] = NULL;

  j = 0;
  logfile = gstatus_info->logfiles;
  /* Send del job msg to monitorps for each logfile beloning to this
   * tape grouping.
   */
  while (logfile) {
	if (strcmp(logfile->fromhost, host) == 0 &&
		(strcmp(logfile->tapeid, tapeid) == 0 ||
		 (strlen(tapeid) == MAX_TAPEID_LEN-1 && 
		  strstr(logfile->tapeid, tapeid) != NULL))) {
	  send_del_job_to_monitorps(logfile);

	  /*
	   * Construct $hostname.$tapeid.$product.log
	   */
	  selected_items[j] = (char *)calloc(MAX_FILENAME_LEN, sizeof(char));
	  create_logfile_name(logfile->fromhost, 
						  logfile->tapeid, logfile->prog, selected_items[j]);
	  j++;
	}
	logfile = logfile->next;
  }
  /* REmove all reference information for this tape grouping */
  remove_info_for_selected_items(TAPE_GROUP, selected_items, j, 
								 NULL, NULL);
  free_array_of_str(selected_items, j);
  free(selected_items);
  
} /* remove_tape_grping */

/***************************************************************************/
/*                                                                         */
/*                             remove_tape_grpings                         */
/*                                                                         */
/***************************************************************************/
void remove_tape_grpings(FL_OBJECT *browser)
{
  int i;
  int maxline;
  char *linestr;

  if (browser == NULL) return;
  /* 1. Pop up a confirmation panel.
   * 2. Perform function of deleting tape groupings by sending
   *    a message for each group to monitorps.
   *
   */
#ifdef XFORMS_081
  if (!fl_show_question("",
						"Do you really want to delete displayed tape groupings from the log?",
						"")) 
#else
  if (!fl_show_question("Do you really want to delete displayed tape groupings from the log?", 0)) 
#endif
	return;  /* No, cancel */

  /* Yes, delete all product groups */
  maxline = fl_get_browser_maxline(browser);
  for (i = 1; i <= maxline; i++) {
	linestr = (char *) fl_get_browser_line(browser, i);
	linestr = remove_coding_chars(linestr);
	/* linestr should contain: 
	 * host tapeid runtime curr_product prod_runtime prod_status last_job_runtime
	 */
	if (linestr == NULL) 
	  break;
	remove_tape_grping(linestr);
  }

  strcpy(curr_selected_tape_group, "");
  free_array_of_str(curr_selected_prod_groups, MAX_ITEMS);

} /* remove_tape_grpings */

/***************************************************************************/
/*                                                                         */
/*                               do_del_job_button                         */
/*                                                                         */
/***************************************************************************/
void do_del_job_button(FL_OBJECT *ob, long group_type)
{
  long oldmask;
  FD_gms_form *gms_form;
  FD_group_form *grp_form;
  FL_OBJECT *browser;

  oldmask = gms_block_ints();

  if (ob == NULL || ob->u_vdata == NULL) goto DONE;
  switch (group_type) {
  case JOB_GRPING:
	gms_form = (FD_gms_form *) ob->u_vdata;
	if (gms_form->browser == NULL) goto DONE;
	browser = gms_form->browser;
	remove_job_grpings(browser);
	break;
  case PROD_GRPING:
	gms_form = (FD_gms_form *) ob->u_vdata;
	if (gms_form->browser == NULL) goto DONE;
	browser = gms_form->browser;
	remove_prod_grpings(browser);
	break;
  case TAPE_GRPING:
	grp_form = (FD_group_form *) ob->u_vdata;
	if (grp_form->browser == NULL) goto DONE;
	browser = grp_form->browser;
	remove_tape_grpings(browser);
	break;
  default:
	goto DONE;
  }

  update_forms_affected_by_status_info_change(1);
  load_status_forms = 1;


 DONE:
  gms_unblock_ints(oldmask);

}



