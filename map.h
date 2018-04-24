#pragma once

#include <cstdint>

#include <fstream>
#include <algorithm>
#include <memory>

#include "data.h"

/// Обработчик отображения
class handler_map
{
public:
    virtual void init( uint32_t id ) = 0;
    virtual void push( std::string&& v ) = 0;
    virtual void flush() = 0;
    virtual std::shared_ptr<handler_map> clone() = 0;

    data_t& get_data();

protected:
    data_t m_data;
};

using sp_handler_map = std::shared_ptr<handler_map>;

/// Рабочий отображения
class map_worker
{
public:
    map_worker() = default;
    map_worker( sp_handler_map handler, const std::string& file_name, int64_t begin, int64_t end, uint64_t size_file );

    void set_param( sp_handler_map handler, const std::string& file_name, int64_t begin, int64_t end, uint64_t size_file );
    void run();

private:
    void correct_begin_end( std::ifstream& f );

    sp_handler_map m_handler = nullptr;
    std::string m_file_name;
    int64_t m_begin_file_pos = 0;
    int64_t m_end_file_pos = 0;
    uint64_t m_size_file = 0;
};

/// Управляющий отображения
class map
{
public:
    void set_param( const std::string& file_name, uint32_t count_map );
    void set_handler_prototype( sp_handler_map prototype );
    void start();   
    data_t& get_data( uint32_t num );

private:
    std::vector<std::pair<uint64_t, uint64_t>> split( uint64_t size_file, uint64_t count );

    std::vector<data_t> m_shuffle_data;
    sp_handler_map m_prototype;
    std::vector<sp_handler_map> m_handlers_map;
    std::vector<map_worker> m_workers;
    uint64_t m_size_file = 0;
    std::string m_file_name;
    uint32_t m_count_map = 1;
};
