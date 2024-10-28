#pragma once

#include <stdbool.h>

#include "datetime.h"
#include "lib/collections/string.h"
#include "task.h"

#define CATASTROPHIC_ERROR                                              \
	"Oops, we're having some technical difficulties. Please try again " \
	"later.\n"

bool prompt_string(string_t* output);

bool prompt_uint(unsigned* output);

bool prompt_double(double* output);

bool prompt_datetime(parsed_dt_t* output);

bool prompt_address(address_t* output);

bool prompt_mail_id(string_t* output);

bool prompt_mail(mail_t* output);

bool prompt_command(char** line);
