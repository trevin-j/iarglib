#include <iostream>
#include <string>
#include <vector>

#include "iarglib.hpp"

using iarglib::IArger;
using iarglib::RequiresArg;

void exampleDoSomething(const IArger& arger)
{
    std::cout << "The -d or --do-something option was specified." << std::endl;
}

void setupOptions(IArger& arger)
{
    // Set the application name. Used when displaying help or version info, if applicable.
    arger.setAppName("IArgLib Example Application");

    // Add help and version options.
    // If added through this method, help and version messages are automatically generated and handled.
    // You can also add help and version options manually using addOption() or addOptionEvent().
    // With these special options, if continueOnHelp is false, every option beside help or version is ignored, even invalid options.
    arger.addHelpOption("This IArgLib example shows how to use the IArger class to parse command line arguments.");
    arger.addVersionOption("1.0.0");

    // If using built-in help and version options, you can specify if you want the program to continue after displaying help or version info.
    // These default to true so that you have to be aware that false will force the parser to stop after displaying help or version info,
    // even though the program will still continue to run.
    arger.setContinueOnHelp(false);
    arger.setContinueOnVersion(false);

    // Add some manual options.
    arger.addOption("file", "-f|--file", "Example option - The file to read", RequiresArg::Yes);
    arger.addOption("output", "-o|--output", "Example option - The file to write", RequiresArg::Yes);
    arger.addOption("size", "-S|--size", "Example option - The size of the output file", RequiresArg::Yes);
    arger.addOption("read", "-r|--read", "Example option - Read the file", RequiresArg::No);

    arger.addOptionEvent("do-something", "-d|--do-something", "Example option - An example of an event option", RequiresArg::No, exampleDoSomething);
}

int main(int argc, char** argv)
{
    // Initialize the IArger object.
    IArger arger(argc, argv);
    
    // Set up the options.
    setupOptions(arger);

    bool keepRunning = false;

    // It is up to the dev to determine how to handle stuff like invalid options
    try
    {
        // Parse the command line arguments. This is where events are triggered.
        keepRunning = arger.parse();
    }
    catch (const std::exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    }

    if (!keepRunning)
    {
        return 0;
    }

    // Get all options that were passed in.
    std::vector<std::string> options = arger.getPassedOptions();

    // Check if a specific option was passed in.
    bool readOption = arger.optionExists("read");
    if (readOption)
    {
        std::cout << "The -r or --read option was specified." << std::endl;
    }

    // Get the value of a specific option, if it was passed in.
    if (arger.optionExists("file"))
    {
        std::string file = arger.getOptionArgument("file");
        std::cout << "The file to read is: " << file << std::endl;
    }
}