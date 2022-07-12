#include "interpreter.hpp"

// TODO: Move this to a class and make helper functions methods
void interpret(std::vector<Statement> &statements) {
	intptr_t *stack = new intptr_t[128000];
	size_t stackSize = 128000;

	// NOTE: Stack pointer may jump around memory as stack reallocates. This is unavoidable, but really not great.
	intptr_t *sp = stack + stackSize;

	intptr_t valReg = 0, valRax = 0, valRcx = 0;

	intptr_t *regReg = &valReg, *regRax = &valRax, *regRcx = &valRcx;

	// Using a lambda as this is tied to the execution context
	// TODO: Add overload when keeping track of the index is not necessary
	auto get_register_value = [&](const Statement &stm, size_t &idx) -> intptr_t {
		// You can't know how many dereference there will be before the next argument, so you have to have a global
		// index

		// DONE: Recursive register dereference
		// TODO: Add + operator for offsets

		const Value *value = &stm.args[idx];

		size_t dereference = 0;
		while (value->type == Value::Type::OPERATOR) {
			dereference++;

			value = &stm.args[++idx];  // This is catastrophically unsafe
		}

		if (value->type != Value::Type::REG) {
			runtime_error("Interpreter error, value is not a register (Type is %i)", value->type);
		}

		intptr_t *ptr;
		switch (static_cast<Register>(value->val)) {
		case Register::REG:
			ptr = regReg;
			break;
		case Register::RAX:
			ptr = regRax;
			break;
		case Register::RCX:
			ptr = regRcx;
			break;
		case Register::SP:
			ptr = sp;
			break;
		default:
			runtime_error(
				"Uknown register: %i. This is probably an error in the compiler, which should have thrown an error.",
				value->val);
			break;
		}

		for (; dereference > 0; dereference--) {
			ptr = reinterpret_cast<intptr_t *>(*ptr);
		}

		return reinterpret_cast<intptr_t>(ptr);
	};

	// If the value is a register, return get_register_value, else return the normal value
	auto get_value_if_register = [&](const Statement &stm, size_t &idx) -> intptr_t {
		if (stm.args[idx].is_register())
			return get_register_value(stm, idx);
		else
			return stm.args[idx].val;
	};

	auto pop_stack = [&]() -> intptr_t {
		if (sp >= stack + stackSize) {
			runtime_error("No element in stack!");
		}

		return *sp++;
	};

	auto push_stack = [&](intptr_t val) {
		if (sp == stack)  // Reallocates stack
		{
			intptr_t *oldStack = stack;

			stack = new intptr_t[stackSize * 2];  // Would have used realloc but no c++ equivalent

			memcpy(stack + stackSize, oldStack, stackSize);
			sp = stack + stackSize + (sp - oldStack);

			stackSize *= 2;
		}

		sp--;
		*sp = val;
	};

	auto handle_arithmetic_instruction = [&](const Statement &stm, intptr_t (*op)(intptr_t, intptr_t)) {
		intptr_t left, right;
		if (stm.numArgs == 0) {
			left = pop_stack();
			right = pop_stack();
		} else {
			size_t idx = 0;
			left = get_value_if_register(stm, idx);
			idx++;
			right = get_value_if_register(stm, idx);
		}

		push_stack(op(left, right));
	};

	auto handle_shift_instruction = [&](const Statement &stm, intptr_t (*op)(intptr_t, intptr_t)) {
		// shift [amount]
		// shift [operand], [amount]
		size_t idx = 0;
		intptr_t amount = get_value_if_register(stm, idx);
		idx++;

		if (idx == stm.numArgs) {
			push_stack(op(pop_stack(), amount));
		} else {
			intptr_t operand = amount;
			amount = get_value_if_register(stm, idx);

			push_stack(op(operand, amount));
		}
	};

	auto iterator = statements.begin();
	// Search for start label
	for (; iterator != statements.end(); iterator++) {
		if (iterator->ins.type == Instruction::Type::LABEL && iterator->args[0].val == hash("main")) break;
	}

	if (iterator == statements.end()) {
		using namespace std::literals::string_view_literals;

		// TODO: Check this at compile time
		compile_error(0, "No entry point (.start label)");
	}

	std::stack<std::vector<Statement>::iterator> callstack;
	callstack.push(statements.end() - 1);

	for (; iterator != statements.end(); iterator++) {
		auto &stm = *iterator;

		// std::cout << stm << "\n" << "reg ptr: " << regReg << "\nreg value: " << valReg << "\n\n";

		// DONE: The stack should probably grow
		// DONE: The stack grows backwards

		// TODO: Needs a lot more helper functions
		switch (stm.ins.type) {
		case Instruction::Type::PUSH:
			for (size_t i = 0; i < stm.numArgs; i++) {
				push_stack(get_value_if_register(stm, i));
			}
			break;
		case Instruction::Type::POP:
			if (stm.numArgs == 0)
				pop_stack();
			else {
				size_t idx = 0;
				*reinterpret_cast<intptr_t *>(get_register_value(stm, idx)) = pop_stack();
			}
			break;
		case Instruction::Type::SWAP:
			{
				intptr_t val;
				if (stm.numArgs == 0)
					val = 1;
				else {
					size_t idx = 0;
					val = get_value_if_register(stm, idx);
				}

				// NOTE: Casting to unsigned long is okay here, as to get a numerical error, the stack would need to be
				// bigger than the amount of memory in the machine
				if (val < 1 || static_cast<long>(val) >= (stack + stackSize - sp)) {
					runtime_error("Invalid swap amount: %" PRIdPTR ", stack size is %lu", val,
					              (stack + stackSize - sp));
				}

				intptr_t tmp = *sp;

				*sp = *(sp + val);
				*(sp + val) = tmp;
				break;
			}
		case Instruction::Type::ADD:
			handle_arithmetic_instruction(stm, [](auto left, auto right) { return left + right; });
			break;
		case Instruction::Type::SUB:
			handle_arithmetic_instruction(stm, [](auto left, auto right) { return left - right; });
			break;
		case Instruction::Type::MUL:
			handle_arithmetic_instruction(stm, [](auto left, auto right) { return left * right; });
			break;
		case Instruction::Type::DIV:  // DONE: Store remainder somewhere
			handle_arithmetic_instruction(stm, [](auto left, auto right) { return left / right; });
			break;
		case Instruction::Type::MOD:
			handle_arithmetic_instruction(stm, [](auto left, auto right) { return left % right; });
			break;
		case Instruction::Type::AND:
			handle_arithmetic_instruction(stm, [](auto left, auto right) { return left & right; });
			break;
		case Instruction::Type::OR:
			handle_arithmetic_instruction(stm, [](auto left, auto right) { return left | right; });
			break;
		case Instruction::Type::XOR:
			handle_arithmetic_instruction(stm, [](auto left, auto right) { return left ^ right; });
			break;
		case Instruction::Type::NOT:
			if (stm.numArgs == 0) {
				push_stack(~pop_stack());
				//*sp = ~*sp; This does not check if the stack is empty
			} else {
				size_t idx = 0;
				push_stack(~get_value_if_register(stm, idx));
			}
			break;
		case Instruction::Type::LSHIFT:
			handle_shift_instruction(stm, [](auto left, auto right) { return left << right; });
			break;
		case Instruction::Type::RSHIFT:
			handle_shift_instruction(stm, [](auto left, auto right) { return left >> right; });
			break;
		case Instruction::Type::MOV:
			{
				size_t idx = 0;

				// NOTE: Order of assignment evaluation is unspecified
				intptr_t *target = reinterpret_cast<intptr_t *>(get_value_if_register(stm, idx));
				idx++;
				intptr_t source = get_value_if_register(stm, idx);

				*target = source;
				break;
			}
		case Instruction::Type::MOVN:
			{
				size_t idx = 0;

				size_t amount = get_value_if_register(stm, idx);
				idx++;
				intptr_t *target = reinterpret_cast<intptr_t *>(get_value_if_register(stm, idx));
				idx++;
				intptr_t source = get_value_if_register(stm, idx);

				memcpy(target, &source, amount);
			}
		case Instruction::Type::LABEL:  // Pass through
			break;
		case Instruction::Type::JUMP:
		case Instruction::Type::CALL:  // Jump and call both do esssentially the same thing, call just pushes to the
		                               // callstack
			{
				// Find matching label with a linear search
				// Efficicency is shit
				// Labels should be stored in their own vector

				auto match = statements.end();

				if (stm.args[0].val == hash("ret")) {
					match = callstack.top();
					callstack.pop();
				} else {
					for (auto itlabel = statements.begin(); itlabel != statements.end(); itlabel++) {
						if (itlabel->ins.type != Instruction::Type::LABEL) continue;

						if (itlabel->args[0].val == stm.args[0].val) {
							match = itlabel;
							break;
						}
					}
				}

				if (match == statements.end()) {
					// TODO: Move this to "compilation" step
					std::cout << stm;
					runtime_warning("Label does not exists. (%" PRIdPTR ")", stm.args[0].val);
					break;
				}

				if (stm.ins.type == Instruction::Type::CALL)
					callstack.push(iterator);  // iterator is incremented by the for loop

				iterator = match;
				break;
			}
		case Instruction::Type::IFEQ:  // Only evaluates next instruction if condition is true
			// ifeq [operator], [value], [value]
			bool (*op)(intptr_t, intptr_t);
			switch (stm.args[0].val) {
			case hash("eq"):
				op = [](intptr_t left, intptr_t right) { return left == right; };
				break;
			case hash("lt"):
				op = [](intptr_t left, intptr_t right) { return left < right; };
				break;
			case hash("le"):
				op = [](intptr_t left, intptr_t right) { return left <= right; };
				break;
			case hash("gt"):
				op = [](intptr_t left, intptr_t right) { return left > right; };
				break;
			case hash("ge"):
				op = [](intptr_t left, intptr_t right) { return left >= right; };
				break;
			case hash("ne"):
				op = [](intptr_t left, intptr_t right) { return left != right; };
				break;
			default:
				runtime_error("Wrong operation given");  // TODO: Move this to compile time
				break;
			}

			intptr_t left, right;
			if (stm.numArgs == 1) {
				left = pop_stack();
				right = pop_stack();
			} else {
				size_t idx = 1;

				left = get_value_if_register(stm, idx);
				idx++;
				right = get_value_if_register(stm, idx);
			}

			if (!op(left, right)) {
				iterator++;
			}

			break;
		case Instruction::Type::PRINT:
			for (size_t i = 0; i < stm.numArgs; i++) {
				switch (stm.args[i].type) {
				case Value::Type::STRING:
					printf("%s", reinterpret_cast<char *>(stm.args[i].val));
					break;
				case Value::Type::OPERATOR:
				case Value::Type::REG:
					printf("%" PRIdPTR, get_register_value(stm, i));
					break;
				case Value::Type::NUMBER:
					printf("%" PRIdPTR, stm.args[i].val);
					break;
				case Value::Type::LABEL:
					runtime_error("Label used within print. Probable compiler error.");
					break;
				}
			}
			break;
		case Instruction::Type::SYSCALL:
			intptr_t syscallArgs, syscallId;

			if (stm.numArgs == 0) {
				syscallArgs = pop_stack();
				syscallId = pop_stack();
			} else {
				size_t idx = 0;
				syscallArgs = get_value_if_register(stm, idx);
				idx++;
				syscallId = get_value_if_register(stm, idx);
			}

			// TIL order of evaluation of parameters is unspecified
			intptr_t args[4];
			switch (syscallArgs)  // Searching something better than this
			{
			case 0:
				syscall(syscallId);
				break;
			case 1:
				args[0] = pop_stack();

				syscall(syscallId, args[0]);
				break;
			case 2:
				args[0] = pop_stack();
				args[1] = pop_stack();

				syscall(syscallId, args[0], args[1]);
				break;
			case 3:
				args[0] = pop_stack();
				args[1] = pop_stack();
				args[2] = pop_stack();

				syscall(syscallId, args[0], args[1], args[2]);
				break;
			case 4:
				args[0] = pop_stack();
				args[1] = pop_stack();
				args[2] = pop_stack();
				args[3] = pop_stack();

				syscall(syscallId, args[0], args[1], args[2], args[3]);
				break;
			default:
				runtime_error("Too much arguments used with syscall (% " PRIdPTR ", maximum is 4)", syscallArgs);
			}
		case Instruction::Type::NOP:
			break;
		}
	}

	printf("\n[PROGRAM EXITED WITH CODE %" PRIu8 "]\n", static_cast<uint8_t>(valReg));
}
