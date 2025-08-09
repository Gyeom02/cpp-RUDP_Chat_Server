#pragma once
#include <iostream>
#include <string>
#include <random>
#include <ctime>

std::string generateRandomCode(const std::string& userId)
{
	const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	const int len = 8;
	std::string code;

	std::hash<std::string> hasher;
	size_t seed = hasher(userId + std::to_string(time(nullptr)));

	std::mt19937 rng((unsigned int)seed);
	std::uniform_int_distribution<> dist(0, 35);

	for (int i = 0; i < len; ++i)
	{
		code += charset[dist(rng)];
	}
	return code;

}