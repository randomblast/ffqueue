#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>

#define QUEUE_FILE        "queue"
#define EXIT_PREFIX       "exit-"
#define PROGRESS_PREFIX   "progress-"

char *cmd(char *line);
void read_queue();

// {{{ int main(int argc, char **argv)
int main(int argc, char **argv)
{
  while(1)
  {
    read_queue();
    sleep(10);
  }
}
// }}}
// {{{ char *cmd(char *line)
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
char *cmd(char *line)
{
  int len = strlen(line);
  char *cmd = malloc(len + 128);
  char *buf; // Use this to build individual arguments
  int i, wstart = 0;

  for(i = 0;i <= len;i++)
  {
    switch(line[i])
    {
      case '?': // Last word was output file
        line[i] = 0;
        cmd = strcat(cmd, "ffmpeg ");
        cmd = strcat(cmd, &line[wstart]);
        cmd = strcat(cmd, " ");
        wstart = i + 1;
        break;

      case '=': // Last word was a key
        line[i] = 0;
        buf = malloc(strlen(&line[wstart]) + 3);
        sprintf(buf, "-%s ", &line[wstart]);
        cmd = strcat(cmd, buf);
        free(buf);
        wstart = i + 1;
        break;

      case 0:
      case '&': // Last word was a value
        line[i] = 0;
        buf = malloc(strlen(&line[wstart]) + 3);
        sprintf(buf, "\"%s\" ", &line[wstart]);
        cmd = strcat(cmd, buf);
        free(buf);
        wstart = i + 1;
        break;
    }
  }

  return cmd;
}
// }}}
// {{{ void read_queue()
/**
 * Read in commands from the queue and start executing them.
 */
void read_queue()
{
  FILE *qfp; 

  if(!(qfp = fopen(QUEUE_FILE, "r")))
    return;

  char *line = malloc(1024); // TODO make this global/static
  
  while(NULL != (fgets(line, 1024, qfp)))
  {
    // cmd(line);
  }

  free(line);
  fclose(qfp);
}
// }}}

