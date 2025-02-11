#ifndef UNITTEST_CANPIE_FD_H_
#define UNITTEST_CANPIE_FD_H_

void can_main_hw_set_log_file_name(char *file_name);
int can_main_hw_log_open(void);
void can_main_hw_log_close(void);
char* get_can_main_hw_log_file_path(void);
void print_first_few_bytes(char* file_name);

#endif /* UNITTEST_CANPIE_FD_H_ */
