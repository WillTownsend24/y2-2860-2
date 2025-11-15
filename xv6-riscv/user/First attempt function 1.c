void run_command(char *buf, int nbuf, int *pcp) {

  /* Useful data structures and flags. */
  char *arguments[10];
  int numargs = 0;
  /* Flags to mark word start/end */
  int ws = 0;
  //int we = 0;

  /* Flags to mark redirection direction */
  //int redirection_left = 0;
  //int redirection_right = 0;

  /* File names supplied in the command */
  //char *file_name_l = 0;
  //char *file_name_r = 0;

  //int p[2];
  //int pipe_cmd = 0;

  /* Flag to mark sequence command */
  //int sequence_cmd = 0;
  


//Characters = charcters + buf[i]

  //loop variable
  int i = 0;

  //Variable for constructing the word later
  int j = 0;
  /* Parse the command character by character. */
  for (; i < nbuf; i++) {   
       
    // I will find words using spaces to check when they start/finish
    //Stop when reaching end of string
    if (buf[i] == '\0')
      break;  

    //Are we currently sconstructing a word? 0=no
    if (ws == 0){

      //Found space, so not an argument
      if (buf[i] == ' ') {
          ;
      }

      //there is a word
      else {
        ws = 1;
      }
    }
    
    //There is a word 1 = yes
    if (ws == 1){

      if (j == 0) {
        arguments[numargs] = malloc(64);
      }
    
      //Found space, so not an argument
      if (buf[i] == ' ') {
          // Before moving to the next word, add the null terminator.

          //word has ended so complete this substring and add 1 to number of arguments
          arguments[numargs][j] = '\0';
          numargs++;
          j = 0;

          ws = 0;
      }

      //Word continuing
      else {
        ws = 1;

        // allocate memory for word
        arguments[numargs][j] = buf[i];
        //printf(2, "Word: %s\n", arguments[numargs]);
        j++;

        //Confirm end of string with NULL operator?
        arguments[numargs][j] = '\0';
      }
    }

  }
  
  //Code for when the last word doesn't end in a space 
  if (ws == 1) {
    arguments[numargs][j] = '\0';
    numargs++;
  }

  for (int k = 0; k < numargs; k++) {
    fprintf(2, "Final word %d: %s\n", k, arguments[k]);
}

  exit(0);
}