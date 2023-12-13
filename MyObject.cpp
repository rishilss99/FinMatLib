#include "MyObject.h"

using namespace std;

static ostream& operator<<(ostream &out, vector<int> &vec)
{
    out << "**********Print Vector***********" <<"\n";
    for(size_t i = 0; i<vec.size(); i++)
    {
        if(i < vec.size()-1)
            out << vec[i] << " ";
        else
            out << vec[i] << "\n";
    }
    out << "*********************************" <<"\n";
    return out;
}

MyObject::MyObject() : ptr(make_shared<vector<int>>(10))
{
    cout << "Default constructor called"
         << "\n";
}

/*  The copy constructor should create a deep copy  */
MyObject::MyObject(const MyObject &other)
{
    cout << "Copy constructor called"
         << "\n";
    cout << "This shared pointer count(current): " << ptr.use_count() << "\n";
    cout << "Other shared pointer count(currrent): " << other.ptr.use_count() << "\n";
    ptr = make_shared<vector<int>>(*other.ptr);
    cout << "This shared pointer count(updated): " << ptr.use_count() << "\n";
    cout << "Other shared pointer count(updated): " << other.ptr.use_count() << "\n";
}

MyObject::~MyObject()
{
    cout << "Destructor called"
         << "\n";
}

/*  The assignment operator should create a shallow copy  */
MyObject &MyObject::operator=(const MyObject &other)
{
    cout << "Assignment operator called"
         << "\n";
    cout << "This shared pointer count(current): " << ptr.use_count() << "\n";
    cout << "Other shared pointer count(currrent): " << other.ptr.use_count() << "\n";
    ptr.reset();
    ptr = other.ptr;
    cout << "This shared pointer count(updated): " << ptr.use_count() << "\n";
    cout << "Other shared pointer count(updated): " << other.ptr.use_count() << "\n";
    return *this;
}

MyObject operator+(MyObject &a, MyObject &b)
{
    MyObject res;
    ASSERT(a.getVector().size() == b.getVector().size());
    for(size_t i = 0; i<a.getVector().size(); i++)
    {
        res.getVector()[i] = a.getVector()[i] + b.getVector()[i];
    }
    cout << res.getVector();
    return res;
}

static void testOneObject()
{
    MyObject a;
}

static void testTwoObjectsAssignment()
{
    MyObject a;
    MyObject b;
    a = b;
    a.getVector()[0] = 100;
    cout << a.getVector();
    cout << b.getVector();
}

static void testTwoObjectsCopy()
{
    MyObject a;
    MyObject b(a);
    a.getVector()[0] = 100;
    cout << a.getVector();
    cout << b.getVector();
}

static void testTwoObjectsStrangeCopy()
{
    MyObject a;
    MyObject b = a;
    a.getVector()[0] = 100;
    cout << a.getVector();
    cout << b.getVector();
}

static void testTwoObjectsAdd()
{
    MyObject a;
    MyObject b;
    a.getVector()[0] = 1000;
    MyObject c;
    c = a + b;
    cout << c.getVector();
    MyObject d = c;
}

static void testVectorPushBack()
{
    MyObject a;
    auto vectorPushBackFunc = [](MyObject &a)
    {
        vector<MyObject> vec;
        vec.push_back(a);
    };
    vectorPushBackFunc(a);
}


void testMyObject()
{
    TEST(testOneObject);
    TEST(testTwoObjectsAssignment);
    TEST(testTwoObjectsCopy);
    TEST(testTwoObjectsStrangeCopy);
    TEST(testTwoObjectsAdd);
    TEST(testVectorPushBack);
}