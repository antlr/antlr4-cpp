// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#pragma once

namespace antlr4 {
namespace atn {

	class atn_deserialization_options
	{
	private:
		bool _verify_atn;
		bool _generate_rule_bypass_transitions;
		bool _optimize;

	public:
		atn_deserialization_options()
			: _verify_atn(true)
			, _generate_rule_bypass_transitions(false)
			, _optimize(true)
		{
		}

	public:
		static atn_deserialization_options default_options()
		{
			return atn_deserialization_options();
		}

	public:
		bool verify_atn() const
		{
			return _verify_atn;
		}

		void verify_atn(bool value)
		{
			_verify_atn = value;
		}

		bool generate_rule_bypass_transitions() const
		{
			return _generate_rule_bypass_transitions;
		}

		void generate_rule_bypass_transitions(bool value)
		{
			_generate_rule_bypass_transitions = value;
		}

		bool optimize() const
		{
			return _optimize;
		}

		void optimize(bool value)
		{
			_optimize = value;
		}
	};

}
}
