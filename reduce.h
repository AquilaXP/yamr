#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <memory>

#include "data.h"

/// Обработчик свертки
class handler_reduce
{
public:
    virtual void init( uint32_t id ) = 0;
    virtual void move_line( std::string&& line ) = 0;
    virtual void flush() = 0;
    virtual std::shared_ptr<handler_reduce> clone() = 0;
};

using sp_handler_reduce = std::shared_ptr<handler_reduce>;

/// Рабочий свертки
class reduce_worker
{
public:
    void run();
    void set_handler( sp_handler_reduce handler );
    void move_line( std::string&& line );
    void move_data( data_t&& data );
    void flush();

private:
    data_t temp;
    data_t m_data;
    sp_handler_reduce m_handler;
};

/// Управляющий свертки
class reduce
{
public:
    void set_handler( sp_handler_reduce prototype );
    void set_param( uint32_t count_worker );
    void move_data( data_t&& data, uint32_t num );
    void start();

private:
    sp_handler_reduce m_prototype;
    uint32_t m_N = 0;
    std::vector<reduce_worker> m_workers;
    std::vector<data_t> m_data;
};
