#include "CSL/Compiler.h"
#include "GLSL/Compiler.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

using namespace crayon;

void PrintCmdLineArgs(int argc, char* argv[]);

int main(int argc, char* argv[])  {
	// PrintCmdLineArgs(argc, argv);
	if (argc != 2) {
		std::cerr << "Usage: cslc [source.csl]" << std::endl;
		return EXIT_FAILURE;
	}

	std::filesystem::path srcCodePath{argv[1]};
	// csl::Compiler cslCompiler{};
	glsl::Compiler glslCompiler{};
	
	try {
		// cslCompiler.Compile(srcCodePath);
		glslCompiler.Compile(srcCodePath);
	}
	catch (std::logic_error& le) {
		std::cerr << "Logic error has occurred: " << le.what() << std::endl;
		return EXIT_FAILURE;
	}
	catch (std::runtime_error& re) {
		std::cerr << "Runtime error has occurred: " << re.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

void PrintCmdLineArgs(int argc, char* argv[]) {
	for (int i = 0; i < argc; i++) {
		std::cout << i << ": " << argv[i] << "\n";
	}
}