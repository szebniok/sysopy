#ifndef LIB_RECORDS_H
#define LIB_RECORDS_H

void lib_sort(char* filename, int records_count, int record_size);

void lib_copy(char* src_filename, char* dst_filename, int records_count,
              int buffer_size);

#endif