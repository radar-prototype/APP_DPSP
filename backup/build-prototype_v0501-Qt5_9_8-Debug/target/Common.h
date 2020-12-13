#ifndef COMMON_H
#define COMMON_H



#include <net/if.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdint.h> /* For size_t */
#include <getopt.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <sched.h>
#include <semaphore.h>
#include <mqueue.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>


using namespace std;


extern "C"{
#include "rsplBspHead.h"

#include "MsgDDS.h"
}



class Common{

  public :
   
   void init();

   void run();

   void uninit();
   
   void join();

};













#endif /* COMMON_H */

