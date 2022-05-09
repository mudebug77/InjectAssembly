# InjectAssembly
注入c#控制台到非托管进程

注入部分
OpenProcess -> VirtualAllocEx -> 写入shellcode 和 c# 程序 ->CreateRemoteThread

shellcode 部分
CLRCreateInstance v4.0.30319 
通过pDefaultAppDomain->raw_Load_3接口调用加载byte[] 内存中加载c# dll ExecuteAssembly  


https://bbs.pediy.com/thread-272660.htm
