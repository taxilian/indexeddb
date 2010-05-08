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

	class Data
		{
		public:
			enum ECMAType { Undefined, Null, Boolean, Number, Integer, String, Object };

			static const Data getUndefinedData() { return Data(0, 0, Data::Undefined); }

			Data(const void* data, size_t size, ECMAType dataType) 
				{ initializeData(this->data, dataType, data, size); }

			Data(const Data& data)
				{ initializeData(this->data, data.getType(), data.getUntypedValue(), data.getSize() - 1); }

			Data(const std::string& value)
				{ initializeData(data, Data::String, value.c_str(), value.size() + 1); }

			const Data::ECMAType getType() const { return (Data::ECMAType)data[0]; }
			const void* getUntypedValue() const { return getSize() > 1 ? &(data[1]) : NULL; }
			const size_t getSize() const { return data.size(); }

			bool operator==(const Data& rvalue) const
				{ return data == rvalue.data; }
			bool operator==(const std::string& rvalue) const
				{ return getType() == Data::String && strcmp(rvalue.c_str(), (const char*)getUntypedValue()) == 0; }
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
			static void initializeData(std::vector<unsigned char>& data, Data::ECMAType type, const void* value, size_t size)
				{
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