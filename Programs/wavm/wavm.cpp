#include "wavm.h"
#include <stdlib.h>
#include <string.h>
#include "WAVM/Inline/Assert.h"
#include "WAVM/Inline/CLI.h"
#include "WAVM/Inline/Config.h"
#include "WAVM/Inline/Version.h"
#include "WAVM/Logging/Logging.h"

using namespace WAVM;

enum class Command
{
	invalid,

	assemble,
	disassemble,
	help,
	version,

#if WAVM_ENABLE_RUNTIME
	compile,
	run,
#endif
};

Command parseCommand(const char* string)
{
	if(!strcmp(string, "assemble")) { return Command::assemble; }
	else if(!strcmp(string, "disassemble"))
	{
		return Command::disassemble;
	}
	else if(!strcmp(string, "help"))
	{
		return Command::help;
	}
	else if(!strcmp(string, "version"))
	{
		return Command::version;
	}
#if WAVM_ENABLE_RUNTIME
	else if(!strcmp(string, "compile"))
	{
		return Command::compile;
	}
	else if(!strcmp(string, "run"))
	{
		return Command::run;
	}
#endif
	else
	{
		return Command::invalid;
	}
}

static const char* getCommandListHelpText()
{
	return "Commands:\n"
		   "  assemble     Assemble WAST/WAT to WASM\n"
		   "  disassemble  Disassemble WASM to WAST/WAT\n"
#if WAVM_ENABLE_RUNTIME
		   "  compile      Compile a WebAssembly module\n"
#endif
		   "  help         Display help about command-line usage of WAVM\n"
#if WAVM_ENABLE_RUNTIME
		   "  run          Run a WebAssembly program\n"
#endif
		   "  version      Display information about the WAVM version\n";
}

static void showTopLevelHelp(Log::Category outputCategory)
{
	Log::printf(outputCategory,
				"Usage: wavm <command> [command arguments]\n"
				"\n"
				"%s"
				"\n"
				"%s\n",
				getCommandListHelpText(),
				getEnvironmentVariableHelpText());
}

static void showHelpHelp(Log::Category outputCategory)
{
	Log::printf(outputCategory,
				"Usage: wavm help <command>\n"
				"\n"
				"%s",
				getCommandListHelpText());
}

static int execHelpCommand(int argc, char** argv)
{
	if(argc == 1)
	{
		const Command helpCommand = parseCommand(argv[0]);
		switch(helpCommand)
		{
		case Command::assemble: showAssembleHelp(Log::output); return EXIT_SUCCESS;
		case Command::disassemble: showDisassembleHelp(Log::output); return EXIT_SUCCESS;
		case Command::help: showHelpHelp(Log::output); return EXIT_SUCCESS;
		case Command::version: showVersionHelp(Log::output); return EXIT_SUCCESS;
#if WAVM_ENABLE_RUNTIME
		case Command::compile: showCompileHelp(Log::output); return EXIT_SUCCESS;
		case Command::run: showRunHelp(Log::output); return EXIT_SUCCESS;
#endif
		case Command::invalid:
			Log::printf(Log::error,
						"Invalid command: %s\n"
						"\n"
						"%s",
						argv[0],
						getCommandListHelpText());
			return EXIT_FAILURE;

		default: WAVM_UNREACHABLE();
		};
	}
	else
	{
		showHelpHelp(Log::error);
		return EXIT_FAILURE;
	}
}

void showVersionHelp(Log::Category outputCategory)
{
	Log::printf(outputCategory, "Usage: wavm version\n");
}

int execVersionCommand(int argc, char** argv)
{
	if(argc != 0)
	{
		showVersionHelp(Log::error);
		return EXIT_FAILURE;
	}
	Log::printf(Log::output,
				"WAVM version %u.%u.%u\n",
				WAVM_VERSION_MAJOR,
				WAVM_VERSION_MINOR,
				WAVM_VERSION_PATCH);
	return false;
}

int main(int argc, char** argv)
{
	if(!initLogFromEnvironment()) { return EXIT_FAILURE; }

	if(argc < 2)
	{
		showTopLevelHelp(Log::Category::error);
		return EXIT_FAILURE;
	}
	else
	{
		const Command command = parseCommand(argv[1]);
		switch(command)
		{
		case Command::assemble: return execAssembleCommand(argc - 2, argv + 2);
		case Command::disassemble: return execDisassembleCommand(argc - 2, argv + 2);
		case Command::help: return execHelpCommand(argc - 2, argv + 2);
		case Command::version: return execVersionCommand(argc - 2, argv + 2);
#if WAVM_ENABLE_RUNTIME
		case Command::compile: return execCompileCommand(argc - 2, argv + 2);
		case Command::run: return execRunCommand(argc - 2, argv + 2);
#endif

		case Command::invalid:
			Log::printf(Log::error,
						"Invalid command: %s\n"
						"\n"
						"%s",
						argv[1],
						getCommandListHelpText());
			return EXIT_FAILURE;

		default: WAVM_UNREACHABLE();
		};
	}
}
