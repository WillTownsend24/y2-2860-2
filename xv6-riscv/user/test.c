#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"

void run_command(char *buf, int nbuf, int *pcp);  //

//This part works far as I can tell currently
/* Print the prompt ">>> " and read a line of characters
   from stdin. */
int getcmd(char *buf, int nbuf) {

  // Variable for loop later and for input character amount
  int i, r;

  // ##### Place your code here
  //Print the >> and read the input
  fprintf(2, ">>> ");
  r = read(0, buf, nbuf);

  /*Remove the \n from the input, \n is only one character, \0 stops it from
  reading another further */
  for (i = 0; i < nbuf; i++) {

    
    if (buf[i] == '\n'){
      buf[i] = '\0';
      break;

    }

    //Check if we have reached to end of the string to break
    if (buf[i] == '\0') {
      break;
    }
  }
  //return
  return r;
}

void run_command(char *buf, int nbuf, int *pcp) {

  char *arguments[10];
  int numargs = 0;
  /* Flags to mark word start/end */
  int ws = 0;
  int we = 0;

  //loop variable
  int i = 0;

  //Variable for constructing the word later (kept for minimal change)
  int j = 0;

  /* Parse the command character by character. */
  for (; i < nbuf; i++) {

    if (buf[i] == '\0')
      break;

    if (ws == 0) {

      if (buf[i] == ' ') {
        ;
      }
      else {
        ws = 1;
        we = 0;

        /* point directly into buf*/
        if (numargs < 10) {
          arguments[numargs] = &buf[i];
        }
        j = 0;
      }
    }

    if (ws == 1) {

      if (buf[i] == ' ') {
        /* terminate the word in-place */
        buf[i] = '\0';
        ws = 0;
        we = 1;
        if (numargs < 10) numargs++;
        j = 0;
      }
      else {
        we = 0;
        j++;
      }
    }
  }

  /* If last word didn't end with a space and hasn't been counted yet */
  if (ws == 1 && we == 0) {
    if (numargs < 10) numargs++;
    we = 1;
  }

  /* terminate argument list for exec-style calls */
  arguments[numargs] = 0;

  /* Diagnostic output (kept from your version) */
  for (int k = 0; k < numargs; k++) {
    fprintf(2, "Final word %d: %s\n", k, arguments[k]);
  }

   arguments[numargs] = 0;  // exec wants NULL terminated list

  if (numargs == 0)
    return; // empty line

  // --------------------------------------------
  //    Built-ins handled WITHOUT forking
  // --------------------------------------------
  if (strcmp(arguments[0], "cd") == 0) {
    if (arguments[1] == 0) {
      fprintf(2, "cd: missing argument\n");
    } else {
      if (chdir(arguments[1]) < 0) {
        fprintf(2, "cd: cannot cd to %s\n", arguments[1]);
      }
    }
    return;
  }

  if (strcmp(arguments[0], "exit") == 0) {
    exit(0);
  }

  // --------------------------------------------
  //    External commands: fork + exec
  // --------------------------------------------
  int pid = fork();
  if (pid < 0) {
    fprintf(2, "fork failed\n");
    return;
  }

  if (pid == 0) {
    // ------------- CHILD -------------
    exec(arguments[0], arguments);
    fprintf(2, "exec %s failed\n", arguments[0]);
    exit(1);
  }

  // ------------- PARENT -------------
  wait(0);

  return;
}

//Main void for tests
int main(void) {
  char buf[100];
  getcmd(buf, sizeof(buf));
  run_command(buf, sizeof(buf), 0);

  exit(0);
}