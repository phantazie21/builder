#pragma once

extern int max_files;

typedef enum lang{
	PYTHON,
	CPP,
	C,
	JAVA,
	INVALID
} Language;

typedef struct cfg {
	Language language;
	char** files;
	int file_count;
} Config;

void add_file(char* filename);
