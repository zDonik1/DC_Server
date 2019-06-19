#pragma once

#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include "utilities.h"


// ResourceManager class - manages all the resources such as textures, audio
template<typename Derived, typename T>
class ResourceManager
{
public:
    ResourceManager(const std::string& l_pathsFile)
    {
        loadPaths(l_pathsFile);
    }

    virtual ~ResourceManager()
    {
        purgeResources();
    }

    // returns the resource
    T* getResource(const std::string& l_id)
    {
        auto res = find(l_id);
        return (res ? res->first : nullptr);
    }

    std::string getPath(const std::string& l_id)
    {
        auto path = m_paths.find(l_id);
        return (path != m_paths.end() ? path->second : "");
    }

    // if resources exists, increments count of resource, otherwise loads the resource and sets the count to 1
    //   stroring in a map. Should be used in conjunction with getResource() method
    bool requireResource(const std::string& l_id)
    {
        auto res = find(l_id);
        if (res) {
            ++res->second;
            return true;
        }

        auto path = m_paths.find(l_id);
        if (path == m_paths.end())
            return false;

        T* resource = load(path->second);
        if (!resource)
            return false;

        m_resources.emplace(l_id, std::make_pair(resource, 1));
        return true;
    }

    // deletes the allocated memory for resource and erases the entry from the map
    bool releaseResource(const std::string& l_id)
    {
        auto res = find(l_id);
        if (!res)
            return false;

        --(res->second);

        if (!res->second)
            unload(l_id);

        return true;
    }

    // deletes and erases all resources
    void purgeResources()
    {
        while (m_resources.begin() != m_resources.end()) {
            delete m_resources.begin()->second.first;
            m_resources.erase(m_resources.begin());
        }
    }

    // this method utilises Curiously Recurring Template Pattern because the load function for each type of resource
    //    will be different, however this function gets called in requireResource() method
    T* load(const std::string& l_path)
    {
        return static_cast<Derived*>(this)->load(l_path);
    }

private:
    // finds the resource from the map and returns the pair
    std::pair<T*, unsigned int>* find(const std::string& l_id)
    {
        auto itr = m_resources.find(l_id);
        return (itr != m_resources.end() ? &itr->second : nullptr);
    }

    // deletes and erases the resource with specified handle
    bool unload(const std::string& l_id)
    {
        auto itr = m_resources.find(l_id);
        if (itr == m_resources.end())
            return false;

        delete itr->second.first;
        m_resources.erase(itr);
        return true;
    }

    // loads all the paths into a map from "textures.cfg" file
    void loadPaths(const std::string& l_pathFile)
    {
        std::ifstream paths;
        paths.open(Utils::getWorkingDirectory() + l_pathFile);

        if (paths.is_open()) {
            std::string line;

            while (std::getline(paths, line)) {
                std::stringstream keystream(line);
                std::string pathName;
                std::string path;
                keystream >> pathName >> path;

                m_paths.emplace(pathName, path);
            }

            paths.close();
            return;
        }

        std::cerr << "! Failed loading the path file: " << l_pathFile << std::endl;
    }

    // stores a pair of resources and the count of used resources
    std::unordered_map<std::string, std::pair<T*, unsigned int>> m_resources;

    // stores paths to resources
    std::unordered_map<std::string, std::string> m_paths;
};
