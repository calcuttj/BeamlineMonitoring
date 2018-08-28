#ifndef DIPDATATYPE_H_INCLUDED
#define DIPDATATYPE_H_INCLUDED
/**
* Enumeration of the data type supported by DIP
*/
enum DipDataType {
	TYPE_NULL = 0,

	TYPE_BOOLEAN = 1,
	TYPE_BOOLEAN_ARRAY = 10,

	TYPE_BYTE = 2,
	TYPE_BYTE_ARRAY = 20,

	TYPE_SHORT = 3,
	TYPE_SHORT_ARRAY = 30,

	TYPE_INT = 4,
	TYPE_INT_ARRAY = 40,

	TYPE_LONG = 5,
	TYPE_LONG_ARRAY = 50,

	TYPE_FLOAT = 6,
	TYPE_FLOAT_ARRAY = 60,

	TYPE_DOUBLE = 7,
	TYPE_DOUBLE_ARRAY = 70,

	TYPE_STRING = 8,
	TYPE_STRING_ARRAY = 80

};

#endif // DIPDATATYPE_H_INCLUDED
