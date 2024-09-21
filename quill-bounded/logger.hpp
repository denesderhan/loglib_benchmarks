/*
 * SPDX-FileCopyrightText: 2024 Dénes Derhán <denes.derhan@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#define QUILL_USE_BOUNDED_BLOCKING_QUEUE
#define _CRT_SECURE_NO_WARNINGS
#include <quill/Logger.h>
#include <quill/Frontend.h>
#include <quill/Backend.h>
#include <quill/sinks/NullSink.h>
#include <quill/sinks/FileSink.h>
#include <quill/LogMacros.h>

#include <stdexcept>
#include <string_view>
#include <string>

#include <logbench/force_inline.hpp>
#include <logbench/test_in_param.hpp>
#include <logbench/test_out_param.hpp>
#include <type_traits>

template<typename T>
inline constexpr bool is_pow2(T num) {
	static_assert(std::is_unsigned_v<T>, "Only for unsigned!");
	return num && ((num & (num - 1)) == 0);
}

// Define custom Frontend Options
struct CustomFrontendOptions
{
	static constexpr quill::QueueType queue_type = quill::QueueType::BoundedBlocking;
	static constexpr uint32_t initial_queue_capacity = 131'072;
	static constexpr uint32_t blocking_queue_retry_interval_ns = 800;
	static constexpr bool huge_pages_enabled = false;
};

class logger {
public:
	logger(int id = 0) {}
	~logger() {}

	template<typename... Args>
	LOGBENCH_FORCEINLINE void log_test1(Args &&... args) {
		LOG_INFO(logger_instance_, "Thr: {} Log_n: {} Time: {} {} {}" , args...);
	}

	struct log_templ {
		std::string templ;
		std::string time_templ;
		quill::Timezone zone;
	};

	static void set_log_template(log_templ templ)  noexcept {
		log_templ_ = templ;
	}

	static void sys_init(
		logbench::test_in_param const& init_data,
		logbench::test_out_param& out_data)
	{
		out_data.lib_name = lib_name();
		out_data.lib_version = lib_version();
		out_data.formatter_type = init_data.formatter_type;
		out_data.sink_type = init_data.sink_type;
		out_data.garbage_lines = 0;
		
		CustomFrontendOptions cfgf;
		out_data.lib_buffer_size = cfgf.initial_queue_capacity / 2048;

		quill::BackendOptions  cfg;
		if (init_data.buffer_flush_time != -1) {
			out_data.buffer_flush_time = init_data.buffer_flush_time;
			if (init_data.buffer_flush_time == 0) {
				cfg.sleep_duration = (std::chrono::nanoseconds::max)();
			}
			else {
				cfg.sleep_duration = std::chrono::milliseconds(init_data.buffer_flush_time);
			}
		}
		else {
			size_t sleep = std::chrono::duration_cast<std::chrono::milliseconds>(cfg.sleep_duration).count();
			if (sleep == 0) sleep = 1;
			else if (sleep > (std::numeric_limits<int>::max)())
				sleep = (std::numeric_limits<int>::max)();
			out_data.buffer_flush_time = static_cast<int>(sleep);
		}

		if (init_data.sink_flush_time != -1) {
			throw std::runtime_error("Can sink flush time be set? To do impl!");
		}
				
		if (out_data.formatter_type != logbench::formatter_typ::txt) {
			throw std::runtime_error("Formatter type not supported!");
		}
		out_data.formater_impl = "fmtlib_{fmt}";


		quill::Backend::start(cfg);

		std::string log_file{ init_data.log_path };
		log_file += "/log.txt";
		if (out_data.sink_type == logbench::sink_typ::lib) {
			out_data.sink_type = logbench::sink_typ::sort;
		}
		auto sink = std::shared_ptr<quill::Sink>{};
		if (out_data.sink_type == logbench::sink_typ::null) {
			sink = quill::FrontendImpl<CustomFrontendOptions>::create_or_get_sink<quill::NullSink>("sink_id_1");
		}
		else if (out_data.sink_type == logbench::sink_typ::sort) {
			sink = quill::FrontendImpl<CustomFrontendOptions>::create_or_get_sink<quill::FileSink>(log_file);
		}
		else {
			throw std::runtime_error("Sink type not supported!");
		}

		logger_instance_ = quill::FrontendImpl<CustomFrontendOptions>::create_or_get_logger(
			"logr", std::move(sink), quill::PatternFormatterOptions(log_templ_.templ, log_templ_.time_templ, log_templ_.zone));
	}

	static void sys_stop() noexcept {
		logger_instance_->flush_log();
		quill::Backend::stop();
	}

	static constexpr std::string_view lib_name() noexcept {
		return "quill-bounded";
	}

	static std::string_view lib_version() {
		return quill_version_;
	}

	inline static quill::FrontendImpl<CustomFrontendOptions>::logger_t* logger_instance_;
	inline static log_templ log_templ_{"%(time) %(message)", "%Y-%m-%d %H:%M:%S,%Qms %z", quill::Timezone::LocalTime};
	inline static const std::string quill_version_{
		std::to_string(quill::VersionMajor)
		+ '.' + std::to_string(quill::VersionMinor)
		+ '.' + std::to_string(quill::VersionPatch)
	};
};
/*
Logging threads are busy waiting on backend thread without notifying it!

See: include\quill\Logger.h line:166
 else if constexpr (QUILL_QUEUE_TYPE == detail::QueueType::BoundedBlocking)
	{
	  while (write_buffer == nullptr)
	  {
		// not enough space to push to queue, keep trying
		write_buffer =
		  thread_context->spsc_queue<QUILL_QUEUE_TYPE>().prepare_write(static_cast<uint32_t>(total_size));
	  }
	}

The backend thread can only be woken up manually.
See: Quill.h line: 380
 * Wakes up the backend logging thread on demand.
 * The backend logging thread busy waits by design.
 * A use case for this is when you do want the backend logging thread to consume (not??) too much CPU
 * you can configure it to sleep for a long amount of time and wake it up on demand to log.
 * (e.g. cfg.backend_thread_sleep_duration = = std::chrono::hours {240};)
 * This is thread safe and can be called from any thread.
 *
void wake_up_logging_thread();

See: examples\example_wake_up_logging_thread.cpp line: 6
* This example shows how to wake up the backend logging thread if you configure it
 * with a high sleep duration.
 * @note: It is not recommended to configure a sleep duration higher than 1 us as in this example.
 * The only use case of this is when you do not want the backend logging thread to wake up
 * periodically and consume some CPU cycles.
*/
