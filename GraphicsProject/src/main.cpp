#include "Project.hpp"

int main( int argc, char **argv ) 
{
    std::string title("Bjon Li - CS488 Final Project");
    CS488Window::launch(argc, argv, new Project(), 1024, 768, title);

	return 0;
}
