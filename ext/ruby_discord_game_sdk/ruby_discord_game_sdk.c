#include "ruby_discord_game_sdk.h"
#include "user.h"

VALUE rb_mRubyDiscordGameSDK;

void Init_ruby_discord_game_sdk(void) {
    rb_mRubyDiscordGameSDK = rb_define_module("RubyDiscordGameSDK");
    rb_discord_init_user(rb_mDiscordGameSDK);
}
