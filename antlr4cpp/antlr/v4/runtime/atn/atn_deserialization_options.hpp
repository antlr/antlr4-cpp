/*
 *  Copyright (c) 2015 Sam Harwell, Tunnel Vision Laboratories LLC
 *  All rights reserved.
 *
 *  The source code of this document is proprietary work, and is not licensed for
 *  distribution. For information about licensing, contact Sam Harwell at:
 *      sam@tunnelvisionlabs.com
 */
#pragma once

namespace antlr4 {
namespace runtime {
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
}
