using EnvDTE80;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
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
        public Viewport()
        {
            InitializeComponent();
            _host = new SurfaceHost((ushort)ActualWidth, (ushort)ActualHeight);
            //_host.WindowCreated += () => { _ready = true; };
            Loaded += Viewport_Loaded;
        }

        public void Destroy()
        {
            Engine.Viewport.Destroy();
        }

        private bool _busy = false;
        private void Render()
        {
            if (_busy)
                return;
            _busy = true;
            Engine.Viewport.Render();
            _busy = false;
        }
        private async void CompositionTarget_Rendering(object sender, EventArgs e)
        {
            
        }

        private void Viewport_Loaded(object sender, RoutedEventArgs e)
        {
            Loaded -= Viewport_Loaded;
            var window = Window.GetWindow(this);
            var helper = new WindowInteropHelper(window);
            HwndSource.FromHwnd(helper.Handle)?.AddHook(_MsgHook);
            Content = _host;
        }

        private bool _flagRestored = false;
        private IntPtr _MsgHook(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
        {
            switch (msg)
            {
                case 0x0231:    // WM_ENTERSIZEMOVE
                    _host.CanResize = false;
                    break;
                case 0x0214:    // WM_SIZING
                    break;
                case 0x0112:    // WM_SYSCOMMAND
                    break;
                case 0x0232:    // WM_EXITSIZEMOVE
                    _host.CanResize = true;
                    _host.ResizeSwapChain();
                    break;
            }
            return IntPtr.Zero;
        }
        private SurfaceHost _host = null;
    }
}
