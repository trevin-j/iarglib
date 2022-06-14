#ifndef IARGLIB_HPP
#define IARGLIB_HPP

#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>

namespace iarglib
{
    // Declaration only
    class IArger;

    // Enum for specifying whether an option requires arguments or not.
    enum class RequiresArg
    {
        No,
        Yes
    };

    // Struct for storing information about an option.
    struct Option
    {
        // The name of the option.
        std::string name;

        // The identifiers for the option.
        std::vector<std::string> identifiers;

        // The description of the option.
        std::string description;

        // Whether or not the option requires arguments.
        RequiresArg requiresArg;

        // The function to call when the option is specified.
        void (*func)(const IArger&);

        // An argument passed to the option.
        std::string argument;
    };

    // Declaration only
    void printHelp(const IArger& arger);
    void printVersion(const IArger& arger);

    class IArger {

    public:
        // Construct the IArger object with the command line arguments.
        IArger(int argc, char** argv)
        {
            this->argc = argc;
            this->argv = argv;
        }

        // Set the application name. Used when displaying help or version info, if applicable.
        // If addHelpOption() or addVersionOption() are used, it is recommended to use this too.
        void setAppName(const std::string& appName)
        {
            this->appName = appName;
        }

        // Add a manual option to watch for.
        // Unrecognized options and mistakes in the command line arguments will throw errors.
        // Args:
        //     optionName - The name of the option, used to check if it was passed in.
        //     optionDescription - The description of the option, used when displaying help, and not required.
        //     requiresArgs - Whether the option requires an argument. Use RequiresArgs::No for no and RequiresArgs::Yes for yes.
        void addOption(const std::string& optionName, const std::string& identifiers, const std::string& helpMessage, RequiresArg requiresArgs)
        {
            // Split the identifiers by the | character.
            std::vector<std::string> identifierList = split(identifiers, '|');

            Option option;
            option.name = optionName;
            option.identifiers = identifierList;
            option.description = helpMessage;
            option.requiresArg = requiresArgs;

            // Add the option to the map.
            options[optionName] = option;

            // Add the option to the list of all options.
            allOptions.push_back(optionName);
        }

        // Add a manual option to watch for, and execute the specified function when the option is passed in.
        // Unrecognized options and mistakes in the command line arguments will throw errors.
        // Args:
        //     optionName - The name of the option, used to check if it was passed in.
        //     optionDescription - The description of the option, used when displaying help, and not required.
        //     requiresArgs - Whether the option requires an argument. Use RequiresArgs::No for no and RequiresArgs::Yes for yes.
        //     event - The function to execute when the option is passed in.
        void addOptionEvent(const std::string& optionName, const std::string& identifier, const std::string& helpMessage, RequiresArg requiresArgs, void (*eventFunction)(const IArger&))
        {
            // Split the identifiers by the | character.
            std::vector<std::string> identifierList = split(identifier, '|');

            Option option;
            option.name = optionName;
            option.identifiers = identifierList;
            option.description = helpMessage;
            option.requiresArg = requiresArgs;
            option.func = eventFunction;

            // Add the option to the map.
            options[optionName] = option;

            // Add the option to the list of all options.
            allOptions.push_back(optionName);

            // Add the option to the list of options that have events.
            eventOptions.push_back(optionName);
        }

        // Add a help option that prints the specified help message,
        // the application name, and automatically generates a help screen from the option descriptions.
        void addHelpOption(const std::string& helpMessage)
        {
            usingAutoHelp = true;
            addOptionEvent("help", "-h|--help", "Display this help message", RequiresArg::No, printHelp);
        }

        // Set the version of the app, and automatically display a version message when the version option is passed in.
        void addVersionOption(const std::string& version)
        {
            usingAutoVersion = true;
            addOptionEvent("version", "-v|--version", "Display the version of this application", RequiresArg::No, printVersion);
            this->version = version;
        }

        // Set if the program should continue executing after displaying help info.
        // This defaults to true so that you must explicitly be aware that false will stop parsing args after help.
        // All this does is force stop parsing options if it finds the help option.
        // Only works if using built-in help option.
        void setContinueOnHelp(bool continueOnHelp)
        {
            this->continueOnHelp = continueOnHelp;
        }

        // Set if the program should continue executing after displaying version info.
        // This defaults to true so that you must explicitly be aware that false will stop parsing args after version.
        // All this does is force stop parsing options if it finds the version option.
        // If the help option is passed and set to not continue on help, the version option will be ignored
        // as the help option takes precedence.
        // Only works if using built-in version option.
        void setContinueOnVersion(bool continueOnVersion)
        {
            this->continueOnVersion = continueOnVersion;
        }

        // Parse the command line arguments. This is where events are triggered.
        // Returns true if the program should continue executing, false if it should exit.
        bool parse()
        {
            // MAPPING STAGE
            // Run through and map all option identifiers to their names.
            for (auto& option : options)
            {
                for (auto& identifier : option.second.identifiers)
                {
                    identifiersToNames[identifier] = option.first;
                }
            }


            // PARSING STAGE
            int op = 1;
            while (op < argc)
            {
                // Get the option.
                std::string option = argv[op];
                
                // Get option's name from its identifier. If it's not an identifier, it's invalid.
                auto optionNameIterator = identifiersToNames.find(option);

                // If the option is invalid, throw an error.
                if (optionNameIterator == identifiersToNames.end())
                {
                    throw std::runtime_error("Invalid option: " + option);
                }

                // get the option's name.
                std::string optionName = optionNameIterator->second;

                // Add option name to passedOptions.
                passedOptions.push_back(optionName);

                // Get the option from the map.
                Option& optionData = options[optionName];

                // If the option is help
                if (optionName == "help")
                {
                    if (usingAutoHelp)
                    {
                        printHelp(*this);
                        if (!continueOnHelp)
                            return false;
                    }
                }

                // If the option is version
                if (optionName == "version")
                {
                    if (usingAutoVersion)
                    {
                        printVersion(*this);
                        if (!continueOnVersion)
                            return false;
                    }
                }

                // If the option requires an argument, get the argument.
                if (optionData.requiresArg == RequiresArg::Yes)
                {
                    op++;

                    // If the next argument is non existent, throw an error.
                    if (op >= argc)
                    {
                        throw std::runtime_error("Option " + optionName + " requires an argument.");
                    }

                    // Get the argument.
                    optionData.argument = argv[op];

                    // If the argument is empty, throw an error.
                    if (optionData.argument == "")
                    {
                        throw std::runtime_error("Option " + optionName + " requires an argument.");
                    }

                    // If the argument is a different option, throw an error.
                    if (std::count(allOptions.begin(), allOptions.end(), optionData.argument))
                    {
                        throw std::runtime_error("Option " + optionName + " requires an argument.");
                    }
                }

                // If the option has an event, add it to the list of events to trigger at the end.
                if (std::count(eventOptions.begin(), eventOptions.end(), optionName))
                {
                    triggerEvents.push_back(optionData.func);
                }

                op++;
            }

            // TRIGGERING STAGE
            // Run through each event option and trigger the event.
            for (auto& event : triggerEvents)
            {
                event(*this);
            }

            return true;
        }

        // Get the names of all passed in options.
        const std::vector<std::string>& getPassedOptions() const { return passedOptions; }

        // Get all valid options.
        const std::vector<std::string>& getAllOptions() const { return allOptions; }

        // Check if a specific option was passed in.
        bool optionExists(const std::string& optionName) const
        {
            return std::count(passedOptions.begin(), passedOptions.end(), optionName) > 0;
        }

        // Get the value of a specific option.
        const std::string& getOptionArgument(const std::string& optionName) const
        {
            return options.at(optionName).argument;
        }

        // Get app name.
        const std::string& getAppName() const { return appName; }

        // Get app version.
        const std::string& getAppVersion() const { return version; }

    private:
        // Arg count
        int argc;
        // Arg values
        char** argv;
        // Is the program using auto help/version?
        bool usingAutoHelp = false;
        bool usingAutoVersion = false;
        // Continue on help/version?
        bool continueOnHelp = true;
        bool continueOnVersion = true;

        // The application name.
        std::string appName;

        // The application version.
        std::string version;

        // Unordered map of options. Key is the option name, value is the option struct.
        std::unordered_map<std::string, Option> options;

        // List of all accepted options.
        std::vector<std::string> allOptions;

        // List of all event options.
        std::vector<std::string> eventOptions;

        // Vector to keep track of every option that was passed in.
        std::vector<std::string> passedOptions;

        // List of all passed event functions.
        std::vector<void (*)(const IArger&)> triggerEvents;

        // Map of option identifiers to their option names.
        std::unordered_map<std::string, std::string> identifiersToNames;


        // Function to split a string by a delimiter.
        std::vector<std::string> split(std::string str, char delimiter) const
        {
            std::vector<std::string> tokens;
            for (std::string::size_type i = 0; i < str.length(); i++)
            {
                if (str[i] == delimiter)
                {
                    tokens.push_back(str.substr(0, i));
                    str.erase(0, i + 1);
                    i = 0;
                }
            }
            tokens.push_back(str);
            return tokens;
        }
    };

    void printHelp(const IArger& arger)
    {
        std::cout << "Help: " << arger.getAppName() << std::endl;
        for (auto& option : arger.getAllOptions())
        {
            std::cout << "  " << option << std::endl;
        }
    }

    void printVersion(const IArger& arger)
    {
        std::cout << "Version: " << arger.getAppName() << std::endl;
    }
}

#endif