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
		logger::set_log_template({
			"%(time) %(log_level) %(full_path):%(line_number) %(message)",
			"%Y-%m-%d %H:%M:%S,%Qus %z",
			quill::Timezone::LocalTime
			});
		logbench::logtest1<logger> do_test;
		return 0;
	}
	catch (std::exception& ex) {
		std::printf("%s", ex.what());
	}
	return 1;
}
