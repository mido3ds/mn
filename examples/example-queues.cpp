#include <mn/IO.h>
#include <mn/Result.h>
#include <mn/Str.h>
#include <mn/Buf.h>
#include <mn/Thread.h>
#include <mn/Json.h>
#include <mn/Log.h>
#include <mn/Defer.h>
#include <mn/Fabric.h>

// here are some frene variables to play with and see the result of the simulation
constexpr size_t SLEEP_WEIGHT = 100;
constexpr size_t LIST_LIMIT = 10;

// this is a simple tutorial on the basic idea of Communicating Sequential Processes (CSP) and how it applies to mn
// this is also concerned with the implementation of a simple state machine like to accelerate the work of some tasks
// although we have mn::Result for cloud functions but this tutorial doesn't care about error handling/reporting

// our imaginary program is simple we have some cloud server where we want to list resources and based on some random criteria we'll need to
// download the resource then try to parse it and if it succeed we'll want to construct an entity for it
// one of the defining parts of this problem is that in handling the download criteria we need to access some of the existing state which
// might be accessed and potentially changed from the main thread

// let's start with the basic listing function, here are our types for the function
using Entry = mn::Str;
using Entry_List = mn::Buf<Entry>;

// and here's the listing function itself, it simulates a listing query to the cloud
inline static mn::Result<Entry_List>
cloud_list()
{
	mn::log_debug("listing start");
	mn_defer { mn::log_debug("listing end"); };
	// sleep for some time to simulate the request to the cloud
	mn::thread_sleep(1 * SLEEP_WEIGHT);
	auto id_generator = rand();
	Entry_List res{};
	for (size_t i = 0; i < LIST_LIMIT; ++i)
	{
		mn::buf_push(res, mn::strf("program{}", id_generator + i));
	}
	return res;
}

// then the download and creation of the entity part
struct Program
{
	mn::Str key;
};

// here's the download function itself, it takes an entry and tries to download and construct the program
inline static mn::Result<Program>
cloud_download(const Entry& entry)
{
	mn::log_debug("download '{}' start", entry);
	mn_defer { mn::log_debug("download '{}' end", entry); };
	mn::thread_sleep(10 * SLEEP_WEIGHT);
	return Program{clone(entry)};
}


// now we can us the above primitives to simulate the sequential use cases
// the problem with the sequential case is that you need to perform the checking before the download
// and you can't do that from the worker thread, you have to get back to the main thread or use mutexes
// both options are not that good
inline static void
sequential()
{
	// first we list the cloud files
	auto [entries_, list_err] = cloud_list();
	if (list_err)
	{
		mn::log_critical("listing error, {}", list_err);
	}
	auto entries = entries_;
	mn_defer{ destruct(entries); };

	// then we check for the random criteria
	for (auto& entry: entries)
	{
		// if (rand() % 2 == 0)
		{
			auto [program_, download_err] = cloud_download(entry);
			if (download_err)
			{
				mn::log_critical("download error, {}", download_err);
			}
			auto program = program_;
			mn::log_info("program '{}' downloaded", program.key);
			mn::str_free(program.key);
		}
	}
}

// here we can see how we structure the problem into CSP-friendly way

// to facilitate work from multiple workers we'll bring the notion of the application to the forefront
struct App
{
	mn::Fabric fabric;
	std::atomic<bool> app_shutdown;
	std::atomic<bool> close;
	mn::Chan<Entry> program_queue;
	mn::Chan<Entry> download_queue;
};

// we'll need to convert the list to the generator pattern
inline static mn::Chan<Entry>
app_generator_cloud_list(App* app)
{
	// the generator pattern simply takes a function and returns a channel to its output
	auto output = mn::chan_new<Entry>();
	mn::go(app->fabric, [app, output = mn::chan_new(output)] () mutable {
		mn_defer
		{
			mn::chan_close(output);
			mn::chan_free(output);
		};

		while (app->close == false)
		{
			auto [list_, error] = cloud_list();
			if (error)
			{
				mn::log_critical("failed to list cloud programs");
			}
			auto list = list_;
			mn_defer{ destruct(list); };

			for (auto& entry: list)
				mn::chan_send(output, mn::xchg(entry, {}));

			// here we shutdown the application itself for the simulation sake but in real world scenarios you can sleep then recheck the cloud if you wish
			app->app_shutdown = true;
			// mn::thread_sleep(10000);
			break;
		}
	});
	return output;
}

// the criteria step can be done simply by polling the generator output in the main thread if you want
inline static void
app_process_listed_applications(App* app)
{
	for (size_t i = 0; i < 16; ++i)
	{
		if (auto [entry, ok] = mn::chan_recv_try(app->program_queue); ok)
		{
			// if (rand() % 2 == 0)
				mn::chan_send(app->download_queue, entry);
		}
	}
}

// then the download and application creation part can be done like this
inline static void
app_launch_download_worker(App* app)
{
	mn::go(app->fabric, [input = mn::chan_new(app->download_queue)]{
		mn_defer{ mn::chan_free(input); };

		for (auto entry: input)
		{
			mn_defer{destruct(entry);};
			auto [program, download_err] = cloud_download(entry);
			if (download_err)
			{
				mn::log_critical("download error, {}", download_err);
			}
			mn::log_info("program '{}' downloaded", program.key);
			mn::str_free(program.key);
		}
	});
}

inline static void
csp()
{
	// create the program at the start
	App app{};
	app.fabric = mn::fabric_new({});
	app.program_queue = app_generator_cloud_list(&app);
	app.download_queue = mn::chan_new<Entry>();

	for (size_t i = 0; i < 1; ++i)
	{
		app_launch_download_worker(&app);
	}

	// tick the application here
	while (app.app_shutdown == false)
	{
		app_process_listed_applications(&app);
		mn::thread_sleep(10);
	}

	// free the program at the end
	app.close = true;
	mn::chan_free(app.program_queue);

	mn::chan_close(app.download_queue);
	mn::chan_free(app.download_queue);
	mn::fabric_free(app.fabric);
}

int main()
{
	// srand(time(0));
	// sequential();
	csp();
	// mn::print("Hello, World!\n");
	return 0;
}
