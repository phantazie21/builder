#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "header.h"

int max_files = 10;
Config config = {0};
FILE* fptr = NULL;
char* line = NULL;

void set_color_red() {
	fprintf(stdout, "\033[0;31m");
}

void set_color_green() {
	fprintf(stdout, "\033[0;32m");
}

void reset_color() {
	fprintf(stdout, "\033[0m");
}

void add_file(char* filename) {
	config.files[config.file_count] = strdup(filename);
}

char* prepend_char(char *str, char c) {
	size_t len = strlen(str);
	char *new_str = malloc(len + 2);
	new_str[0] = c;
	memcpy(new_str + 1, str, len);
	new_str[len + 1] = '\0';
	return new_str;
}

const char* get_extension(const char *filename) {
	const char *dot = strrchr(filename, '.');
	if (!dot || dot == filename) return "";
	return dot;
}

void get_value(char* value) {
	if (config.extension) free(config.extension);
	static char* language_str[] = {"cpp", "c", "java"};
	for (int i = 0; i < INVALID; i++) {
		if (!strcmp(language_str[i], value)) {
			config.language = i;
			config.extension = prepend_char(language_str[i], '.');
			return;
		}
	}
	config.language = INVALID;
	config.extension = NULL;
}

void cleanup() {
	for (int i = 0; i < config.file_count; i++) {
		free(config.files[i]);
	}
	free(config.files);
	free(config.extension);
	free(config.name);
	free(line);
	if (fptr) fclose(fptr);
}

bool build() {
	char* files = strdup(config.files[0]);
	size_t files_size = strlen(files) + 1;
	size_t buffer_size = 0;
	size_t output_size = 0;
	char* buffer = NULL;
	for (int i = 1; i < config.file_count; i++) {
		size_t extra_len = strlen(config.files[i]) + 1;
		files_size += extra_len;
		files = realloc(files, files_size);
		strcat(files, " ");
		strcat(files, config.files[i]);
	}
	switch (config.language) {
		case C:
			buffer_size = files_size;
			output_size = strlen(config.name);
			buffer_size += output_size;
			buffer_size += 10;
			buffer = malloc(buffer_size);
			snprintf(buffer, buffer_size, "gcc -o %s %s\n", config.name, files);
			system(buffer);
			free(buffer);
			free(files);
			return true;
		case CPP:
			buffer_size = files_size;
			output_size = strlen(config.name);
			buffer_size += output_size;
			buffer_size += 10;
			buffer = malloc(buffer_size);
			snprintf(buffer, buffer_size, "g++ -o %s %s\n", config.name, files);
			system(buffer);
			free(buffer);
			free(files);
			return true;
		case JAVA:
			break;
		default:
			break;
	}
}

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Usage: builder <config_file>\nFor help, type: builder help\n");
		return 0;
	}
	if (!strcmp(argv[1], "help")) {
		printf("builder is a build system with easy-to-use config language and support for many languages.\nUsage: builder <config_file>.\nFor example file, type: builder example\n");
		return 0;
	}
	else if (!strcmp(argv[1], "example")) {
		fptr = fopen("config", "w");
		fprintf(fptr, "language: c\nname: example-project\nfiles: main.c");
		if (fptr) fclose(fptr);
		printf("builder: example file is created.\n");
		return 0;
	}
	fptr = fopen(argv[1], "r");
	if (!fptr) {
		printf("Couldn't open file %s.\n", argv[1]);
		return 1;
	}
	set_color_green();
	printf("builder: successfully opened %s.\n", argv[1]);
	reset_color();
	size_t len = 0;
	ssize_t nread;
	config.files = malloc(max_files * sizeof(char*));
	if (!config.files) {
		set_color_red();
		printf("builder: malloc failed...\n");
		reset_color();
		cleanup();
		return 1;
	}
	while((nread = getline(&line, &len, fptr)) != -1) {
		line[strcspn(line, "\n")] = 0;
		char* word = strtok(line, " ");
		if (!strcmp(word, "files:")) {
			printf("builder: parsing files...\n");
			word = strtok(NULL, " ");
			while (word && config.file_count < max_files) {
				//printf("%d: %s\n", config.file_count + 1, word);
				if (!strcmp(get_extension(word), config.extension)) {
					add_file(word);
					config.file_count++;
					word = strtok(NULL, " ");
				}
				else {
					set_color_red();
					printf("builder: %s is not a %s language file...\n", word, config.extension + 1);
					reset_color();
					cleanup();
					return 1;
				}
			}
		}
		else if (!strcmp(word, "language:")) {
			word = strtok(NULL, " ");
			get_value(word);
			if (config.language == INVALID) {
				set_color_red();
				printf("builder: invalid language set...\n");
				reset_color();
				cleanup();
				return 1;
			}
			set_color_green();
			printf("builder: language is set to %s (extension: %s).\n", word, config.extension);
			reset_color();
		}
		else if (!strcmp(word, "name:")) {
			word = strtok(NULL, " ");
			config.name = strdup(word);
			set_color_green();
			printf("builder: project name is '%s'\n", config.name);
			reset_color();
		}
	}
	set_color_green();
	printf("builder: files parsed.\nbuilder: starting the build...\n");
	reset_color();
	if (build()) {
		set_color_green();
		printf("builder: build completed.\n");
		reset_color();
	}
	else {
		set_color_red();
		printf("builder: an error occurred during build process...\n");
		reset_color();
	}
	cleanup();
	return 0;
}
