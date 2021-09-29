#include "interpreter.hpp"

void interpret_linux(std::vector<Expression> &expressions)
{
	constexpr int STACK_SIZE = 10000;
	int stack[STACK_SIZE];
	int *sp = stack;
	
	// Register is apparently a reserved c++ keyword *shrugs*
	int reg = 0;

	auto fatal_err = [](const char *str) -> void
	{
		printf("\x1b[41mFatal error: %s.\n", str);
		throw std::runtime_error(str);
	};

	auto warning_err = [](const char *str) -> void
	{
		printf("\x1b[43mWarning: %s.\n", str);
	};
	
	auto iterator = expressions.begin();
	// Search for start label
	for(; iterator != expressions.end(); iterator++)
	{
		if(iterator->ins.type == Instruction::Type::LABEL && iterator->value[0].val == hash(".start"))
			break;
	}

	if(iterator == expressions.end())
	{
		fatal_err("No entry point (.start)");
	}

	for(; iterator != expressions.end(); iterator++)
	{
		auto &it = *iterator;

		// Error checking
		if(sp < stack)
		{
			// TODO: Add flavor text (... at line xx and instruction xx)
			fatal_err("Stack underflow");
		}

		if(sp > stack + STACK_SIZE)
		{
			warning_err("Stack overflow, probable segmentation fault");
		}
		
		switch(it.ins.type)
		{
			case Instruction::Type::PUSH:
				if(it.numValues == 0)
				{
					// Push register value instead
					sp++;
					*sp = reg;
				}
				for(int i = 0; i < it.numValues; i++)
				{
					sp++;	
					*sp = it.value[i].val;
				}
				break;
			case Instruction::Type::POP:
				// Pop current stack value to register
				reg = *sp;
				sp--;
				break;
			case Instruction::Type::PRINT:
				for(int i = 0; i < it.numValues; i++)
				{
					// Will add support for more types later
					if(it.value[i].type == Value::Type::STRING)
					{
						printf("%s", handle_escape_sequences(reinterpret_cast<char *>(it.value[i].val)));
					}
					else
					{
						printf("%" PRId64, it.value[i].val);
					}
				}
				break;
			case Instruction::Type::IFEQ: // Only evaluates next instruction if condition is true
				// ifeq [comparator],[value]
				
				break;
			case Instruction::Type::JUMP:
			{
				// Find matching label with a linear search
				// Efficicency is shit
				// Labels should be stored in their own vector
				
				auto match = expressions.end();
				for(auto itlabel = expressions.begin(); itlabel != expressions.end(); itlabel++)
				{
					if(itlabel->ins.type != Instruction::Type::LABEL) continue;

					if(itlabel->value->val == it.value->val)
					{
						match = itlabel;
						break;
					}
				}

				if(match == expressions.end())
				{
					// TODO: Move this to "compilation" step
					warning_err("Label does not exists."); 
					break;
				}
				
				iterator = match;
				break;
			}
			default:
				break;
		}
	}
}
