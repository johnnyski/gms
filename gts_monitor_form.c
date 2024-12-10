/* Form definition file generated with fdesign. */

#include "forms.h"
#include <stdlib.h>
#include "gts_monitor_form.h"

FD_gms_form *create_form_gms_form(void)
{
  FL_OBJECT *obj;
  FD_gms_form *fdui = (FD_gms_form *) fl_calloc(1, sizeof(*fdui));

  fdui->gms_form = fl_bgn_form(FL_NO_BOX, 800, 306);
  obj = fl_add_box(FL_UP_BOX,0,0,800,306,"");
    fl_set_object_callback(obj,do_nothing,0);

  fdui->menu_grp = fl_bgn_group();
  obj = fl_add_box(FL_UP_BOX,0,0,800,24,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
  fdui->help_menu = obj = fl_add_menu(FL_PULLDOWN_MENU,726,4,59,14,"Help");
    fl_set_object_shortcut(obj,"^H",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,do_help_menu,0);
  fdui->file_menu = obj = fl_add_menu(FL_PULLDOWN_MENU,10,4,58,16,"File");
    fl_set_object_shortcut(obj,"^F",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_file_menu,2);
  fl_end_group();

  fdui->view_menu = obj = fl_add_menu(FL_PULLDOWN_MENU,66,4,58,15,"View");
    fl_set_object_shortcut(obj,"^V",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_view_menu,2);

  fdui->browser_grp = fl_bgn_group();
  obj = fl_add_text(FL_NORMAL_TEXT,18,243,762,21,"List of product groupings.   Click on an item above for detail.");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_lcolor(obj,FL_DARKVIOLET);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMES_STYLE+FL_EMBOSSED_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
  fdui->label1 = obj = fl_add_text(FL_NORMAL_TEXT,21,60,769,21,"label1                                                                                                                                                                                                    ");
    fl_set_object_lcolor(obj,FL_DARKVIOLET);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_North);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->browser = obj = fl_add_browser(FL_HOLD_BROWSER,18,81,762,162,"");
    fl_set_object_color(obj,FL_COL1,FL_MCOL);
    fl_set_object_lcolor(obj,FL_RIGHT_BCOL);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthEast);
    fl_set_object_callback(obj,do_nothing,0);
  fl_end_group();


  fdui->legend_grp = fl_bgn_group();
  obj = fl_add_box(FL_UP_BOX,0,266,798,41,"");
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthEast);
  obj = fl_add_box(FL_FRAME_BOX,10,274,780,24,"");
    fl_set_object_gravity(obj, FL_South, FL_South);
  obj = fl_add_text(FL_NORMAL_TEXT,706,278,72,15,"'-' Stopped");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_RIGHT_BCOL);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,153,276,43,18,"Failed");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_RED);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,191,276,49,18,"Warning");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,239,279,56,13,"Running");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_GREEN);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,338,276,54,19,"Unknown");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_CYAN);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,634,279,72,16,"'*' Finished");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_RIGHT_BCOL);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,468,276,103,19,"Everything was:");
    fl_set_object_lcolor(obj,FL_RIGHT_BCOL);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
  obj = fl_add_text(FL_NORMAL_TEXT,388,279,79,14,"'R'  Running");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_RIGHT_BCOL);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,11,279,63,14,"LEGENDS");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,71,276,88,18,"Something is: ");
    fl_set_object_lcolor(obj,FL_RIGHT_BCOL);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
  obj = fl_add_box(FL_UP_BOX,464,274,1,21,"");
    fl_set_object_lcolor(obj,FL_RIGHT_BCOL);
    fl_set_object_gravity(obj, FL_South, FL_South);
  obj = fl_add_text(FL_NORMAL_TEXT,571,279,65,16,"Successful");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,286,276,57,18,"Aborted");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_DARKTOMATO);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  fl_end_group();

  fdui->action_menu = obj = fl_add_menu(FL_PULLDOWN_MENU,124,6,54,12,"Action");
    fl_set_object_shortcut(obj,"^A",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_action_menu,2);

  fdui->toolbar_grp = fl_bgn_group();
  obj = fl_add_box(FL_SHADOW_BOX,2,24,796,34,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
  fdui->help_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,744,24,39,28,"");
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,do_help,0);
  fdui->save_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,99,24,41,28,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_write_browser_to_file,2);
  fdui->print_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,59,24,40,28,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_print_all_from_browser,2);
  fdui->cancel_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,19,24,40,28,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_close_form,2);
  fdui->del_job_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,140,24,38,28,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fl_end_group();

  fl_end_form();

  fdui->gms_form->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

FD_ps_info_form *create_form_ps_info_form(void)
{
  FL_OBJECT *obj;
  FD_ps_info_form *fdui = (FD_ps_info_form *) fl_calloc(1, sizeof(*fdui));

  fdui->ps_info_form = fl_bgn_form(FL_NO_BOX, 810, 254);
  obj = fl_add_box(FL_UP_BOX,0,0,810,254,"");
    fl_set_object_callback(obj,do_nothing,0);
  fdui->browser = obj = fl_add_browser(FL_NORMAL_BROWSER,10,57,788,153,"");
    fl_set_object_lcolor(obj,FL_RIGHT_BCOL);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthEast);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_box(FL_UP_BOX,0,0,808,31,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);

  fdui->menu_grp = fl_bgn_group();
  fdui->action_menu = obj = fl_add_menu(FL_PULLDOWN_MENU,14,4,61,18,"Action");
    fl_set_object_shortcut(obj,"#A",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_action_menu,0);
  fdui->configure_menu = obj = fl_add_menu(FL_PULLDOWN_MENU,84,5,89,17,"Configure");
    fl_set_object_shortcut(obj,"#C",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_config_menu,0);
  fl_end_group();

  fdui->help_menu = obj = fl_add_menu(FL_PULLDOWN_MENU,711,6,70,16,"Help");
    fl_set_object_shortcut(obj,"#H",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,do_help_menu,0);
  fdui->label1 = obj = fl_add_text(FL_NORMAL_TEXT,13,31,788,25,"label1                                                                                                                                                 ");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_DARKVIOLET);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
    fl_set_object_callback(obj,do_nothing,0);

  fdui->legend_grp = fl_bgn_group();
  obj = fl_add_box(FL_FRAME_BOX,277,212,524,32,"");
    fl_set_object_gravity(obj, FL_South, FL_South);
  obj = fl_add_text(FL_NORMAL_TEXT,486,219,306,16," '*' - More sublevel(s) exist but are not showing.");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
  obj = fl_add_text(FL_NORMAL_TEXT,367,218,119,16," '+' - Sublevel.");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
  obj = fl_add_text(FL_NORMAL_TEXT,288,217,76,18,"Symbol Codes:");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
  fl_end_group();

  obj = fl_add_box(FL_FRAME_BOX,12,213,257,31,"");
    fl_set_object_gravity(obj, FL_South, FL_South);
  fdui->progs_only_button = obj = fl_add_lightbutton(FL_RADIO_BUTTON,71,218,95,21,"Programs Only");
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_toggle_show_all_levels_button,0);
  fdui->every_level_button = obj = fl_add_lightbutton(FL_RADIO_BUTTON,170,218,91,20,"All Proceses");
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_toggle_show_network_top_button,0);
  obj = fl_add_text(FL_NORMAL_TEXT,27,222,42,17,"Show:");
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
  fl_end_form();

  fdui->ps_info_form->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

FD_select_item_form *create_form_select_item_form(void)
{
  FL_OBJECT *obj;
  FD_select_item_form *fdui = (FD_select_item_form *) fl_calloc(1, sizeof(*fdui));

  fdui->select_item_form = fl_bgn_form(FL_NO_BOX, 310, 220);
  obj = fl_add_box(FL_UP_BOX,0,0,310,220,"");
    fl_set_object_callback(obj,do_nothing,0);
  fdui->label = obj = fl_add_text(FL_NORMAL_TEXT,30,10,250,30,"Select name(s):");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->browser = obj = fl_add_browser(FL_MULTI_BROWSER,30,50,250,110,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthEast);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->cancel_button = obj = fl_add_button(FL_NORMAL_BUTTON,200,170,70,30,"Close");
    fl_set_button_shortcut(obj,"^C",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_SouthEast, FL_SouthEast);
    fl_set_object_callback(obj,do_close_form,-1);
  fdui->clear_button = obj = fl_add_button(FL_NORMAL_BUTTON,120,170,70,30,"Clear");
    fl_set_button_shortcut(obj,"^R",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthWest);
    fl_set_object_callback(obj,do_clear_selected_items,0);
  fdui->ok_button = obj = fl_add_button(FL_NORMAL_BUTTON,40,170,70,30,"OK");
    fl_set_button_shortcut(obj,"^O",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthWest);
  fl_end_form();

  fdui->select_item_form->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

FD_get_input_form *create_form_get_input_form(void)
{
  FL_OBJECT *obj;
  FD_get_input_form *fdui = (FD_get_input_form *) fl_calloc(1, sizeof(*fdui));

  fdui->get_input_form = fl_bgn_form(FL_NO_BOX, 328, 126);
  obj = fl_add_box(FL_UP_BOX,0,0,328,126,"");
    fl_set_object_callback(obj,do_nothing,0);
  fdui->clear_button = obj = fl_add_button(FL_NORMAL_BUTTON,129,81,77,24,"Clear");
    fl_set_button_shortcut(obj,"^R",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_clear_input,0);
  fdui->cancel_button = obj = fl_add_button(FL_NORMAL_BUTTON,225,81,79,24,"Cancel");
    fl_set_button_shortcut(obj,"^C",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_close_form,-1);
  fdui->ok_button = obj = fl_add_button(FL_NORMAL_BUTTON,30,79,78,26,"OK");
    fl_set_button_shortcut(obj,"^O",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
  fdui->input = obj = fl_add_input(FL_NORMAL_INPUT,52,42,228,25,"");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->label = obj = fl_add_text(FL_NORMAL_TEXT,32,10,258,25,"Enter name:");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
    fl_set_object_callback(obj,do_nothing,0);
  fl_end_form();

  fdui->get_input_form->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

FD_logfile_form *create_form_logfile_form(void)
{
  FL_OBJECT *obj;
  FD_logfile_form *fdui = (FD_logfile_form *) fl_calloc(1, sizeof(*fdui));

  fdui->logfile_form = fl_bgn_form(FL_NO_BOX, 638, 308);
  obj = fl_add_box(FL_UP_BOX,0,0,638,308,"");
    fl_set_object_callback(obj,do_nothing,0);

  fdui->menu_grp = fl_bgn_group();
  obj = fl_add_box(FL_UP_BOX,0,0,637,30,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
  fdui->help_menu = obj = fl_add_menu(FL_PULLDOWN_MENU,578,7,51,12,"Help");
    fl_set_object_shortcut(obj,"#H",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,do_help_menu,0);
  fdui->action_menu = obj = fl_add_menu(FL_PULLDOWN_MENU,64,9,55,10,"Action");
    fl_set_object_shortcut(obj,"#A",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_action_menu,0);
  fdui->file_menu = obj = fl_add_menu(FL_PULLDOWN_MENU,8,9,49,10,"File");
    fl_set_object_shortcut(obj,"#F",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_file_menu,0);
  fl_end_group();


  fdui->browser_grp = fl_bgn_group();
  fdui->label = obj = fl_add_text(FL_NORMAL_TEXT,14,68,620,20,"label                                                                                                                                 ");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_DARKVIOLET);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->browser = obj = fl_add_browser(FL_NORMAL_BROWSER,12,88,614,185,"");
    fl_set_object_color(obj,FL_COL1,FL_MCOL);
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthEast);
    fl_set_object_callback(obj,do_nothing,0);
  fl_end_group();


  fdui->legend_grp = fl_bgn_group();
  obj = fl_add_box(FL_UP_BOX,0,274,639,34,"");
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthEast);
  obj = fl_add_box(FL_FRAME_BOX,403,280,227,21,"");
    fl_set_object_gravity(obj, FL_South, FL_South);
  obj = fl_add_text(FL_NORMAL_TEXT,409,284,75,13,"Symbol Codes:");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
  obj = fl_add_text(FL_NORMAL_TEXT,540,284,70,14,"* Finished");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_RIGHT_BCOL);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,484,284,60,14,"- Stopped");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_RIGHT_BCOL);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_box(FL_FRAME_BOX,9,280,398,21,"");
    fl_set_object_gravity(obj, FL_South, FL_South);
  obj = fl_add_text(FL_NORMAL_TEXT,278,286,61,12,"Unknown");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_CYAN);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,338,286,64,12,"Successful");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,182,284,54,14,"Running");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_GREEN);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,126,286,52,12,"Warning ");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,12,284,67,12,"Color Codes:");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthWest);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,80,286,45,12,"Failed");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_RED);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  fl_end_group();

  obj = fl_add_text(FL_NORMAL_TEXT,230,286,48,12,"Aborted");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_DARKTOMATO);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);

  fdui->toolbar_grp = fl_bgn_group();
  obj = fl_add_box(FL_SHADOW_BOX,0,29,639,37,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
  fdui->help_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,587,28,37,29,"");
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,do_help,0);
  fdui->save_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,54,28,36,29,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_write_browser_to_file,0);
  fdui->print_all_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,160,28,39,29,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->print_yellow_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,126,28,33,29,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->print_red_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,90,28,36,29,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->del_job_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,200,28,35,29,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->cancel_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,15,28,39,29,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_close_form,-1);
  fl_end_group();

  fl_end_form();

  fdui->logfile_form->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

FD_status_form *create_form_status_form(void)
{
  FL_OBJECT *obj;
  FD_status_form *fdui = (FD_status_form *) fl_calloc(1, sizeof(*fdui));

  fdui->status_form = fl_bgn_form(FL_NO_BOX, 697, 486);
  obj = fl_add_box(FL_UP_BOX,0,0,697,486,"");
    fl_set_object_callback(obj,do_nothing,0);

  fdui->menu_grp = fl_bgn_group();
  obj = fl_add_box(FL_UP_BOX,1,0,697,30,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
  fdui->help_menu = obj = fl_add_menu(FL_PULLDOWN_MENU,620,8,58,15,"Help");
    fl_set_object_shortcut(obj,"#H",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,do_help_menu,0);
  fdui->action_menu = obj = fl_add_menu(FL_PULLDOWN_MENU,11,11,62,12,"Action");
    fl_set_object_shortcut(obj,"#A",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_action_menu,4);
  fl_end_group();


  fdui->bars_grp = fl_bgn_group();
  fdui->box = obj = fl_add_frame(FL_DOWN_FRAME,64,67,609,370,"");
    fl_set_object_color(obj,FL_COL1,FL_COL1);
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthEast);
  fdui->slider = obj = fl_add_slider(FL_VERT_NICE_SLIDER,645,65,28,372,"");
    fl_set_object_color(obj,FL_COL1,FL_TOP_BCOL);
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,do_get_scroll_percent,0);
  fdui->chart5 = obj = fl_add_chart(FL_BAR_CHART,186,370,435,51,"                                  \n                             \n                             ");
    fl_set_object_boxtype(obj,FL_FLAT_BOX);
    fl_set_object_lcolor(obj,FL_DARKTOMATO);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->label5 = obj = fl_add_text(FL_NORMAL_TEXT,66,370,111,51,"");
    fl_set_object_lcolor(obj,FL_DARKTOMATO);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthEast);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->bar5 = obj = fl_add_text(FL_NORMAL_TEXT,15,375,40,30,"0000");
    fl_set_object_lcolor(obj,FL_RIGHT_BCOL);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->chart4 = obj = fl_add_chart(FL_BAR_CHART,188,294,436,50,"                                  \n                             \n                            ");
    fl_set_object_boxtype(obj,FL_FLAT_BOX);
    fl_set_object_lcolor(obj,FL_DARKTOMATO);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->label4 = obj = fl_add_text(FL_NORMAL_TEXT,64,287,110,53,"");
    fl_set_object_lcolor(obj,FL_DARKTOMATO);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthEast);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->bar4 = obj = fl_add_text(FL_NORMAL_TEXT,14,297,40,30,"0000");
    fl_set_object_lcolor(obj,FL_RIGHT_BCOL);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->chart3 = obj = fl_add_chart(FL_BAR_CHART,186,222,436,50,"                                \n                             \n                              ");
    fl_set_object_boxtype(obj,FL_FLAT_BOX);
    fl_set_object_lcolor(obj,FL_DARKTOMATO);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->label3 = obj = fl_add_text(FL_NORMAL_TEXT,64,217,112,51,"");
    fl_set_object_lcolor(obj,FL_DARKTOMATO);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthEast);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->bar3 = obj = fl_add_text(FL_NORMAL_TEXT,14,226,40,31,"0000");
    fl_set_object_lcolor(obj,FL_RIGHT_BCOL);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->chart2 = obj = fl_add_chart(FL_BAR_CHART,188,154,434,49,"                                     \n                            \n                              \n                            ");
    fl_set_object_boxtype(obj,FL_FLAT_BOX);
    fl_set_object_lcolor(obj,FL_DARKTOMATO);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->label2 = obj = fl_add_text(FL_NORMAL_TEXT,64,146,111,53,"");
    fl_set_object_lcolor(obj,FL_DARKTOMATO);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthEast);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->bar2 = obj = fl_add_text(FL_NORMAL_TEXT,14,157,40,29,"0000");
    fl_set_object_lcolor(obj,FL_RIGHT_BCOL);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->chart1 = obj = fl_add_chart(FL_BAR_CHART,185,80,436,50,"                           \n                              \n                            ");
    fl_set_object_boxtype(obj,FL_FLAT_BOX);
    fl_set_object_lcolor(obj,FL_DARKTOMATO);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->label1 = obj = fl_add_text(FL_NORMAL_TEXT,64,76,110,50,"");
    fl_set_object_lcolor(obj,FL_DARKTOMATO);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->bar1 = obj = fl_add_text(FL_NORMAL_TEXT,14,87,40,30,"0000");
    fl_set_object_lcolor(obj,FL_RIGHT_BCOL);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,6,44,40,40,"Job\n #");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_DARKVIOLET);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->label = obj = fl_add_text(FL_NORMAL_TEXT,195,39,436,20,"");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_DARKVIOLET);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->label22 = obj = fl_add_text(FL_NORMAL_TEXT,184,181,429,30,"000  files     ");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->label33 = obj = fl_add_text(FL_NORMAL_TEXT,196,251,421,28,"000  files     ");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->label44 = obj = fl_add_text(FL_NORMAL_TEXT,196,322,421,30,"000  files     ");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->label55 = obj = fl_add_text(FL_NORMAL_TEXT,195,401,416,29,"000  files   ");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthEast);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->label11 = obj = fl_add_text(FL_NORMAL_TEXT,190,109,427,28,"000  files     ");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,do_nothing,0);
  fl_end_group();

  obj = fl_add_text(FL_NORMAL_TEXT,59,37,132,22,"Total number of jobs:  ");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_DARKVIOLET);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->view_menu = obj = fl_add_menu(FL_PULLDOWN_MENU,98,9,55,15,"View");
    fl_set_object_shortcut(obj,"^V",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_view_menu,4);

  fdui->legend_grp = fl_bgn_group();
  obj = fl_add_box(FL_UP_BOX,0,441,697,45,"");
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthEast);
  obj = fl_add_box(FL_FRAME_BOX,440,449,248,29,"");
    fl_set_object_gravity(obj, FL_South, FL_South);
  obj = fl_add_text(FL_NORMAL_TEXT,447,456,81,16,"Symbol Codes:");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
  obj = fl_add_text(FL_NORMAL_TEXT,590,459,78,15,"* Finished");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_RIGHT_BCOL);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,527,459,67,15,"- Stopped");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_RIGHT_BCOL);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_box(FL_FRAME_BOX,10,449,434,29,"");
    fl_set_object_gravity(obj, FL_South, FL_South);
  obj = fl_add_text(FL_NORMAL_TEXT,298,456,68,16,"Unknown");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_CYAN);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,368,456,68,16,"Successful");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,190,456,60,15,"Running");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_GREEN);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,134,454,56,20,"Warning ");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,14,458,74,16,"Color Codes:");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthWest);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,84,458,47,15,"Failed");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_RED);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,250,456,52,16,"Aborted");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_DARKTOMATO);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  fl_end_group();

  fl_end_form();

  fdui->status_form->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

FD_test_form *create_form_test_form(void)
{
  FL_OBJECT *obj;
  FD_test_form *fdui = (FD_test_form *) fl_calloc(1, sizeof(*fdui));

  fdui->test_form = fl_bgn_form(FL_NO_BOX, 180, 294);
  obj = fl_add_box(FL_UP_BOX,0,0,180,294,"");
    fl_set_object_callback(obj,do_nothing,0);
  fdui->log_files_button = obj = fl_add_button(FL_NORMAL_BUTTON,20,20,130,40,"Read Log Files");
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_callback(obj,do_read_logfiles,0);
  fdui->cancel_button = obj = fl_add_button(FL_NORMAL_BUTTON,20,230,130,40,"Cancel");
    fl_set_object_callback(obj,do_close_form,-1);
  fdui->update_history_button = obj = fl_add_button(FL_NORMAL_BUTTON,20,60,130,40,"Update  History");
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_callback(obj,do_test_update_history,0);
  fdui->get_stat_info_button = obj = fl_add_button(FL_NORMAL_BUTTON,20,100,130,40,"get status");
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_callback(obj,do_test_get_stat_info,0);
  fdui->flush_button = obj = fl_add_button(FL_NORMAL_BUTTON,20,140,130,40,"Flush");
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMES_STYLE);
    fl_set_object_callback(obj,do_flush,0);
  fdui->show_loglist_button = obj = fl_add_button(FL_NORMAL_BUTTON,20,180,130,40,"Show Log List");
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,12);
    fl_set_object_callback(obj,do_show_logfile_list,0);
  fl_end_form();

  fdui->test_form->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

FD_report_form_old *create_form_report_form_old(void)
{
  FL_OBJECT *obj;
  FD_report_form_old *fdui = (FD_report_form_old *) fl_calloc(1, sizeof(*fdui));

  fdui->report_form_old = fl_bgn_form(FL_NO_BOX, 738, 360);
  obj = fl_add_box(FL_UP_BOX,0,0,738,360,"");
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_box(FL_SHADOW_BOX,0,30,740,34,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);

  fdui->menu_grp = fl_bgn_group();
  obj = fl_add_box(FL_UP_BOX,0,0,740,30,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
  fdui->help_menu = obj = fl_add_menu(FL_PULLDOWN_MENU,667,5,55,17,"Help");
    fl_set_object_shortcut(obj,"#H",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,do_help_menu,0);
  fdui->action_menu = obj = fl_add_menu(FL_PULLDOWN_MENU,74,6,66,16,"Action");
    fl_set_object_shortcut(obj,"#A",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_action_menu,1);
  fdui->file_menu = obj = fl_add_menu(FL_PULLDOWN_MENU,9,6,56,16,"File");
    fl_set_object_shortcut(obj,"#F",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_file_menu,1);
  fl_end_group();


  fdui->toolbar_grp = fl_bgn_group();
  fdui->help_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,683,30,39,28,"");
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,do_help,0);
  fdui->save_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,47,30,38,28,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_write_browser_to_file,1);
  fdui->print_all_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,180,30,39,28,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->print_yellow_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,142,30,38,28,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->print_red_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,104,30,38,28,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->detail_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,237,30,39,28,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->cancel_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,9,30,38,28,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_close_form,1);
  fdui->clear_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,276,30,37,28,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_clear_selected_items,1);
  fl_end_group();


  fdui->browser_grp = fl_bgn_group();
  fdui->browser = obj = fl_add_browser(FL_MULTI_BROWSER,10,129,718,191,"");
    fl_set_object_color(obj,FL_COL1,FL_MCOL);
    fl_set_object_lcolor(obj,FL_DARKTOMATO);
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthEast);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->label2 = obj = fl_add_text(FL_NORMAL_TEXT,10,105,697,20,"label2                                                                                                                                                                                               ");
    fl_set_object_lcolor(obj,FL_DARKVIOLET);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->label1 = obj = fl_add_text(FL_NORMAL_TEXT,11,86,694,18,"label1                                                                                                                                                                                               ");
    fl_set_object_lcolor(obj,FL_DARKVIOLET);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
    fl_set_object_callback(obj,do_nothing,0);
  fl_end_group();

  obj = fl_add_text(FL_NORMAL_TEXT,0,64,740,20,"List job statuses from logfile(s).   Highlight item(s) below and select 'Detail' for detail.");
    fl_set_object_boxtype(obj,FL_RSHADOW_BOX);
    fl_set_object_lcolor(obj,FL_DARKVIOLET);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMES_STYLE+FL_EMBOSSED_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);

  fdui->legend_grp = fl_bgn_group();
  obj = fl_add_box(FL_UP_BOX,0,319,738,40,"");
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthEast);
  obj = fl_add_box(FL_FRAME_BOX,466,325,262,24,"");
    fl_set_object_gravity(obj, FL_South, FL_South);
  obj = fl_add_text(FL_NORMAL_TEXT,472,331,87,13,"Symbol Codes:");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
  obj = fl_add_text(FL_NORMAL_TEXT,623,333,83,13,"* Finished");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_RIGHT_BCOL);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,558,333,70,13,"- Stopped");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_RIGHT_BCOL);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_box(FL_FRAME_BOX,11,325,459,24,"");
    fl_set_object_gravity(obj, FL_South, FL_South);
  obj = fl_add_text(FL_NORMAL_TEXT,302,330,73,14,"Unknown");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_CYAN);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,379,331,72,14,"Successful");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,190,330,62,16,"Running");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_GREEN);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,132,330,60,16,"Warning ");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,15,332,79,14,"Color Codes:");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthWest);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,84,330,50,16,"Failed");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_RED);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,252,330,52,16,"Aborted");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_DARKTOMATO);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  fl_end_group();

  fl_end_form();

  fdui->report_form_old->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

FD_msg_form *create_form_msg_form(void)
{
  FL_OBJECT *obj;
  FD_msg_form *fdui = (FD_msg_form *) fl_calloc(1, sizeof(*fdui));

  fdui->msg_form = fl_bgn_form(FL_NO_BOX, 724, 294);
  obj = fl_add_box(FL_UP_BOX,0,0,724,294,"");
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_box(FL_UP_BOX,0,255,723,38,"");
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthEast);
  obj = fl_add_box(FL_FRAME_BOX,15,263,692,22,"");
    fl_set_object_gravity(obj, FL_South, FL_South);
  fdui->browser = obj = fl_add_browser(FL_NORMAL_BROWSER,12,52,698,202,"");
    fl_set_object_color(obj,FL_COL1,FL_COL1);
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthEast);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->label = obj = fl_add_text(FL_NORMAL_TEXT,11,28,671,21,"Text                                                                                              ");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_DARKVIOLET);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
    fl_set_object_callback(obj,do_nothing,0);

  fdui->menu_grp = fl_bgn_group();
  obj = fl_add_box(FL_UP_BOX,0,0,724,26,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
  fdui->help_menu = obj = fl_add_menu(FL_PULLDOWN_MENU,656,5,52,16,"Help");
    fl_set_object_shortcut(obj,"#H",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,do_help_menu,0);
  fdui->file_menu = obj = fl_add_menu(FL_PULLDOWN_MENU,12,6,68,14,"File");
    fl_set_object_shortcut(obj,"^F",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fl_end_group();


  fdui->threshold_grp = fl_bgn_group();
  fdui->threshold_counter = obj = fl_add_counter(FL_NORMAL_COUNTER,603,264,100,19,"");
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,update_exceeded_threshold_form,0);
  fdui->threshold_label = obj = fl_add_text(FL_NORMAL_TEXT,467,265,135,17,"Threshold (in hour):");
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
  obj = fl_add_box(FL_BORDER_BOX,466,265,2,18,"");
    fl_set_object_gravity(obj, FL_South, FL_South);
  fl_end_group();


  fdui->legend_grp = fl_bgn_group();
  obj = fl_add_text(FL_NORMAL_TEXT,302,266,77,15,"Unknown");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_CYAN);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,380,266,81,15,"Successful");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,202,266,66,16,"Running");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_GREEN);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,142,268,70,14,"Warning ");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,20,268,75,13,"Color Codes:");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthWest);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,90,268,59,14,"Failed");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_RED);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,262,266,48,16,"Aborted");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_DARKTOMATO);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  fl_end_group();

  fl_end_form();

  fdui->msg_form->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

FD_add_del_show_form *create_form_add_del_show_form(void)
{
  FL_OBJECT *obj;
  FD_add_del_show_form *fdui = (FD_add_del_show_form *) fl_calloc(1, sizeof(*fdui));

  fdui->add_del_show_form = fl_bgn_form(FL_NO_BOX, 414, 214);
  obj = fl_add_box(FL_UP_BOX,0,0,414,214,"");
  fdui->available_browser = obj = fl_add_browser(FL_SELECT_BROWSER,10,32,193,116,"");
    fl_set_object_gravity(obj, FL_North, FL_North);
    fl_set_object_callback(obj,do_select_item_from_browser,10);
  fdui->selected_browser = obj = fl_add_browser(FL_SELECT_BROWSER,210,32,193,115,"");
    fl_set_object_gravity(obj, FL_North, FL_North);
    fl_set_object_callback(obj,do_select_item_from_browser,10);
  fdui->input = obj = fl_add_input(FL_NORMAL_INPUT,50,153,346,20,"Name");
    fl_set_object_lcolor(obj,FL_DARKVIOLET);
    fl_set_object_gravity(obj, FL_North, FL_North);
  fdui->label1 = obj = fl_add_text(FL_NORMAL_TEXT,6,10,172,22,"Available Names to Monitor:");
    fl_set_object_lcolor(obj,FL_DARKVIOLET);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->label2 = obj = fl_add_text(FL_NORMAL_TEXT,208,10,166,22,"Selected Names to Monitor:");
    fl_set_object_lcolor(obj,FL_DARKVIOLET);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
  fdui->add_button = obj = fl_add_button(FL_NORMAL_BUTTON,9,178,81,24,"Add");
    fl_set_button_shortcut(obj,"^A",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_North, FL_North);
  fdui->clear_button = obj = fl_add_button(FL_NORMAL_BUTTON,201,180,88,21,"Clear");
    fl_set_button_shortcut(obj,"^R",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_North, FL_North);
    fl_set_object_callback(obj,do_clear_input,0);
  fdui->cancel_button = obj = fl_add_button(FL_NORMAL_BUTTON,302,179,88,23,"Close");
    fl_set_button_shortcut(obj,"^C",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_North, FL_North);
    fl_set_object_callback(obj,do_close_form,-1);
  fdui->delete_button = obj = fl_add_button(FL_NORMAL_BUTTON,102,179,87,23,"Delete");
    fl_set_button_shortcut(obj,"^D",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_North, FL_North);
  fl_end_form();

  fdui->add_del_show_form->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

FD_group_form *create_form_group_form(void)
{
  FL_OBJECT *obj;
  FD_group_form *fdui = (FD_group_form *) fl_calloc(1, sizeof(*fdui));

  fdui->group_form = fl_bgn_form(FL_NO_BOX, 800, 406);
  obj = fl_add_box(FL_UP_BOX,0,0,800,406,"");
  obj = fl_add_box(FL_UP_BOX,0,348,800,58,"");
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthEast);

  fdui->label_grp = fl_bgn_group();
  fdui->label1 = obj = fl_add_text(FL_NORMAL_TEXT,13,68,772,18,"label1                                                                                                                                                                                                    ");
    fl_set_object_lcolor(obj,FL_DARKVIOLET);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_North);
    fl_set_object_callback(obj,do_nothing,0);
  fl_end_group();


  fdui->menu_grp = fl_bgn_group();
  obj = fl_add_box(FL_UP_BOX,0,0,797,28,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
  fdui->help_menu = obj = fl_add_menu(FL_PULLDOWN_MENU,727,5,58,16,"Help");
    fl_set_object_shortcut(obj,"^H",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,do_help_menu,0);
  fdui->windows_menu = obj = fl_add_menu(FL_PULLDOWN_MENU,544,5,76,17,"Windows");
    fl_set_object_shortcut(obj,"^W",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_windows_menu,0);
  fdui->options_menu = obj = fl_add_menu(FL_PULLDOWN_MENU,402,4,62,18,"Options");
    fl_set_object_shortcut(obj,"^O",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_options_menu,0);
  fdui->configure_menu = obj = fl_add_menu(FL_PULLDOWN_MENU,240,7,74,17,"Configure");
    fl_set_object_shortcut(obj,"^C",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_config_menu,0);
  fdui->report_menu = obj = fl_add_menu(FL_PULLDOWN_MENU,159,6,66,16,"Report");
    fl_set_object_shortcut(obj,"^R",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_report_menu,0);
  fdui->status_menu = obj = fl_add_menu(FL_PULLDOWN_MENU,80,7,59,16,"Status");
    fl_set_object_shortcut(obj,"^S",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_status_menu,0);
  fdui->file_menu = obj = fl_add_menu(FL_PULLDOWN_MENU,10,7,61,17,"File");
    fl_set_object_shortcut(obj,"^F",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_file_menu,3);
  fl_end_group();


  fdui->stat_label_grp = fl_bgn_group();
  obj = fl_add_text(FL_NORMAL_TEXT,12,327,120,21,"Last Received Msg:");
    fl_set_object_lcolor(obj,FL_RIGHT_BCOL);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->status_label = obj = fl_add_text(FL_NORMAL_TEXT,135,327,651,21,"Text");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_color(obj,FL_INACTIVE,FL_MCOL);
    fl_set_object_lcolor(obj,FL_WHEAT);
    fl_set_object_lalign(obj,FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  fl_end_group();


  fdui->browser_grp = fl_bgn_group();
  fdui->browser = obj = fl_add_browser(FL_HOLD_BROWSER,12,100,775,201,"");
    fl_set_object_color(obj,FL_COL1,FL_MCOL);
    fl_set_object_lcolor(obj,FL_RIGHT_BCOL);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthEast);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->divider_bar = obj = fl_add_box(FL_BORDER_BOX,323,102,2,199,"");
    fl_set_object_color(obj,FL_INACTIVE,FL_INACTIVE);
    fl_set_object_lcolor(obj,FL_BOTTOM_BCOL);
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthWest);
  fl_end_group();

  obj = fl_add_text(FL_NORMAL_TEXT,10,304,408,20,"List of host and tape groupings.   Click on an item above for detail.");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_lcolor(obj,FL_DARKVIOLET);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMES_STYLE+FL_EMBOSSED_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
  fdui->view_menu = obj = fl_add_menu(FL_PULLDOWN_MENU,474,6,58,15,"View");
    fl_set_object_shortcut(obj,"^V",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_view_menu,3);
  fdui->label2 = obj = fl_add_text(FL_NORMAL_TEXT,12,86,773,15,"label2");
    fl_set_object_lcolor(obj,FL_DARKVIOLET);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
  obj = fl_add_text(FL_NORMAL_TEXT,60,354,377,16,"Box(es) will be colored appropriately to draw attention.");
    fl_set_object_lcolor(obj,FL_RIGHT_BCOL);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);

  fdui->signal_grp = fl_bgn_group();
  fdui->unknown_light_button = obj = fl_add_lightbutton(FL_RADIO_BUTTON,649,352,60,15,"");
    fl_set_button_shortcut(obj,"^M",1);
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_color(obj,FL_COL1,FL_CYAN);
    fl_set_object_lcolor(obj,FL_RIGHT_BCOL);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->running_light_button = obj = fl_add_lightbutton(FL_RADIO_BUTTON,580,353,62,14,"");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
    fl_set_object_lcolor(obj,FL_RIGHT_BCOL);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->warning_light_button = obj = fl_add_lightbutton(FL_RADIO_BUTTON,518,352,61,16,"");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_RIGHT_BCOL);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->failure_light_button = obj = fl_add_lightbutton(FL_RADIO_BUTTON,458,353,61,14,"");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_color(obj,FL_COL1,FL_RED);
    fl_set_object_lcolor(obj,FL_RIGHT_BCOL);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  fl_end_group();


  fdui->legend_grp = fl_bgn_group();
  obj = fl_add_box(FL_FRAME_BOX,8,372,780,26,"");
    fl_set_object_gravity(obj, FL_South, FL_South);
  obj = fl_add_text(FL_NORMAL_TEXT,150,374,46,16,"Failed");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_RED);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,198,374,48,16,"Warning");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,250,376,54,14,"Running");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_GREEN);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,363,375,55,16,"Unknown");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_CYAN);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,696,376,75,19,"'*' Finished");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_RIGHT_BCOL);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,502,375,93,19,"Everything was:");
    fl_set_object_lcolor(obj,FL_RIGHT_BCOL);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
  obj = fl_add_text(FL_NORMAL_TEXT,422,376,77,14,"'R'  Running");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_RIGHT_BCOL);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,11,378,60,14,"LEGENDS");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,69,375,81,17,"Something is: ");
    fl_set_object_lcolor(obj,FL_RIGHT_BCOL);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
  obj = fl_add_box(FL_UP_BOX,499,373,1,23,"");
    fl_set_object_lcolor(obj,FL_RIGHT_BCOL);
    fl_set_object_gravity(obj, FL_South, FL_South);
  obj = fl_add_text(FL_NORMAL_TEXT,607,377,69,17,"Successful");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,309,375,48,16,"Aborted");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_DARKTOMATO);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  fl_end_group();


  fdui->data_trans_stat_grp = fl_bgn_group();
  obj = fl_add_text(FL_NORMAL_TEXT,418,304,96,20,"Comm.  status:");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_lcolor(obj,FL_DARKVIOLET);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMES_STYLE+FL_EMBOSSED_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
  fdui->data_trans_status_label = obj = fl_add_text(FL_NORMAL_TEXT,516,304,272,20,"");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_color(obj,FL_DARKCYAN,FL_TOP_BCOL);
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMES_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  fl_end_group();

  fdui->action_menu = obj = fl_add_menu(FL_PULLDOWN_MENU,324,6,59,16,"Action");
    fl_set_object_shortcut(obj,"^A",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_action_menu,3);

  fdui->toolbar_grp = fl_bgn_group();
  obj = fl_add_box(FL_SHADOW_BOX,3,28,794,39,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
  fdui->help_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,739,28,40,31,"");
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,do_help,0);
  fdui->neighbors_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,402,28,40,31,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,popup_add_del_show_form,2);
  fdui->progs_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,366,28,40,31,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,popup_add_del_show_form,1);
  fdui->report_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,330,28,40,31,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,popup_summary_report_form,0);
  fdui->save_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,140,28,40,31,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_write_browser_to_file,3);
  fdui->print_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,100,28,40,31,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_print_all_from_browser,3);
  fdui->reset_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,60,28,40,31,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_reset,0);
  fdui->exit_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,20,28,40,31,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,exit_system,0);
  fdui->detail_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,176,28,40,31,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,popup_product_grouping_form,0);
  fdui->ps_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,252,28,40,31,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,popup_ps_form,0);
  fdui->bar_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,216,28,37,31,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,popup_status_form,0);
  fdui->all_data_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,292,28,39,31,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,popup_all_data_form,0);
  fdui->exceeded_threshold_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,442,28,40,31,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,popup_exceeded_threshold_form,2);
  fdui->del_job_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,482,28,40,31,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fl_end_group();

  fl_end_form();

  fdui->group_form->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

FD_busy_form *create_form_busy_form(void)
{
  FL_OBJECT *obj;
  FD_busy_form *fdui = (FD_busy_form *) fl_calloc(1, sizeof(*fdui));

  fdui->busy_form = fl_bgn_form(FL_NO_BOX, 300, 100);
  obj = fl_add_box(FL_UP_BOX,0,0,300,100,"");
  fdui->label = obj = fl_add_text(FL_NORMAL_TEXT,20,20,260,40,"Welcome to GVS Monitoring System!");
    fl_set_object_boxtype(obj,FL_SHADOW_BOX);
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMES_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthEast);
  fdui->status_label = obj = fl_add_text(FL_NORMAL_TEXT,20,65,260,20,"Initializing...");
    fl_set_object_color(obj,FL_COL1,FL_INACTIVE);
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMES_STYLE);
  fl_end_form();

  fdui->busy_form->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

FD_report_form *create_form_report_form(void)
{
  FL_OBJECT *obj;
  FD_report_form *fdui = (FD_report_form *) fl_calloc(1, sizeof(*fdui));

  fdui->report_form = fl_bgn_form(FL_NO_BOX, 798, 360);
  obj = fl_add_box(FL_UP_BOX,0,0,798,360,"");
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_box(FL_SHADOW_BOX,0,30,798,33,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);

  fdui->menu_grp = fl_bgn_group();
  obj = fl_add_box(FL_UP_BOX,0,3,798,27,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
  fdui->help_menu = obj = fl_add_menu(FL_PULLDOWN_MENU,735,9,55,17,"Help");
    fl_set_object_shortcut(obj,"#H",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,do_help_menu,0);
  fdui->file_menu = obj = fl_add_menu(FL_PULLDOWN_MENU,9,6,56,16,"File");
    fl_set_object_shortcut(obj,"#F",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_file_menu,1);
  fl_end_group();


  fdui->toolbar_grp = fl_bgn_group();
  fdui->help_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,744,30,39,28,"");
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,do_help,0);
  fdui->save_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,47,30,38,28,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_write_browser_to_file,1);
  fdui->print_all_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,158,30,39,28,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->print_yellow_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,120,30,38,28,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->print_red_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,84,30,38,28,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->refresh_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,196,30,39,28,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_refresh_summary_form,0);
  fdui->cancel_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,9,30,38,28,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_close_form,1);
  fl_end_group();


  fdui->browser_grp = fl_bgn_group();
  fdui->browser = obj = fl_add_browser(FL_NORMAL_BROWSER,15,90,765,228,"");
    fl_set_object_color(obj,FL_COL1,FL_MCOL);
    fl_set_object_lcolor(obj,FL_DARKTOMATO);
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthEast);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->label = obj = fl_add_text(FL_NORMAL_TEXT,18,66,759,21,"label                                                                                                                                                                                            ");
    fl_set_object_lcolor(obj,FL_DARKVIOLET);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
    fl_set_object_callback(obj,do_nothing,0);
  fl_end_group();


  fdui->legend_grp = fl_bgn_group();
  obj = fl_add_box(FL_UP_BOX,0,321,798,39,"");
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthEast);
  obj = fl_add_box(FL_FRAME_BOX,464,328,326,26,"");
    fl_set_object_gravity(obj, FL_South, FL_South);
  obj = fl_add_text(FL_NORMAL_TEXT,478,334,88,16,"Everyting was:");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
  obj = fl_add_text(FL_NORMAL_TEXT,686,334,76,16,"* Finished");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_box(FL_FRAME_BOX,12,328,456,26,"");
    fl_set_object_gravity(obj, FL_South, FL_South);
  obj = fl_add_text(FL_NORMAL_TEXT,378,332,70,16,"Unknown");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_CYAN);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,586,336,72,14,"Successful");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,270,330,60,20,"Running");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_GREEN);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,214,332,58,18,"Warning ");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_YELLOW);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,16,334,62,16,"LEGENDS");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthWest);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,164,332,50,18,"Failed");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_RED);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  obj = fl_add_text(FL_NORMAL_TEXT,328,332,52,18,"Aborted");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_DARKTOMATO);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_nothing,0);
  fl_end_group();

  fdui->view_menu = obj = fl_add_menu(FL_PULLDOWN_MENU,146,8,66,16,"View");
    fl_set_object_shortcut(obj,"#v",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_view_menu,1);
  fdui->configure_menu = obj = fl_add_menu(FL_PULLDOWN_MENU,72,8,66,16,"Configure");
    fl_set_object_shortcut(obj,"#c",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,do_config_menu,1);
  obj = fl_add_text(FL_NORMAL_TEXT,80,332,84,18,"Something is:");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthWest);
    fl_set_object_callback(obj,do_nothing,0);
  fdui->tape_button = obj = fl_add_checkbutton(FL_PUSH_BUTTON,366,36,94,20,"Tape Summary");
    fl_set_object_gravity(obj, FL_North, FL_North);
    fl_set_object_callback(obj,select_and_show_summary_report,1);
  fdui->level_button = obj = fl_add_checkbutton(FL_PUSH_BUTTON,486,36,122,20,"GV Level  Summary");
    fl_set_object_gravity(obj, FL_North, FL_North);
    fl_set_object_callback(obj,select_and_show_summary_report,2);
  fdui->product_button = obj = fl_add_checkbutton(FL_PUSH_BUTTON,604,38,116,18,"Product Summary");
    fl_set_object_gravity(obj, FL_North, FL_North);
    fl_set_object_callback(obj,select_and_show_summary_report,4);
  fdui->del_job_button = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,234,30,39,28,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,popup_status_fields_selection_form,1);
  fl_end_form();

  fdui->report_form->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

FD_gv_levels_form *create_form_gv_levels_form(void)
{
  FL_OBJECT *obj;
  FD_gv_levels_form *fdui = (FD_gv_levels_form *) fl_calloc(1, sizeof(*fdui));

  fdui->gv_levels_form = fl_bgn_form(FL_NO_BOX, 395, 329);
  obj = fl_add_box(FL_UP_BOX,0,0,395,329,"");
  obj = fl_add_frame(FL_UP_FRAME,1,129,390,145,"");
  obj = fl_add_frame(FL_UP_FRAME,0,278,395,51,"");
  fdui->level_browser = obj = fl_add_browser(FL_SELECT_BROWSER,26,32,340,67,"Current Level Definitions");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_North, FL_South);
    fl_set_object_callback(obj,do_select_item_from_browser,11);
  fdui->program_input = obj = fl_add_input(FL_NORMAL_INPUT,74,235,118,27,"Program");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_East, FL_East);
  fdui->add_button = obj = fl_add_button(FL_NORMAL_BUTTON,23,292,82,24,"Add");
    fl_set_button_shortcut(obj,"#A",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_modify_gv_levels_table,1);
  fdui->label = obj = fl_add_text(FL_NORMAL_TEXT,24,6,338,25,"");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lcolor(obj,FL_DARKVIOLET);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
  fdui->delete_button = obj = fl_add_button(FL_NORMAL_BUTTON,111,292,82,24,"Delete");
    fl_set_button_shortcut(obj,"#d",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_modify_gv_levels_table,2);
  fdui->clear_button = obj = fl_add_button(FL_NORMAL_BUTTON,202,292,82,24,"Clear");
    fl_set_button_shortcut(obj,"#r",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_clear_input,0);
  fdui->cancel_button = obj = fl_add_button(FL_NORMAL_BUTTON,292,292,82,24,"Close");
    fl_set_button_shortcut(obj,"#C",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_close_form,0);
  fdui->available_progs_browser = obj = fl_add_browser(FL_SELECT_BROWSER,27,143,337,67,"Available Programs");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_West, FL_East);
    fl_set_object_callback(obj,do_select_item_from_browser,11);
  fdui->level_input = obj = fl_add_input(FL_NORMAL_INPUT,247,234,116,28,"Level");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_East, FL_East);
  fl_end_form();

  fdui->gv_levels_form->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

FD_status_fields_selection_form *create_form_status_fields_selection_form(void)
{
  FL_OBJECT *obj;
  FD_status_fields_selection_form *fdui = (FD_status_fields_selection_form *) fl_calloc(1, sizeof(*fdui));

  fdui->status_fields_selection_form = fl_bgn_form(FL_NO_BOX, 408, 456);
  obj = fl_add_box(FL_UP_BOX,0,0,408,456,"");
  obj = fl_add_frame(FL_ENGRAVED_FRAME,2,52,410,334,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthEast);
  fdui->host_input = obj = fl_add_input(FL_NORMAL_INPUT,144,54,224,40,"* Host");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_West, FL_East);
  fdui->tapeid_input = obj = fl_add_input(FL_NORMAL_INPUT,144,110,224,40,"Tape ID");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_West, FL_East);
  fdui->product_input = obj = fl_add_input(FL_NORMAL_INPUT,144,166,224,40,"* Product");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_West, FL_East);
  fdui->sdate_input = obj = fl_add_input(FL_NORMAL_INPUT,146,274,224,40,"** Start Date\n (mm/dd/yy [hh:mm:ss])");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
  obj = fl_add_frame(FL_ENGRAVED_FRAME,0,386,408,70,"");
    fl_set_object_gravity(obj, FL_South, FL_South);
  fdui->ok_button = obj = fl_add_button(FL_NORMAL_BUTTON,62,410,88,28,"OK");
    fl_set_button_shortcut(obj,"#O",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_delete_summary_info,0);
  fdui->clear_button = obj = fl_add_button(FL_NORMAL_BUTTON,164,410,88,28,"Clear");
    fl_set_button_shortcut(obj,"#r",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_clear_status_fields_form,0);
  fdui->cancel_button = obj = fl_add_button(FL_NORMAL_BUTTON,268,410,86,28,"Cancel");
    fl_set_button_shortcut(obj,"#C",1);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
    fl_set_object_callback(obj,do_close_form,0);
  fdui->label = obj = fl_add_text(FL_NORMAL_TEXT,26,10,364,34,"Enter Fields for Job Deletion");
    fl_set_object_boxtype(obj,FL_SHADOW_BOX);
    fl_set_object_lcolor(obj,FL_DARKVIOLET);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
  fdui->level_input = obj = fl_add_input(FL_INT_INPUT,144,218,224,40,"* Level #");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_West, FL_East);
  obj = fl_add_text(FL_NORMAL_TEXT,74,336,304,32,"  * Field may not be applied depending on summary type.\n** Optional.");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
    fl_set_object_gravity(obj, FL_South, FL_South);
  fl_end_form();

  fdui->status_fields_selection_form->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

