#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void lsh_loop(void);
char *lsh_read_line(void);
char **lsh_splite_line(char *line);
int lsh_launch(char **args);
int lsh_execute(char **args);
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
int lsh_num_builtins(void);

char *builtin_str[]= {
    "cd",
    "help",
    "exit"
};

int (*builtion_func[])(char **)= {
    &lsh_cd,
    &lsh_help,
    &lsh_exit
};


int main(int argc,char **argv) {
    lsh_loop();
    return EXIT_SUCCESS;
}


void lsh_loop() {
    char *line;
    char **args;
    int status;

    do {
        printf(">");
        line = lsh_read_line();
        args = lsh_splite_line(line);
        status = lsh_execute(args);
        free(line);
        free(args);
    } while(status);
}


/* #define LSH_RL_BUFSIZE 1024 */
/* char *lsh_read_line(void) { */
/*     int bufsize = LSH_RL_BUFSIZE; */
/*     int position = 0; */
/*     char *buffer = malloc(sizeof(char)*bufsize); */
/*     int c; */
/*     if(!buffer) { */
/*         perror("lsh: allocation error\n"); */
/*         exit(EXIT_FAILURE); */
/*     } */
/*     while(1) { */
/*         c = getchar(); */
/*         if(c==EOF || c=='\n') { */
/*             buffer[position] = '\0'; */
/*             return buffer; */
/*         } else { */
/*             buffer[position]=c; */
/*         } */
/*         position++; */
/*         if(position >= bufsize) { */
/*             bufsize += LSH_RL_BUFSIZE; */
/*             buffer = realloc(buffer,bufsize); */
/*             if(!buffer) { */
/*                 perror("lsh: allocation error\n"); */
/*                 exit(EXIT_FAILURE); */
/*             } */
/*         } */
/*     } */
/* } */

char *lsh_read_line(void) {
    char *line = NULL;
    ssize_t bufsize = 0;
    if(getline(&line,&bufsize,stdin)==-1) {
        if(feof(stdin)) {
            exit(EXIT_SUCCESS);
        } else {
            perror("lsh: getline\n");
            exit(EXIT_FAILURE);
        }
    }
    return line;
}


#define LSK_TOK_BUFSIZE 64
#define LSK_TOK_DELIM " \t\r\n\a"
char **lsh_splite_line(char *line) {
    int bufsize = LSK_TOK_BUFSIZE;
    int position =0;
    char **tokens = malloc(bufsize*sizeof(char*));
    char *token;
    if(!tokens) {
        perror("lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }
    token = strtok(line, LSK_TOK_DELIM);
    while(token!=NULL) {
        tokens[position]=token;
        position++;

        if(position>bufsize) {
            bufsize += LSK_TOK_BUFSIZE;
            tokens =realloc(tokens, sizeof(bufsize*sizeof(char*)));
            if(!tokens) {
                perror("Lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, LSK_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}


int lsh_launch(char **args) {
    pid_t pid,wpid;
    int status;
    pid=fork();
    if(pid==0) {
        if(execvp(args[0],args)==-1) {
            perror("lsh");
        }
    } else if(pid<0) {
        perror("lsh: fork error\n");
    } else {
        do {
            wpid = waitpid(pid,&status,WUNTRACED);
        } while(!WIFEXITED(status)&&!WIFSIGNALED(status));
    }
    return 1;
}


int lsh_execute(char **args) {
    if(args[0]==NULL) {
        return 1;
    }
    for (int i=0;i<lsh_num_builtins();i++){
        if(strcmp(args[0],builtin_str[i])==0) {
          return (*builtion_func[i])(args);
        }
    }
    return lsh_launch(args);
}


int lsh_cd(char **args) {
    if(args[1]==NULL) {
        fprintf(stderr,"lsh: expected argument to \"cd\"\n");
    } else {
        if(chdir(args[1])!=0) {
            perror("lsh");
        }
    }
    return 1;
}

int lsh_exit(char **args) {
    return 0;
}

int lsh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

int lsh_help(char **args) {
    printf("This is a simple shell");
    printf("This following are built in:");
    for(int i=0; i<lsh_num_builtins(); i++) {
        printf("%s\n",builtin_str[i]);
    }
    printf("Use the man command for information on other programs.\n");
    return 1;
}




