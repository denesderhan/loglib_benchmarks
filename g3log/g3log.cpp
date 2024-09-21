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
		logger::set_time_template("%Y-%m-%d %H:%M:%S.%f3 ");
		auto worker = g3::LogWorker::createLogWorker();
		logger::worker_ = worker.get();
		logbench::logtest1<logger> do_test;
		return 0;
	}
	catch (std::exception& ex) {
		std::printf("%s", ex.what());
	}
	return 1;
}
