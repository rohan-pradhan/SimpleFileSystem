#include "tests.h"
/*
Simple testing which only tests the basic features in a limited fashion. 
For all tests, -1 is considered error and 0 is considered success. 
*/
int simple_test(){
  printf("\n-------------------------------\nInitializing Simple test.\n--------------------------------\n\n");
  char **write_buf;
  int file_id[ABS_CAP_FD];
  char *file_names[ABS_CAP_FD];
  int *write_ptr = calloc(ABS_CAP_FD, sizeof(int));
  int *file_size = calloc(ABS_CAP_FD, sizeof(int));
  int num_file = 5;
  int iterations = 5;
  int err_no = 0;

  write_buf = calloc(MAX_FD, sizeof(char *));
  for(int i = 0; i < MAX_FD; i++)
    write_buf[i] = calloc(MAX_WRITE_BYTE, sizeof(char));


  //Init fresh file system
  mksfs(1);
  //So now we should be able to open and close files ...
  printf("\n 1 FildID[%d] = %d\n", 0, file_id[0]);
  test_open_new_files(file_names, file_id, num_file, &err_no);
  printf("\n 2 FildID[%d] = %d\n", 0, file_id[0]);
  test_close_files(file_names, file_id, num_file, &err_no);
  printf("\n 3 FildID[%d] = %d\n", 0, file_id[0]);
  //Reopen them after closing
  test_open_old_files(file_names, file_id, num_file, &err_no);
  printf("\n 4 FildID[%d] = %d\n", 0, file_id[0]);
  //Now will attempt to write to file, very small chunk of text
  for(int i = 0; i < iterations; i++){
    printf("\n 5a FildID[%d] = %d\n", 0, file_id[0]);
    test_simple_write_files(file_id, file_size, write_ptr, write_buf, num_file, &err_no);
    printf("\n 5b FildID[%d] = %d\n", 0, file_id[0]);
    test_simple_read_files(file_id, file_size, write_buf, num_file, &err_no);
    printf("\n 5c FildID[%d] = %d\n", 0, file_id[0]);
    test_read_all_files(file_id, file_size, write_buf, num_file, &err_no);
    printf("\n 5d FildID[%d] = %d\n", 0, file_id[0]);
    if(i < iterations - 1) //Fun with frseek and fwseek
      test_seek(file_id, file_size, write_ptr, write_buf, num_file, 10, &err_no);
     printf("\n 5e FildID[%d] = %d\n", 0, file_id[0]);
  }
  //test close + reading
  printf("\n 6 FildID[%d] = %d\n", 0, file_id[0]);
  test_close_files(file_names, file_id, num_file, &err_no);
  printf("\n 7 FildID[%d] = %d\n", 0, file_id[0]);
  test_open_old_files(file_names, file_id, num_file, &err_no);
  printf("\n 8 FildID[%d] = %d\n", 0, file_id[0]);
  test_read_all_files(file_id, file_size, write_buf, num_file, &err_no);
  printf("\n 9 FildID[%d] = %d\n", 0, file_id[0]);

  //testing Remove
  test_remove_files(file_id, file_size, write_ptr, file_names, write_buf, num_file, &err_no);
  printf("\n 10 FildID[%d] = %d\n", 0, file_id[0]);
  free_name_element(file_names, num_file);
  printf("\n 11 FildID[%d] = %d\n", 0, file_id[0]);
  //Make new files and attempt to write in again. 
  test_open_new_files(file_names, file_id, num_file, &err_no);
  printf("\n 12 FildID[%d] = %d\n", 0, file_id[0]);
  //printf("FileID: %d\n", file_id);
  test_simple_write_files(file_id, file_size, write_ptr, write_buf, num_file, &err_no);
  printf("\n 13 FildID[%d] = %d\n", 0, file_id[0]);
  test_read_all_files(file_id, file_size, write_buf, num_file, &err_no);
  printf("\n 14 FildID[%d] = %d\n", 0, file_id[0]);
  
  //test names + size
  test_get_file_name(file_names, num_file, &err_no);
  printf("\n 15 FildID[%d] = %d\n", 0, file_id[0]);
  test_get_file_size(file_size, file_names, num_file, &err_no);
  printf("\n 16 FildID[%d] = %d\n", 0, file_id[0]);
  
  printf("\n-------------------------------\nSimple test Finished.\nCurrent Error Num: %d\n--------------------------------\n\n", err_no);

  free(file_size);
  for(int i = 0; i < MAX_FD; i++){
    free(write_buf[i]);
  }
  free(write_buf);
  free(write_ptr);
  for(int i = 0; i < num_file; i++){
    free(file_names[i]);
  }
  return 0;
}

/* The main testing program
 */
int main(int argc, char **argv){
  simple_test();
}