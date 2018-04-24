#pragma once

#include "map.h"

class my_handler_map : public handler_map
{
public:
    void init( uint32_t id ) override {}
    void push( std::string&& v ) override
    {
        m_data.emplace_back( std::move( v ) );
    }
    std::shared_ptr<handler_map> clone() override
    {
        return std::make_shared<my_handler_map>();
    }
    void flush() override
    {
        sort();
    }

private:
    void sort()
    {
        std::sort( m_data.begin(), m_data.end() );
    }
};
