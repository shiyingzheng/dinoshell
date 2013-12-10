/*Shiying Zheng and Ben Stern hw10*/

#define _POSIX_SOURCE
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "arraylist.h"

#define LINE_MAX 1024
#define TOK_MAX 100
