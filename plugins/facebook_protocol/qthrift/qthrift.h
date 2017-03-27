/*
 * %kadu copyright begin%
 * Copyright 2017 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "libs/boost/variant/get.hpp"
#include "libs/boost/variant/variant.hpp"

#include <QtCore/QByteArray>
#include <experimental/optional>
#include <map>
#include <vector>

namespace QThrift
{

enum class FieldType : int8_t
{
	Stop = 0,
	Bool1 = 1,
	Bool = 2,
	Int8 = 3,
	Int16 = 4,
	Int32 = 5,
	Int64 = 6,
	Double = 7,
	String = 8,
	List = 9,
	Set = 10,
	Map = 11,
	Struct = 12
};

template<typename T>
// struct List;
using List = std::vector<T>;
struct Struct;

template<typename T>
struct Type
{
	static const FieldType type;
};

template<>
struct Type<int32_t>
{
	static const FieldType type = FieldType::Int32;
};

template<>
struct Type<Struct>
{
	static const FieldType type = FieldType::Struct;
};

using Value = boost::variant<
		bool,
		int32_t,
		int64_t,
		QByteArray,
		List<int32_t>,
		List<Struct>,
		Struct>;

struct FieldHeader
{
	FieldType type;
	int16_t id;
};
/*
template<typename T>
struct List
{
	std::vector<T> items;
};
*/
struct Struct
{
	using Fields = std::map<int16_t, Value>;
	Fields fields;

	Struct(std::initializer_list<Fields::value_type> fields) : fields{std::move(fields)} {}

	template<typename T>
	std::experimental::optional<T> get(int16_t i) const
	{
		auto it = fields.find(i);
		if (it == std::end(fields))
			return std::experimental::nullopt;
		auto r = boost::get<T>(&(it->second));
		if (!r)
			return std::experimental::nullopt;
		return *r;
	}
};

}
