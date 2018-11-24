#ifndef DISCORDPP_FRAMEWORK_HH
#define DISCORDPP_FRAMEWORK_HH

#include <iostream>
#include <string>

#include <discordpp/bot.hh>

namespace discordpp
{
    using json = nlohmann::json;

    class Framework {

    public:
        Bot * bot;

        Framework(Bot * bot): bot(bot) {}

        void messageReply(json message, std::string content)
        {
            bot->call(
                    "/channels/" + message["channel_id"].get<std::string>() + "/messages",
                    {{"content", content}},
                    "POST"
            );
            bot->send(3, {
                    {
                        "game", {
                            {"name", "with code"},
                            {"type", 0}
                        }
                    },
                    {"status", "online"},
                    {"afk", false},
                    {"since", "null"}
            });
        }

        void messageReply(json message, const char * content)
        {
            this->messageReply(message, std::string(content));
        }

    };
}

#endif // DISCORDPP_FRAMEWORK_HH