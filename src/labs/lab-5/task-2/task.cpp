#include "task.h"

#include <fstream>

void encoder::initCipher() {
	S.resize(256);

	for (size_t i = 0; i != 256; ++i) {
		S[i] = static_cast<int>(i);
	}

	int j = 0;

	for (size_t i = 0; i != 256; ++i) {
		j = (j + S[i] + static_cast<int>(key[i % key.size()])) % 256;
		std::swap(S[i], S[j]);
	}
}

void encoder::encodeBytes(std::vector<char>& bytes) {
	size_t i = 0, j = 0;

	for (char& byte : bytes) {
		i = (i + 1) % 256;
		j = (j + S[i]) % 256;
		std::swap(S[i], S[j]);

		int m = S[(S[i] + S[j]) % 256];
		byte = static_cast<char>(byte ^ m);
	}
}

void encoder::encode(const std::string& inputPath,
                     const std::string& outputPath, bool encrypt) {
	this->initCipher();

	std::ifstream input(inputPath, std::ifstream::binary);
	std::ofstream output(outputPath, std::ofstream::binary);

	if (!input.is_open()) {
		throw std::runtime_error("can't open input file for reading");
	}
	if (!output.is_open()) {
		throw std::runtime_error("can't open output file for writing");
	}

	std::vector<char> buffer(4096);

	while (input.read(buffer.data(), buffer.size())) {
		this->encodeBytes(buffer);
		output.write(buffer.data(), input.gcount());
	}

	if (input.gcount() > 0) {
		this->encodeBytes(buffer);
		output.write(buffer.data(), input.gcount());
	}
}

void encoder::setKey(const std::vector<std::byte>& key_) { key = key_; }
