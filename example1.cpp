#include "VirtualMachine.cpp"

int main()
{
    VirtualMachine vm(10, 100); //创建一个有10个寄存器100字节的虚拟机

    const char *helloWorldString = "Hello, World!"; //定义指针
    size_t stringLength = strlen(helloWorldString) + 1; //获取字符串长度

    vm.loadMemory(helloWorldString, 0, stringLength); //将字符串加载到内存的“0”寄存器中，并告诉内存字符串长度

    vm.addInstruction(OpCode::LOG, {0}); //打印寄存器“0”的值
    
    vm.executeInstructions(); //执行所有指令

    return 0;
}