#pragma once

#include "lib/collections/string.h"
#include "lib/collections/vector.h"

#include "datetime.h"

typedef struct address {
	/** Город (непустая строка). */
	string_t city;
	/** Улица (непустая строка). */
	string_t street;
	/** Номер дома (натуральное число). */
	unsigned house;
	/** Корпус (строка). */
	string_t housing;
	/** Номер квартиры (натуральное число). */
	unsigned apartment;
	/** Индекс получателя (строка из шести символов цифр). */
	string_t zip_code;
} address_t;

address_t address_create_empty(void);

void address_destroy(address_t* address);

typedef struct mail {
	/** Адрес получателя. */
	address_t address;
	/** Вес посылки (неотрицательное вещественное число). */
	double weight;
	/** Почтовый идентификатор (строка из 14 символов цифр). */
	string_t id;
	/** Время создания. */
	parsed_dt_t creation_date;
	/** Время вручения. */
	parsed_dt_t delivery_date;
} mail_t;

mail_t mail_create_empty(void);

void mail_destroy(mail_t* mail);

DEFINE_VECTOR(vector_mail_t, mail_t, mail)

typedef struct post {
	address_t* address;
	vector_mail_t mail;
} post_t;

typedef enum remove_result {
	REMOVE_OK,
	REMOVE_NOT_FOUND,
	REMOVE_FAILED
} remove_result_t;

post_t post_create(address_t* address);

void post_destroy(post_t* post);

bool post_insert_mail(post_t* post, mail_t mail);

remove_result_t post_remove_mail(post_t* post, string_t* id);

bool post_expired_mail(post_t* post, vector_mail_t* out);

bool post_non_expired_mail(post_t* post, vector_mail_t* out);
