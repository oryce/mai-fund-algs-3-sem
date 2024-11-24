#pragma once

#include <string>
#include <vector>

class encoder {
   private:
	std::vector<std::byte> key;

	/** RC4 S-Box */
	std::vector<int> S;

	void initCipher();
	void encodeBytes(std::vector<char>& bytes);

   public:
	encoder(const std::vector<std::byte>& key_) : key(key_) {}

	void encode(const std::string& inputPath, const std::string& outputPath,
	            bool encrypt);
	void setKey(const std::vector<std::byte>& key);
};
