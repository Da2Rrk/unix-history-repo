	public void end_logfile ();
	public void sync_logfile ();
	public int ch_seek ();
	public int ch_end_seek ();
	public int ch_beg_seek ();
	public POSITION ch_length ();
	public POSITION ch_tell ();
	public int ch_forw_get ();
	public int ch_back_get ();
	public int ch_nbuf ();
	public void ch_flush ();
	public void ch_pipe ();
	public void ch_nonpipe ();
	public void cmd_reset ();
	public int len_cmdbuf ();
	public int cmd_erase ();
	public int cmd_char ();
	public int cmd_int ();
	public void cmd_putstr ();
	public char * get_cmdbuf ();
	public void ungetcc ();
	public void ungetsc ();
	public void commands ();
	public int cmd_decode ();
	public int add_cmdtable ();
	public void add_hometable ();
	public void help ();
	public POSITION forw_line ();
	public POSITION back_line ();
	public void prewind ();
	public void plinenum ();
	public int pappend ();
	public void pdone ();
	public int gline ();
	public void null_line ();
	public POSITION forw_raw_line ();
	public POSITION back_raw_line ();
	public void clr_linenum ();
	public void add_lnum ();
	public int find_linenum ();
	public POSITION find_pos ();
	public int currline ();
	public void strtcpy ();
	public char * save ();
	public VOID_POINTER ecalloc ();
	public char * skipsp ();
	public void quit ();
	public int edit ();
	public void edit_list ();
	public int edit_first ();
	public int edit_last ();
	public int edit_next ();
	public int edit_prev ();
	public int edit_index ();
	public void cat_file ();
	public void use_logfile ();
	public void scan_option ();
	public void toggle_option ();
	public int single_char_option ();
	public char * opt_prompt ();
	public int isoptpending ();
	public void nopendopt ();
	public int getnum ();
	public void opt_o ();
	public void opt__O ();
	public void opt_l ();
	public void opt__L ();
	public void opt_k ();
	public void opt_t ();
	public void opt__T ();
	public void opt_p ();
	public void opt__P ();
	public void opt_b ();
	public void opt_v ();
	public void opt_W ();
	public void opt_query ();
	public void init_option ();
	public struct option * findopt ();
	public int iread ();
	public void intread ();
	public long get_time ();
	public char * errno_message ();
	public char * errno_message ();
	public void init_charset ();
	public int binary_char ();
	public int control_char ();
	public char * prchar ();
	public char * homefile ();
	public char * find_helpfile ();
	public char * fexpand ();
	public char * glob ();
	public char * glob ();
	public char * bad_file ();
	public POSITION filesize ();
	public char * bad_file ();
	public POSITION filesize ();
	public void lsystem ();
	public int pipe_mark ();
	public int pipe_data ();
	public void put_line ();
	public void flush ();
	public void putchr ();
	public void putstr ();
	public void error ();
	public void ierror ();
	public int query ();
	public POSITION position ();
	public void add_forw_pos ();
	public void add_back_pos ();
	public void pos_clear ();
	public void pos_init ();
	public int onscreen ();
	public int empty_screen ();
	public int empty_lines ();
	public void get_scrpos ();
	public int adjsline ();
	public IFILE next_ifile ();
	public IFILE prev_ifile ();
	public int nifile ();
	public IFILE get_ifile ();
	public char * get_filename ();
	public int get_index ();
	public void store_pos ();
	public void get_pos ();
	public void match_brac ();
	public void forw ();
	public void back ();
	public void forward ();
	public void backward ();
	public int get_back_scroll ();
	public void jump_forw ();
	public void jump_back ();
	public void repaint ();
	public void jump_percent ();
	public void jump_line_loc ();
	public void jump_loc ();
	public int search ();
	public void init_mark ();
	public int badmark ();
	public void setmark ();
	public void lastmark ();
	public void gomark ();
	public POSITION markpos ();
	public void init_prompt ();
	public char * pr_expand ();
	public char * eq_message ();
	public char * pr_string ();
	public void raw_mode ();
	public void get_scrsize ();
	public void get_term ();
	public void init ();
	public void deinit ();
	public void home ();
	public void add_line ();
	public void lower_left ();
	public void bell ();
	public void vbell ();
	public void clear ();
	public void clear_eol ();
	public void so_enter ();
	public void so_exit ();
	public void ul_enter ();
	public void ul_exit ();
	public void bo_enter ();
	public void bo_exit ();
	public void bl_enter ();
	public void bl_exit ();
	public void backspace ();
	public void putbs ();
	public void fake_interrupt ();
	public HANDLER winch ();
	public HANDLER winch ();
	public void init_signals ();
	public void psignals ();
	public void findtag ();
	public int tagsearch ();
	public void open_getchr ();
	public int getchr ();
