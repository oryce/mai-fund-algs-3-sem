#include "task.h"

#include <stdio.h>
#include <time.h>

#include "lib/chars.h"
#include "lib/convert.h"

static int compare_mail(const void* p1, const void* p2) {
	if (!p1 || !p2 || p1 == p2) return 0;

	const mail_t* a = (const mail_t*)p1;
	const mail_t* b = (const mail_t*)p2;

	int order;

	order = string_compare(&a->address.zip_code, &b->address.zip_code);
	if (order) return order;
	order = string_compare(&a->id, &b->id);
	if (order) return order;

	return 0;
}

static int compare_mail_create_dt(const void* p1, const void* p2) {
	if (!p1 || !p2 || p1 == p2) return 0;

	const mail_t* a = (const mail_t*)p1;
	const mail_t* b = (const mail_t*)p2;

	return datetime_compare(&a->creation_date.datetime,
	                        &b->creation_date.datetime);
}

IMPL_VECTOR(vector_mail_t, mail_t, mail, {&compare_mail})

address_t address_create_empty(void) {
	return (address_t){.city = string_create_empty(),
	                   .street = string_create_empty(),
	                   .housing = string_create_empty(),
	                   .zip_code = string_create_empty()};
}

void address_destroy(address_t* address) {
	string_destroy(&address->city);
	string_destroy(&address->street);
	string_destroy(&address->housing);
	string_destroy(&address->zip_code);
}

mail_t mail_create_empty(void) {
	return (mail_t){.address = address_create_empty(),
	                .id = string_create_empty(),
	                .creation_date = {.source = string_create_empty()},
	                .delivery_date = {.source = string_create_empty()}};
}

void mail_destroy(mail_t* mail) {
	address_destroy(&mail->address);
	string_destroy(&mail->id);
	string_destroy(&mail->creation_date.source);
	string_destroy(&mail->delivery_date.source);
}

post_t post_create(address_t* address) {
	return (post_t){.address = address, .mail = vector_mail_create()};
}

void post_destroy(post_t* post) {
	address_destroy(post->address);

	for (size_t i = 0; i != vector_mail_size(&post->mail); ++i) {
		mail_t* mail = vector_mail_get(&post->mail, i);
		if (mail) mail_destroy(mail);
	}

	vector_mail_destroy(&post->mail);
}

bool post_insert_mail(post_t* post, mail_t mail) {
	vector_mail_t* v = &post->mail;

	// Find an insertion point for |mail|, such that the elements are in
	// ascending order.
	size_t idx = 0;

	while (idx < vector_mail_size(v) &&
	       compare_mail(&mail, vector_mail_get(v, idx)) != -1) {
		++idx;
	}

	return vector_mail_insert(v, idx, mail);
}

remove_result_t post_remove_mail(post_t* post, string_t* id) {
	size_t i;
	size_t len = vector_mail_size(&post->mail);

	for (i = 0; i != len; ++i) {
		mail_t* mail = vector_mail_get(&post->mail, i);
		if (string_compare(&mail->id, id) == 0) break;
	}

	string_destroy(id);
	if (i == len) return REMOVE_NOT_FOUND;

	mail_t removed;
	if (!vector_mail_remove(&post->mail, i, &removed)) {
		return REMOVE_FAILED;
	}

	mail_destroy(&removed);
	return REMOVE_OK;
}

static bool post_filter_mail(post_t* post, vector_mail_t* out, bool expired) {
	*out = vector_mail_create();
	datetime_t current = datetime_current();

	for (size_t i = 0; i != vector_mail_size(&post->mail); ++i) {
		mail_t* mail = vector_mail_get(&post->mail, i);
		int order = datetime_compare(&current, &mail->delivery_date.datetime);

		/* expired && current > delivered || !expired && current < delivered */
		if ((expired && order == 1) || (!expired && order == -1)) {
			if (!vector_mail_push_back(out, *mail)) return false;
		}
	}

	// Sort by creation datetime.
	qsort((mail_t*)vector_mail_to_array(out), vector_mail_size(out),
	      sizeof(mail_t), &compare_mail_create_dt);

	return true;
}

bool post_expired_mail(post_t* post, vector_mail_t* out) {
	return post_filter_mail(post, out, true);
}

bool post_non_expired_mail(post_t* post, vector_mail_t* out) {
	return post_filter_mail(post, out, false);
}
