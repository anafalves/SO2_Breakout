#pragma once
#ifdef UNICODE
#define tcout wcout
#define tcin wcin
#define tstring wstring
#define tifstream wifstream
#define tofstream wofstream
#define tstringstream wstringstream
#define tistringstream wistringstream
#define tostringstream wostringstream
#else
#define tcout cout
#define tcin cin
#define tstring string
#define tifstream ifstream
#define tofstream ofstream
#define tstringstream stringstream
#define tistringstream istringstream
#define tostringstream ostringstream
#endif