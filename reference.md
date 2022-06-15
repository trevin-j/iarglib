## Reference

Here is a simple reference for everything contained in IArgLib.

### Index

* [iarglib::RequiresArg](#iarglibrequiresarg)
* [iarglib::IArger](#iarglibiarger)
    * [IArger::IArger](#iargeriarger)
    * [IArger::setAppName](#iargersetappname)
    * [IArger::addOption](#iargeraddoption)
    * [IArger::addOptionEvent](#iargeraddoptionevent)
    * [IArger::addHelpOption](#iargeraddhelpoption)
    * [IArger::addVersionOption](#iargeraddversionoption)
    * [IArger::setContinueOnHelp](#iargersetcontinueonhelp)
    * [IArger::setContinueOnVersion](#iargersetcontinueonversion)
    * [IArger::parse](#iargerparse)
    * [IArger::getPassedOptions](#iargergetpassedoptions)
    * [IArger::getAllOptions](#iargergetalloptions)
    * [IArger::getOptionData](#iargergetoptiondata)
    * [IArger::optionExists](#iargeroptionexists)
    * [IArger::getOptionArgument](#iargergetoptionargument)
    * [IArger::getAppName](#iargergetappname)
    * [IArger::getAppVersion](#iargergetappversion)
* [iarglib::Option](#iargliboption)

### `iarglib::RequiresArg`

RequiresArg is a enum that represents whether or not an option requires an argument. The possible values are `RequiresArg::Yes` and `RequiresArg::No`.

### `iarglib::IArger`

The IArger class is the main class of IArgLib. It is used to setup the options, and to parse the input.

#### `IArger::IArger`

`IArger::IArger(int argc, char** argv)`

The class constructor simply initializes the IArger object. Pass in the number of arguments and the arguments.

#### `IArger::setAppName`

`void IArger::setAppName(const std::string& appName)`

This method sets the name of the application. This is used in the built in help and version messages. If you don't intend on using the automatic help and version messages, you can dismiss this method.

#### `IArger::addOption`

`void IArger::addOption(const std::string& optionName, const std::string& identifiers, const std::string& helpMessage, RequiresArg requiresArgs)`

This method is used to add an option. If the user passes an option that you have not added, it will throw an exception.

Arguments:

* `optionName`: The name of the option. This is how you will check if the option was passed. This is not what the user will pass as an argument.
* `identifiers`: The identifiers that the user can pass to the option. This is what the user will pass as an argument. You can use multiple identifiers by separating them with a vertical bar. For example, passing `"-f|--file"` will mean that the user can pass `-f` or `--file`.
* `helpMessage`: The help message that will be displayed when the user passes the `-h` or `--help` option. This is only used if you use the built in help message. If you intend to create your own help message, you can dismiss this method by passing an empty string.
* `requiresArgs`: Whether or not the option requires an argument. Use the `RequiresArg` enum to set this.

#### `IArger::addOptionEvent`

`void IArger::addOptionEvent(const std::string& optionName, const std::string& identifier, const std::string& helpMessage, RequiresArg requiresArgs, void (*eventFunction)(const IArger&))`

This method is used to add an option which will call a function when the option is passed. It can also be checked just like the `addOption` method.

Arguments:

* `optionName`: The name of the option. This is how you will check if the option was passed. This is not what the user will pass as an argument.
* `identifiers`: The identifiers that the user can pass to the option. This is what the user will pass as an argument. You can use multiple identifiers by separating them with a vertical bar. For example, passing `"-f|--file"` will mean that the user can pass `-f` or `--file`.
* `helpMessage`: The help message that will be displayed when the user passes the `-h` or `--help` option. This is only used if you use the built in help message. If you intend to create your own help message, you can dismiss this method by passing an empty string.
* `requiresArgs`: Whether or not the option requires an argument. Use the `RequiresArg` enum to set this.
* `eventFunction`: The function that will be called when the option is passed. Must return void and take a single `const IArger&` argument.

#### `IArger::addHelpOption`

`void IArger::addHelpOption(const std::string& helpMessage)`

`void IArger::addHelpOption(const std::string& helpMessage, const std::string& postMessage)`

This method is used to add the automatic help option. The help identifiers are `-h` and `--help`. The automatic help option uses the help message you supply, as well as the application name, and the help messages of each option to generate a help message. If you desire, you can manually create your own help option, and not use this method.

The optional `postMessage` argument is used to display a message after the help message.

#### `IArger::addVersionOption`

`void IArger::addVersionOption(const std::string& version)`

`void IArger::addVersionOption(const std::string& version, const std::string& postMessage)`

This method is used to add the automatic version option. The version identifiers are `-v` and `--version`. The automatic version option uses the version and app name you supply to generate a version message. If you desire, you can manually create your own version option, and not use this method.

The optional `postMessage` argument is used to display a message after the version message.

#### `IArger::setContinueOnHelp`

`void IArger::setContinueOnHelp(bool continueOnHelp)`

This method is used to set whether the program should continue executing after the help option is passed. If you set this to `true`, the program will continue executing after the help option is passed. If you set this to `false`, IArger will stop parsing once the help option is passed. At that point, the `parse` method will return `false`. You will need to check the return value of `parse` to see if the program should continue executing.

#### `IArger::setContinueOnVersion`

`void IArger::setContinueOnVersion(bool continueOnVersion)`

This method is used to set whether the program should continue executing after the version option is passed. If you set this to `true`, the program will continue executing after the version option is passed. If you set this to `false`, IArger will stop parsing once the version option is passed. At that point, the `parse` method will return `false`. You will need to check the return value of `parse` to see if the program should continue executing.

#### `IArger::parse`

`bool IArger::parse()`

This method is used to parse the input. It will return `true` if the program should continue executing, and `false` if the program should stop executing. It is during this method that all the events are triggered.

This method consists of three stages. The first stage is the *Mapping Stage*. This stage is where IArger maps the option names to the option identifiers. This allows you to check if an option was passed by name, and not have to check all possible identifiers.

The second stage is the *Parsing Stage*. This stage is where IArger checks that everything is valid input. It also adds events to be triggered later, and it runs the built in help or version messages if applicable.

The third stage is the *Event Stage*. This stage is where IArger triggers the events that were added to the program.

#### `IArger::getPassedOptions`

`const std::vector<std::string>& IArger::getPassedOptions() const`

This method can be used to get a vector of all the option names that were passed.

#### `IArger::getAllOptions`

`const std::vector<std::string>& IArger::getAllOptions() const`

This method can be used to get a vector of all the option names that were added. Internally, this method is used to generate a help message.

#### `IArger::getOptionData`

`const option& IArger::getOptionData(const std::string& optionName) const`

This method can be used to get the option data for a specific option.

#### `IArger::optionExists`

`bool IArger::optionExists(const std::string& optionName) const`

Use this method to check if the user passed a specific option. Remember to pass the name of the option, not the identifiers.

#### `IArger::getOptionArgument`

`const std::string& IArger::getOptionArgument(const std::string& optionName) const`

This method can be used to get the argument that was passed to an option. Remember to pass the name of the option, not the identifiers.

#### `IArger::getAppName`

`const std::string& IArger::getAppName() const`

Get the application name that you specified. Internally, this method is used to generate a help and version message.

#### `IArger::getAppVersion`

`const std::string& IArger::getVersion() const`

This method can be used to get the version that you specified. Internally, this method is used to generate a version message.

### `iarglib::Option`

The `Option` struct is used internally to store information about an option. Most often you will not need to use this struct.

```cpp
namespace IArgLib {
    struct Option {
        std::string name;
        std::vector<std::string> identifiers;
        std::string description;
        RequiresArg requiresArg;
        void (*func)(const IArger&);
        std::string argument;
    };
}
```