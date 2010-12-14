#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>

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

int main(int argc, char **argv)
{
  // Test case for cmd()
  if(argc > 1)
    printf("%s\n", cmd(argv[1]));
}

