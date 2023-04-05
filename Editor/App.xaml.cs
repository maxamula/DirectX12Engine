using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using System.Windows;

namespace Editor
{
    /// <summary>
    /// Логика взаимодействия для App.xaml
    /// </summary>
    public partial class App : Application
    {
        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool AllocConsole();
        [DllImport("kernel32.dll")]
        static extern IntPtr GetStdHandle(int handle);
        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);
            AllocConsole();
            Engine.Core.Init();
        }

        protected override void OnExit(ExitEventArgs e)
        {
            base.OnExit(e);
            Engine.Core.Shutdown();
        }
    }
}
