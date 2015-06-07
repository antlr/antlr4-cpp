// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#pragma once

#include <memory>
#include <vector>

#include "atn_deserialization_options.hpp"

namespace antlr4 {
namespace atn {

	class grammar_atn;

	class atn_deserializer
	{
	private:
		const atn_deserialization_options deserialization_options;

	public:
		explicit atn_deserializer()
			: deserialization_options(atn_deserialization_options::default_options())
		{
		}

		explicit atn_deserializer(atn_deserialization_options deserialization_options)
			: deserialization_options(deserialization_options)
		{
		}

	public:
		std::shared_ptr<grammar_atn> deserialize(std::vector<uint16_t> const& data) const;
	};

}
}
