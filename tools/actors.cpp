#include "actors.hpp"

std::map<std::string, ActorType> getActorTypes(std::string filename)
{
    // Load the json file
    nlohmann::json j;
    {
        std::ifstream inf;
        inf.exceptions(std::ifstream::badbit | std::ifstream::failbit);
        inf.open(filename);
        inf >> j;
    }

    // Now, for each key, create the specific object
    std::map<std::string, ActorType> actorTypes;

    for (const auto& p : j.items())
    {
        ActorType actorType;
        actorType.includeName = p.value().at("header").get<std::string>();

        for (const auto& p2 : p.value().at("params"))
        {
            Parameter param;
            param.name = p2.at("name").get<std::string>();

            auto type = p2.at("type").get<std::string>();
            if (type == "literal") param.type = ParamType::Literal;
            else if (type == "string") param.type = ParamType::String;
            else if (type == "color") param.type = ParamType::Color;
            else throw std::domain_error("Unrecognized param type " + type + " while parsing actors.json!");

            actorType.parameters.push_back(std::move(param));
        }

        actorTypes.emplace(p.key(), std::move(actorType));
    }

    return actorTypes;
}