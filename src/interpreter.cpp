#include "interpreter.hpp"
#include <cstdint>

void interpret(std::vector<Expression> &expressions)
{
	constexpr int STACK_SIZE = 10000;
	intptr_t stack[STACK_SIZE];
	
	intptr_t valReg = 0, valEax = 0, valVoid = 0;
	
	intptr_t *regReg = &valReg, *regEax = &valEax, *regVoid = &valVoid;
	
	intptr_t *sp = stack-1;

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
	
	// If the value is a register, return its value, else return the value of the value
	auto get_value_if_register = [&](const Value &value) -> intptr_t
	{
		if(value.is_register())
			return get_register_value(value);
		else
			return value.val;
	};

	auto pop_stack = [&]() -> intptr_t
	{
		if(sp == stack - 1)
		{
			runtime_error("No element in stack!");
		}

		return *sp--;
	};
	
	auto push_stack = [&](intptr_t val)
	{
		sp++;
		*sp = val;
	};

	auto handle_arithmetic_instruction = [&](const Expression &expr, intptr_t (*op)(intptr_t, intptr_t))
	{
			intptr_t left, right;
			if(expr.numValues == 0)
			{
				left = pop_stack();
				right = pop_stack();
			}
			else
			{
				left = get_value_if_register(expr.value[0]);
				right = get_value_if_register(expr.value[1]);
			}	
			
			push_stack( op(left, right) );
	};
	
	auto handle_shift_instruction = [&](const Expression &expr, intptr_t (*op)(intptr_t, intptr_t))
	{
		// shift [amount]
		// shift [value], [amount]
		//
		intptr_t amount = get_value_if_register( expr.value[expr.numValues == 2] );
		
		if(expr.numValues == 1)
		{
			push_stack( pop_stack() << amount );
		}
		else
		{
			if(expr.value[0].is_register())
				push_stack( op( get_register_value(expr.value[0]), amount ) );
			else
				push_stack( op( expr.value[0].val, amount ) );
		}

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
		// TODO: Check this at compile time
		compile_error(0, 0, "No entry point (.start)");
	}

	for(; iterator != expressions.end(); iterator++)
	{
		auto &it = *iterator;

		// Error checking
		if(sp < stack-1)
		{
			runtime_error("Stack underflow");
		}
		
		// TODO: The stack should probably grow
		if(sp > stack + STACK_SIZE)
		{
			runtime_error("Stack overflow");
		}
		
		// TODO: Needs a lot more helper functions 
		switch(it.ins.type)
		{
			case Instruction::Type::PUSH:
				for(int i = 0; i < it.numValues; i++)
				{
					if(it.value[i].is_register())
						push_stack(get_register_value(it.value[i]));
					else
						push_stack(it.value[i].val);
				}
				break;
			case Instruction::Type::POP:
				// TODO: Implement offset
				if(it.numValues == 1)
				{
					*reinterpret_cast<intptr_t *>(get_register_value(it.value[0])) = pop_stack();
				}
				else pop_stack();
				break;
			case Instruction::Type::ADD:
				handle_arithmetic_instruction(it, [](auto left, auto right){ return left + right; });
				break;
			case Instruction::Type::SUB:
				handle_arithmetic_instruction(it, [](auto left, auto right){ return left - right; });
				break;
			case Instruction::Type::MUL:
				handle_arithmetic_instruction(it, [](auto left, auto right){ return left * right; });
				break;
			case Instruction::Type::DIV: // DONE: Store remainder somewhere
				handle_arithmetic_instruction(it, [](auto left, auto right){ return left / right; });
				break;
			case Instruction::Type::MOD:
				handle_arithmetic_instruction(it, [](auto left, auto right){ return left % right; });
				break;
			case Instruction::Type::AND:
				handle_arithmetic_instruction(it, [](auto left, auto right){ return left & right; });
				break;
			case Instruction::Type::OR:
				handle_arithmetic_instruction(it, [](auto left, auto right){ return left | right; });
				break;
			case Instruction::Type::XOR:
				handle_arithmetic_instruction(it, [](auto left, auto right){ return left ^ right; });
				break;
			case Instruction::Type::NOT:
				if(it.numValues == 0)
				{
					push_stack( ~pop_stack() );
					//*sp = ~*sp; This does not check if the stack is empty
				}
				else
				{
					if(it.value[0].is_register())
						push_stack( ~get_register_value(it.value[0]) );
					else
						push_stack( ~it.value[0].val );
				}
				break;
			case Instruction::Type::LSHIFT:
				handle_shift_instruction(it, [](auto left, auto right){ return left << right; });
				break;
			case Instruction::Type::RSHIFT:
				handle_shift_instruction(it, [](auto left, auto right){ return left >> right; });
				break;
			case Instruction::Type::MOV:
				*reinterpret_cast<intptr_t *>(get_register_value(it.value[0])) = get_value_if_register(it.value[1]);
				break;
			case Instruction::Type::LABEL: // Pass through
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
					std::cout << it;
					runtime_warning("Label does not exists. (%" PRIdPTR ")", it.value[0].val);
					break;
				}
				
				iterator = match;
				break;
			}
			case Instruction::Type::IFEQ: // Only evaluates next instruction if condition is true
				// ifeq [operator], [value], [value]
				bool (*op)(intptr_t, intptr_t);
				switch(it.value[0].val)
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
				if(it.numValues == 1)
				{
					left = pop_stack();
					right = pop_stack();
				}
				else
				{
					if( it.value[1].is_register() )
						left = get_register_value(it.value[1]);
					else
						left = it.value[1].val;

					if( it.value[2].is_register() )
						right = get_register_value(it.value[2]);
					else
						right = it.value[2].val;
				}
				
				if(!op(left, right))
				{
					iterator++;
				}

				break;
			case Instruction::Type::PRINT:
				for(int i = 0; i < it.numValues; i++)
				{
					switch(it.value[i].type)
					{
						case Value::Type::STRING:
							printf("%s", handle_escape_sequences(*reinterpret_cast<std::string *>(it.value[i].val)).c_str());	
							break;
						case Value::Type::REG:
							printf("%p", reinterpret_cast<void *>(get_register_value(it.value[i])));
							break;
						case Value::Type::REG_VALUE:
							printf("%" PRIdPTR, get_register_value(it.value[i]));
							break;
						case Value::Type::NUMBER:
							printf("%" PRIdPTR, it.value[i].val);
							break;
						case Value::Type::LABEL:
							runtime_error("A label was somehow used in a print.");
							break;
					}
				}
				break;
			case Instruction::Type::SYSCALL:
				if(it.numValues == 0)
				{
					
				}
			default:
				break;
		}
	}
}
