#pragma once

extern int max_files;

typedef enum lang{
	CPP,
	C,
	JAVA,
	INVALID
} Language;

typedef struct cfg {
	Language language;
	char* extension;
	char** files;
	int file_count;
	char* name;
} Config;

void add_file(char* filename);
