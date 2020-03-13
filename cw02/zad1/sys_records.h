#ifndef SYS_RECORDS_H
#define SYS_RECORDS_H

void sys_generate(char* filename, int records_count, int record_size);

void sys_sort(char* filename, int records_count, int record_size);

void sys_copy(char* src_filename, char* dst_filename, int records_count,
              int buffer_size);

#endif