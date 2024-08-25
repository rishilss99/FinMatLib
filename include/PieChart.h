#pragma once

#include "stdafx.h"

class PieChart {
public:
    PieChart();
    void setTitle( const std::string& title );
    void addEntry( const std::string& label,
                   double value );
    void writeAsHTML( std::ostream& out ) const;
    void writeAsHTML( const std::string& file ) const;
private:
    std::string title;
    std::vector<std::string> labels;
    std::vector<double> values;
};


void testPieChart();