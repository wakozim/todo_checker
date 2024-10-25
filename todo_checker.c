#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <dirent.h>

#define ASSERT assert
#define REALLOC realloc
#define FREE free

#define DA_INIT_CAP 256
// Append an item to a dynamic array
#define da_append(da, item)                                                          \
    do {                                                                             \
        if ((da)->count >= (da)->capacity) {                                         \
            (da)->capacity = (da)->capacity == 0 ? DA_INIT_CAP : (da)->capacity*2;   \
            (da)->items = REALLOC((da)->items, (da)->capacity*sizeof(*(da)->items)); \
            ASSERT((da)->items != NULL && "Buy more RAM lol");                       \
        }                                                                            \
                                                                                     \
        (da)->items[(da)->count++] = (item);                                         \
    } while (0)


typedef struct Todo {
    char *line;
    int priority;
} Todo;

typedef struct Todos {
    Todo *items;
    int count;
    int capacity;
} Todos;


void sort_todos(Todos *todos)
{
    for (int i = 0; i < todos->count; i++) {
        for (int j = 0; j < todos->count - 1; j++) {
            if (todos->items[j].priority < todos->items[j+1].priority) {
                Todo todo = todos->items[j];
                todos->items[j] = todos->items[j+1];
                todos->items[j+1] = todo;
            }
        }
    }
}


int print_file_todos(char filepath[], size_t *todos_count)
{
    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        fprintf(stderr, "[ERROR] Can't open %s: %s\n", filepath, strerror(errno));
        return 1;
    }

    Todos todos = {0};

    char *line = NULL;
    size_t line_size = 0;
    ssize_t read = 0;
    int lines = 0;
    while ((read = getline(&line, &line_size, file)) != -1) {
        lines++;
        if (strlen(line) < 5) continue;
        char *todo_loc = strstr(line, "TODO");
        if (todo_loc != NULL) {
            int todo_text_len = strlen(line) + strlen(filepath) + 1024;
            char *todo_text = calloc(todo_text_len, sizeof(char));
            sprintf(todo_text, "   %s:%d:%ld: %s", filepath, lines, todo_loc - line, line);

            // Calculate the priority based on the number of O
            int priority = 0;
            todo_loc += 3; // Skip TOD
            while (*todo_loc == 'O') {
                priority++;
                todo_loc++;
            }

            Todo todo = { .line = todo_text, .priority = priority };
            da_append(&todos, todo);
        }
    }
    free(line);

    if (todos.count > 0) {
        printf("TODO in %s:\n", filepath);
        sort_todos(&todos);
        for (int i = 0; i < todos.count; i++) {
            printf("%s", todos.items[i].line);
            if (todos_count != NULL) *todos_count += 1;
            free(todos.items[i].line);
        }
        printf("%d TODO was found in %s\n", todos.count, filepath);
        free(todos.items);
    } else {
        printf("No TODO was found in %s\n", filepath);
    }

    fclose(file);
    return 0;
}


int main(int argc, char **argv)
{
    if (argc == 1) {
        fprintf(stderr, "[ERROR] No input file was provided!\n");
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "   %s [FILE|DIR]...\n", argv[0]);
        return 1;
    }

    for (int j = 1; j < argc; j++) {
        DIR *d = opendir(argv[j]);
        if (d) {
            size_t todos_count = 0;
            struct dirent *dir;
            while ((dir = readdir(d)) != NULL) {
                if (dir->d_type != DT_REG) continue;
                char filepath[1024] = {0};
                const char *format = argv[j][strlen(argv[j]) - 1] == '/' ? "%s%s" : "%s/%s" ;
                sprintf(filepath, format, argv[j], dir->d_name);
                print_file_todos(filepath, &todos_count);
            }
            printf("[INFO] %ld TODO found in %s\n", todos_count, argv[j]);
            closedir(d);
        }
        else {
            print_file_todos(argv[j], NULL);
        }

        if (j != argc - 1) {
            printf("\n\n");
        }
    }
    return 0;
}
