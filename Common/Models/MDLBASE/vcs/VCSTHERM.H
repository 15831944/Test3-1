/*----------------------------------------------------------------------*/
/* VCSTHERM.H header file for asctherm                                  */
/* 7/11/90 BDB created                                                  */
/*----------------------------------------------------------------------*/
#ifndef _VCSTHERM
#define _VCSTHERM

typedef    unsigned int    INDEX;
#define    FALSE           0
#define    TRUE            !FALSE
#define    MAX_C_UNI_ELEM  7
#define    MAX_UNI_ELEM    85
#define    NAME            26

/* Define the basic structures */

enum    Elem  { Nil = 0,H, He, Li, Be, B, C , N , O , F , Ne,
      Na, Mg, Al, Si, P , S , Cl, Ar, K , Ca,
      Sc, Ti, V , Cr, Mn, Fe, Co, Ni, Cu, Zn,
      Ga, Ge, As, Se, Br, Kr, Rb, Sr, Y, Zr, Nb, Mo,
      Tc, Ru, Rh, Pd, Ag, Cd, In, Sn
};


enum    Object { UNDEF, COMP, STREAM, PIPE, NODE, UNIT, MOLE, TEMP, PRESS, OP_CODE, NDE_UTL };

struct Element_array {
    enum    Elem     element[MAX_C_UNI_ELEM];
    double            no_of_element[MAX_C_UNI_ELEM];
};

struct Phase_detail {
    double   H0,
      S0,
      C1,
      C2,
      C3,
      C4,
      lower_temp,
      higher_temp;
};

struct Compound {
    char    name[NAME];
    char    phase_code[3];
    int     elemts;
    double   m_mass;                        /* molecular mass of the compound    */
    INDEX   total_moles;                   /* Points to the global moles array        */
    struct  Element_array    elem_list;    /* Points to the elemental definition array */
    struct  Phase_detail     p_det;        /* Points to the phase detail definition array */
};

#ifdef __cplusplus
extern "C" {
#endif
void    c_set( INDEX cmp, char *name, char *p_code, int no_of_elements, double m_mass,
         INDEX mass, struct Element_array *e_list,
         struct Phase_detail *init_phase);
double    c_mole_mass( INDEX cmp );
double    c_delta_Cp( INDEX cmp, double Temp );
double    c_delta_H( INDEX cmp, double Temp );
double    c_delta_S( INDEX cmp, double Temp );
double    c_delta_G( INDEX cmp, double Temp );
int      c_num_uni_elemts( INDEX cmp );                        /* Return the no of unique elements           */
void     c_uni_elemts( INDEX cmp, struct Element_array *E_List );    /* Return a list of unique elements in E_List */
double    c_get_mole( INDEX cmp );                            /* Return the number of moles             */
void     c_set_mole( INDEX cmp, double mass );                /* Set the number of moles             */
double    c_get_mass( INDEX cmp );                            /* Return the mass                         */
void     c_set_mass( INDEX cmp, double mass );                /* Set the mass                         */
double    c_heat( INDEX cmp, double Temp );                    /* Return Heat content of the compound        */
double    c_Cp( INDEX cmp, double Temp );                      /* Return the heat capacity at temp           */
void     c_zero_mole( INDEX cmp );                            /* zero compound moles */
char     *c_name( INDEX cmp );                                /* return pointer to the compound name          */
char   *c_get_phase_code( INDEX cmp );            /* return the temperature range/phase code */
#ifdef __cplusplus
}
#endif

enum    Phase  { PURE, GASMIX, MIX1, MIX2, MIX3, MIX4, MIX5 };
#define MAX_S_COMPS       20
#define MARKER            0xFFFF
#define UNDEFM            0XFFFE
/* A stream consists of a dictionary of compounds and a single homogenous    */
/* mixed phase.                                                             */
struct Stream {
    int             no_of_comp;                 /* Number of compounds in this stream    */
    char            s_name[NAME];
    enum Phase      s_phase;                    /* Phase type of this stream            */
    double           s_alpha;                    /* Indicate if the Thermo calc should be ideal or non-ideal */
    INDEX           s_moles;                    /* Total moles for this stream            */
    INDEX           comp[MAX_S_COMPS];          /* List of compounds that the stream owns */
    INDEX           act_c[MAX_S_COMPS];         /* Points to the activity formalism array */
    double           frac[MAX_S_COMPS];          /* Mole fraction of each compound defined in the stream */
};

#ifdef __cplusplus
extern "C" {
#endif
void    s_clear( INDEX strm, INDEX mole_ptr);        /* Clear the stream                        */
void    s_set( INDEX strm, char *name, enum Phase phase_code, double alpha);
int     s_add( INDEX strm, INDEX C );                /* Add a compound to the stream         */
int     s_del( INDEX strm, INDEX C );                /* Delete compound from the stream      */
void    s_reset_uni_elem( INDEX strm );                /* Reset s_uni_mat if the total  */
                /* moles of a compound changed */
int     s_no_of_comp( INDEX strm );                    /* Return no of compounds in the stream */
INDEX   s_get_comp( INDEX strm, int comp);            /* Get the compound INDEX number of comp*/
int     s_num_uni_elemts( void );                    /* Return the no of unique elements     */
void    s_uni_elemt_add( double *E_Mat );            /* Add the streams unique elements to E_Mat */
double   s_get_mole( INDEX strm );                    /* Return the number of moles           */
void    s_set_mole( INDEX strm, double mass );        /* Set the number of moles               */
double   s_get_mass( INDEX strm );                    /* Return the mass                       */
void    s_set_mass( INDEX strm, double mass );        /* Set the mass                           */
void    s_reset_frac_mole( INDEX strm );            /* Reset the fraction and total moles for the stream */
void    s_copy_frac_mole( INDEX sstrm, INDEX dstrm );/* Copy the stream's fraction array from source to destination
                                                        - this will qonly work for streams with identical streams */
double   s_heat( INDEX strm, double Temp );            /* Return the total heat content of the stream*/
double   s_Cp( INDEX strm, double Temp );                /* Return the total Cp content of the stream*/
double   s_get_alpha( INDEX strm );                    /* Return the alpha value for this stream */
void    s_set_alpha( INDEX strm, double alpha );        /* Set the alpha value for this stream */
char   *s_name( INDEX strm );                        /* Return a pointer to the stream name */
INDEX s_phase( INDEX strm );             /* get the phase for this stream */
#ifdef __cplusplus
}
#endif

#define    MAX_P_STREAMS 7      /* One for each phase - MUST CORRESPOND  to "Phase" above and MAXPHASE in SOLVE3.H */

/* A pipe consists of multiple streams (one stream per phase) which could */
/* influence the composition of the streams.                              */
/* The compositions of the streams in the pipe can change if the          */
/* temperature or the pressure of the pipe changes (similar to a node).   */
/* The action of the change we will call an incident.                     */
struct Pipe {
    char    p_name[NAME];            /* name of this pipe                */
    int     no_of_streams;            /* Number of streams in this pipe   */
    INDEX   p_temp,                    /* Temperature of the pipe          */
            p_press,                /* Pressure of the pipe             */
            new_p_temp,             /* New temperature for the incidence*/
            new_p_press,            /* New pressure for the incidence   */
            p_moles;                /* Total moles for this pipe        */
    INDEX   Str[MAX_P_STREAMS];        /* List of streams                  */
    INDEX   disrupt;                /* Pointer to the function solving  */
                                    /* the incident (usually new Temp and */
                                    /* Press). If new_P_Temp and new_P_Press */
                                    /* is equal to P_Temp and P_Press resp.  */
                                    /* the function would abort.             */
};

#ifdef __cplusplus
extern "C" {
#endif
void    p_clear( INDEX pipe, INDEX mole_ptr );/* Initialise  pipe                 */
void    p_set( INDEX pipe, char *name, INDEX Temp, INDEX Press, INDEX N_Temp,
         INDEX N_Press, INDEX func );
int    p_add( INDEX pipe, INDEX S );        /* Add a stream to the pipe            */
int    p_del( INDEX pipe, INDEX S );        /* Delete stream from the pipe      */
// ---- old function name ------------
//int  p_num( INDEX pipe );                /* Return no of streams in the pipe */
int    p_no_of_streams( INDEX pipe );        /* Return no of streams in the pipe */
INDEX  p_get_stream( INDEX pipe, int strm );/* Get the INDEX no of strm        */
INDEX  p_temp_index( INDEX pipe );         /* return the index to the temperature */
INDEX  p_press_index( INDEX pipe );        /* return the index to the pressure */
INDEX  p_ntemp_index( INDEX pipe );         /* return the index to the temperature */
INDEX  p_npress_index( INDEX pipe );        /* return the index to the pressure */
void   p_reset_uni_elem( INDEX pipe );        /* Reset p_uni_mat if the total  */
                      /* moles of a compound changed */
int    p_num_uni_elemts( void );            /* Return the no of unique elements in the pipe */
void   p_uni_elemt_add( double *Mat );        /* Add the unique elements of the pipe to Mat*/
double  p_get_mole( INDEX pipe );            /* Return the number of moles in the pipe */
void   p_set_mole( INDEX pipe, double mass);/* Set the number of moles in the pipe */
double  p_heat( INDEX pipe, double T );        /* Return the entalphy of the pipe    */
double  p_get_mass( INDEX pipe );            /* Return mass in the pipe             */
void   p_set_mass( INDEX pipe, double mass);/* Set the mass in the pipe         */
double  p_Cp( INDEX pipe, double T );        /* Return the total Cp of the pipe    */
double  p_get_temp( INDEX pipe );            /* Return the temperature of the pipe */
void   p_set_temp( INDEX pipe, double T );    /* Set the temperature of the pipe */
double  p_get_press( INDEX pipe );            /* Return the pressure of the pipe    */
void   p_set_press( INDEX pipe, double P );    /* Set the pressure of the pipe    */
double  p_get_ntemp( INDEX pipe );            /* Return the new temperature of the pipe */
void   p_set_ntemp( INDEX pipe, double T );    /* Set the new temperature of the pipe */
double  p_get_npress( INDEX pipe );            /* Return the new pressure of the pipe    */
void   p_set_npress( INDEX pipe, double P );    /* Set the new pressure of the pipe    */
int    p_incident( INDEX pipe );            /* Invoke the the function (*disrupt)() solving */
char   *p_name( INDEX pipe );                /* return a pointer to name of the pipe   */
INDEX  p_solver_index( INDEX pipe );    /* get the solver type */
#ifdef __cplusplus
}
#endif
                                            /* the influence of the new pipe temperature    */
                                            /* and new pipe pressure                        */

#define MAX_N_PIPES    8        /* No. of 'IN' pipes and 'OUT' pipes */
enum Direction {IN, OUT};
#define MAX_SOLVER    6
enum Solver {NONE, VCS, VCSTHERM, SPLITTER, ADDER, ALL};

/* A node will re-distribute the compounds of the pipes going into the node  */
/* among the pipes going out of the node, given the temperature and pressure */
/* of the node.                                                                 */
/* Each node can have up to MAX_SOLVER number of solvers - we will start     */
/* solving with the first and then continue until num_of_solvers             */
struct Node {
    char    n_name[NAME];                /* Name of this node                 */
    int     no_of_P_in,                    /* Number of pipes into node         */
            no_of_P_out;                /* Number of pipes out off node      */
    INDEX   pipe_in[MAX_N_PIPES],        /* List of the pipe into the node    */
            pipe_out[MAX_N_PIPES];        /* List of the pipe out of the node  */
  INDEX pipe_loss;          /* material loss across a node into a pipe */
  INDEX   n_temp,                     /* Temperature of the node at which reactions occur  */
            n_press;                    /* Pressure of the node at which the reactions occur */
    int     num_of_solvers;                /* Number of node solvers            */
  INDEX   N_solvers[MAX_SOLVER];        /* Chronological array of function    */
                    /* pointers to solve the node problem.*/
  INDEX   split[MAX_N_PIPES];            /* For a node that splits an incoming */
                    /* pipe - > define the split ratio index   */
                    /* into nde_util_array                  */
  INDEX   heat_in;                    /* Heat input into the system         */
                    /* as an index into nde_util_array      */
};


#ifdef __cplusplus
extern "C" {
#endif
void    n_clear( INDEX node );                            /* Clear node                    */
void    n_set( INDEX node, char *name, INDEX Temp, INDEX Press, int Num_solvers, INDEX func[], INDEX splt[], INDEX heat );
int     n_add( INDEX node, INDEX P, enum Direction io);    /* Add a pipe to the node        */
int     n_del( INDEX node, INDEX P, enum Direction io);    /* Delete a pipe from the node   */
// --------- old function name --------------
//int      n_num( INDEX node, enum Direction io);            /* Return the number of pipes    */
int     n_no_of_pipes( INDEX node, enum Direction io);    /* Return the number of pipes    */
                            /* going into or out of the node */
int   n_add_losses( INDEX node, INDEX pipe);  /* add the pipe with the losses going out of the node */
int   n_are_there_losses ( INDEX node );      /* check if there are losses */
INDEX   n_get_losses_pipe( INDEX node );      /* return the pipe containing the losses */
INDEX   n_get_pipe( INDEX node, int pipe, enum Direction io );/* Get the INDEX no of pipe in/out */
int     n_get_solvers( INDEX node, int *list );       /* Return the number and list of the solvers */
void    n_reset_uni_elem( INDEX node );                    /* Reset n_uni_mat if the total  */
                            /* moles of a compound changed */
int     n_num_uni_elemts( void );                        /* Return the no of unique elements in the pipe   */
void    n_uni_elemt_add( double *Mat );                    /* Add the node's unique elements to Mat  */
double   n_heat( INDEX node, double input );                /* Return the heat generated/adsorbed by the node */
                            /* (+)->exothermic (-)->endothermic       */
                            /* input is the heat put into this node   */
double   n_Cp_out( INDEX node );                            /* Total heat capacity out of node        */
double   n_Cp_in( INDEX node );                            /* Total heat capacity into node          */
double   n_get_temp( INDEX node );                        /* Return the temperature of the node */
void    n_set_temp( INDEX node, double T );                /* Set the temperature of the node */
double   n_get_press( INDEX node );                        /* Return the pressure of the node    */
void    n_set_press( INDEX node, double P );                /* Set the pressure of the node    */
int     n_solve_N( INDEX node, enum Solver model );        /* The function to solve the node problem */
void    n_set_pipe_Out_temp( INDEX node );                /* Set the temperature of the pipes going */
                            /* Out of the node                        */
char   *n_name(INDEX node );                            /* Return a pointeer to the node name     */
double   n_get_heat( INDEX node );                        /* Get the heat input from the node */
void    n_set_heat( INDEX node, double value );            /* Set the heat input for the node */
void    n_set_split( INDEX node, int out_pipe, double value );/* set the split ratio */
double   n_get_split( INDEX node, int out_pipe);             /* get the split ratio  */
double   n_get_mass( INDEX node, enum Direction io );



int     alloc_memory( void );
void    free_memory( void );
int     alloc_vcs_memory( void );
void    free_vcs_memory( void );
int calc_slag_pipe_simple_compositions( enum Elem first_comp, enum Elem second_comp, double *num_moles );
int initialise_pipe_simple_compositions( INDEX pipe );

#ifdef __cplusplus
}
#endif


struct control {
            /* Lets define the sizes of the arrays so that we can detect changes*/
            /* in the VCSTHERM.DAT file as this would be the first structure saved */
            INDEX   max_mole,
                    max_temp,
                    max_press,
                    max_nd_utl,
                    max_comp,
                    max_stream,
                    max_pipe,
                    max_node,
                    max_opcode;



            /* Record of the number of each class defined */
            INDEX   tot_mole,
                    tot_temp,
                    tot_press,
                    tot_nde_utl,
                    tot_comp,
                    tot_strms,
                    tot_pipes,
                    tot_nodes,
                    tot_opcodes;

};


#ifndef _OPCODE
#define _OPCODE
struct OPCODE
{
 int mnem;
 union
 {
  unsigned int entry;        /* for the GET mnemonic - array oreinted */
  void *index;                /* for the GET mnemonic - pionter oreinted */
  double op_value;
 }x;
};
#endif

/* Global data vars as defined in data.c */

extern    double        *s_uni_mat;                        /* in data.c            */
extern    double        *p_uni_mat;                        /* in data.c            */
extern    double        *n_uni_mat;                        /* in data.c            */
extern    double        *mole_array;                    /* in data.c            */
extern    double        *temp_array;                    /* in data.c            */
extern    double        *press_array;                    /* in data.c            */
extern    double        *nde_utl_array;                    /* in data.c            */
extern    struct        Compound    *comp_array;        /* in data.c            */
extern    struct        Stream        *stream_array;        /* in data.c            */
extern    struct        Pipe        *pipe_array;        /* in data.c            */
extern    struct        Node        *node_array;        /* in data.c            */
extern    struct        OPCODE        *opcode_array;      /* in data.c            */
extern    struct        control    cntrl;                    /* in data.c            */
extern    char        error[];                        /* in data.c            */
extern    int (*solver_def[])(enum Object, INDEX);/* in data.c    */
extern    char        Solver_txt[][10];                /* in data.c            */
extern    char        Pipe_Solv_txt[][10];            /* in data.c            */
extern    char        Phase_txt[][10];                /* in data.c            */
extern    char        Elem_txt[][3];                    /* in data.c            */
extern    double       Elem_mass[];                    /* in data.c            */
extern    char        Object_txt[][15];                /* in data.c            */


#endif
