#include <iostream>
#include <vector>
#include <unordered_map>
#include <stack>
#include <cstring>
#include <cstdint>
#include <fstream>
#include <functional>

enum class OpCode : uint32_t
{
    NOP,        // 无操作
    LOAD,       // 加载数据到寄存器
    STORE,      // 从寄存器存储数据
    ADD,        // 加法
    SUB,        // 减法
    MUL,        // 乘法
    DIV,        // 除法
    JMP,        // 无条件跳转
    JZ,         // 如果零则跳转
    JNZ,        // 如果非零则跳转
    CALL,       // 调用子程序
    RET,        // 返回
    PUSH,       // 压栈
    POP,        // 出栈
    AND,        // 位与
    OR,         // 位或
    XOR,        // 位异或
    NOT,        // 位非
    CMP,        // 比较
    HLT,        // 停止执行
    LEA,        // 加载有效地址
    SHR,        // 逻辑右移
    SHL,        // 逻辑左移
    SAR,        // 算术右移
    SHRA,       // 算术左移 (通常与SHL相同)
    MOD,        // 取模
    NEG,        // 取反 (数值)
    INC,        // 自增
    DEC,        // 自减
    JS,         // 如果符号位设置则跳转
    JC,         // 如果进位位设置则跳转
    JNC,        // 如果没有进位位设置则跳转
    JNS,        // 如果没有符号位设置则跳转
    TEST,       // 测试位
    MOV,        // 寄存器间移动数据
    LOG,        // 打印
    CALLFROMCPP // 调用C++函数
};

class VirtualMachine
{
private:
    std::vector<uint32_t> registers;
    std::vector<uint8_t> memory;
    std::stack<uint32_t> stack;
    uint32_t programCounter = 0; // 程序计数器
    std::unordered_map<std::string, std::function<void(uint32_t)>> functionTable;
    bool signFlag = false;                                              // 符号位标志
    bool carryFlag = false;                                             // 进位位标志
    std::vector<std::pair<OpCode, std::vector<uint32_t>>> instructions; // 存储指令序列

public:
    VirtualMachine(size_t numRegisters, size_t memorySize)
        : registers(numRegisters, 0), memory(memorySize, 0) {}

    void defineFunction(const std::string &name, std::function<void(uint32_t)> func)
    {
        functionTable[name] = func;
    }

    void loadMemory(const char *data, size_t address, size_t length)
    {
        std::memcpy(&memory[address], data, length);
    }

    void setRegister(uint32_t index, uint32_t value)
    {
        if (index < registers.size())
        {
            registers[index] = value;
        }
    }

    uint32_t getRegister(uint32_t index)
    {
        if (index < registers.size())
        {
            return registers[index];
        }
        return 0;
    }

    void setProgramCounter(uint32_t pc)
    {
        programCounter = pc;
    }

    uint32_t getProgramCounter() const
    {
        return programCounter;
    }

    void addInstruction(OpCode op, const std::vector<uint32_t> &operands)
    {
        instructions.emplace_back(op, operands);
    }

    void execute(OpCode op, const std::vector<uint32_t> &operands)
    {
        switch (op)
        {
        case OpCode::NOP:
            // 无操作
            break;
        case OpCode::LOAD:
            registers[operands[0]] = memory[operands[1]];
            break;
        case OpCode::STORE:
            memory[operands[1]] = static_cast<uint8_t>(registers[operands[0]]);
            break;
        case OpCode::ADD:
            registers[operands[2]] = registers[operands[0]] + registers[operands[1]];
            break;
        case OpCode::SUB:
            registers[operands[2]] = registers[operands[0]] - registers[operands[1]];
            break;
        case OpCode::MUL:
            registers[operands[2]] = registers[operands[0]] * registers[operands[1]];
            break;
        case OpCode::DIV:
            if (registers[operands[1]] != 0)
            {
                registers[operands[2]] = registers[operands[0]] / registers[operands[1]];
            }
            break;
        case OpCode::JMP:
            setProgramCounter(operands[0]);
            break;
        case OpCode::JZ:
            if (getRegister(operands[0]) == 0)
            {
                setProgramCounter(operands[1]);
            }
            break;
        case OpCode::JNZ:
            if (getRegister(operands[0]) != 0)
            {
                setProgramCounter(operands[1]);
            }
            break;
        case OpCode::CALL:
            stack.push(getProgramCounter() + 1);
            setProgramCounter(operands[0]);
            break;
        case OpCode::RET:
            if (!stack.empty())
            {
                setProgramCounter(stack.top());
                stack.pop();
            }
            break;
        case OpCode::PUSH:
            stack.push(registers[operands[0]]);
            break;
        case OpCode::POP:
            if (!stack.empty())
            {
                registers[operands[0]] = stack.top();
                stack.pop();
            }
            break;
        case OpCode::AND:
            registers[operands[2]] = registers[operands[0]] & registers[operands[1]];
            break;
        case OpCode::OR:
            registers[operands[2]] = registers[operands[0]] | registers[operands[1]];
            break;
        case OpCode::XOR:
            registers[operands[2]] = registers[operands[0]] ^ registers[operands[1]];
            break;
        case OpCode::NOT:
            registers[operands[1]] = ~registers[operands[0]];
            break;
        case OpCode::CMP:
            signFlag = ((int32_t)registers[operands[0]] < (int32_t)registers[operands[1]]);
            carryFlag = (registers[operands[0]] >= registers[operands[1]]);
            break;
        case OpCode::HLT:
            // 停止执行
            break;
        case OpCode::LEA:
            registers[operands[0]] = operands[1];
            break;
        case OpCode::SHR:
            registers[operands[2]] = registers[operands[0]] >> (operands[1] & 0x1F);
            break;
        case OpCode::SHL:
            registers[operands[2]] = registers[operands[0]] << (operands[1] & 0x1F);
            break;
        case OpCode::SAR:
            registers[operands[2]] = (int32_t)registers[operands[0]] >> (operands[1] & 0x1F);
            break;
        case OpCode::SHRA:
            registers[operands[2]] = registers[operands[0]] << (operands[1] & 0x1F);
            break;
        case OpCode::MOD:
            if (registers[operands[1]] != 0)
            {
                registers[operands[2]] = registers[operands[0]] % registers[operands[1]];
            }
            break;
        case OpCode::NEG:
            registers[operands[1]] = -registers[operands[0]];
            break;
        case OpCode::INC:
            registers[operands[0]]++;
            break;
        case OpCode::DEC:
            registers[operands[0]]--;
            break;
        case OpCode::JS:
            if (signFlag)
            {
                setProgramCounter(operands[0]);
            }
            break;
        case OpCode::JC:
            if (carryFlag)
            {
                setProgramCounter(operands[0]);
            }
            break;
        case OpCode::JNC:
            if (!carryFlag)
            {
                setProgramCounter(operands[0]);
            }
            break;
        case OpCode::JNS:
            if (!signFlag)
            {
                setProgramCounter(operands[0]);
            }
            break;
        case OpCode::TEST:
            signFlag = ((int32_t)registers[operands[0]] < 0);
            carryFlag = (registers[operands[0]] != 0);
            break;
        case OpCode::MOV:
            registers[operands[1]] = registers[operands[0]];
            break;
        case OpCode::LOG:
        {
            const char *str = reinterpret_cast<const char *>(&memory[operands[0]]);
            std::cout << str << std::endl;
        }
        break;
        case OpCode::CALLFROMCPP:
            if (functionTable.find(std::to_string(operands[0])) != functionTable.end())
            {
                functionTable[std::to_string(operands[0])](operands[1]);
            }
            else
            {
                std::cerr << "无法找到方法" << std::endl;
            }
            break;
        default:
            // 处理未知操作码
            std::cerr << "未知操作码" << std::endl;
            break;
        }
    }

    void executeInstructions()
    {
        for (const auto &[op, operands] : instructions)
        {
            execute(op, operands);
        }
    }

    void saveStateToFile(const std::string &filename) const
    {
        std::ofstream file(filename, std::ios::binary);
        if (file.is_open())
        {
            // 写寄存器
            file.write(reinterpret_cast<const char *>(registers.data()), registers.size() * sizeof(uint32_t));
            // 写内存
            file.write(reinterpret_cast<const char *>(memory.data()), memory.size());
            // 写堆栈
            std::stack<uint32_t> tempStack = stack; // 复制堆栈
            size_t stackSize = tempStack.size();
            file.write(reinterpret_cast<const char *>(&stackSize), sizeof(size_t));
            while (!tempStack.empty())
            {
                file.write(reinterpret_cast<const char *>(&tempStack.top()), sizeof(uint32_t));
                tempStack.pop();
            }
            // 写程序计数器
            file.write(reinterpret_cast<const char *>(&programCounter), sizeof(uint32_t));
            // 写标志位
            file.write(reinterpret_cast<const char *>(&signFlag), sizeof(bool));
            file.write(reinterpret_cast<const char *>(&carryFlag), sizeof(bool));
            // 写指令序列
            size_t instructionCount = instructions.size();
            file.write(reinterpret_cast<const char *>(&instructionCount), sizeof(size_t));
            for (const auto &[op, operands] : instructions)
            {
                file.write(reinterpret_cast<const char *>(&op), sizeof(OpCode));
                size_t operandCount = operands.size();
                file.write(reinterpret_cast<const char *>(&operandCount), sizeof(size_t));
                file.write(reinterpret_cast<const char *>(operands.data()), operands.size() * sizeof(uint32_t));
            }

            file.close();
        }
        else
        {
            std::cerr << "无法保存状态机" << std::endl;
        }
    }

    void loadStateFromFile(const std::string &filename)
    {
        std::ifstream file(filename, std::ios::binary);
        if (file.is_open())
        {
            // 清空现有的状态
            registers.assign(registers.size(), 0);
            memory.assign(memory.size(), 0);
            instructions.clear();

            // 读寄存器
            file.read(reinterpret_cast<char *>(registers.data()), registers.size() * sizeof(uint32_t));
            // 读内存
            file.read(reinterpret_cast<char *>(memory.data()), memory.size());
            // 读堆栈
            size_t stackSize;
            file.read(reinterpret_cast<char *>(&stackSize), sizeof(size_t));
            for (size_t i = 0; i < stackSize; ++i)
            {
                uint32_t value;
                file.read(reinterpret_cast<char *>(&value), sizeof(uint32_t));
                stack.push(value);
            }
            // 读程序计数器
            file.read(reinterpret_cast<char *>(&programCounter), sizeof(uint32_t));
            // 读标志位
            file.read(reinterpret_cast<char *>(&signFlag), sizeof(bool));
            file.read(reinterpret_cast<char *>(&carryFlag), sizeof(bool));
            // 读指令序列
            size_t instructionCount;
            file.read(reinterpret_cast<char *>(&instructionCount), sizeof(size_t));
            for (size_t i = 0; i < instructionCount; ++i)
            {
                OpCode op;
                file.read(reinterpret_cast<char *>(&op), sizeof(OpCode));
                size_t operandCount;
                file.read(reinterpret_cast<char *>(&operandCount), sizeof(size_t));
                std::vector<uint32_t> operands(operandCount);
                file.read(reinterpret_cast<char *>(operands.data()), operands.size() * sizeof(uint32_t));
                instructions.emplace_back(op, operands);
            }

            file.close();

            // 自动执行指令序列
            executeInstructions();
        }
        else
        {
            std::cerr << "无法加载保存的状态机" << std::endl;
        }
    }
};
