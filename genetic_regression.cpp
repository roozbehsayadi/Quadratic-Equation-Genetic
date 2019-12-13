
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include <algorithm>
#include <random>

const int DOTS_COUNT = 1000;	// Number of inputs
const int POPULATION = 100;		// Number of population per generation
const int GENERATION_COUNT = 1000;	// Number of generations

const double MUTATION_PROBABILITY = 0.4;
const int MAX_BITS_TO_CHANGE = 3;	// How many bits can a mutation change? 

const std::string FITNESS_FLOW_FILENAME = "fitness_flow";	// We print the best fitness of each generation in this file, if user wants.
std::ofstream fitness_out;

namespace utilities { 

	union doubleBinary { 
		double dValue;
		long long iValue;
	};

	double doubleRand( double fMin, double fMax ) { 
		double f = (double) rand() / RAND_MAX;
		return fMin + f * ( fMax - fMin );
	}

	std::string decimalToBinaryString( long long a ) { 
		std::string binary = "";
		long long mask = 1;
		for ( int i = 0; i < 64; i++ ) { 
			if ( ( mask & a ) >= 1 )
				binary = "1" + binary;
			else
				binary = "0" + binary;
			mask <<= 1;
		}
		return binary;
	}

	std::string doubleToBinaryString( double in ) { 
		doubleBinary temp;
		temp.dValue = in;
		return utilities::decimalToBinaryString( temp.iValue );
	}

	double binaryStringToDouble( std::string s ) { 
		long long sValue = std::stoll( s, NULL, 2 );
		doubleBinary temp; 
		temp.iValue = sValue;
		return temp.dValue;
	}

}

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

bool printFitness;

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
	for ( unsigned int i = 0; i < DOTS_COUNT; i++ ) 
		returnValue += pow( trainPoints[i].y - f( c, trainPoints[i].x ), 2 ) / 100;
	return returnValue;
}

void printLosses() { 
	for ( unsigned int i = 0; i < coefficients.size(); i++ ) 
		std::cout << costFunction( coefficients.at( i ) ) << std::endl;
	std::cout << std::endl;
}

void printPopulation() { 
	for ( unsigned int i = 0; i < coefficients.size(); i++ ) 
		std::cout << coefficients.at( i ) << "  ";
	std::cout << std::endl;
}

int compareLoss( const Coefficient &c1, const Coefficient &c2 ) { 
	double cost1 = costFunction( c1 );
	double cost2 = costFunction( c2 );
	return cost1 < cost2;
}

std::vector<double> crossOver( double a1, double a2, int slicePosition ) { 
	//std::cout << "a1, a2: " << a1 << ' ' << a2 << std::endl;
	std::string a1String = utilities::doubleToBinaryString( a1 );
	std::string a2String = utilities::doubleToBinaryString( a2 );
	std::vector<double> returnValue( 2 );
	//std::cout << "Slice Position: " << slicePosition << std::endl;
	//std::cout << a1String << "        " << a2String << std::endl;
	//std::cout << a1String.substr( 0, slicePosition ) + a2String.substr( slicePosition, std::string::npos ) << "        " << a2String.substr( 0, slicePosition ) + a1String.substr( slicePosition, std::string::npos ) << std::endl;
	returnValue[0] = utilities::binaryStringToDouble( a1String.substr( 0, slicePosition ) + a2String.substr( slicePosition, std::string::npos ) );
	returnValue[1] = utilities::binaryStringToDouble( a2String.substr( 0, slicePosition ) + a1String.substr( slicePosition, std::string::npos ) );
	return returnValue;
}

std::vector<Coefficient> crossOver( const Coefficient &c1, const Coefficient &c2 ) { 

	Coefficient child1, child2;

	double weight1 = (double)rand() / RAND_MAX;
	double weight2 = 1 - weight1;
	child1.a = c1.a * weight1 + c2.a * weight2;
	child1.b = c1.b * weight1 + c2.b * weight2;
	child1.c = c1.c * weight1 + c2.c * weight2;

	child2.a = c1.a * weight2 + c2.a * weight1;
	child2.b = c1.b * weight2 + c2.b * weight1;
	child2.c = c1.c * weight2 + c2.c * weight1;

	std::vector<Coefficient> returnValue( 2 );
	returnValue[0] = child1; returnValue[1] = child2;
	return returnValue;

}

void mutateDouble( double &a ) {

	double probability = (double) rand() / RAND_MAX;
	if ( probability <= MUTATION_PROBABILITY )
		a = ( (double)rand() / RAND_MAX ) * 100 - 50;
	
}

void mutateCoefficient( Coefficient &c ) {
	mutateDouble( c.a );
	mutateDouble( c.b );
	mutateDouble( c.c );
}

void crossOver() { 

	size_t initialPopulation = coefficients.size();
	for ( unsigned int i = 0; i < initialPopulation; i += 2 ) { 
		//std::cout << "About to crossover " << i << ' ' << "and" << ' ' << i + 1 << std::endl;
		auto childsTemp = crossOver( coefficients[i], coefficients[i+1] );
		for ( unsigned int j = 0; j < childsTemp.size(); j++ )
			mutateCoefficient( childsTemp[j] );
		//for_each( childsTemp.begin(), childsTemp.end(), mutateCoefficient );
		coefficients.insert( coefficients.end(), childsTemp.begin(), childsTemp.end() );
		//std::cout << "Pop size: " << coefficients.size() << std::endl;
	}

}

void dropOff() { 
	coefficients.erase( coefficients.begin() + ( coefficients.size() / 2 ), coefficients.end() );
}

void sortPopulation() { 
	sort( coefficients.begin(), coefficients.end(), compareLoss );
}

void writeFitness() {
	fitness_out << costFunction( *coefficients.begin() ) << std::endl;
	// for ( unsigned int i = 0; i < coefficients.size(); i++ ) 
	// 	fitness_out << costFunction( coefficients[i] ) << " ";
	// fitness_out << std::endl;
}

int main ( int argc, char **argv ) {

	if ( argc == 2 && std::string(argv[1]) == "PRINT_FITNESS" )
		printFitness = true;
	
	if ( printFitness ) 
		fitness_out.open( FITNESS_FLOW_FILENAME );

/*	std::cout << utilities::decimalToBinaryString( 4611484187377963200 ) << std::endl;
	exit( 0 );*/

	srand( time( NULL ) );

	readPoints();

	generateFirstPopulation();
//	printPopulation();
/*	printLosses();
	exit( 0 );*/
	sortPopulation();
	if ( printFitness ) 
		writeFitness();

	for ( int i = 0; i < GENERATION_COUNT; i++ ) {

		crossOver();
		sortPopulation();
		dropOff();
		if ( printFitness) 
			writeFitness();

	}

	std::cout << "Answer: " << coefficients[0] << std::endl;
	system( ( "python3 plotter.py " + std::to_string( coefficients[0].a ) + " " + std::to_string( coefficients[0].b ) + " " + std::to_string( coefficients[0].c ) ).c_str() );

	if ( printFitness )
		fitness_out.close();

	return 0;

}

