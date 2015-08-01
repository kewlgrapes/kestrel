// kestrel.h -- main program interface specification

// Prerequisites for use:
//   In kestrel.c, but nowhere else,
//     EXTERN must be defined first

#ifndef EXTERN
        #define EXTERN extern
#endif

//
// Information from the command line
//

EXTERN const char * main_progname; // the program name
EXTERN const char * main_infile;   // the input file name, NULL if from stdin
EXTERN const char * main_outfile;  // the output file name, NULL if to stdout

// any additional command line option values go here

// epilogue
#undef EXTERN
