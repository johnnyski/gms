/* gms_time.h
 *
 */


#ifndef __GMS_TIME_H__
#define __GMS_TIME_H__ 1
#ifndef MAX_NAME_LEN
#define MAX_NAME_LEN          40
#endif
time_t construct_time(int yr, int mon, int day, int hr, int min, int sec);
void time_secs2strs(time_t time_item, char *time_str, char *date_str);
void strs_to_time(char *date, char *time, time_t *t);



#endif
