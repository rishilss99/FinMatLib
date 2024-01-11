#include "threadingexamples.h"

#include "stdafx.h"
#include "testing.h"

using namespace std;


void primalityTest( int* pointerToInt ) {
	int toTest = *pointerToInt;
	for (int i=2; i<toTest; i++) {
		if ((toTest % i)==0) {
			INFO( toTest << " is not prime" );
			return;
		}
	}
	INFO( toTest << " is prime" );
}

void testPrimes() {
	int values[3] = {1299817,1299821,1299827};
	thread t1( &primalityTest, &values[0] );
	thread t2( &primalityTest, &values[1] );
	thread t3( &primalityTest, &values[2] );
	t1.join();
	t2.join();
	t3.join();
}

double integrate( double (*f)( double ),
				  double a,
				  double b,
				  int nSteps ) {
	double total = 0.0;
	double h = (b-a)/nSteps;
	for (int i=0; i<nSteps; i++) {
		double x = a + i*h + 0.5*h;
		total += h*f(x);
	}
	return total;
}

void testIntegrateByFunctionPointer() {
	double value = integrate( &sin, 0, 1, 1000 );
	ASSERT_APPROX_EQUAL( -cos(1.0)+cos(0.0), value, 0.01);
}

class Account {
public:
	double balance;
	mutex mtx;
};




namespace bad {
bool debitAccount(Account& account, double amount) {
	if (account.balance >= amount) {
		account.balance -= amount;
		return true;
	}
	return false;
}
}

namespace better {
bool debitAccount(Account& account,
  				  double amount) {
	account.mtx.lock();
	bool ret = false;
	if (account.balance >= amount) {
		account.balance -= amount;
		ret = true;
	}
	account.mtx.unlock(); // don't do this
	return ret;
}
}

namespace best {
bool debitAccount(Account& account,
		            double amount) {
	lock_guard<mutex> lock(account.mtx);
	if (account.balance >= amount) {
		account.balance -= amount;
		return true;
	}
	return false;
}
}


bool transferMoney(Account& from,
	Account& to,
	double quantity) {
	lock_guard<mutex> lock1(from.mtx);
	if (from.balance<quantity) {
		return false;
	}
	lock_guard<mutex> lock2(to.mtx);
	from.balance -= quantity;
	to.balance += quantity;
	return true;
}





void testThreadingExamples() {
	TEST( testPrimes );
	TEST( testIntegrateByFunctionPointer );
}