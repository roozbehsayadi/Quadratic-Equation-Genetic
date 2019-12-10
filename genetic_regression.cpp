
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include <algorithm>

const int DOTS_COUNT = 1000;
const int POPULATION = 100;
const int GENERATION_COUNT = 100;

namespace utilities { 

	double doubleRand( double fMin, double fMax ) { 
		double f = (double) rand() / RAND_MAX;
		return fMin + f * ( fMax - fMin );
	}

	std::string decimalToBinaryString( long long a ) { 
		std::string binary = "";
		int mask = 1;
		for ( int i = 0; i < 64; i++ ) { 
			if ( ( mask & a ) >= 1 ) 
				binary = "1" + binary;
			else
				binary = "0" + binary;
			mask <<= 1;
		}
		return binary;
	}

}

union doubleBinary { 
	double dValue;
	long long iValue;
};

struct Point { 
	int x, y;
	Point( int x, int y ) { 
		this->x = x;
		this->y = y;
	}
	Point() {}
};

struct Coefficient { 
	double a, b, c;
	Coefficient() {}
	Coefficient( double a, double b, double c ) { 
		this->a = a;
		this->b = b;
		this->c = c;
	}
	friend std::ostream &operator<<( std::ostream&, const Coefficient& );
};

std::ostream &operator<<( std::ostream &out, const Coefficient &c ) { 
	out << '{' << c.a << ',' << c.b << ',' << c.c << '}';
	return out;
}

std::vector<Point> trainPoints( DOTS_COUNT );
std::vector<Coefficient> coefficients( POPULATION );

std::string doubleToBinary( double in ) { 
	doubleBinary temp; 
	temp.dValue = in;
	return utilities::decimalToBinaryString( temp.iValue );
}

void readPoints() {
	std::ifstream x_fin( "x_train.csv" );
	std::ifstream y_fin( "y_train.csv" );
	for ( int i = 0; i < DOTS_COUNT; i++ ) { 
		double temp;
		x_fin >> temp;
		trainPoints[ i ].x = temp;
		y_fin >> temp;
		trainPoints[ i ].y = temp;
	}
	x_fin.close();
	y_fin.close();
}

double f( Coefficient cofs, int x ) { 
	return cofs.a * x * x + cofs.b * x + cofs.c;
}

void generateFirstPopulation() { 

	for ( unsigned int i = 0; i < coefficients.size(); i++ ) { 
		Coefficient &temp = coefficients[ i ];
		temp.a = utilities::doubleRand( -50.0, 50.0 );
		temp.b = utilities::doubleRand( -50.0, 50.0 );
		temp.c = utilities::doubleRand( -50.0, 50.0 );
	}

}

double costFunction( const Coefficient &c ) { 
	double returnValue = 0;
	for ( unsigned int i = 0; i < coefficients.size(); i++ ) 
		returnValue += pow( trainPoints[i].y - f( c, trainPoints[i].x ), 2 );
	return returnValue;
}

void printLosses() { 
	for ( unsigned int i = 0; i < coefficients.size(); i++ ) 
		std::cout << costFunction( coefficients.at( i ) ) << std::endl;
	std::cout << std::endl;
}

int compareLoss( const Coefficient &c1, const Coefficient &c2 ) { 
	double cost1 = costFunction( c1 );
	double cost2 = costFunction( c2 );
	return cost1 < cost2;
}

void sortPopulation() { 

	sort( coefficients.begin(), coefficients.end(), compareLoss );

}

int main () {

	srand( time( NULL ) );

	readPoints();

	generateFirstPopulation();
/*	printLosses();
	exit( 0 );*/
	sortPopulation();

	for ( int i = 0; i < GENERATION_COUNT; i++ ) {

//		crossOver();
//		mutate();
		sortPopulation();
//		dropOff();

	}

	return 0;

}

