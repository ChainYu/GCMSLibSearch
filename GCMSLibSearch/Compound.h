#pragma once
//#include "StdAfx.h" //
#include <iostream>


class Peak {
public:
	Peak(void):_compoundID(0), _peakCount(0), _peakData(""), _matchDegree(-1) { }
	virtual ~Peak(void) { }

	int _compoundID;
	int _peakCount;
	std::string _peakData;
	int _matchDegree; //ƥ���
};

class PeakPoint {
public:
	PeakPoint():_compoundID(0), _x(0), _y(0) { }
	PeakPoint(int id):_compoundID(id), _x(0), _y(0) { }

    int _compoundID;
	int _x;
	int _y;
};

class FilterPoint {
public:
	FilterPoint(): _peakPoint(), _yrx(0), _rank(0) { }
	FilterPoint(int id): _peakPoint(id), _yrx(0), _rank(0) { }

	PeakPoint _peakPoint;
	int _yrx;
	int _rank;

	void printXY() { std::cout <<"("<< _peakPoint._x << "," << _peakPoint._y <<")"<< std::endl; }
	void printY() { std::cout << _peakPoint._y << std::endl; }
};

class Compound {

public:
	Compound(void);
	virtual ~Compound(void);

	void print();
	void printBrief();

public:
	int _compoundID;
	std::string _compoundName;
	std::string _formula;
	int _massWeight;
	std::string _casNo;
	int _peakCount;
	int _maxX; //���������
	std::string _peakData;
	int _matchDegree; //ƥ���
};

class LibConfig {
public:
	LibConfig():_dbPath1st(""), _dbPath2nd(""), _dbPath3rd(""), 
		_minMass1st(0), _minMass2nd(0), _minMass3rd(0), 
		_matchLimitNumber(20), 
		_isUnique(false) { }
	~LibConfig() { }

	std::string _dbPath1st; 
	std::string _dbPath2nd; 
	std::string _dbPath3rd; 
	int _minMass1st;
	int _minMass2nd;
	int _minMass3rd;
	int _upperMass;
	int _lowerMass;
	int _matchLimitNumber;
	bool _isUnique;
};