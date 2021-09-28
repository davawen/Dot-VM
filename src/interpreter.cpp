#include "interpreter.hpp"

void interpret(std::vector<Instruction> &instructions)
{
	constexpr int STACK_SIZE = 10000;
	int stack[STACK_SIZE];
	int *sp = stack;
	
	// Register is apparently a reserved c++ keyword *shrugs*
	int reg = 0;

	auto fatalErr = [](const char *str) -> void
	{
		printf("\x1b[41mFatal error: %s.\n", str);
		throw std::runtime_error(str);
	};

	auto warningErr = [](const char *str) -> void
	{
		printf("\x1b[43mWarning: %s.\n", str);
	};
	
	auto iterator = instructions.begin();
	// Search for start label
	for(; iterator != instructions.end(); iterator++)
	{
		if(iterator->type == Instruction::Type::LABEL && iterator->value->val == hash(".start"))
			break;
	}

	if(iterator == instructions.end())
	{
		fatalErr("No entry point (.start)");
	}

	for(; iterator != instructions.end(); iterator++)
	{
		auto &it = *iterator;

		// Error checking
		if(sp < stack)
		{
			// TODO: Add flavor text (... at line xx and instruction xx)
			fatalErr("Stack underflow");
		}

		if(sp > stack + STACK_SIZE)
		{
			warningErr("Stack overflow, probable segmentation fault");
		}
		// std::cout << it << '\n';
		
		switch(it.type)
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
				
				auto match = instructions.end();
				for(auto itlabel = instructions.begin(); itlabel != instructions.end(); itlabel++)
				{
					if(itlabel->type != Instruction::Type::LABEL) continue;

					if(itlabel->value->val == it.value->val)
					{
						match = itlabel;
						break;
					}
				}

				if(match == instructions.end())
				{
					warningErr("Label does not exists."); // This should be checked at "compile" time
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
