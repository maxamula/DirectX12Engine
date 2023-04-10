using Editor.GameProject;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Interop;

namespace Editor.Utils
{
    class SurfaceHost : HwndHost
    {
        public SurfaceHost(ushort width, ushort height)
        {
            _width = width;
            _height = height;
        }
        public bool CanResize { get; set; } = true;
        public void ResizeSwapChain()
        {
            if(swapchainSize != controlSize)
            {
                swapchainSize = controlSize;
                Window.Resize((ushort)controlSize.Width, (ushort)controlSize.Height);
            }    
        }
        public Engine.Window Window { get; private set; }
        Size controlSize = Size.Empty;
        Size swapchainSize = Size.Empty;
        protected override void OnRenderSizeChanged(SizeChangedInfo sizeInfo)
        {
            base.OnRenderSizeChanged(sizeInfo);
            controlSize = sizeInfo.NewSize;
            if(CanResize)
            {
                ResizeSwapChain();
            }
        }
        protected override HandleRef BuildWindowCore(HandleRef hwndParent)
        {
            try
            {
                Window = new Engine.Window(hwndParent.Handle);
                return new HandleRef(this, Window.GetHandle());
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.Message);
                this.Dispose();
                return new HandleRef(this, IntPtr.Zero);
            }
        }

        protected override void DestroyWindowCore(HandleRef hwnd)
        {
            Window.Destroy();
        }

        // Intermediate data
        private ushort _width;
        private ushort _height;
    }
}
