#ifndef DISCORDPP_FRAMEWORK_HH
#define DISCORDPP_FRAMEWORK_HH

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <regex>
#include <functional>
#include <boost/filesystem.hpp>

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

        std::string matchPrompt(std::string const command)
        {
            // takes a string with the following format:
            // {prompt} command args
            // and returns a vector<string> of args
            std::regex cmd(prompt + "(.+)");

            std::cmatch match;
            std::regex_match(command.c_str(), match, cmd);

            if ( !match.empty() )
            {
                std::vector<std::string> results({});
                std::string r ("");
                for ( auto m : match )
                    results.push_back(m);
                results.erase(results.begin());
                for ( auto &res : results )
                    r.append(res);
                return r;
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

    std::string tokenFromFile(std::string tokenFilePath)
    {
        const std::string err = "CRITICAL: There is no such file as "
                + tokenFilePath + "! Copy the example login.dat to make one.\n";

        if ( boost::filesystem::exists( tokenFilePath ) )
        {
            std::ifstream tokenFile;
            tokenFile.open(tokenFilePath);

            std::string token;

            if (tokenFile.is_open()) {
                std::getline(tokenFile, token);
            } else {
                std::cerr << err;
                exit(1);
            }
            tokenFile.close();

            return token;
        }
        else
        {
            std::cerr << err;
            exit(1);
        }
    }

}

#endif // DISCORDPP_FRAMEWORK_HH
