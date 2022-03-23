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

## Chapter 2: Writing a Ruby C Extension

The Ruby C internals are not documented very well, which absolutely sucks. Most of the stuff online are like
> Haha, Here's a tutorial to write a hello world Ruby extension in C. Don't you feel accomplished? :)
Without actually going into the details that I need to write this Extension.

There are [online tutorials](https://www.rubyguides.com/2018/03/write-ruby-c-extension/), which explain how to create Ruby things from C land (like creating a hash, or reading a Ruby `int` to a proper C `int`), but not much else. And tbh, for that functionality, I learned the most by reading mkxp's mri-binding code.
The best documentatoin I found at the time (There might be newer ones that are good, but I relied on this very heavily) is [this](https://silverhammermba.github.io/emberb/c/) which, among other things, explains how you can actually keep one value in C around in a Ruby obejct, which we need for `struct`s.

So for all the 3 things that `c/discord_game_sdk.h` exposes; `enum`s, `struct`s & functions, `enum`s are the simplest part and just be done in pure Ruby (it's just declaring a bunch of variables), which I did [here](https://github.com/rkevin-arch/ruby_discord_game_sdk/blob/main/lib/ruby_discord_game_sdk/enums.rb).
The only important thing is making sure the integer values in Ruby match the actual `int`s in the header file, because we're passing in the value from Ruby land directly into the functions.

## Chapter 3: Wrapping a C Struct in Ruby

As for structs, this is likely the most complicated part.  
(ugh, I just realized I made the code much more difficult to understand as a beginner because I use macros everywhere)  
C Macros are a very powerful, but absolutely evil tool and I [rkevin] hate them with every fiber of my being.  
TBH, working on ModShot (the OpenAL stuff) and this are literally the only 2 cases where I think using macros are fin and not just plain evil.  

All the macro definitions are in `helpers.h`, which I will not go over and will just post the macro expanded versions of instead so it's easier for me to explain.  
So in `activity.h`, we have the following:

```C
extern VALUE rb_cDiscordActivity;

DECLARE_DISCORD_TYPE(activity, struct DiscordActivity)
void rb_discord_init_activity(VALUE module);
```

which after macro expansion becomes this:

```C
extern VALUE rb_cDiscordActivity;

size_t rb_discord_activity_dsize(const void* data);
extern const rb_data_type_t rb_discord_activity_type;
VALUE rb_discord_activity_alloc(VALUE self);
ctype* rb_discord_activity_get_struct(VALUE self);
void rb_discord_activity_set_struct(VALUE self, struct DiscordActivity* data);
void rb_discord_init_activity(VALUE module);
```

Again, in the header files, we only have the declarations of what functions there are (and what potential static variables there are), but not the actual definitions themselves. Those are in `activity.c`.  
In there, we first have the actual global variable `VALUE rb_cDiscordActivity` that will be used by the main module (it'll become the `DiscordGameSDK::Activity` class).  
Read the doc I posted before, [specifically this section](https://silverhammermba.github.io/emberb/c/#data), for some info on how we create something in C and keep it alive in Ruby land.  
Basically, we need a `const rb_data_type_t rb_discord_activity_type` object (there's some fancy syntactic sugar here. some weird ruby internal stuff, don't question it) that is used by Ruby to wrap up and arbitrary block of Memory into a Ruby object that you can then access from C land.  

- A `rb_discord_activity_alloc` function that allocated the memory for the new object.
- A `rb_discord_activity_dsize` function to tell Ruby how big our data is (for Ruby internal tracking purposes).

And that's mostly it (I took some shortcuts with memory freeing, will explain in a bit)
So all of that is wrapped in the `DEFINE_DISCORD_TYPE(activity, struct DiscordActivity)` macro, which expands to the following:

```C
size_t rb_discord_activity_dsize(const void* data) {
    return sizeof(struct DiscordActivity);
}

const rb_data_type_t rb_discord_activity_type = {
    .wrap_struct_name = "DiscordGameSDK_activity",
    .function = {
        .dmark = NULL,
        .dfree = free,
        .dsize = rb_discord_activity_dsize,
    },
    .data = NULL,
    .flags = RUBY_TYPED_FREE_IMMEDIATELY,
};

VALUE rb_discord_activity_alloc(VALUE self) {
    /* allocate memory for newly created object */
    struct DiscordActivity* data = malloc(sizeof(struct DiscordActivity));
    memset(data, 0, sizeof(struct DiscordActivity));
    return TypedData_Wrap_Struct(self, &rb_discord_activity_type, data);
}

struct DiscordActivity* rb_discord_activity_get_struct(VALUE self) {
    /* given a ruby object, return the underlying c struct */
    ctype* data;
    TypedData_Get_Struct(self, struct DiscordActivity, &rb_discord_activity_type, data);
    return data;
}

void rb_discord_activity_set_struct(VALUE self, ctype* data) {
    /* given a struct, copy its data into the ruby object */
    memcpy(rb_discord_activity_get_struct(self), data, sizeof(struct DiscordActivity));
}
```

When we're allocating a new object, I use `malloc` to create a block of memory in C that's of size `sizeof(struct DiscordActivity)`, use `memset` to initalize it to all 0s, then use the ruby helper function `TypedData_Wrap_struct` (along with the `rb_discord_activity_type` definition we wrote) to return the Ruby object.  
In the definition of `rb_discord_activity_type`, we set `.function.dfree` to the `free` function, which is a C builtin function to cleanup memory created by `malloc`. It'll be called by Ruby automatically when the object is destroyed, so we don't end up with leaked memory.  
Finally, the `rb_discord_activity_get_struct` and `rb_discord_activity_set_struct` are just helper functions for me to get the underlying `stuct DiscordActivity` from a Ruby object.  
Later in a function called by our module's init function, there are

```C
void rb_discord_init_activity(VALUE module) {
    rb_cDiscordActivity = rb_define_class_under(module, "Activity", rb_cObject);
    rb_define_alloc_func(rb_cDiscordActivity, rb_discord_activity_alloc);
    ...
}
```

So module will be `DiscordGameSDK`, and `rb_cDiscordActivity` is a newly created class under it, so it maps to Ruby's `DiscordGameSDK::Activity`, then we set the `alloc` class to `rb_discord_activity_alloc`. So whenever a new object is created, it calls that alloc func to get it's own chunk of memory carved out for a `struct DiscordActivity`.  

After all that, we can finally get to the getters and setters for them.  
I'm just gonna use a sing one as an example - All the diffrent types work slightly differently, but this s already way too long.  
I have a macro `DEFINE_ATTRIBUTE_INT(activity, type)` that creates the C functions for the getters and setters, then later `EXPOSE_ATTRIBUTE(rb_cDiscord_Activity, activity, type)` in `rb_discord_init_activity` to expose those C functions to Ruby land.  
The functions definitions expanded by `DEFINE_ATTRIBUTE_INT(activity, type)` looks like this

```C
VALUE rb_discord_activity_get_type(VALUE self) {
    return INT2NUM(rb_discord_activity_get_struct(self)->type);
}
VALUE rb_discord_activity_set_type(VALUE self, VALUE val) {
    rb_discord_activity_get_struct(self)->type = NUM2INT(val);
    return Qnil;
}
```

So y'know, fairly standard stuff that you might see in mkxp. Just some Ruby/C type conversions.  
The part that exposes it on `rb_cDiscordActivity`, the `DiscordGameSDK::Activity` class, is

```C
EXPOSE_ATTRIBUTE(rb_cDiscordActivity, activity, type), which expands to:
rb_define_method(rb_cDiscordActivity, "type", rb_discord_activity_get_type, 0);
rb_define_method(rb_cDiscordActivity, "type=", rb_discord_activity_set_type, 1);
```

So that's the getters and setters!  
When you try to get `activity.type` in Ruby, it calls `rb_discord_activity_get_type` with 0 arguments, which uses the `rb_discord_activity_get_struct` helper function to get the underlying C struct, access it's type variable, then return it as a Ruby number.  
Other attributes work similarly (with some occasional nuance), like strings having a max length (and having to be `strncpy`'d over), or for things like setting activity.  
`assets_large_image` in Ruby actually sets `activity->assets.large_large` in C.  
(I can technically make them actual nested objects, but that's way to much work)
