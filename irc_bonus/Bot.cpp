#pragma once
#include <iostream>
#include "Client.hpp"
/* Create a sassy bot 
- if client calls out the bot, Bot will appear saying "What do you want?"

- if client calls it a second time: "Again? What is it this time?"

- anytime the client asks for help, the bot will just say "I don't want to help you"

** could have an array of strings like, "I dont want to help you", "leave me alone", "get a life", etc...*/

class Bot{
    std::string     _botName;
    std::string*    _popUpResponse;
    std::string*    _helpResponse;
    std::map<Client *, bool> _clientList; //will we need to keep track of clients? bool for _isFirstResponse
};