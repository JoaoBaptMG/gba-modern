#pragma once

#include "pch.hpp"

enum class ParamType { Literal, String, Color };

struct Parameter
{
    std::string name;
    ParamType type;
};

struct ActorType final
{
    std::string includeName;
    std::vector<Parameter> parameters;
};

std::map<std::string, ActorType> getActorTypes(std::string filename);
