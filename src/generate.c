#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <argp.h>
#include <string.h>
#include <time.h>

#define BUFFER_SIZE 4096

static char args_doc[] = "-l NUM";
static struct argp_option options[] = {
    {"path",    'p', "FILE",                  0, "output file"},
    {"length",  'l', "NUM",                   0, "lenght of array to generate"},
    {"max",     'M', "NUM",                   0, ""},
    {"min",     'm', "NUM",                   0, ""},
    {"float",   'f',     0, OPTION_ARG_OPTIONAL, ""},
    {"verbose", 'v',     0, OPTION_ARG_OPTIONAL, ""},
    { 0 }
};

struct arguments {
    char *filename;
    int length;
    int min;
    int max;
    bool isFloat;
    bool isVerbose;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;
    switch (key) {
        case 'p':
            arguments->filename = arg;
            break;
        case 'l':
            arguments->length = atoi(arg);
            break;
        case 'M':
            arguments->max = atoi(arg);
            break;
        case 'm':
            arguments->min = atoi(arg);
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
    
    srand(time(NULL));
    int min = arguments.min;
    int max = arguments.max;
    
    file = fopen(arguments.filename, "wb");
    if(file == NULL) {
        perror(arguments.filename);
        return EXIT_FAILURE;
    }

    if(arguments.isFloat) {
        char maxtoken[80];

        sprintf(maxtoken, "%.6lf ", (float)max);
        int token_sz = strlen(maxtoken);
        int ntok = (BUFFER_SIZE / token_sz) % (arguments.length + 1);  
        /* optimal number of tokens per write buffer
           where lenght is total number of tokens    */

        char *buffer = (char *) calloc(ntok * token_sz + 1,  sizeof(char));
        char token[token_sz];
        int totallen = 0;
        int i = 0; /* token number indicator */
        while(i < arguments.length) {
            int j = 0; /* token number per buffer indicator; total = ntok */
            while (j < ntok && i < arguments.length) {
                sprintf(token, "%.6lf ", ((float)rand()/(float)(RAND_MAX)) * max);
                totallen += (strlen(token));
                strcat(buffer, token); 
                i++;
                j++;
            }
            /* buffer is full and ready to write */
            fwrite(buffer, sizeof(char), totallen * sizeof(char), file);
            totallen = 0;
            memset(buffer, 0, (ntok * token_sz + 1) * sizeof(char));
            j = 0;
        }

        free(buffer);
    } else {
        char maxtoken[80];
        sprintf(maxtoken, "%d ", max);
        int token_sz = strlen(maxtoken);

        int ntok = (BUFFER_SIZE / token_sz) % (arguments.length + 1);

        char *buffer = (char *) calloc(ntok * token_sz + 1,  sizeof(char));       
        char token[token_sz];
        int totallen = 0;
        int i = 0; /* token number indicator */
        while(i < arguments.length) {
            int j = 0; /* token number per buffer indicator; total = ntok */
            while (j < ntok && i < arguments.length) {
                sprintf(token, "%d ", min + rand() % (max - min));
                totallen += strlen(token);
                strcat(buffer, token); 
                i++;
                j++;
            }
            /* buffer is full and ready to write */
            fwrite(buffer, sizeof(char), totallen * sizeof(char), file);
            totallen = 0;
            memset(buffer, 0, (ntok * token_sz + 1) * sizeof(char));
            j = 0;
        }

        free(buffer);
    }

    fputs("\0" , file);
    fclose(file);
    return EXIT_SUCCESS;
}
