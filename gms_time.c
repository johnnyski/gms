/* gms_time.c
 *
 */
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "gms_time.h"

/****************************************************************************/
/*                                                                          */
/*                               year_4to2_digit                            */
/* Copied from gv_utils/gv_time.c                                           */
/****************************************************************************/
int year_4to2_digit(int yr)
{
/* year_4to2_digit: COnvert 4 digit year to 2 digit.
 * If yr < 2000, subtract it with 1900; else subtract 2000 with it.
 */
  if (yr < 100) return yr; /* already in 2 digit */
	
  if (yr < 2000)
	yr -= 1900;
  else yr -= 2000;
  return yr;
}

/****************************************************************************/
/*                                                                          */
/*                         construct_time                                   */
/* Copied from gv_utils/gv_time.c                                           */
/****************************************************************************/
time_t construct_time(int yr, int mon, int day, int hr, int min, int sec)
{
  /* yr: yyyy|yy
   * Return time in seconds
   */
  struct tm time_item;

  memset(&time_item, '\0', sizeof(struct tm));
  /* Adjust year -- The number of years since 1900.
   */
  time_item.tm_year = year_4to2_digit(yr);
  /* adjust accordingly since tm_mon starts at 0 */
  time_item.tm_mon = mon - 1;
  time_item.tm_mday = day;

  time_item.tm_hour = hr;
  time_item.tm_min = min;
  time_item.tm_sec =  sec;
  time_item.tm_isdst = -1;
  /* daylight savings time flag: used default */
  time_item.tm_isdst = -1;


  return (mktime(&time_item));
}


/***************************************************************************/
/*                                                                         */
/*                     strs_to_time                                        */
/*                                                                         */
/***************************************************************************/
void strs_to_time(char *date, char *time_str, time_t *t)
{
  /* Convert date and time strings to time.  
   */
  struct tm time_item;

  if (date == NULL || t == NULL) return;

  memset(&time_item, '\0', sizeof(struct tm));
  /* No date */
  if (strlen(date) == 0) return;

  sscanf(date, "%d/%d/%d", &(time_item.tm_mon),
		 &(time_item.tm_mday),  &(time_item.tm_year));
  if (time_str != NULL) {
	sscanf(time_str, "%d:%d:%d", &(time_item.tm_hour),
		   &(time_item.tm_min),  &(time_item.tm_sec));
  }
  /* adjust accordingly if it starts at 0 */
  time_item.tm_mon--;

  /* daylight savings time flag: used default */
  time_item.tm_isdst = -1;


  *t = mktime(&time_item);
  /*
fprintf(stderr, "timec: %s, date: %s, time: %s\n", ctime(t), date,time_str);
*/


} /* strs_to_time */

/***************************************************************************/
/*                                                                         */
/*                            time_secs2strs                               */ 
/*                                                                         */
/***************************************************************************/
void time_secs2strs(time_t time_item, char *time_str, char *date_str)
{
  /* Convert time in seconds to time_str and date_str, where,
   * time_str: hh:mm:ss
   * date_str: mm/dd/yy
   */
  char mon_str[4], yr_str[5];
  int mon_i, day, i;
  struct _mon {
	char mon_str[4];
	int mon;
  };
  static struct _mon mon_table[12] = {
	{"Jan", 1}, {"Feb", 2}, {"Mar", 3}, {"Apr", 4}, {"May", 5},
	{"Jun", 6}, {"Jul", 7}, {"Aug", 8}, {"Sep", 9}, {"Oct", 10}, 
	{"Nov", 11}, {"Dec", 12}};


  char *tmp_time_str;
  char tmp_date[MAX_NAME_LEN];

  tmp_time_str = ctime(&time_item);
  if (tmp_time_str == NULL) return;
  /* tmp_time_str has this form: "Wed Jun 30 21:49:08 1993\n"
   * 
   */
  memset(mon_str, '\0', 4);
  memset(yr_str, '\0', 5);
  if (sscanf(tmp_time_str, "%*s %s %d %s %s", mon_str, &day, 
			 time_str, yr_str) != 4) {
	fprintf(stderr, "time_str format <%s> returned by ctime is unexpected.\n", tmp_time_str);
	return;
  }
  mon_i = 0;
  for (i = 0; i < 12; i++) {
	if (strcmp(mon_str, mon_table[i].mon_str) == 0)
	  mon_i = mon_table[i].mon;
  }
  memset(tmp_date, '\0', MAX_NAME_LEN);
  /* Skip the first 2 char from yr_str (19) */
  sprintf(tmp_date, "%.2d/%.2d/%.2s", mon_i, day, &yr_str[2]);
  strcat(date_str, tmp_date);
} /* time_secs2strs */


