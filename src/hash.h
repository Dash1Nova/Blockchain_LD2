#ifndef HASH_H
#define HASH_H

#include <string>
#include <vector>

std::string hash(const std::vector<uint8_t>& data);
std::vector<uint8_t> readFile(const std::string& filename);
std::vector<uint8_t> convertTextTB(const std::string& text);

#endif