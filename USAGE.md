# Ruby Discord Game SDK

Ok so, Here's a quick explanation of the Ruby Discord Game SDK.  
Powered by "I technically have a meeting at 10am, but I have insomina so fuck it."  
Improved by GLS' having the same energy.

## CHAPTER 0: DYNAMIC LINKING BASICS

You can download the official Discord GameSDK [Here](https://dl-game-sdk.discordapp.net/2.5.6/discord_game_sdk.zip)

You'll see a `c/discord_game_sdk.h` This is a header file, which basically declares everything(functions, enums, structs, etc) you can use. In this case, it declares everything you can access in `lib/x86_64/discord_game_sdk.(so/dll)` (`so` for linux, `dll` for Windows), which contains the actual code that gets executed.

(If you've worked with C/C++, you can skip this. You can actually probably just skip this anyway lol.)  
For various reasons, sometimes you need to compile certain functions into a format that another program can use, but not necessarily in source-code form.
This can be for reasons like:

1. Certain functions are used commonly, so might as well extract them into a library, which can be stored on the machine and referenced by any program. This means they won't have to implement that code into their own program, and can instead just reference the library and reuse pre-existing code.

2. For super large projects (Like Chrome, Firefox, Linux, etc), it's nice to compile things seperately and link them alltogether. This helps speed up compilation of said program and can allow faster development and testing, as you can update individual blocks of the program instead of having to do everything at once.

3. For cases where you have some functions you'd like others to call, but don't want to distribute the source-code for (Like the Discord GameSDK), libraries allow you to share that function without having to send the source, which the enduser would have to compile. Normally, a `.h` header file is included so the calling program is able to know what exists in said library.

(Quick tangent on different types of linking)  
There are actually 2 different ways to add a library to your final program; Static Linking & Dynamic Linking.  
Example 2 is an example of Static Linking, where you create a bunch of `.o` object files (not exectuable themselves) and link them all together into the final executable at compile.  
Whereas Example 1 & 3 are examples of Dynamic Linking, where none of the library's code is included in your actual executable, but is instead Dynamically loaded into memory when the program is started.  
This explains the naming of `.so` on Linux, as it stands for `Shared Object`, and `.dll` on Windows, as it stands for `Dynamic Link Library`.

## Chapter 1: Basics of using the C SDK

So, going back to the Ruby Discord Game SDK.
Our goal is to expose all the functionality offerec in `c/discord_game_sdk.h`, since that's the header that has everything.
if you look at the file, you see 3 distinct blocks of code; A bunch of `enums`, another bunch of `structs` and finally, some function declarations.

`enum`s are basically named integers (so `DiscordActivityType_Listening` is actually equivalent to just using the number 2).
It helps keep things readable and you don't need to write down a bunch of magic numbers cause the enum names should be fairly good documentation.
`struct`s, are like custom datatypes in C. You can access the individual members of the `struct` by doing `mystruct.variable`. However, you cannot have functions inside of a `struct` in the same way you would in Ruby Classes.
(Well technically you can with Function Pointers, but I'll explain them later.)
And finally, the actual function definitions. In this case, there's one main function `DiscordCreate` that you call to initialize the SDK. All further interactions with it [the API] are using the struct.
An example of using this to Initalize the Discord GameSDK in C would be

```C
// Create DiscordCreateParams
struct DiscordCreateParams params;
// Set it to all defaults. This function exists in discord_game_sdk.h
// The & is for some pointer magic so don't worry about it too much, if you do plan on modifying things in this repo please lmk cuz pointers are confusing as fuck
DiscordCreateParamsSetDefault(&params);
// Set client id and flags
params.client_id = 123456789;
params.flags = DiscordCreateFlags_Default;
// Create a pointer to a struct IDiscordCore.
// This struct will hold almost everything related to discord SDK
// This pointer points to nothing for now. It will be modified by the init function to actually contain infomation useful to the operation of the SDK.
struct IDiscordCore* core;
// Actually call the init function, and store the result
enum EDiscordResult result = DiscordCreate(DISCORD_VERSION, &params, &core);
if (result != DiscordResult_Ok) {
    throw_an_error_or_something();
}
// If you're here, you should be good!
```

Now, things get a bit more confusing, as we now have to deal with function pointers.
If you look at the `IDiscordCore` struct, you'll see it has a member like `struct IDiscordActivityManager* (*get_activity_manager)(struct IDiscordCore* core)`.
This is basically a reference to a function that you can read, modify and call.
In this case, it would be a function that takes in `struct IDiscordCore *` and returns `struct IDiscordActivityManager*`.
These pointers will be set by `DiscordCreate` to contain an actual function that does stuff.
To use any of the features of the SDK (e.g setting an activity), you actually need it's corresponding manager to do the work. So you'll need to get the Activity Manager and do stuff with it.
Continuing our earlier example:

```C
// Now we need to get the Activity Manager
// Don't worry too much about the use of `object.something` vs `object->something`, it has a lot to do with pointers. you definitely need to worry about it if you're writing code especially with double pointers, but lmk before you do that and i can help
// Create a pointer to an activity manager
struct IDiscordActivityManager* activityManager;
// Initialize it
activityManager = core->get_application_manager(core);
// And now we can call functions on it! look for function pointers (yes those fuckers again) in the `struct IDiscordActivityManager` definition. calling a simple one as an example
activityManager->register_steam(activityManager, 420530);
```

Finally, if you want to do complicated things, like setting the activity, you sometimes need another `struct` to store all the data to pass into the function.

```C
struct DiscordActivity activity;
activity.type = DiscordActivityType_Playing
activity.state = "In an area of the game"
activityManager->update_activity(activityManager, &activity, dontworryaboutit, dontworryaboutit);
```

(The two `dontworryaboutit` parameters are related to callback functions which I am definitely not going into details about, nor am I gonna explain the nuance with nested structs.)

Ok. Time to actually talk Ruby.
