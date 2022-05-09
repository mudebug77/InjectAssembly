using Mudebug;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace TragetAssembly2
{
    internal class Program
    {
#if DEBUG
        [DllImport("kernel32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool AllocConsole();
#endif

        public static MainForm MainForm;
        static void Main(string[] args)
        {
#if DEBUG
            AllocConsole();
            Console.WriteLine($"{DateTime.UtcNow} 啟動:{string.Join("\n", args)}");
#endif
            AppEnvironment.AppArgs = args[0];

            var LibarayArg = AppEnvironment.AppArgs.Split('\t');
            AppEnvironment.AppDirectory = LibarayArg[0];
            Console.WriteLine($"AppDirectory:{AppEnvironment.AppDirectory}");

            Environment.SetEnvironmentVariable("PATH", Environment.GetEnvironmentVariable("PATH") + ";" + AppEnvironment.AppDirectory);


            var MainThread = new Thread(() =>
            {
                MainForm = new MainForm();
                MainForm.ShowDialog();
            });
            MainThread.Start();

        }
    }
}
