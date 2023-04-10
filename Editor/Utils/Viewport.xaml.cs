using EnvDTE80;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Threading;

namespace Editor.Utils
{
    /// <summary>
    /// Логика взаимодействия для Viewport.xaml
    /// </summary>
    public partial class Viewport : UserControl
    {
        static class Win32
        {
            // Constants
            public const int SRCCOPY = 0x00CC0020;

            // Structures
            [StructLayout(LayoutKind.Sequential)]
            public struct POINT
            {
                public int X;
                public int Y;
            }

            [StructLayout(LayoutKind.Sequential)]
            public struct RECT
            {
                public int Left;
                public int Top;
                public int Right;
                public int Bottom;
            }

            // Functions
            [DllImport("user32.dll")]
            public static extern IntPtr GetDC(IntPtr hwnd);

            [DllImport("user32.dll")]
            public static extern int ReleaseDC(IntPtr hwnd, IntPtr hdc);

            [DllImport("gdi32.dll")]
            public static extern bool DeleteObject(IntPtr hObject);

            [DllImport("gdi32.dll")]
            public static extern IntPtr SelectObject(IntPtr hdc, IntPtr hgdiobj);

            [DllImport("gdi32.dll")]
            public static extern bool DeleteDC(IntPtr hdc);

            [DllImport("gdi32.dll", SetLastError = true)]
            public static extern bool BitBlt(
                IntPtr hdcDest, int xDest, int yDest, int width, int height,
                IntPtr hdcSrc, int xSrc, int ySrc, int rop);
        }
        public Viewport()
        {
            InitializeComponent();
            CompositionTarget.Rendering += CompositionTarget_Rendering;
            Loaded += Viewport_Loaded;
        }

        public void Destroy()
        {
            _host.Window.Destroy();
        }

        public void Screenshot()
        {
            
        }

        private void CompositionTarget_Rendering(object sender, EventArgs e)
        {
            _host?.Window.Render();
        }

        public void BeginRender() => _host.Window.Render();
        private void Viewport_Loaded(object sender, RoutedEventArgs e)
        {
            Loaded -= Viewport_Loaded;
            _host = new SurfaceHost((ushort)ActualWidth, (ushort)ActualHeight);

            var window = Window.GetWindow(this);

            var helper = new WindowInteropHelper(window);
            HwndSource.FromHwnd(helper.Handle)?.AddHook(_MsgHook);
            Content = _host;
            //_host.Window.BeginRender();
        }

        private bool _flagRestored = false;
        private IntPtr _MsgHook(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
        {
            switch (msg)
            {
                case 0x0231:    // WM_ENTERSIZEMOVE
                    _host.CanResize = false;
                    _temp = _host;
                    _host = null;
                    break;
                case 0x0214:    // WM_SIZING
                    break;
                case 0x0112:    // WM_SYSCOMMAND
                    break;
                case 0x0232:    // WM_EXITSIZEMOVE
                    _host = _temp;
                    _host.CanResize = true;
                    _host.ResizeSwapChain();
                    break;
            }
            return IntPtr.Zero;
        }

        private SurfaceHost _temp = null;
        private SurfaceHost _host = null;
    }
}
