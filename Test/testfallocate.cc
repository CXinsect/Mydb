#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include <sys/types.h>
#include <string>
#include <string.h>
#include <sys/stat.h>
#include <iostream>

int main (void) {
    int fd = open("2.txt",O_WRONLY | O_CREAT,0644);
    fallocate(fd,FALLOC_FL_KEEP_SIZE,0,20);
    struct stat st;
    std::string path = "2.txt";
    stat(path.c_str(),&st);
    std::cout << st.st_size << std::endl; 
    lseek(fd,20,SEEK_SET);
    write(fd,"c",1);
    stat(path.c_str(),&st);
    std::cout << st.st_size << std::endl; 
    close(fd);
    // fd = open("2.txt",O_WRONLY,0644);
    // ftruncate(fd,15);
    // lseek(fd,10,SEEK_SET);
    // write(fd,"c",1);
    // close(fd);

}