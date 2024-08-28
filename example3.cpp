#include "VirtualMachine.cpp"
#include <sstream>

// 定义C++函数用于打印 "Hello, World!"
void HelloWorldFunction(uint32_t /* unused */)
{
    std::cout << "Hello From C++!" << std::endl;
}

void ValueFunction(uint32_t a)
{
    std::cout << "Value: " << a << std::endl;
}

int main()
{
    VirtualMachine vm(10, 100); // 创建一个有10个寄存器100字节的虚拟机

    vm.defineFunction("0", HelloWorldFunction); // 注册一个C++函数，储存器名称为0
    vm.defineFunction("1", ValueFunction);      // 注册一个C++函数，储存器名称为1

    const std::string stateFile = "vm_state.bin"; //文件名称
    if (std::ifstream(stateFile)) //文件是否存在
    {
        // 如果文件存在，加载状态
        vm.loadStateFromFile(stateFile);
    }
    //不存在则创建状态
    else 
    {
        vm.addInstruction(OpCode::CALLFROMCPP, {0});         // 调用C++函数(无参数)
        vm.addInstruction(OpCode::CALLFROMCPP, {1, 114514}); // 调用C++函数(1个参数)

        const char *helloWorldString = "Hello, World!";     // 定义指针
        size_t stringLength = strlen(helloWorldString) + 1; // 获取字符串长度

        vm.loadMemory(helloWorldString, 0, stringLength); // 将字符串加载到内存的“0”寄存器中，并告诉内存字符串长度

        vm.addInstruction(OpCode::LOG, {0}); // 打印寄存器“0”的值

        vm.executeInstructions(); // 执行所有指令
                                  
        vm.saveStateToFile(stateFile); //  保存当前状态
    }

    return 0;
}