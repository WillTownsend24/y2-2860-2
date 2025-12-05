#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

//This part works far as I can tell currently
//Print the prompt ">>> " and read a line of characters
int getcmd(char *buf, int nbuf) {

  // ##### Place your code here
  // Variable for loop later and for input character amount
  int i, r;
  
  //Print the >> and read the input
  fprintf(2, ">>> ");
  r = read(0, buf, nbuf);
  
  //Remove the \n from the input, \n is only one character and \0 stops it from going further
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

/*
  A recursive function which parses the command
  at *buf and executes it.
*/
__attribute__((noreturn))
void run_command(char *buf, int nbuf, int *pcp) {

  /* Useful data structures and flags. */
  char *arguments[10];
  int numargs = 0;
  /* Flags to mark word start/end */
  int ws = 1;
  int we = 0;

  /* Flags to mark redirection direction */
  int redirection_left = 0;
  int redirection_right = 0;

  /* File names supplied in the command */
  char *file_name_l = 0;
  char *file_name_r = 0;

  int p[2];
  int pipe_cmd = 0;

  /* Flag to mark sequence command */
  int sequence_cmd = 0;

  //loop variable
  int i = 0;
  //Variable for constructing the word later (kept for minimal change)
  int j = 0;
  
  // ***** ADDED small scan for <, >, | (my added comments only) *****
  /* Parse the command character by character. */
  for (; i < nbuf; i++) {

    /* Parse the current character and set-up various flags:
       sequence_cmd, redirection, pipe_cmd and similar. */

    /* ##### Place your code here. */
    if (buf[i] == '<') {
      buf[i] = '\0';
      redirection_left = 1;
      i++;
      while (buf[i] == ' ') i++;
      file_name_l = &buf[i];
      while (buf[i] != ' ' && buf[i] != '\0') i++;
      buf[i] = '\0';
    }
    if (buf[i] == '>') {
      buf[i] = '\0';
      redirection_right = 1;
      i++;
      while (buf[i] == ' ') i++;
      file_name_r = &buf[i];
      while (buf[i] != ' ' && buf[i] != '\0') i++;
      buf[i] = '\0';
    }
    if (buf[i] == '|') {
      buf[i] = '\0';
      pipe_cmd = 1;
      i++;
      while (buf[i] == ' ') i++;
      break; // only single pipe
    }
    // ******************************************************************

    if (!(redirection_left || redirection_right)) {
      /* No redirection, continue parsing command. */

      // Place your code here.
      // Parse code for breaking down the command character by character.
      if (buf[i] == '\0')
        break;
      if (ws == 1) {
        if (buf[i] == ' ') {
          ;
        }
        else {
          ws = 0;
          we = 0;
          /* point directly into buf*/
          if (numargs < 10) {
            arguments[numargs] = &buf[i];
          }
          j = 0;
        }
      }
      if (ws == 0) {
        if (buf[i] == ' ') {
          /* terminate the word in-place */
          buf[i] = '\0';
          ws = 1;
          we = 1;
          if (numargs < 10) numargs++;
          j = 0;
        }
        else {
          we = 0;
          j++;
        }
      }
    } else {
      /* Redirection command. Capture the file names. */

      // ##### Place your code here.
    }
  }

  // If last word didn't end with a space and hasn't been counted yet
  if (ws == 0 && we == 0) {
    if (numargs < 10) numargs++;
    we = 1;
  }
  
  //terminate argument list? I don't know why we have to do this
  arguments[numargs] = 0;

  /* Code I used to test if my parse output was working
  for (int k = 0; k < numargs; k++) {
    fprintf(2, "Final word %d: %s\n", k, arguments[k]);
  }
    */

  if (numargs == 0)
    exit(0); // empty line

  /*
    Sequence command. Continue this command in a new process.
    Wait for it to complete and execute the command following ';'.
  */
  if (sequence_cmd) {
    sequence_cmd = 0;
    if (fork() != 0) {
      wait(0);
      // ##### Place your code here.
      // Call run_command recursively
    }
  }

  /*
    If this is a redirection command,
    tie the specified files to std in/out.
  */
  // ***** ADDED redirection handling (my comment) *****
  if (redirection_left) {
    // ##### Place your code here.
    int fd = open(file_name_l, 0);
    if (fd >= 0) {
      dup(fd);
      close(fd);
    }
  }
  if (redirection_right) {
    // ##### Place your code here.
    int fd = open(file_name_r, O_WRONLY | O_CREATE);
    if (fd >= 0) {
      dup(fd);
      close(fd);
    }
  }
  // ****************************************************

  /* Parsing done. Execute the command. */

  /*
    If this command is a CD command, write the arguments to the pcp pipe
    and exit with '2' to tell the parent process about this.
  */
  // These ones are pre built in so just call them i think
  if (strcmp(arguments[0], "cd") == 0) {
    // ##### Place your code here.
    // if no argument given, print message
    if (arguments[1] == 0) {
      fprintf(2, "cd: missing directory\n");
    } else {
      // try to change directory
      if (chdir(arguments[1]) < 0) {
        fprintf(2, "cd: cannot change to %s\n", arguments[1]);
      }
    }
    exit(0); // don't fork for cd
  } else {
    /*
      Pipe command: fork twice. Execute the left hand side directly.
      Call run_command recursion for the right side of the pipe.
    */
    // ***** ADDED PIPE HANDLING (my comment) *****
    if (pipe_cmd) {
      // ##### Place your code here.
      pipe(p);
      int pid1 = fork();
      if (pid1 == 0) {
        // chuld fork
        close(p[0]);
        dup(p[1]);
        close(p[1]);
        exec(arguments[0], arguments);
        fprintf(2, "exec %s failed\n", arguments[0]);
        exit(1);
      }
      int pid2 = fork();
      if (pid2 == 0) {
        close(p[1]);
        dup(p[0]);
        close(p[0]);
        run_command(&buf[i], nbuf - i, pcp);
      }
      close(p[0]);
      close(p[1]);
      wait(0);
      wait(0);
      exit(0);
    // ***************************************************
    } else {
      // ##### Place your code here.
      // Simple command; call exec()
      // implementation of the fork and execution
      exec(arguments[0], arguments);
      fprintf(2, "exec %s failed\n", arguments[0]);
      exit(1);
    }
  }
  exit(0);
}

int main(void) {

  static char buf[100];

  int pcp[2];
  pipe(pcp);

  /* Read and run input commands. */
  while(getcmd(buf, sizeof(buf)) >= 0){
    if(buf[0] == '\0')
      continue;
    
    if(strcmp(buf, "exit") == 0)
      exit(0);
    
    if(fork() == 0)
      run_command(buf, 100, pcp);

    /*
      Check if run_command found this is
      a CD command and run it if required.
    */
    int child_status;
    // ##### Place your code here
    // parent fork
    wait(&child_status);
  }
  exit(0);
}