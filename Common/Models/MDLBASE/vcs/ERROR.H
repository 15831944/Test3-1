/* Error codes used by all of the VCS COMPILER AND SOLVER routines are 
/* defined here the data base error codes go from 1 to 8 and start with D_ */

#define NO_ERR      0
#define NO_MEMBER   9     /* in stream.c */
#define NO_MEMORY   10
#define TOO_MANY    11
#define OPEN_FAILURE  12      /* in setup.c */
#define SSCAN_FAIL      13
#define ATOI_FAIL       14
#define ATOF_FAIL   15
#define ILLEGAL_TOKEN   16
#define WARNING         17
#define NO_DEF          18
#define STRTOD_FAIL   19
#define NO_UNI      20
#define FORWARD_DEC   21
#define OUT_OF_RANGE  22
#define FATAL_ERR   23
#define VCS_SPECIE_OVFLOW 24
#define SOLVER_FAILURE  25
#define VCS_NO_CONV   26
#define VCS_PHASE_ERR 27
#define VCS_ELEM_OVFLOW 25
#define P_EXPRESS_ERR   26        /* in parser.c */
#define P_NO_FORM   27              /* in parser.c */
#define ACT_DEFINED   28
#define PIPE_CANT_SPLIT 29  /* Pipe can not split itself - only a node can split a pipe */
#define NODE_CANT_SPLIT_MANY 30 /* A node can not split many in comimin pipes */
#define SPLIT_VAL       31  /* Splitter ratios do not accumalte to 1 */
#define LOSS_DEFINED    32  /* The pipe defining the node loss was defined before */
               /* Clips interface error codes start here */
#define HASH_DUP_KEY    41
#define HASH_NO_MEM     42
#define VCSTHERM_CON    43
