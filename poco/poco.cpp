/*
 * SPDX-FileCopyrightText: 2024 Dénes Derhán <denes.derhan@gmail.com>
 *
 * SPDX-License-Identifier: BSL-1.0
 */
#include <logger.hpp>
#include <logbench/logtest1.hpp>

#include <cstdio>

int main()
{
	try {
		logger::set_log_template("%L%Y-%m-%d %H:%M:%S.%F %Z %p %U:%u %t");
		logbench::logtest1<logger> do_test;
		return 0;
	}
	catch (std::exception& ex) {
		std::printf("%s", ex.what());
	}
	return 1;
}
