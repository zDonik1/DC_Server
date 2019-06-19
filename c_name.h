#pragma once

#include "c_base.h"

class C_Name : public C_Base
{
public:
    C_Name() : C_Base(Component::Name)
    {
    }

    void readIn(std::stringstream &l_stream)
    {
    }

    const std::string &getName() const
    {
        return m_name;
    }

    void setName(const std::string &l_name)
    {
        m_name = l_name;
    }

private:
    std::string m_name;
};
