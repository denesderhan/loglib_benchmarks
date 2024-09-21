/*
 * SPDX-FileCopyrightText: 2024 Dénes Derhán <denes.derhan@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */
#include <logger.hpp>
#include <logbench/logtest1.hpp>

#include <cstdio>

int main()
{
	try {
		logbench::logtest1<logger> do_test;
		return 0;
	}
	catch (std::exception& ex) {
		std::printf("%s", ex.what());
	}
	return 1;
}
