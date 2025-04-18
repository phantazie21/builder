#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

int max_files = 10;
Config config = {};

void set_color_red() {
	fprintf(stdout, "\033[0;31m");
}

void reset_color() {
	fprintf(stdout, "\033[0m");
}

void add_file(char* filename) {
	config.files[config.file_count] = strdup(filename);
	return;
}

Language get_value(char* value) {
	static char const* language_str[] = {"python", "cpp", "c", "java"};
	for (int i = 0; i < INVALID; i++) {
		if (!strcmp(language_str[i], value))
			return i;
	}
	return INVALID;
}

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Usage: builder <config_file>\nFor help, type: builder help\n");
		return 0;
	}
	FILE* fptr = fopen(argv[1], "r");
	if (!fptr) {
		printf("Couldn't open file %s.\n", argv[1]);
		return 1;
	}
	printf("Successfully opened %s.\n", argv[1]);
	size_t len = 0;
	char* line = NULL;
	ssize_t nread;
	if (nread = getline(&line, &len, fptr) == -1) {
		set_color_red();
		printf("builder: couldn't read the first line...\n");
		reset_color();
		free(line);
		fclose(fptr);
		return 1;
	}
	line[strcspn(line, "\n")] = 0;
	char* word = strtok(line, " ");
	if (!strcmp(word, "language:")) {
		word = strtok(NULL, " ");
		config.language = get_value(word);
		if (config.language == INVALID) {
			set_color_red();
			printf("builder: invalid language set...\n");
			reset_color();
			free(line);
			fclose(fptr);
			return 1;
		}
		printf("builder: language is set to %s.\n", word);
	}
	else {
		set_color_red();
		printf("builder: language not set at first line...\n");
		reset_color();
		return 1;
	}
	
	config.files = malloc(max_files * sizeof(char*));
	if (!config.files) {
		printf("malloc failed...\n");
		return 1;
	}
	printf("builder: parsing files...\n");
	while((nread = getline(&line, &len, fptr)) != -1) {
		line[strcspn(line, "\n")] = 0;
		char* word = strtok(line, " ");
		if (!strcmp(word, "files:")) {
			word = strtok(NULL, " ");
			while (word && config.file_count < max_files) {
				printf("%d: %s\n", config.file_count + 1, word);
				add_file(word);
				word = strtok(NULL, " ");
				config.file_count++;
			}
		}
	}
	printf("builder: files parsed.\nbuilder: starting the build...\n");
	for (int i = 0; i < config.file_count; i++) {
		free(config.files[i]);
	}
	free(config.files);
	free(line);
	fclose(fptr);
	return 0;
}
