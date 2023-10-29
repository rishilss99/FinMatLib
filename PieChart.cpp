#include "PieChart.h"

#include "textfunctions.h"

using namespace std;





/**
 *  Write the necessary junk for the top of the chart
 */
static void writeTopBoilerPlateOfPieChart( ostream& out) {
    out<<("<html>\n");
    out<<("<head>\n");
    out<<("<!--Load the AJAX API-->\n");
    out<<("<script type=\"text/javascript\"");
    out<<("src=\"https://www.google.com/jsapi\">");
    out<<("</script>\n");
    out<<("<script type=\"text/javascript\">\n");
    out<<("google.load('visualization', '1.0', {'packages':['corechart']});\n");
    out<<("google.setOnLoadCallback(drawChart);\n");
    out<<("function drawChart() {\n");
    out<<("var data = new google.visualization.DataTable();\n");
    out<<("data.addColumn('string', 'Label');\n");
    out<<("data.addColumn('number', 'Value');\n");
}


/**
 *  Write the intesting bit of the Pie Chart
 */
static void writeDataOfPieChart( ostream& out,
                                const vector<string>& labels,
                                const vector<double>& values) {
    out<< "data.addRows([\n";
    int nLabels = labels.size();
    for (int i=0; i<nLabels; i++) {
        string label = labels[i];
        double value = values[i];
        out<<"['"<<escapeJavascriptString(label)<<"', "<<value<<"]";
        if (i!=nLabels-1) {
            out<<",";
        }
        out<<"\n";
    }
    out<<"]);\n";
}

/**
 *  Write the boring bit at the bottom of a pie chart
 */
static void writeBottomBoilerPlateOfPieChart( ostream & out, std::string title ) {
    out<<"var options = {'title':'"<<escapeJavascriptString(title)<<"',\n";
    out<<"'width':400,\n";
    out<<"'height':300\n";
    out<<"};\n";
    out<<"var chart = new google.visualization.PieChart(document.getElementById('chart_div'));\n";
    out<<"chart.draw(data, options);\n";
    out<<"}\n";
    out<<"</script>\n";
    out<<"</head>\n";
    out<<"<body>\n";
    out<<"<div id='chart_div'>\n";
    out<<"</body>\n";
    out<<"</html>";
}


////////////////////////
// Pie Chart implementation
////////////////////////


PieChart::PieChart() :
    title("A Pie Chart") {
}



void PieChart::setTitle( const std::string& t ) {
    title = t;
}

void PieChart::addEntry( const string& label,
                         double value ) {
    labels.push_back( label );
    values.push_back( value );
}

void PieChart::writeAsHTML( ostream& out ) const {
    writeTopBoilerPlateOfPieChart(out);
    writeDataOfPieChart( out, labels, values );
    writeBottomBoilerPlateOfPieChart( out, title );
}


void PieChart::writeAsHTML( const string& file ) const {
    ofstream out;
    out.open(file.c_str());
    writeAsHTML( out );
    out.close();
}





///////////////////////////////////////////
//
//   Tests for pie chart
//
///////////////////////////////////////////



static void testPieChartData() {
    // this test automates the checking
    stringstream out;
    vector<string> labels(3);
    vector<double> vals(3);
    for (int i=0; i<3; i++) {
        stringstream ss;
        ss<<"A Label "<<i;
        labels[i] =ss.str();
        INFO( labels[i] );
        vals[i]=(double)i;
    }
    writeDataOfPieChart( out,
              labels,
              vals );
    string asString = out.str();

    stringstream expected;
    expected<<"data.addRows([\n";
    expected<<"['A Label 0', 0],\n";
    expected<<"['A Label 1', 1],\n";
    expected<<"['A Label 2', 2]\n";
    expected<<"]);\n";
    string expectedStr = expected.str();
    ASSERT( asString==expectedStr );
}

static void testPieChartClass() {
    // just checks that the class compiles etc.
    PieChart pieChart;
    pieChart.addEntry("Mushrooms",200);
    pieChart.addEntry("Salami",100);
    pieChart.addEntry("Spinach",150);
    pieChart.setTitle("Pizza Toppings");

    pieChart.writeAsHTML( "PizzaPie.html" );
}



void testPieChart() {
    TEST( testPieChartData );
    TEST( testPieChartClass );
}
