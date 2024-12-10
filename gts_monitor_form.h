/** Header file generated with fdesign on Tue Dec 14 02:19:01 1999.**/

#ifndef FD_gms_form_h_
#define FD_gms_form_h_

/** Callbacks, globals and object handlers **/
extern void do_nothing(FL_OBJECT *, long);
extern void do_help_menu(FL_OBJECT *, long);
extern void do_file_menu(FL_OBJECT *, long);
extern void do_view_menu(FL_OBJECT *, long);
extern void do_action_menu(FL_OBJECT *, long);
extern void do_help(FL_OBJECT *, long);
extern void do_write_browser_to_file(FL_OBJECT *, long);
extern void do_print_all_from_browser(FL_OBJECT *, long);
extern void do_close_form(FL_OBJECT *, long);

extern void do_nothing(FL_OBJECT *, long);
extern void do_action_menu(FL_OBJECT *, long);
extern void do_config_menu(FL_OBJECT *, long);
extern void do_help_menu(FL_OBJECT *, long);
extern void do_toggle_show_all_levels_button(FL_OBJECT *, long);
extern void do_toggle_show_network_top_button(FL_OBJECT *, long);

extern void do_nothing(FL_OBJECT *, long);
extern void do_close_form(FL_OBJECT *, long);
extern void do_clear_selected_items(FL_OBJECT *, long);

extern void do_nothing(FL_OBJECT *, long);
extern void do_clear_input(FL_OBJECT *, long);
extern void do_close_form(FL_OBJECT *, long);

extern void do_nothing(FL_OBJECT *, long);
extern void do_help_menu(FL_OBJECT *, long);
extern void do_action_menu(FL_OBJECT *, long);
extern void do_file_menu(FL_OBJECT *, long);
extern void do_help(FL_OBJECT *, long);
extern void do_write_browser_to_file(FL_OBJECT *, long);
extern void do_close_form(FL_OBJECT *, long);

extern void do_nothing(FL_OBJECT *, long);
extern void do_help_menu(FL_OBJECT *, long);
extern void do_action_menu(FL_OBJECT *, long);
extern void do_get_scroll_percent(FL_OBJECT *, long);
extern void do_view_menu(FL_OBJECT *, long);

extern void do_nothing(FL_OBJECT *, long);
extern void do_read_logfiles(FL_OBJECT *, long);
extern void do_close_form(FL_OBJECT *, long);
extern void do_test_update_history(FL_OBJECT *, long);
extern void do_test_get_stat_info(FL_OBJECT *, long);
extern void do_flush(FL_OBJECT *, long);
extern void do_show_logfile_list(FL_OBJECT *, long);

extern void do_nothing(FL_OBJECT *, long);
extern void do_help_menu(FL_OBJECT *, long);
extern void do_action_menu(FL_OBJECT *, long);
extern void do_file_menu(FL_OBJECT *, long);
extern void do_help(FL_OBJECT *, long);
extern void do_write_browser_to_file(FL_OBJECT *, long);
extern void do_close_form(FL_OBJECT *, long);
extern void do_clear_selected_items(FL_OBJECT *, long);

extern void do_nothing(FL_OBJECT *, long);
extern void do_help_menu(FL_OBJECT *, long);
extern void update_exceeded_threshold_form(FL_OBJECT *, long);

extern void do_select_item_from_browser(FL_OBJECT *, long);
extern void do_clear_input(FL_OBJECT *, long);
extern void do_close_form(FL_OBJECT *, long);

extern void do_nothing(FL_OBJECT *, long);
extern void do_help_menu(FL_OBJECT *, long);
extern void do_windows_menu(FL_OBJECT *, long);
extern void do_options_menu(FL_OBJECT *, long);
extern void do_config_menu(FL_OBJECT *, long);
extern void do_report_menu(FL_OBJECT *, long);
extern void do_status_menu(FL_OBJECT *, long);
extern void do_file_menu(FL_OBJECT *, long);
extern void do_view_menu(FL_OBJECT *, long);
extern void do_action_menu(FL_OBJECT *, long);
extern void do_help(FL_OBJECT *, long);
extern void popup_add_del_show_form(FL_OBJECT *, long);
extern void popup_summary_report_form(FL_OBJECT *, long);
extern void do_write_browser_to_file(FL_OBJECT *, long);
extern void do_print_all_from_browser(FL_OBJECT *, long);
extern void do_reset(FL_OBJECT *, long);
extern void exit_system(FL_OBJECT *, long);
extern void popup_product_grouping_form(FL_OBJECT *, long);
extern void popup_ps_form(FL_OBJECT *, long);
extern void popup_status_form(FL_OBJECT *, long);
extern void popup_all_data_form(FL_OBJECT *, long);
extern void popup_exceeded_threshold_form(FL_OBJECT *, long);


extern void do_nothing(FL_OBJECT *, long);
extern void do_help_menu(FL_OBJECT *, long);
extern void do_file_menu(FL_OBJECT *, long);
extern void do_help(FL_OBJECT *, long);
extern void do_write_browser_to_file(FL_OBJECT *, long);
extern void do_refresh_summary_form(FL_OBJECT *, long);
extern void do_close_form(FL_OBJECT *, long);
extern void do_view_menu(FL_OBJECT *, long);
extern void do_config_menu(FL_OBJECT *, long);
extern void select_and_show_summary_report(FL_OBJECT *, long);
extern void popup_status_fields_selection_form(FL_OBJECT *, long);

extern void do_select_item_from_browser(FL_OBJECT *, long);
extern void do_modify_gv_levels_table(FL_OBJECT *, long);
extern void do_clear_input(FL_OBJECT *, long);
extern void do_close_form(FL_OBJECT *, long);

extern void do_delete_summary_info(FL_OBJECT *, long);
extern void do_clear_status_fields_form(FL_OBJECT *, long);
extern void do_close_form(FL_OBJECT *, long);


/**** Forms and Objects ****/
typedef struct {
	FL_FORM *gms_form;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *menu_grp;
	FL_OBJECT *help_menu;
	FL_OBJECT *file_menu;
	FL_OBJECT *view_menu;
	FL_OBJECT *browser_grp;
	FL_OBJECT *label1;
	FL_OBJECT *browser;
	FL_OBJECT *legend_grp;
	FL_OBJECT *action_menu;
	FL_OBJECT *toolbar_grp;
	FL_OBJECT *help_button;
	FL_OBJECT *save_button;
	FL_OBJECT *print_button;
	FL_OBJECT *cancel_button;
	FL_OBJECT *del_job_button;
} FD_gms_form;

extern FD_gms_form * create_form_gms_form(void);
typedef struct {
	FL_FORM *ps_info_form;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *browser;
	FL_OBJECT *menu_grp;
	FL_OBJECT *action_menu;
	FL_OBJECT *configure_menu;
	FL_OBJECT *help_menu;
	FL_OBJECT *label1;
	FL_OBJECT *legend_grp;
	FL_OBJECT *progs_only_button;
	FL_OBJECT *every_level_button;
} FD_ps_info_form;

extern FD_ps_info_form * create_form_ps_info_form(void);
typedef struct {
	FL_FORM *select_item_form;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *label;
	FL_OBJECT *browser;
	FL_OBJECT *cancel_button;
	FL_OBJECT *clear_button;
	FL_OBJECT *ok_button;
} FD_select_item_form;

extern FD_select_item_form * create_form_select_item_form(void);
typedef struct {
	FL_FORM *get_input_form;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *clear_button;
	FL_OBJECT *cancel_button;
	FL_OBJECT *ok_button;
	FL_OBJECT *input;
	FL_OBJECT *label;
} FD_get_input_form;

extern FD_get_input_form * create_form_get_input_form(void);
typedef struct {
	FL_FORM *logfile_form;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *menu_grp;
	FL_OBJECT *help_menu;
	FL_OBJECT *action_menu;
	FL_OBJECT *file_menu;
	FL_OBJECT *browser_grp;
	FL_OBJECT *label;
	FL_OBJECT *browser;
	FL_OBJECT *legend_grp;
	FL_OBJECT *toolbar_grp;
	FL_OBJECT *help_button;
	FL_OBJECT *save_button;
	FL_OBJECT *print_all_button;
	FL_OBJECT *print_yellow_button;
	FL_OBJECT *print_red_button;
	FL_OBJECT *del_job_button;
	FL_OBJECT *cancel_button;
} FD_logfile_form;

extern FD_logfile_form * create_form_logfile_form(void);
typedef struct {
	FL_FORM *status_form;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *menu_grp;
	FL_OBJECT *help_menu;
	FL_OBJECT *action_menu;
	FL_OBJECT *bars_grp;
	FL_OBJECT *box;
	FL_OBJECT *slider;
	FL_OBJECT *chart5;
	FL_OBJECT *label5;
	FL_OBJECT *bar5;
	FL_OBJECT *chart4;
	FL_OBJECT *label4;
	FL_OBJECT *bar4;
	FL_OBJECT *chart3;
	FL_OBJECT *label3;
	FL_OBJECT *bar3;
	FL_OBJECT *chart2;
	FL_OBJECT *label2;
	FL_OBJECT *bar2;
	FL_OBJECT *chart1;
	FL_OBJECT *label1;
	FL_OBJECT *bar1;
	FL_OBJECT *label;
	FL_OBJECT *label22;
	FL_OBJECT *label33;
	FL_OBJECT *label44;
	FL_OBJECT *label55;
	FL_OBJECT *label11;
	FL_OBJECT *view_menu;
	FL_OBJECT *legend_grp;
} FD_status_form;

extern FD_status_form * create_form_status_form(void);
typedef struct {
	FL_FORM *test_form;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *log_files_button;
	FL_OBJECT *cancel_button;
	FL_OBJECT *update_history_button;
	FL_OBJECT *get_stat_info_button;
	FL_OBJECT *flush_button;
	FL_OBJECT *show_loglist_button;
} FD_test_form;

extern FD_test_form * create_form_test_form(void);
typedef struct {
	FL_FORM *report_form_old;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *menu_grp;
	FL_OBJECT *help_menu;
	FL_OBJECT *action_menu;
	FL_OBJECT *file_menu;
	FL_OBJECT *toolbar_grp;
	FL_OBJECT *help_button;
	FL_OBJECT *save_button;
	FL_OBJECT *print_all_button;
	FL_OBJECT *print_yellow_button;
	FL_OBJECT *print_red_button;
	FL_OBJECT *detail_button;
	FL_OBJECT *cancel_button;
	FL_OBJECT *clear_button;
	FL_OBJECT *browser_grp;
	FL_OBJECT *browser;
	FL_OBJECT *label2;
	FL_OBJECT *label1;
	FL_OBJECT *legend_grp;
} FD_report_form_old;

extern FD_report_form_old * create_form_report_form_old(void);
typedef struct {
	FL_FORM *msg_form;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *browser;
	FL_OBJECT *label;
	FL_OBJECT *menu_grp;
	FL_OBJECT *help_menu;
	FL_OBJECT *file_menu;
	FL_OBJECT *threshold_grp;
	FL_OBJECT *threshold_counter;
	FL_OBJECT *threshold_label;
	FL_OBJECT *legend_grp;
} FD_msg_form;

extern FD_msg_form * create_form_msg_form(void);
typedef struct {
	FL_FORM *add_del_show_form;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *available_browser;
	FL_OBJECT *selected_browser;
	FL_OBJECT *input;
	FL_OBJECT *label1;
	FL_OBJECT *label2;
	FL_OBJECT *add_button;
	FL_OBJECT *clear_button;
	FL_OBJECT *cancel_button;
	FL_OBJECT *delete_button;
} FD_add_del_show_form;

extern FD_add_del_show_form * create_form_add_del_show_form(void);
typedef struct {
	FL_FORM *group_form;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *label_grp;
	FL_OBJECT *label1;
	FL_OBJECT *menu_grp;
	FL_OBJECT *help_menu;
	FL_OBJECT *windows_menu;
	FL_OBJECT *options_menu;
	FL_OBJECT *configure_menu;
	FL_OBJECT *report_menu;
	FL_OBJECT *status_menu;
	FL_OBJECT *file_menu;
	FL_OBJECT *stat_label_grp;
	FL_OBJECT *status_label;
	FL_OBJECT *browser_grp;
	FL_OBJECT *browser;
	FL_OBJECT *divider_bar;
	FL_OBJECT *view_menu;
	FL_OBJECT *label2;
	FL_OBJECT *signal_grp;
	FL_OBJECT *unknown_light_button;
	FL_OBJECT *running_light_button;
	FL_OBJECT *warning_light_button;
	FL_OBJECT *failure_light_button;
	FL_OBJECT *legend_grp;
	FL_OBJECT *data_trans_stat_grp;
	FL_OBJECT *data_trans_status_label;
	FL_OBJECT *action_menu;
	FL_OBJECT *toolbar_grp;
	FL_OBJECT *help_button;
	FL_OBJECT *neighbors_button;
	FL_OBJECT *progs_button;
	FL_OBJECT *report_button;
	FL_OBJECT *save_button;
	FL_OBJECT *print_button;
	FL_OBJECT *reset_button;
	FL_OBJECT *exit_button;
	FL_OBJECT *detail_button;
	FL_OBJECT *ps_button;
	FL_OBJECT *bar_button;
	FL_OBJECT *all_data_button;
	FL_OBJECT *exceeded_threshold_button;
	FL_OBJECT *del_job_button;
} FD_group_form;

extern FD_group_form * create_form_group_form(void);
typedef struct {
	FL_FORM *busy_form;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *label;
	FL_OBJECT *status_label;
} FD_busy_form;

extern FD_busy_form * create_form_busy_form(void);
typedef struct {
	FL_FORM *report_form;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *menu_grp;
	FL_OBJECT *help_menu;
	FL_OBJECT *file_menu;
	FL_OBJECT *toolbar_grp;
	FL_OBJECT *help_button;
	FL_OBJECT *save_button;
	FL_OBJECT *print_all_button;
	FL_OBJECT *print_yellow_button;
	FL_OBJECT *print_red_button;
	FL_OBJECT *refresh_button;
	FL_OBJECT *cancel_button;
	FL_OBJECT *browser_grp;
	FL_OBJECT *browser;
	FL_OBJECT *label;
	FL_OBJECT *legend_grp;
	FL_OBJECT *view_menu;
	FL_OBJECT *configure_menu;
	FL_OBJECT *tape_button;
	FL_OBJECT *level_button;
	FL_OBJECT *product_button;
	FL_OBJECT *del_job_button;
} FD_report_form;

extern FD_report_form * create_form_report_form(void);
typedef struct {
	FL_FORM *gv_levels_form;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *level_browser;
	FL_OBJECT *program_input;
	FL_OBJECT *add_button;
	FL_OBJECT *label;
	FL_OBJECT *delete_button;
	FL_OBJECT *clear_button;
	FL_OBJECT *cancel_button;
	FL_OBJECT *available_progs_browser;
	FL_OBJECT *level_input;
} FD_gv_levels_form;

extern FD_gv_levels_form * create_form_gv_levels_form(void);
typedef struct {
	FL_FORM *status_fields_selection_form;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *host_input;
	FL_OBJECT *tapeid_input;
	FL_OBJECT *product_input;
	FL_OBJECT *sdate_input;
	FL_OBJECT *ok_button;
	FL_OBJECT *clear_button;
	FL_OBJECT *cancel_button;
	FL_OBJECT *label;
	FL_OBJECT *level_input;
} FD_status_fields_selection_form;

extern FD_status_fields_selection_form * create_form_status_fields_selection_form(void);

#endif /* FD_gms_form_h_ */
