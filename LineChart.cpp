#include "LineChart.h"
#include "textfunctions.h"

using namespace std;

/**
 *  Write the necessary junk for the top of the chart
 */
static void writeTopBoilerPlateOfLineChart( ostream& out ) {
    out<<("<html>\n");
    out<<("<head>\n");
    out<<("<script type=\"text/javascript\" src=\"https://www.google.com/jsapi\"></script>\n");
    out<<("<script type=\"text/javascript\">\n");
    out<<("google.load('visualization', '1.0', {'packages':['corechart']});\n");
    out<<("google.setOnLoadCallback(drawChart);\n");
    out<<("function drawChart() {\n");

}

/**
 *  Write the necessary junk for the top of the chart
 */
static void writeBottomBoilerPlateOfLineChart( ostream& out, const string& title ) {
    out<<"var options = {title: '" << escapeJavascriptString(title) << "', 'width':800, 'height':800};\n";
    out<<"var chart = new google.visualization.LineChart(document.getElementById('chart_div'));\n";
    out<<"chart.draw(data, options);\n";
    out<<"}\n";
    out<<"</script>\n";
    out<<"</head>\n";
    out<<"<body>\n";
    out<<"<div id='chart_div'>\n";
    out<<"</body>\n";
    out<<"</html>";
}

/**
 *   Write the data of the line chart
 */
static void writeLineChartData( ostream& out,
                               const vector<double>& xPoints,
                               const vector<double>& yPoints ) {
    ASSERT( xPoints.size()==yPoints.size() );
    out<<"var data = google.visualization.arrayToDataTable([\n";
    out<<"['x values','y values'],\n";
    int n = xPoints.size();
    for (int i=0; i<n; i++) {
        double x = xPoints[i];
        double y = yPoints[i];
        out << "[" << x <<", "<<y<<"]";
        if (i!=n-1) {
            out<<",";
        }
        out << "\n";
    }
    out<<"]);\n";
}

template<typename T>
LineChart<T>::LineChart() :
  title("A LineChart") {
}

template<typename T>
void LineChart<T>::setTitle( const string& t ) {
    title = t;
}

template<typename T>
void LineChart<T>::setSeries( const vector<T>& xVec, const vector<T>& yVec ) {
    x = xVec;
    y = yVec;
}

template<typename T>
void LineChart<T>::writeAsHTML( ostream& out ) const {
    writeTopBoilerPlateOfLineChart( out );
    writeLineChartData( out, x, y );
    writeBottomBoilerPlateOfLineChart( out, title );
}

template<typename T>
void LineChart<T>::writeAsHTML( const string& file ) const {
    ofstream out;
    out.open( file.c_str() );
    writeAsHTML( out );
    out.close();
}

template class LineChart<double>;

//////////////////////////////
//
//   TESTS
//
//////////////////////////////


static void testWriteLineChartData() {
    vector<double> x;
    vector<double> y;
    for (int i=0; i<3; i++) {
        x.push_back(i);
        y.push_back(i*i);
    }
    stringstream s;
    writeLineChartData( s, x, y );
    string actual = s.str();
    string expected = string("")
                     + "var data = google.visualization.arrayToDataTable([\n"
                     + "['x values','y values'],\n"
                     + "[0, 0],\n"
                     + "[1, 1],\n"
                     + "[2, 4]\n"
                     + "]);\n";
    cout << actual;
    cout << expected;
    ASSERT( actual==expected );
}


void testLineChart() {
    TEST( testWriteLineChartData );
}