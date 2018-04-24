#pragma once

#include <cstdint>
#include <vector>

#include "data.h"

class shuffle
{
public:
    void set_param( uint32_t count_map, uint32_t count_reduce );
    void move_data( data_t&& data, uint32_t num );
    void start();
    data_t& get_data( uint32_t num );
private:
    uint32_t m_count_map = 0;
    uint32_t m_count_reduce = 0;

    std::vector<data_t> m_data_by_reduce;
    std::vector<std::vector<data_t>> m_personal_data;
    std::vector<data_t> m_datas;
};