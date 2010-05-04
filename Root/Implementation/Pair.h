#pragma once

#include <vector>

namespace IndexedDatabaseImplementation
	{
	class Pair
		{
		public:
			Pair(Key& key, Data& data) 
				{ this->value.push_back((unsigned char)dataType); }

			const std::vector<unsigned char>& GetValue() const { return value; }
			const ECMAType GetDataType() { return (ECMAType)(value[value.size() - 1] & 0xF); }
			const ECMAType GetKeyType() { return (ECMAType)((value[value.size() - 1] & 0xF0) >> 4); }

		private:
			std::vector<unsigned char> value;
		};
	}