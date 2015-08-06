#define BUFFER_SIZE 4096

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <argp.h>
#include <string.h>

static char args_doc[] = "-l NUM";
static struct argp_option options[] = {
    {"path",    'p', "FILE",                  0, "output file"},
    {"length",  'l', "NUM",                   0, "lenght of array to generate"},
    {"max",     'M', "NUM",                   0, ""},
    {"min",     'm', "NUM",                   0, ""},
    {"float",   'f',     0, OPTION_ARG_OPTIONAL, ""},
    {"verbose", 'v',     0, OPTION_ARG_OPTIONAL, ""
    { 0 }
};

struct arguments {
    char *path;
    int length;
    int min;
    int max;
    bool isFloat;
    bool isVerbose;
}

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;
    switch (key) {
        case 'p':
            arguments->path = arg;
            break;
        case 'l':
            arguments->length = arg;
            break;
        case 'M':
            arguments->max = arg;
            break;
        case 'm':
            arguments->min = arg;
            break;
        case 'f':
            arguments->isFloat = true;
            break;
        case 'v':
            arguments->isVerbose = true;
            break;
        case ARGP_KEY_ARG:
            break;
        case ARGP_KEY_INIT:
            arguments->isFloat = false;
            arguments->isVerbose = false;
            break;
        case ARGP_KEY_END:
            if(state->arg_num > 0)
                argp_usage(state);
            break;
        default: return ARGP_ERR_UNKNOWN;
    }
    return EXIT_SUCCESS;
}

static struct argp argp = {options, parse_opt, args_doc, 0, 0, 0, 0};

int main(int argc, char *argv[]) {
    struct arguments arguments;
    argp_parse(&argp, argc, argv, 0, 0, &arguments);
    
    FILE *file;

    int min = arguments.min;
    int max = arguments.max;
    
    file = fopen(filename, "wb");
    if(file == NULL) {
        perror(filename);
        return EXIT_FAILURE;
    }

    if(arguments.isFloat) {
        char maxtoken[80];
        float data[length];
 
        for(int i = 0; i < arguments.length; ++i) {
            int r = min + (float) rand() / (float) (RAND_MAX / (max - min));
            sprintf(out, "%lf ", r);
        }

        snprintf(maxtoken, "%lf ", (float)max);
        int token_sz = strlen(maxtoken);

        int ntok = BUFFER_SIZE / token_sz; /* optimal number of tokens per write buffer
                                       where lenght is total number of tokens */

        char *buffer = (char *) malloc( (ntok * token_sz + 1 ) * sizeof(char));

        int i = 0; /* token number indicator */
        while(i < lenght) {
            int j = 0; /* token number per buffer indicator; total = ntok */
            while(j < ntok) {
                snprintf(buffer, "%lf ", data[i]);
                i++;
                j++;
            }
            /* buffer is full and ready to write */

            j = 0;
        } 

        free(buffer); 
    } else {
        int data[length];
        for(int i = 0; i < arguments.length; ++i) {
            data[i] = min + rand() % (max - min);
        }
    }


}
