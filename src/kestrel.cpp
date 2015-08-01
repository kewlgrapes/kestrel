// kestrel.cpp : main program for kestrel compiler
//   author: members of the Venture group

// problems:
//   doesn't default to stdin/stdout
//   need help messages
//   need better error reporting
using namespace std;
#include <list>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "sets.h"
#include "lexical.h"
#include "blocks.h"
#include "codegenerator.h"
#include "kestrel.h"
#include "locale.h"

const char *main_progname = LOC_DEFAULTNAME; // name of compiler
list<char*> input_files; 
char *output_file;

void read_command_args(int argc, char **argv) {
  // this probably won't happen
  if (argv == NULL) error_fatal(ER_NOINPUT);
  
  // get the name of process
  if ((argc > 0) && (argv[0] != NULL))
    main_progname = argv[0];

  // read command line arguments
  for (int i = 1; i < argc; i++) {
    // flag argument
    if (*argv[i] == LOC_FLAGCHAR) {
      argv[i]++;
      
      // HELP FLAG
      if ((!strcmp((argv[i]), "?")) || (!strcmp((argv[i]), "help"))) {
        // determine type of help to display...
        print_help(HLP_DEFAULT);
        exit(EXIT_SUCCESS);
      }

      // OUTPUT FLAG
      else if (*argv[i] == 'o'){ // identifies -o
	argv[i]++;
        
        // ./kestrel * -o example.o
        if (*argv[i] == '\0')
          if (++i == argc) break;

        // ./kestrel * -o=example.o
        else if (*argv[i] == '=') argv[i]++;

        // ./kestrel * -oexample.o
        else {}

        // an output file was already given
        if (output_file) 
          error_warn(ER_MULTOUT);

        output_file = argv[i];
      } 

      // COMPILE FLAG
      else if (*argv[i] == 'c') { // identifies -c
        argv[i]++; // strip off the letter c
        
        // ./kestrel -c example.k
        if (*argv[i] == '\0')
          if (++i == argc) break;

        // ./kestrel -c=example.k
        else if (*argv[i] == '=') argv[i]++;

        // ./kestrel -cexample.k
        else {}

        input_files.push_back(argv[i]);
      }
      
      // UNKNOWN FLAG
      else 
        error_warn(ER_OPTION, main_progname);
    }

    // INPUT FILE
    else
      input_files.push_back(argv[i]);
  }
}

// generate an output file name for a given input file
// example.k => example.o  example -> example.o
void gen_output_name(char *in_name, char *out_name) {
  if (!in_name) {
    out_name = NULL;
    return;
  }

  int dir_pos = -1;
  int ext_pos = -1;
 
  // find start position of extension
  int i = 0;
  while (in_name[i]) {
    if (in_name[i] == '/')
      dir_pos = i;
    else if (in_name[i] == '.')
      ext_pos = i;
    i++;
  }

  // copy the input file name, starting after the last '/'
  strcpy(out_name, in_name + (dir_pos + 1));
  
  // if there is an extension, discard it
  if (ext_pos != -1) out_name[ext_pos - dir_pos - 1] = '\0';

  // append .s extension
  strcat(out_name, ".s");
}

// for each input file, run compiler and output to file.s
void compile() {
  char ofn[80]; // output file name
  list<char*>::iterator fn; // input file name
  for (fn = input_files.begin(); fn != input_files.end(); fn++) {
    // open files
    lex_open(*fn);    
    gen_output_name(*fn, ofn);
    code_open(ofn);
    
    // generate code!
    code_prologue(*fn);
    parse_program();
    code_epilogue();
    fclose(code_out);
  }
}

// combine output of multiple object files into a single file
void link() {
}

int main(int argc, char **argv) {
  read_command_args(argc, argv);

  // compile
  if (!input_files.size()) input_files.push_back(NULL);
  compile();

  //if (output_file) link();
}
