## Tutorial

### Step 1: Setting up the library and initializing the parser

To start, we need to include the header file, just like any other library. This library is header-only, so you don't need to link to it.

```cpp
#include "iarglib.hpp"
```

Next, we are going to add a couple using statements to shorten the code. This is optional, of course. There are two classes that we need within the `iarglib` namespace: `IArger` and `RequiresArg`.

```cpp
using iarglib::IArger;
using iarglib::RequiresArg;
```

Now, within our main function, we are going to create a new instance of the `IArger` class, and immediately give it our command line arguments. Don't forget to include the `argc` and `argv` arguments in your main function. This `IArger` class is our Input Argument Parser.

```cpp
int main(int argc, char** argv)
{
    IArger arger(argc, argv);

}
```

Great, now it is initialized and we are ready to set up the options.

### Step 2: Setting up the options

Exactly how you choose to add options is up to you, but for the sake of keeping our main function clean, I will be creating a simple function called `setupOptions` to set up all of the options.

Above our main function, define a function called `setupOptions` that takes an `IArger` reference.

```cpp
void setupOptions(IArger& arger)
{

}
```

Within this function, we are going to do everything we need to do to set up the options. The most basic parser would only need the `addOption` method, but we have some more methods that will make it a little bit easier on us. For example, if we want our program to have a help option, and a version option, instead of implementing those ourselves we can just tell IArger to add them for us.

To do this, we will first tell IArger the name of our program. Then, we will add the help option and version option. This is done below.

```cpp
void setupOptions(IArger& arger)
{
    // Set the application name.
    arger.setAppName("My Program");

    // Add the help option. It takes 1 argument: the help message to display above the help text for the options.
    arger.addHelpOption("My Program is a simple example program using the IArgLib library.");

    // Add the version option. It takes 1 argument: the version number.
    arger.addVersionOption("1.0.0");
}
```

Now, IArger knows to handle help and version options for us! But, right now, if we finished the rest of the necessary code and ran the program with the help or version option along with other options, it will run all of them. Which, maybe that's what we want, but right now let's make it so that after it does either the version or help option, it stops parsing.

To do this, we will use the methods `setContinueOnHelp` and `setContinueOnVersion`. By default, these are both true, because later we will have to understand how to handle it when IArger stops parsing on either of those. For now, though, let's just add these methods after `arger.addVersionOption("1.0.0")`.

```cpp
    // Set the help option to stop parsing if the help option is passed
    arger.setContinueOnHelp(false);

    // Set the version option to stop parsing if the version option is passed
    arger.setContinueOnVersion(false);
```

Great! Now, we are ready to add additional options. Let's start with basic options. These options can be handled after IArger parses the input. To handle basic options, you can just check if they exist later on, and even get their value if such a value exists.

To add a basic option, we use the `addOption` method. This method takes in 4 arguments: 1) the name of the option, 2) the identifier of the option, 3) the help message for the option, and 4) whether or not the option accepts an argument value. Now we are going to use the enum class `RequiresArg` to specify whether or not the option accepts an argument value. Let's add a basic option that accepts an argument value, and another basic option that does not.

```cpp
    // Add a basic option that accepts an argument value
    arger.addOption("my-option", "-m", "This is a basic option that accepts an argument value.", RequiresArg::Yes);

    // Add a basic option that does not accept an argument value
    arger.addOption("my-option-no-arg", "-n", "This is a basic option that does not accept an argument value.", RequiresArg::No);
```

Great, now with these basic options, we will be able to check if they were passed, and get their value if applicable. And, since we set up IArger to use automatic help messages, we will be able to see the help message if the user passes the help option. If we had chosen not to use automatic help messages, we wouldn't have any need to pass a help message argument, so it could be passed as an empty string.

So now that we have a couple basic options, let's add a new kind of option: an event option. Event options allow us to set a function to be called whenever the user passes that option. In fact, under the hood, IArger uses event functions to handle both automatic help and version options. To use event options, we will use the `addOptionEvent` method. This method takes in the same arguments as `addOption`, except with one extra argument: the function to call if the option is passed.

The function that we pass to `addOptionEvent` must have a void return type, and only accept one argument: `const IArger&`. Let's define a function to be called whenever the user passes a certain option. Define this function above the `setupOptions` function.

```cpp
void myEventFunction(const IArger& arger)
{
    std::cout << "My event function was called!" << std::endl;
}
```

Remember to include the iostream header at the top of the file! (`#include <iostream>`)

Now let's add this event option underneath the last option that we added.

```cpp
    // Add an event option
    arger.addOptionEvent("my-event-option", "-e", "This is an event option.", RequiresArg::No,myEventFunction);
```

Great, now if the user passes the `-e` option, our event function will be called when we tell IArger to parse the input.

Now we just need to call our `setupOptions` function to finish setting up. Add the following line to the main function:

```cpp
    setupOptions(arger);
```

Before we move on, let's quickly discuss the order of options. With only *one* exception, Event option functions are always called in the order in which the user passes them. The one exception to this rule is if you choose to use automatic help or version messages. In this case, the help and version messages take first priority, then it will follow in the order in which the options were added. Whether help or version is used first depends on the order in which the user passes the help or version options. Since we set IArger to stop after help and version messages, only one will be registered if the user passes both.

### Step 3: Parsing the input

Now that we have finished adding allowed options, we are ready to parse the input. Return the the main function, and we will add more code beneath the call to `setupOptions`.

To parse the input, we simply make a call to the `parse` method. Since we have already set up IArger, and since we already passed it the arguments, the `parse` method requires no parameters. We do have to have a return value, however. This method returns a boolean value that represents whether or not the program should continue running. The reason we have to return a boolean value is because we want to stop the program if the user passes the help or version options. Since we told IArger to stop after help and version messages, this boolean will return false if the user passes either of those options.

If IArger tells us to stop, we should probably exit the program. Let's add the following line to the main function:

```cpp
    // Parse the input. Keep track of whether or not the program should continue running.
    bool shouldContinue = arger.parse();

    // If the program should stop, exit the program with an success code.
    if(!shouldContinue)
    {
        exit(0);
    }
```

Notice how we returned a success code. It's important to remember that the boolean returned by `parse` is only a flag to tell us whether or not the program should continue running. If there was an error while parsing, the program will throw an exception. We will handle this exception in the next step.

The main type of exception that could be thrown is an exception due to invalid input. Let's catch these errors, then display them, and then finally exit with an error code.

Modify the lines that we just added to look like the following:

```cpp
    // Bool to keep track of whether or not the program should continue running.
    bool shouldContinue = false;

    // Try to parse the input.
    try
    {
        shouldContinue = arger.parse();
    }
    catch(const std::exception& e)
    {
        // Display the error message.
        std::cout << "Error: " << e.what() << std::endl;

        // Exit with an error code.
        exit(1);
    }

    // If the program should stop, exit with an success code.
    if(!shouldContinue)
    {
        exit(0);
    }
```

Now, this should handle all of the possibilities that IArger can throw at us. If you run this and pass `-e` as an option, you should be able to see that it already works! It already calles event functions! But now we just need to learn how to manually check if an option was passed, and get its value if applicable.

### Step 4: Checking options

Checking options is super simple and easy! There are a couple ways to do so, and we'll cover them both.

First, we can use the method `getPassedOptions` to get a std::vector of all of the options that were passed. Second, we could use the method `optionExists` to check if a certain option was passed. Let's see an example.

```cpp
    // Get a vector of all of the options that were passed.
    std::vector<std::string> passedOptions = arger.getPassedOptions();

    // Display the first option that was passed.
    std::cout << "The first option that was passed was: " << passedOptions[0] << std::endl;

    // Check if the -n option was passed.
    if(arger.optionExists("my-option-no-arg"))
    {
        std::cout << "The -n option was passed!" << std::endl;
    }
```

Great! Now, let's talk about checking the value of an option. Remember that you can only check the value of an option that requires an argument. You also should check that the option exists before trying to get its value. To get its value, use the `getOptionArgument` method. Check the following example:

```cpp
    // If the -m option was passed, get its value.
    if(arger.optionExists("my-option"))
    {
        std::cout << "The -n option was passed with the value: " << arger.getOptionArgument("my-option") << std::endl;
    }
```

Ok now notice how we didn't check using the identifier! Instead we check using the *name* of the option. The identifier is what the user passes. The name is useful because IArger allows multiple options to mean the same thing! This is done by adding a vertical bar in the identifier parameter to separate identifiers. For example, if we wanted to allow the user to pass both `-t` and `--talkative`, we would add the following line to the `setupOptions` function:

```cpp
    // Add a boolean option with a name.
    arger.addOptionBool("talkative", "-t|--talkative", "Make this program talkative", RequiresArg::No);
```

With this, we only have to check *once* if the user passed either `-t` or `--talkative`.

Lastly, remember how our event function gets passed the IArger? Well, that allows us to make checks from within that function! Any of the checks that we just learned about in this step work within event functions!

This works because the `parse` method is split up into 3 stages. The first stage is called the *Mapping Stage*. During this stage, IArger maps each option name to its possible identifiers. This allows you to make one check on the option name, and it automatically checks for any possible identifiers.

The second stage is the *Parsing Stage*. This stage is where IArger checks that everything is valid input. It also adds events to be triggered later, and it runs the built in help or version messages if applicable.

The third and final stage is the *Triggering Stage*. This stage is where IArger triggers the events that were processed in the parsing stage.

Since all events are triggered in the last stage, all the parsing is already done and therefore you are allowed to make checks in the event functions.