#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TML_MAX_LENGTH 10000 

typedef struct {
    char* name;
    char* description;
    char** param_names;
    char** param_descriptions;
    int num_params;
} TMLData;

void load_from_file(TMLData* tml, const char* file_path) {
    FILE* file = fopen(file_path, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(file_size + 1);
    if (!buffer) {
        fclose(file);
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    if (fread(buffer, 1, file_size, file) != file_size) {
        fclose(file);
        free(buffer);
        perror("Error reading file");
        exit(EXIT_FAILURE);
    }
    fclose(file);

    buffer[file_size] = '\0';
    tml->name = NULL;
    tml->description = NULL;
    tml->num_params = 0;

    char* start_tag = NULL;
    char* end_tag = NULL;

    start_tag = strstr(buffer, "<name>");
    end_tag = strstr(buffer, "</name>");
    if (start_tag && end_tag && end_tag > start_tag) {
        tml->name = (char*)malloc(end_tag - start_tag);
        if (!tml->name) {
            free(buffer);
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }
        strncpy(tml->name, start_tag + 6, end_tag - start_tag - 6);
        tml->name[end_tag - start_tag - 6] = '\0';
    }

    start_tag = strstr(buffer, "<description>");
    end_tag = strstr(buffer, "</description>");
    if (start_tag && end_tag && end_tag > start_tag) {
        tml->description = (char*)malloc(end_tag - start_tag);
        if (!tml->description) {
            free(tml->name);
            free(buffer);
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }
        strncpy(tml->description, start_tag + 13, end_tag - start_tag - 13);
        tml->description[end_tag - start_tag - 13] = '\0';
    }

    start_tag = strstr(buffer, "<params>");
    end_tag = strstr(buffer, "</params>");
    if (start_tag && end_tag && end_tag > start_tag) {
        char* params_text = (char*)malloc(end_tag - start_tag);
        if (!params_text) {
            free(tml->name);
            free(tml->description);
            free(buffer);
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }
        strncpy(params_text, start_tag + 8, end_tag - start_tag - 8);
        params_text[end_tag - start_tag - 8] = '\0';

        char* token = strtok(params_text, "\n");
        while (token) {
            tml->num_params++;
            token = strtok(NULL, "\n");
        }

        tml->param_names = (char**)malloc(tml->num_params * sizeof(char*));
        tml->param_descriptions = (char**)malloc(tml->num_params * sizeof(char*));
        if (!tml->param_names || !tml->param_descriptions) {
            free(tml->name);
            free(tml->description);
            free(buffer);
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }

        token = strtok(params_text, "\n");
        int i = 0;
        while (token) {
            char* colon = strchr(token, ':');
            if (colon) {
                *colon = '\0';
                tml->param_names[i] = (char*)malloc(strlen(token) + 1);
                tml->param_descriptions[i] = (char*)malloc(strlen(colon + 1) + 1);
                if (!tml->param_names[i] || !tml->param_descriptions[i]) {
                    free(tml->name);
                    free(tml->description);
                    for (int j = 0; j < i; j++) {
                        free(tml->param_names[j]);
                        free(tml->param_descriptions[j]);
                    }
                    free(tml->param_names);
                    free(tml->param_descriptions);
                    free(buffer);
                    perror("Memory allocation failed");
                    exit(EXIT_FAILURE);
                }
                strcpy(tml->param_names[i], token);
                strcpy(tml->param_descriptions[i], colon + 1);
                i++;
            }
            token = strtok(NULL, "\n");
        }

        free(params_text);
    }

    free(buffer);
}

void parse(TMLData* tml) {
    printf("{\n");
    printf("  \"type\": \"function\",\n");
    printf("  \"function\": {\n");
    printf("    \"name\": \"%s\",\n", tml->name);
    printf("    \"description\": \"%s\",\n", tml->description);
    printf("    \"parameters\": {\n");
    printf("      \"type\": \"object\",\n");
    printf("      \"properties\": {\n");

    for (int i = 0; i < tml->num_params; i++) {
        printf("        \"%s\": {\n", tml->param_names[i]);
        printf("          \"type\": \"string\",\n");
        printf("          \"description\": \"%s\"\n", tml->param_descriptions[i]);
        printf("        }%s\n", i < tml->num_params - 1 ? "," : "");
    }

    printf("      },\n");
    printf("      \"required\": [");
    for (int i = 0; i < tml->num_params; i++) {
        printf("\"%s\"%s", tml->param_names[i], i < tml->num_params - 1 ? ", " : "");
    }
    printf("]\n");
    printf("    }\n");
    printf("  }\n");
    printf("}\n");
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <file_path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    TMLData tml;
    load_from_file(&tml, argv[1]);
    parse(&tml);

    free(tml.name);
    free(tml.description);
    for (int i = 0; i < tml.num_params; i++) {
        free(tml.param_names[i]);
        free(tml.param_descriptions[i]);
    }
    free(tml.param_names);
    free(tml.param_descriptions);

    return EXIT_SUCCESS;
}
