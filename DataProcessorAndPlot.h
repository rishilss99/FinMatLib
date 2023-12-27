#pragma once

#include "stdafx.h"
#include "LineChart.h"

template <typename T>
class DataProcessorAndPlot
{
public:
    DataProcessorAndPlot(std::string &filePath);

    void readData();

    size_t getDataSize() const
    {
        return dataRowVec_.size();
    }

    std::vector<T> getTopDataRow() const
    {
        return dataRowVec_.front();
    }

    template <typename F, typename U>
    void plotLineChart(F &func, U &unpackcaller, std::string plotTitle, std::string outputFile, size_t ignoreColIdx = SIZE_MAX, size_t xAxisCol = 0);

private:
    void validateFilePath() const;

    std::vector<std::vector<T>> dataRowVec_;
    const std::string filePath_;
    LineChart<T> lineChart_;
};

void testDataProcessorAndPlot();