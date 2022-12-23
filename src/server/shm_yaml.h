#pragma once
#include <optional>
#include <yaml-cpp/yaml.h>
#include <string>
#include <cassert>
#include "../Core/Log.h"

namespace YAML
{
    template <typename T>
    struct as_if<T, std::optional<T> >
    {
        explicit as_if(const YAML::Node& node_) : node(node_) {}
        const YAML::Node& node;

        const std::optional<T> operator()() const
        {
            std::optional<T> val;
            T t;
            if (node.m_pNode && YAML::convert<T>::decode(node, t))
                val = std::move(t);

            return val;
        }
    };

    template <>
    struct as_if<std::string, std::optional<std::string> >
    {
        explicit as_if(const ::YAML::Node& node_) : node(node_) {}

        const ::YAML::Node& node;

        std::optional<std::string> operator()() const
        {
            std::optional<std::string> val;
            std::string t;
            if (node.m_pNode && ::YAML::convert<std::string>::decode(node, t))
                val = std::move(t);

            return val;
        }
    };
}

namespace shm::yaml
{
    template<typename T>
    std::optional<T> AsIf(YAML::Node const& node)
    {
        return YAML::as_if<T, std::optional<T>>(node)();
    }
}
