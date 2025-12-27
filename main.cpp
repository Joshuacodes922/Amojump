#include "VulkanEngine.cpp"
#include <glm/glm.hpp>
int main()
{
#if defined(__clang__)
    std::cout << "Compiler: Clang " << __clang_version__ << std::endl;
#elif defined(__GNUC__) || defined(__GNUG__)
    std::cout << "Compiler: GCC " << __VERSION__ << std::endl;
#elif defined(_MSC_VER)
    std::cout << "Compiler: MSVC " << _MSC_VER << std::endl;
#else
    std::cout << "Unknown compiler" << std::endl;
#endif
	HelloTriangle app;

	try {
		app.run();
	}
	catch (const std::exception e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
    return 0;
}
