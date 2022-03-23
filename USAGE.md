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
