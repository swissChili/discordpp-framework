#ifndef DISCORDPP_FRAMEWORK_HH
#define DISCORDPP_FRAMEWORK_HH

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <regex>
#include <functional>

#include <discordpp/bot.hh>

namespace discordpp
{
    using json = nlohmann::json;

    class Framework {

    public:

        std::string playing;
        std::string prompt;

        Framework(Bot *bot
                , std::string playing= ""
                , std::string prompt = "") : bot(bot), playing(playing), prompt(prompt)
        {
            bot->addHandler("MESSAGE_CREATE", [&](discordpp::Bot* bot, json msg){
                this->handle(msg);
            });
        }

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
                            {"name", this->playing},
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

        void handle(json const msg)
        {
            std::cout << "Checking Ownership\n";
            if ( !isMine(msg) )
            {
                std::cout << "Not Mine!\n";
                const std::string match = matchPrompt(msg["content"]);
                std::cout << "Searching for match!" << match << "found\n";

                auto it = commands.find(match);
                std::cout << "Finding lambda \n";

                if ( it != commands.end() )
                    it->second(match, msg);
            }
        }

        void setCommand(std::string command, std::function<void(std::string, json)> handler)
        {
            // now that's ugly
            commands.insert( std::pair<std::string,
                    std::function<void(std::string, json)>>
                    (command, handler) );
        }

        std::string matchPrompt(std::string command)
        {
            // takes a string with the following format:
            // {prompt} command args
            // and returns a vector<string> of args
            std::regex cmd(prompt.append("(.+)"));

            std::cmatch match;
            std::regex_match(command.c_str(), match, cmd);

            if ( !match.empty() )
            {
                for ( auto m : match )
                    std::cout << m << "\n\n";
                return match[1];
            }
            else
                return std::string("");
        }

        std::string matchPrompt(const char * command)
        {
            return this->matchPrompt(std::string(command));
        }

        bool isMine(json msg)
        {
            return bot->me_["id"] == msg["author"]["id"];
        }

    protected:
        Bot * bot;
        std::map<std::string, std::function<void(std::string, json)>> commands;

    };
}

#endif // DISCORDPP_FRAMEWORK_HH
