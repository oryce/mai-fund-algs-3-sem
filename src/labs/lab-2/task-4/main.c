#include "tasks.h"

static error_t test_convex(void) {
	error_t error;
	bool square_convex;
	bool right_triangle_convex;
	bool star_convex;
	bool l_shaped_convex;
	bool m_shaped_convex;
	bool pentagon_convex;
	bool self_intersecting_convex;

	error = task_convex(&square_convex, 1e-9, 4, (point_t){1, 1},
	                    (point_t){2, 1}, (point_t){2, 2}, (point_t){1, 2});
	if (error) return error;
	error = task_convex(&right_triangle_convex, 1e-9, 3, (point_t){1, 1},
	                    (point_t){2, 1}, (point_t){1, 2});
	if (error) return error;
	error =
	    task_convex(&star_convex, 1e-9, 5, (point_t){1, 1}, (point_t){1.4, 1.8},
	                (point_t){1.8, 1}, (point_t){0.8, 1.4}, (point_t){2, 1.4});
	if (error) return error;
	error =
	    task_convex(&l_shaped_convex, 1e-9, 6, (point_t){1.2, 1.6},
	                (point_t){1.2, 2.4}, (point_t){1.4, 2.4},
	                (point_t){1.4, 1.8}, (point_t){2, 1.8}, (point_t){2, 1.6});
	if (error) return error;
	error = task_convex(&m_shaped_convex, 1e-9, 6, (point_t){1.2, 1.8},
	                    (point_t){1.2, 2.6}, (point_t){1.531, 2.145},
	                    (point_t){1.975, 2.543}, (point_t){1.886, 2.113},
	                    (point_t){2, 1.8});
	if (error) return error;
	error = task_convex(&pentagon_convex, 1e-9, 5, (point_t){1.294, 1.736},
	                    (point_t){1.007, 2.120}, (point_t){1.470, 2.439},
	                    (point_t){1.936, 2.192}, (point_t){1.836, 1.747});
	if (error) return error;
	error =
	    task_convex(&self_intersecting_convex, 1e-9, 5, (point_t){0.910, 1.804},
	                (point_t){0.835, 2.497}, (point_t){1.505, 2.525},
	                (point_t){0.548, 2.206}, (point_t){1.577, 1.944});
	if (error) return error;

	printf("Square convex: %d\n", square_convex);
	printf("Right triangle convex: %d\n", right_triangle_convex);
	printf("Star convex: %d\n", star_convex);
	printf("L-shaped figure convex: %d\n", l_shaped_convex);
	printf("M-shaped figure convex: %d\n", m_shaped_convex);
	printf("Pentagon convex: %d\n", pentagon_convex);
	printf("Self-intersecting figure convex: %d\n", self_intersecting_convex);

	return 0;
}

static error_t test_polynomials(void) {
	error_t error;
	double polynomial5;
	double polynomial2;

	error = task_polynomial(&polynomial5, 2.5, 5, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0);
	if (error) return error;
	error = task_polynomial(&polynomial2, 2.5, 1, 2.0, 7.5);
	if (error) return error;

	printf("x^5+2x^4+3x^3+4x^2+5x+6 = %lf where x = %lf\n", polynomial5, 2.5);
	printf("2x+7.5 = %lf where x = %lf\n", polynomial2, 2.5);

	return 0;
}

static error_t test_kaprekars(void) {
	error_t error;
	vector_ptr_t kaprekars;

	const int nBase10 = 51;
	error = task_kaprekar(
	    &kaprekars, 10, nBase10, "1", "9", "45", "55", "99", "297", "703",
	    "999", "2223", "2728", "4879", "4950", "5050", "7272", "7777", "9999",
	    "17344", "22222", "77778", "82656", "95121", "99999", "142857",
	    "148149", "181819", "187110", "208495", "318682", "329967", "351352",
	    "356643", "390313", "461539", "466830", "499500", "500500", "533170",
	    "538461", "609687", "643357", "648648", "670033", "681318", "791505",
	    "812890", "818181", "851851", "857143", "961038", "994708", "999999");

	if (error) {
		vector_ptr_destroy(&kaprekars);
		return error;
	}
	if (vector_ptr_size(&kaprekars) != nBase10) {
		vector_ptr_destroy(&kaprekars);
		return ERR_CHECK;
	}

	vector_ptr_destroy(&kaprekars);

	const int nBase16 = 72;
	error = task_kaprekar(
	    &kaprekars, 16, nBase16, "1", "6", "A", "F", "33", "55", "5B", "78",
	    "88", "AB", "CD", "FF", "15F", "334", "38E", "492", "4ED", "7E0", "820",
	    "B13", "B6E", "C72", "CCC", "EA1", "FA5", "FFF", "191A", "2A2B", "3C3C",
	    "4444", "5556", "6667", "7F80", "8080", "9999", "AAAA", "BBBC", "C3C4",
	    "D5D5", "E6E6", "FFFF", "1745E", "20EC2", "2ACAB", "2D02E", "30684",
	    "3831F", "3E0F8", "42108", "47AE1", "55555", "62FCA", "689A3", "7278C",
	    "76417", "7A427", "7FE00", "80200", "85BD9", "89AE5", "89BE9", "8D874",
	    "9765D", "9D036", "AAAAB", "AF0B0", "B851F", "BDEF8", "C1F08", "C7CE1",
	    "CF97C", "D5355");

	if (error) {
		vector_ptr_destroy(&kaprekars);
		return error;
	}
	if (vector_ptr_size(&kaprekars) != nBase16) {
		vector_ptr_destroy(&kaprekars);
		return ERR_CHECK;
	}

	vector_ptr_destroy(&kaprekars);
	return 0;
}

static error_t main_(void) {
	error_t error;

	error = test_convex();
	if (error) return error;
	error = test_polynomials();
	if (error) return error;
	error = test_kaprekars();
	if (error) return error;

	return 0;
}

int main(void) {
	error_t error = main_();
	if (error) {
		error_print(error);
		return error;
	}
}
