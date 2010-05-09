/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#ifndef BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_DATA_H
#define BRANDONHAYNES_INDEXEDDB_IMPLEMENTATION_DATA_H

#include <string>
#include <vector>

namespace BrandonHaynes {
namespace IndexedDB { 
namespace Implementation { 

	///<summary>
	/// This class represents a data value in the Indexed Database API implementation.  Data instances
	/// are exchanged between the API layer and implementation layer.
	///</summary>
	class Data
		{
		public:
			// An enumeration of available ECMA types (see ECMA-262 5th Ed §8)
			enum ECMAType { Undefined, Null, Boolean, Number, Integer, String, Object };

			// Utility method to generate an undefined data value
			static const Data getUndefinedData() { return Data(0, 0, Data::Undefined); }

			// Creates a data value with the underlying data, given size, and specified type
			Data(const void* data, size_t size, ECMAType dataType) 
				{ initializeData(this->data, dataType, data, size); }

			// Copy constructor for the Data class
			Data(const Data& data)
				{ initializeData(this->data, data.getType(), data.getRawValue(), data.getSize() - 1); }

			// Creates a data value containing the specified string
			Data(const std::string& value)
				{ initializeData(data, Data::String, value.c_str(), value.size() + 1); }

			// Gets the type associated with this data instance
			const Data::ECMAType getType() const { return (Data::ECMAType)data[0]; }
			// Gets the raw data associated with this instance
			const void* getRawValue() const { return getSize() > 1 ? &(data[1]) : NULL; }
			// Gets the size of this data instance (including type information)
			const size_t getSize() const { return data.size(); }

			// Comparisons between data instances are applied to the underlying raw vector 
			// (note that this does not order types; comparisons involving instances with varying types is undefined)
			bool operator==(const Data& rvalue) const
				{ return data == rvalue.data; }
			bool operator==(const std::string& rvalue) const
				{ return getType() == Data::String && strcmp(rvalue.c_str(), (const char*)getRawValue()) == 0; }
			bool operator>(const Data& rvalue) const
				{ return data > rvalue.data; }
			bool operator>=(const Data& rvalue) const
				{ return data >= rvalue.data; }
			bool operator<(const Data& rvalue) const
				{ return data < rvalue.data; }
			bool operator<=(const Data& rvalue) const
				{ return data <= rvalue.data; }

			operator const void*() const
				{ return &data[0]; }

		private:
			std::vector<unsigned char> data;

		protected:
			// Helper method to initialize the data vector with the given data
			static void initializeData(std::vector<unsigned char>& data, Data::ECMAType type, const void* value, size_t size)
				{
				// Set the vector size, insert the type, and then insert the entire data
				data.reserve(size + 1);
				data.push_back((unsigned char)type);
				if(value != NULL)
					data.insert(data.end(), (unsigned char*)value, (unsigned char*)value + size);
				}
		};
	}
}
}

#endif