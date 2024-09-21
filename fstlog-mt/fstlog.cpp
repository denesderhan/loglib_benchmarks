/*
 * SPDX-FileCopyrightText: 2024 Dénes Derhán <denes.derhan@gmail.com>
 *
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */
#include <logger.hpp>
#include <logbench/logtest1.hpp>

#include <cstdio>

int main()
{
    try {
#ifdef FSTLOG_DEBUG
		logger::set_log_template_txt("{timestamp:.6%Y-%m-%d %H:%M:%S %z} {level} {thread} {logger} {function} {file}:{line} {message}");
#else
		//FIX THIS replace "severity" with "level"???
		logger::set_log_template_txt("{timestamp:.6%Y-%m-%d %H:%M:%S %z} {level} {file}:{line} {message}");
#endif
        logger::set_log_template_bin(fstlog::ut_cast(fstlog::log_metaargs::All));
        logbench::logtest1<logger> do_test;
        return 0;
    }
    catch (std::exception& ex) {
        std::printf("%s", ex.what());
    }
    return 1;
}
