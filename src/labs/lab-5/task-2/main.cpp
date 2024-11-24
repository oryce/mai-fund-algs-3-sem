#include "task.h"

int main() {
	encoder encoder(
	    {std::byte{0x01}, std::byte{0x02}, std::byte{0x03}, std::byte{0x04}});

	encoder.encode("input.txt", "encoded.txt", false);

	encoder.encode("encoded.txt", "decoded.txt", false);
}