#pragma once

#include "stdafx.h"

template <typename T>
class LineChart {
public:
    LineChart();
    void setTitle( const std::string& title );
    void setSeries( const std::vector<T>& x,
                    const std::vector<T>& y );
    void writeAsHTML( std::ostream& out ) const;
    void writeAsHTML( const std::string& file ) const;
private:
    std::string title;
    std::vector<T> x;
    std::vector<T> y;
};


void testLineChart();