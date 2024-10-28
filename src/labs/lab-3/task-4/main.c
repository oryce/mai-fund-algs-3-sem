#include <stdio.h>

#include "prompt.h"
#include "task.h"

static bool cmd_insert_mail(post_t* post) {
	mail_t mail = mail_create_empty();

	if (!prompt_mail(&mail) || !post_insert_mail(post, mail)) {
		fprintf(stderr, CATASTROPHIC_ERROR);
		mail_destroy(&mail);

		return false;
	}

	printf("OK.\n");
	return true;
}

static bool cmd_remove_mail(post_t* post) {
	string_t id = string_create_empty();

	if (!prompt_mail_id(&id)) {
		fprintf(stderr, CATASTROPHIC_ERROR);
		string_destroy(&id);

		return false;
	}

	remove_result_t result = post_remove_mail(post, &id);
	string_destroy(&id);

	switch (result) {
		case REMOVE_OK:
			printf("OK.\n");
			return true;
		case REMOVE_NOT_FOUND:
			fprintf(stderr, "No mail with such ID.\n");
			return true;
		case REMOVE_FAILED:
			fprintf(stderr, CATASTROPHIC_ERROR);
			return false;
	}
}

static void print_mail(vector_mail_t* vec) {
	printf(
	    "ID             Weight            Creation time       Delivery time\n");

	for (size_t i = 0; i != vector_mail_size(vec); ++i) {
		mail_t* mail = vector_mail_get(vec, i);
		printf("%14s %17lf %s %s\n", string_to_c_str(&mail->id), mail->weight,
		       string_to_c_str(&mail->creation_date.source),
		       string_to_c_str(&mail->delivery_date.source));
	}
}

static bool cmd_filter_mail(post_t* post, bool expired) {
	vector_mail_t vec;

	bool result = expired ? post_expired_mail(post, &vec)
	                      : post_non_expired_mail(post, &vec);

	if (!result) {
		fprintf(stderr, CATASTROPHIC_ERROR);
		vector_mail_destroy(&vec);

		return false;
	}

	print_mail(&vec);
	vector_mail_destroy(&vec);

	return true;
}

static void command_loop(post_t* post) {
	char* line = NULL;

	while (prompt_command(&line)) {
		char* newline = strrchr(line, '\n');
		if (newline) *newline = '\0';

		switch (*line) {
			case 'h':
				printf(
				    "Available commands:\n"
				    "  - 'a': insert new mail into the post office\n"
				    "  - 'r': remove mail from the post office\n"
				    "  - 'p': prints all mail\n"
				    "  - 'e': print expired mail\n"
				    "  - 'n': print non-expired mail\n"
				    "  - 'h': print this message\n"
				    "  - 'q': quit the application\n");
				break;
			case 'a':
				if (!cmd_insert_mail(post)) {
					free(line);
					return;
				}
				break;
			case 'r':
				if (!cmd_remove_mail(post)) {
					free(line);
					return;
				}
				break;
			case 'p':
				print_mail(&post->mail);
				break;
			case 'e':
				if (!cmd_filter_mail(post, true)) {
					free(line);
					return;
				}
				break;
			case 'n':
				if (!cmd_filter_mail(post, false)) {
					free(line);
					return;
				}
				break;
			case 'q':
				free(line);
				return;
			case '!':
				// Comment in the input file.
				break;
			default:
				printf("Unknown command. Type 'h' for help.\n");
		}
	}

	free(line);
}

int main(void) {
	printf(
	    "Welcome to the Auswitz post office. Arbeit Macht Frei!\n"
	    "Enter the post office's address:\n");

	address_t address = address_create_empty();

	if (!prompt_address(&address)) {
		fprintf(stderr, CATASTROPHIC_ERROR);
		address_destroy(&address);

		return 1;
	}

	post_t post = post_create(&address);

	command_loop(&post);

	post_destroy(&post);
}
