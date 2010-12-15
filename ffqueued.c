#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <openssl/md5.h>

#define QUEUE_FILE        "queue"
#define SWAP_FILE         ".queue.swp"
#define RUN_LOG_PREFIX    "running/"
#define DONE_LOG_PREFIX   "finished/"

#define LINE_LENGTH       1024
#define ARGS_LENGTH       1536
#define CMD_LENGTH        1664

#define MAX_PROCESSES     4

// {{{ Function Definitions

char *parse_args(char *line);
void ffqueued_init();
char *queue_pop();
void read_queue();
void update_progress();
void check_exits();

// }}}
// {{{ Variable Definitions

int running;    ///< Number of running ffmpeg processes
char *line;     ///< urlencoded line read from queue file
char *args;     ///< Assembled arguments as ffmpeg expects them
char *cmd;      ///< Final buffer to send to system()

// }}}
// {{{ int main(int argc, char **argv)
int main(int argc, char **argv)
{
  ffqueued_init();
  while(1)
  {
    read_queue();
    update_progress();
    check_exits();
    sleep(10);
  }
}
// }}}
// {{{ void ffqueued_init()
/**
 * Set up our environment
 */
void ffqueued_init()
{
  line = malloc(LINE_LENGTH);
  args = malloc(ARGS_LENGTH);
  cmd = malloc(CMD_LENGTH);
}

// }}}
// {{{ char *parse_args(char *line)
/**
 * Turns a urlencoded array into an ffmpeg command.
 * @param line The string to decode.
 *
 * The first argument should be the desired output file, the rest should be standard ffmpeg arguments.
 *
 * @example "output_file.m4v?i=input_file.avi&vcodec=h264&b=1536"
 *
 * @return A command that can be passed to the shell to execute
 */
char *parse_args(char *line)
{
  if(line == NULL) return NULL;

  int len = strlen(line);

  // We can't possible have a command shorter than 5 chars. (a?i=b)
  if(len < 5) return NULL;

  memset(args, 0, ARGS_LENGTH);

  char *buf; // Use this to build individual arguments
  int i, wstart = 0;

  for(i = 0;i <= len;i++)
  {
    switch(line[i])
    {
      case '?': // Last word was output file
        line[i] = 0;
        args = strcat(args, &line[wstart]);
        args = strcat(args, " ");
        wstart = i + 1;
        break;

      case '=': // Last word was a key
        line[i] = 0;
        buf = malloc(strlen(&line[wstart]) + 3);
        sprintf(buf, "-%s ", &line[wstart]);
        args = strcat(args, buf);
        free(buf);
        wstart = i + 1;
        break;

      case '\n':
      case 0:
      case '&': // Last word was a value
        line[i] = 0;
        buf = malloc(strlen(&line[wstart]) + 3);
        sprintf(buf, "\"%s\" ", &line[wstart]);
        args = strcat(args, buf);
        free(buf);
        wstart = i + 1;
        break;
    }
  }

  return args;
}
// }}}
// {{{ char *queue_pop()
/**
 * Pop the top line off the queue
 */
char *queue_pop()
{
  FILE *qfp = fopen(QUEUE_FILE, "r");

  if(qfp && fgets(line, LINE_LENGTH, qfp))
  {
    // Write the rest of the file somewhere temporary
    FILE *tfp = fopen(SWAP_FILE, "w");
    while(tfp && !feof(qfp))
      fputc(fgetc(qfp), tfp);
    if(tfp) fclose(tfp);

    fclose(qfp);

    // move truncated file onto QUEUE_FILE
    unlink(QUEUE_FILE);
    rename(SWAP_FILE, QUEUE_FILE);

    return line;
  }

  return NULL;
}
// }}}
// {{{ void read_queue()
/**
 * Read in commands from the queue and start executing them.
 */
void read_queue()
{
  char *hash;

  while(NULL != queue_pop())
  {
    args = parse_args(line);

    memset(cmd, 0, CMD_LENGTH);

    hash = MD5(line, strlen(line), NULL);
    
    snprintf(
      cmd
    , CMD_LENGTH
    , "ffmpeg %s& 2> %s%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx &"
    , args
    , RUN_LOG_PREFIX, hash[ 0], hash[ 1], hash[ 2], hash[ 3], hash[ 4], hash[ 5], hash[ 6], hash[ 7]
                    , hash[ 8], hash[ 9], hash[10], hash[11], hash[12], hash[13], hash[14], hash[15]
    );
    system(cmd);
  }

}
// }}}
// {{{ void update_progress()
/**
 * Scrape ffmpeg output and update progress files
 */
void update_progress()
{
}
// }}}
// {{{ void check_exits()
/**
 * wait() on ffmpeg processes that have finished
 */
void check_exits()
{
  
}
// }}}

