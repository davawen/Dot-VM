#include "interpreter.hpp"

void interpret(std::vector<Statement> &statements)
{
	std::stack<intptr_t> stack;
	intptr_t *sp = &stack.top();
	
	intptr_t valReg = 0, valEax = 0, valVoid = 0;
	
	intptr_t *regReg = &valReg, *regEax = &valEax, *regVoid = &valVoid;
	
	
	// Using a lambda as this is tied to the execution context
	auto get_register_value = [&](const Value &value) -> intptr_t
	{
		// TODO: Recursive register dereference
		
		if(value.type != Value::Type::REG && value.type != Value::Type::REG_VALUE)
		{
			runtime_error("Interpreter error, value is not a register (Type is %i)", value.type);
		}

		intptr_t *ptr;
		switch(static_cast<Register>(value.val))
		{
			case Register::REG:
				ptr = regReg;
				break;
			case Register::EAX:
				ptr = regEax;
				break;
			case Register::SP:
				ptr = sp;
				break;
			case Register::VOID:
				ptr = regVoid;
				break;
			default:
				runtime_error("Uknown register: %i", value.val);
				break;
		}
		
		if(value.type == Value::Type::REG) return reinterpret_cast<intptr_t>(ptr);
		else return *ptr; // REG_VALUE
	};
	
	// If the value is a register, return get_register_value, else return the normal value
	auto get_value_if_register = [&](const Value &value) -> intptr_t
	{
		if(value.is_register())
			return get_register_value(value);
		else
			return value.val;
	};

	auto pop_stack = [&]() -> intptr_t
	{
		if(stack.empty())
		{
			runtime_error("No element in stack!");
		}
		
		intptr_t value = stack.top();
		stack.pop();
		sp--;

		return value;
	};
	
	auto push_stack = [&](intptr_t val)
	{
		stack.push(val);
		sp++;
	};

	auto handle_arithmetic_instruction = [&](const Statement &stm, intptr_t (*op)(intptr_t, intptr_t))
	{
			intptr_t left, right;
			if(stm.numArgs == 0)
			{
				left = pop_stack();
				right = pop_stack();
			}
			else
			{
				left = get_value_if_register(stm.args[0]);
				right = get_value_if_register(stm.args[1]);
			}	
			
			push_stack( op(left, right) );
	};
	
	auto handle_shift_instruction = [&](const Statement &stm, intptr_t (*op)(intptr_t, intptr_t))
	{
		// shift [amount]
		// shift [value], [amount]
		//
		intptr_t amount = get_value_if_register( stm.args[stm.numArgs == 2] );
		
		if(stm.numArgs == 1)
		{
			push_stack( pop_stack() << amount );
		}
		else
		{
			if(stm.args[0].is_register())
				push_stack( op( get_register_value(stm.args[0]), amount ) );
			else
				push_stack( op( stm.args[0].val, amount ) );
		}

	};

	auto iterator = statements.begin();
	// Search for start label
	for(; iterator != statements.end(); iterator++)
	{
		if(iterator->ins.type == Instruction::Type::LABEL && iterator->args[0].val == hash(".start"))
			break;
	}

	if(iterator == statements.end())
	{
		// TODO: Check this at compile time
		compile_error(0, 0, "No entry point (.start)");
	}
	
	std::stack<std::vector<Statement>::iterator> callstack;
	callstack.push(statements.end());

	for(; iterator != statements.end(); iterator++)
	{
		auto &stm = *iterator;

		// DONE: The stack should probably grow
		
		// TODO: Needs a lot more helper functions 
		switch(stm.ins.type)
		{
			case Instruction::Type::PUSH:
				for(int i = 0; i < stm.numArgs; i++)
				{
					push_stack(get_value_if_register(stm.args[i]));
				}
				break;
			case Instruction::Type::POP:
				// TODO: Implement offset
				if(stm.numArgs == 1)
				{
					*reinterpret_cast<intptr_t *>(get_register_value(stm.args[0])) = pop_stack();
				}
				else pop_stack();
				break;
			case Instruction::Type::ADD:
				handle_arithmetic_instruction(stm, [](auto left, auto right){ return left + right; });
				break;
			case Instruction::Type::SUB:
				handle_arithmetic_instruction(stm, [](auto left, auto right){ return left - right; });
				break;
			case Instruction::Type::MUL:
				handle_arithmetic_instruction(stm, [](auto left, auto right){ return left * right; });
				break;
			case Instruction::Type::DIV: // DONE: Store remainder somewhere
				handle_arithmetic_instruction(stm, [](auto left, auto right){ return left / right; });
				break;
			case Instruction::Type::MOD:
				handle_arithmetic_instruction(stm, [](auto left, auto right){ return left % right; });
				break;
			case Instruction::Type::AND:
				handle_arithmetic_instruction(stm, [](auto left, auto right){ return left & right; });
				break;
			case Instruction::Type::OR:
				handle_arithmetic_instruction(stm, [](auto left, auto right){ return left | right; });
				break;
			case Instruction::Type::XOR:
				handle_arithmetic_instruction(stm, [](auto left, auto right){ return left ^ right; });
				break;
			case Instruction::Type::NOT:
				if(stm.numArgs == 0)
				{
					push_stack( ~pop_stack() );
					//*sp = ~*sp; This does not check if the stack is empty
				}
				else
				{
					push_stack( ~get_value_if_register(stm.args[0]) );
				}
				break;
			case Instruction::Type::LSHIFT:
				handle_shift_instruction(stm, [](auto left, auto right){ return left << right; });
				break;
			case Instruction::Type::RSHIFT:
				handle_shift_instruction(stm, [](auto left, auto right){ return left >> right; });
				break;
			case Instruction::Type::MOV:
				*reinterpret_cast<intptr_t *>(get_register_value(stm.args[0])) = get_value_if_register(stm.args[1]);
				break;
			case Instruction::Type::LABEL: // Pass through
				break;
			case Instruction::Type::JUMP:
			case Instruction::Type::CALL: // Jump and call both do esssentially the same thing, call just pushes to the callstack
			{
				// Find matching label with a linear search
				// Efficicency is shit
				// Labels should be stored in their own vector
				
				auto match = statements.end();
				
				if(stm.args[0].val == hash("$$"))
				{
					match = callstack.top();
					callstack.pop();
				}
				else
				{
					for(auto itlabel = statements.begin(); itlabel != statements.end(); itlabel++)
					{
						if(itlabel->ins.type != Instruction::Type::LABEL) continue;

						if(itlabel->args[0].val == stm.args[0].val)
						{
							match = itlabel;
							break;
						}
					}
				}

				if(match == statements.end())
				{
					// TODO: Move this to "compilation" step
					std::cout << stm;
					runtime_warning("Label does not exists. (%" PRIdPTR ")", stm.args[0].val);
					break;
				}
				
				if(stm.ins.type == Instruction::Type::CALL) callstack.push(iterator); // iterator is incremented by the for loop

				iterator = match;
				break;
			}
			case Instruction::Type::IFEQ: // Only evaluates next instruction if condition is true
				// ifeq [operator], [value], [value]
				bool (*op)(intptr_t, intptr_t);
				switch(stm.args[0].val)
				{
					case hash("eq"):
						op = [](intptr_t left, intptr_t right){ return left == right; };
						break;
					case hash("lt"):
						op = [](intptr_t left, intptr_t right){ return left < right; };
						break;
					case hash("le"):
						op = [](intptr_t left, intptr_t right){ return left <= right; };
						break;
					case hash("gt"):
						op = [](intptr_t left, intptr_t right){ return left > right; };
						break;
					case hash("ge"):
						op = [](intptr_t left, intptr_t right){ return left >= right; };
						break;
					case hash("ne"):
						op = [](intptr_t left, intptr_t right){ return left != right; };
						break;
					default:
						runtime_error("Wrong operation given"); // TODO: Move this to compile time
						break;
				}
				
				intptr_t left, right;
				if(stm.numArgs == 1)
				{
					left = pop_stack();
					right = pop_stack();
				}
				else
				{
					if( stm.args[1].is_register() )
						left = get_register_value(stm.args[1]);
					else
						left = stm.args[1].val;

					if( stm.args[2].is_register() )
						right = get_register_value(stm.args[2]);
					else
						right = stm.args[2].val;
				}
				
				if(!op(left, right))
				{
					iterator++;
				}

				break;
			case Instruction::Type::PRINT:
				for(int i = 0; i < stm.numArgs; i++)
				{
					switch(stm.args[i].type)
					{
						case Value::Type::STRING:
							printf("%s", handle_escape_sequences(*reinterpret_cast<std::string *>(stm.args[i].val)).c_str());	
							break;
						case Value::Type::REG:
							printf("%p", reinterpret_cast<void *>(get_register_value(stm.args[i])));
							break;
						case Value::Type::REG_VALUE:
							printf("%" PRIdPTR, get_register_value(stm.args[i]));
							break;
						case Value::Type::NUMBER:
							printf("%" PRIdPTR, stm.args[i].val);
							break;
						case Value::Type::LABEL:
							runtime_error("A label was somehow used in a print.");
							break;
					}
				}
				break;
			case Instruction::Type::SYSCALL:
				intptr_t syscallArgs, syscallId;

				if(stm.numArgs == 0)
				{
					syscallArgs = pop_stack();
					syscallId = pop_stack();
				}
				else
				{
					syscallArgs = stm.args[0].val;
					syscallId = stm.args[1].val;
				}
				
				switch(syscallArgs) // Searching something better than this
				{
					case 0:
						syscall(syscallId);
						break;
					case 1:
						syscall(syscallId, pop_stack());
						break;
					case 2:
						syscall(syscallId, pop_stack(), pop_stack());
						break;
					case 3:
						syscall(syscallId, pop_stack(), pop_stack(), pop_stack());
						break;
					case 4:
						syscall(syscallId, pop_stack(), pop_stack(), pop_stack(), pop_stack());
						break;
				}
			default:
				break;
		}
	}
}
