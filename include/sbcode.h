#pragma once

enum SBC_Type {
	SBC_INT,
	SBC_STRING,
	SBC_LIST,
	SBC_DICT
};

typedef struct {
	enum SBC_Type type;

	void * data;
} SBC_Rep;
